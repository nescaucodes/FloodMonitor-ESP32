#include <WiFi.h>
#include <WebServer.h>
#include "BluetoothSerial.h"

// Verificação do Bluetooth
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth nao esta ativo! Ative-o nas configuracoes da placa.
#endif

const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678";

// Porta padrão HTTP (80)
WebServer server(80);
BluetoothSerial SerialBT;

// Variáveis globais
volatile int distanciaSimulada = 0; // Evita leitura bagunçada entre tarefas diferentes

// Trava para proteger o acesso a variável compartilhada
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

unsigned long tempoAnterior = 0; // Guarda o milissegundo do último aviso
const long intervaloBluetooth = 2000; // Espera de 2 segundos


void taskBluetooth(void * parameter) {
  for (;;) {
    unsigned long tempoAtual = millis(); // Pega o tempo atual

    // Se a diferença do tempo atual e o último envio for maior que 2 segundos...
    if (tempoAtual - tempoAnterior >= intervaloBluetooth) {
      tempoAnterior = tempoAtual; // Atualiza o cronômetro para a próxima contagem
      // int distanciaSimulada = random(10, 67);

      int novoValor = random(10, 67); 

      // Impede que outra task leia/escreva a variável enquanto está sendo atualizada
      portENTER_CRITICAL(&mux);
      distanciaSimulada = novoValor;

      // Libera o acesso a variável compartilhada
      portEXIT_CRITICAL(&mux);

      // Transmite para o Bluetooth
      SerialBT.print("Nível do Rio: ");
      SerialBT.print(distanciaSimulada);
      SerialBT.println(" cm");
    }
    vTaskDelay(pdMS_TO_TICKS(10)); // libera o processador por 10 ticks
  }
}

// Upa os dados do server
void enviarDadosSensor() {
  // o chip gera um número aleatório
  // distanciaSimulada = random(10, 67); 

  int valorAtual;
  
  // Lê a variável de forma segura
  portENTER_CRITICAL(&mux);
  valorAtual = distanciaSimulada;
  portEXIT_CRITICAL(&mux);

  // Monta a String no formato padrão JSON: {"distancia": XX}
  String json = "{\"distancia\": " + String(valorAtual) + "}";

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

  // Inicializa o Bluetooth 

  SerialBT.begin("Monitor de Enchentes");
  Serial.println("Bluetooth Ativo");

  // Quando o JavaScript do site disparar um pedido para 'http://192.168.4.1/dados',
  // o ESP32 vai interceptar e rodar a função 'enviarDadosSensor'
  server.on("/dados", enviarDadosSensor);

  // Usando uma task fixa no core 0 para o Bluetooth
  xTaskCreatePinnedToCore(
    taskBluetooth,
    "Bluetooth",
    10000,
    NULL,
    1,
    NULL,
    1
  );

  // Liga o servidor
  server.begin();
  Serial.println("Pronto para receber requisicoes do site!");
}

void loop() {
  // Mantém o servidor ativo recebendo os pedidos da rede Wi-Fi
  server.handleClient(); // core 1 cuida somente do WI-FI
}