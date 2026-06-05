#include <WiFi.h>
#include <WebServer.h>
#include <NimBLEDevice.h>

const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678";

// Porta padrão HTTP (80)
WebServer server(80);
NimBLECharacteristic* pCaracteristica;

// Variáveis globais
volatile int distanciaSimulada = 0; // Evita leitura bagunçada entre tarefas diferentes
const long intervaloBLE = 2000; // Guarda o milissegundo do último aviso


void taskBLE(void * parameter) {
  for (;;) {
    distanciaSimulada = random(10, 67);

    // Monta o texto que será enviado ao celular
    String valor = "Nível do Rio: " + String(distanciaSimulada) + " cm";

    // atualiza o valor BLE
    pCaracteristica->setValue(valor.c_str()); 

    // envia para o celular conectado
    pCaracteristica->notify(); 
    vTaskDelay(pdMS_TO_TICKS(intervaloBLE));
  }
}

// Callback automatico quando um celular desconecta do BLE
class ServerCallbacks: public NimBLEServerCallbacks {
  void onDisconnect(NimBLEServer* pServer) {
    // reinicia o anúnciio para reconexão
    NimBLEDevice::getAdvertising()->start();
  }
};

// Upa os dados do server
void enviarDadosSensor() {
  // Monta a String no formato padrão JSON: {"distancia": XX}
  String json = "{\"distancia\": " + String(distanciaSimulada) + "}";

  // Evita que o navegador bloqueie a requisição por segurança.
  server.sendHeader("Access-Control-Allow-Origin", "*");
  
  // Envia a resposta final para o JS com o status 200 (de sucesso)
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  // Inicializa o gerador de números aleatórios
  randomSeed(analogRead(0));

  // Configura o ESP32 no modo Access Point
  WiFi.softAP(ssid_ap, password_ap);
  
  // Mostra no monitor serial o IP (Por padrão é 192.168.4.1)
  Serial.print("Servidor Wi-Fi ativo! IP do ESP32: ");
  Serial.println(WiFi.softAPIP()); 


  // Inicializa o BLE 
  NimBLEDevice::init("Monitor de Enchentes"); // nome que aparece no celular

  // Cria o servidor BLE
  NimBLEServer* pServer = NimBLEDevice::createServer();

  // Registra o callback de desconexão no servidor
  pServer->setCallbacks(new ServerCallbacks());


  // Cria o serviço BLE com um UUID único
  NimBLEService* pServico = pServer->createService("12345678-1234-1234-1234-123456789abc");

  // Cria a característica que vai carregar os dados
  pCaracteristica = pServico->createCharacteristic(
    "abcdefab-1234-1234-1234-abcdefabcdef",

    // o celular vai receber automaticamente 
    NIMBLE_PROPERTY::NOTIFY
  );

  // Inicia o serviço
  pServico->start();

  // Anuncia o dispositivo para celulares próximos
  NimBLEDevice::getAdvertising()->start(); // começa a anunciar o dispositivo
  Serial.println("BLE Ativo");

  server.on("/dados", enviarDadosSensor);
  server.begin();

  xTaskCreatePinnedToCore(taskBLE, "BLE", 10000, NULL, 1, NULL, 0);
}

/*
  --- Código Antigo ---
  // Quando o JavaScript do site disparar um pedido para 'http://192.168.4.1/dados',
  // o ESP32 vai interceptar e rodar a função 'enviarDadosSensor'
  server.on("/dados", enviarDadosSensor);

  // Liga o servidor
  server.begin();
  Serial.println("Pronto para receber requisicoes do site!");
*/

void loop() {
  // Mantém o servidor ativo recebendo os pedidos da rede Wi-Fi
  server.handleClient(); // core 1 cuida somente do WI-FI
}