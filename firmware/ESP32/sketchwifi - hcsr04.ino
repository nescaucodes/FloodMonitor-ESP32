```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <NimBLEDevice.h>

// Pinos
#define TRIG_PIN 26
#define ECHO_PIN 14
#define BUZZER_PIN 25
#define LED1 18
#define LED2 22
#define LED3 23

// Wi-Fi
const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678";

// Servidor HTTP
WebServer server(80);

// BLE
NimBLECharacteristic* pCaracteristica;

// Variável compartilhada entre tasks
volatile float distanciaReal = 0;
const long intervaloBLE = 2000;

// ─────────────────────────────────────────
// Task: Sensor + LEDs + Buzzer (Core 1)
// ─────────────────────────────────────────
void taskSensor(void * parameter) {
  unsigned long tempoBuzzer = 0;
  bool buzzerAtivo = false;

  for (;;) {
    // Dispara pulso ultrassônico
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Calcula distância
    long duracao = pulseIn(ECHO_PIN, HIGH);
    distanciaReal = duracao * 0.0343 / 2;

    Serial.print("Distancia: ");
    Serial.print(distanciaReal);
    Serial.println(" cm");

    // LEDs por nível
    digitalWrite(LED1, distanciaReal < 30 ? HIGH : LOW);
    digitalWrite(LED2, distanciaReal < 20 ? HIGH : LOW);
    digitalWrite(LED3, distanciaReal < 11 ? HIGH : LOW);

    // Buzzer não-bloqueante
    if (distanciaReal > 0 && distanciaReal < 11 && !buzzerAtivo) {
      digitalWrite(BUZZER_PIN, HIGH);
      tempoBuzzer = millis();
      buzzerAtivo = true;
    }
    if (buzzerAtivo && millis() - tempoBuzzer >= 5000) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerAtivo = false;
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

// ─────────────────────────────────────────
// Task: BLE (Core 0)
// ─────────────────────────────────────────
void taskBLE(void * parameter) {
  delay(1000);
  for (;;) {
    String valor = "Nível do Rio: " + String(distanciaReal) + " cm";
    pCaracteristica->setValue(valor.c_str());
    pCaracteristica->notify();
    vTaskDelay(pdMS_TO_TICKS(intervaloBLE));
  }
}

// ─────────────────────────────────────────
// Callback: Reconexão BLE
// ─────────────────────────────────────────
class ServerCallbacks: public NimBLEServerCallbacks {
  void onDisconnect(NimBLEServer* pServer) {
    delay(500);
    pServer->startAdvertising();
  }
};

// ─────────────────────────────────────────
// Rota Wi-Fi: Envia dados ao site
// ─────────────────────────────────────────
void enviarDadosSensor() {
  String json = "{\"distancia\": " + String(distanciaReal) + "}";
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "application/json", json);
}

// ─────────────────────────────────────────
// Setup
// ─────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  WiFi.softAP(ssid_ap, password_ap);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  NimBLEDevice::init("Monitor de Enchentes");
  NimBLEServer* pServer = NimBLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());
  NimBLEService* pServico = pServer->createService("12345678-1234-1234-1234-123456789abc");
  pCaracteristica = pServico->createCharacteristic(
    "abcdefab-1234-1234-1234-abcdefabcdef",
    NIMBLE_PROPERTY::NOTIFY
  );
  pServico->start();
  delay(100);
  NimBLEDevice::getAdvertising()->start();
  Serial.println("BLE Ativo");

  server.on("/dados", enviarDadosSensor);
  server.begin();

  xTaskCreatePinnedToCore(taskSensor, "Sensor", 10000, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(taskBLE,    "BLE",    20000, NULL, 1, NULL, 0);
}

// ─────────────────────────────────────────
// Loop: Wi-Fi
// ─────────────────────────────────────────
void loop() {
  server.handleClient();
}
```