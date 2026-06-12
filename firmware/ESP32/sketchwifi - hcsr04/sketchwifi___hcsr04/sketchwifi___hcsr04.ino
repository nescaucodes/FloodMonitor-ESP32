#include <WiFi.h>
#include <WebServer.h>
#include <NimBLEDevice.h>

// Pinos dos componentes
#define TRIG_PIN    26
#define ECHO_PIN    14
#define BUZZER_PIN  25
#define LED1        18
#define LED2        22
#define LED3        23

// Configurações do Wi-Fi
const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678";

// Porta padrão HTTP (80)
WebServer server(80);


// Configurações do BLE
NimBLECharacteristic* pCaracteristica;


// Variáveis globais

// Distância medida pelo sensor (substituiu distanciaSimulada)
volatile float distanciaReal = 0;

// Evita leitura bagunçada entre tarefas diferentes
volatile int distanciaSimulada = 0;

// Intervalo de envio BLE em milissegundos
const long intervaloBLE = 2000;


// Task: Sensor HC-SR04 + LEDs + Buzzer
// Roda no Core 1
void taskSensor(void * parameter) {
  unsigned long tempoBuzzer = 0; // Guarda quando o buzzer foi ativado
  bool buzzerAtivo = false;      // Controle não-bloqueante do buzzer

  for (;;) {
    // Limpa o TRIG
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Envia pulso de 10 µs
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Lê o tempo do ECHO
    long duracao = pulseIn(ECHO_PIN, HIGH);

    // Calcula a distância em cm
    distanciaReal = duracao * 0.0343 / 2;
    distanciaSimulada = (int)distanciaReal; // sincroniza com a variável do Wi-Fi e BLE

    Serial.print("Distancia: ");
    Serial.print(distanciaReal);
    Serial.println(" cm");

    // LEDs acendem conforme o nível de perigo
    digitalWrite(LED1, HIGH);
    delay(500);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(500);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    delay(500);
    digitalWrite(LED3, LOW);

    // Se estiver a menos de 11 cm, ativa o buzzer sem bloquear
    if (distanciaReal > 0 && distanciaReal < 11 && !buzzerAtivo) {
      digitalWrite(BUZZER_PIN, HIGH);
      tempoBuzzer = millis(); // registra o momento de ativação
      buzzerAtivo = true;
    }

    // Desliga o buzzer após 5 segundos (duração do bip)
    if (buzzerAtivo && millis() - tempoBuzzer >= 5000) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerAtivo = false;
    }

    vTaskDelay(pdMS_TO_TICKS(100)); // lê o sensor a cada 100ms
  }
}


// Task: BLE
// Roda no Core 0
void taskBLE(void * parameter) {
  // Aguarda o BLE inicializar completamente
  delay(1000);

  for (;;) {
    // Monta o texto que será enviado ao serial monitor
    Serial.println("Nível do Rio: " + String(distanciaSimulada) + " cm");

    // Monta o texto que será enviado ao celular
    String valor = "Nível do Rio: " + String(distanciaSimulada) + " cm";

    // Atualiza o valor BLE
    pCaracteristica->setValue(valor.c_str());

    // Envia para o celular conectado
    pCaracteristica->notify();

    vTaskDelay(pdMS_TO_TICKS(intervaloBLE));
  }
}


// Callback automático quando um celular
// desconecta do BLE
class ServerCallbacks: public NimBLEServerCallbacks {
  void onDisconnect(NimBLEServer* pServer) {
    // Aguarda estabilizar antes de reanunciar
    delay(500);

    // Usa o servidor diretamente para reiniciar o anúncio
    pServer->startAdvertising();
  }
};

// Rota Wi-Fi: envia dados ao site
void enviarDadosSensor() {
  // Monta a String no formato padrão JSON: {"distancia": XX}
  String json = "{\"distancia\": " + String(distanciaSimulada) + "}";

  // Evita que o navegador bloqueie a requisição por segurança
  server.sendHeader("Access-Control-Allow-Origin", "*");

  // Envia a resposta final para o JS com o status 200 (de sucesso)
  server.send(200, "application/json", json);
}


void setup() {
  Serial.begin(115200);

  // Inicializa o gerador de números aleatórios
  randomSeed(analogRead(0));

  // Configura os pinos dos componentes
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  // Configura o ESP32 no modo Access Point
  WiFi.softAP(ssid_ap, password_ap);

  // Mostra no monitor serial o IP (Por padrão é 192.168.4.1)
  Serial.print("Servidor Wi-Fi ativo! IP do ESP32: ");
  Serial.println(WiFi.softAPIP());

  // Inicializa o BLE com o nome que aparece no celular
  NimBLEDevice::init("Monitor de Enchentes");

  // Cria o servidor BLE
  NimBLEServer* pServer = NimBLEDevice::createServer();

  // Registra o callback de desconexão no servidor
  pServer->setCallbacks(new ServerCallbacks());

  // Cria o serviço BLE com um UUID único
  NimBLEService* pServico = pServer->createService("12345678-1234-1234-1234-123456789abc");

  // Cria a característica que vai carregar os dados
  // NOTIFY = o celular recebe automaticamente
  pCaracteristica = pServico->createCharacteristic(
    "abcdefab-1234-1234-1234-abcdefabcdef",
    NIMBLE_PROPERTY::NOTIFY
  );

  // Inicia o serviço
  pServico->start();

  // Espera o serviço se estabilizar
  delay(100);

  // Anuncia o dispositivo para celulares próximos
  NimBLEDevice::getAdvertising()->start();
  Serial.println("BLE Ativo");

  // Quando o JavaScript do site disparar um pedido para 'http://192.168.4.1/dados',
  // o ESP32 vai interceptar e rodar a função 'enviarDadosSensor'
  server.on("/dados", enviarDadosSensor);

  // Liga o servidor
  server.begin();


/*
  // Inicia as tasks nos dois cores
  xTaskCreatePinnedToCore(taskSensor, "Sensor", 10000, NULL, 1, NULL, 1); // core 1
  xTaskCreatePinnedToCore(taskBLE,    "BLE",    20000, NULL, 1, NULL, 0); // core 0
}
*/

 xTaskCreatePinnedToCore(taskBLE, "BLE", 20000, NULL, 1, NULL, 0);
}

// Mantém o servidor Wi-Fi ativo
void loop() {
  // Mantém o servidor ativo recebendo os pedidos da rede Wi-Fi
  server.handleClient(); // core 1 cuida somente do Wi-Fi
}