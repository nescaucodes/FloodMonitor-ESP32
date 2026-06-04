#include <WiFi.h>
#include <WebServer.h>

const char* ssid_ap     = "RX580 eu te amo";
const char* password_ap = "12345678";

// Porta padrão HTTP (80)
WebServer server(80);

// Upa os dados do server
void enviarDadosSensor() {
  // o chip gera um número aleatório
  // simulando a leitura do sensor ultrassônico (ex: distância da água entre 10 e 50 cm)
  int distanciaSimulada = random(10, 67); 
  
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

  // Quando o JavaScript do site disparar um pedido para 'http://192.168.4.1/dados',
  // o ESP32 vai interceptar e rodar a função 'enviarDadosSensor'
  server.on("/dados", enviarDadosSensor);

  // Liga o servidor
  server.begin();
  Serial.println("Pronto para receber requisicoes do site!");
}

void loop() {
  // Mantém o servidor ativo recebendo os pedidos da rede Wi-Fi
  server.handleClient();
}