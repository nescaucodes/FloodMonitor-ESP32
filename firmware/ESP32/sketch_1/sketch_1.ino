#include <WiFi.h>
#include <WebServer.h>

// 🔴 DEFINA O NOME E A SENHA DA REDE QUE O ESP32 VAI CRIAR:
const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678"; // Mínimo 8 caracteres

WebServer server(80);

void paginaPrincipal() {
  server.send(200, "text/plain", "Conectado diretamente no ESP32 com sucesso!");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nIniciando Ponto de Acesso...");

  // Configura o ESP32 para Criar uma rede própria
  WiFi.softAP(ssid_ap, password_ap);

  // O IP padrão do ESP32 nesse modo quase sempre é 192.168.4.1
  Serial.print("Rede criada! IP do Servidor: ");
  Serial.println(WiFi.softAPIP()); 

  server.on("/", paginaPrincipal);
  server.begin();
}

void loop() {
  server.handleClient();
}