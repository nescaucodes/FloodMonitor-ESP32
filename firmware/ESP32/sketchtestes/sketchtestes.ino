#define TRIG_PIN 32
#define ECHO_PIN 33
#define BUZZER_PIN 25
#define LED1 18  // Led Verde 
#define LED2 22  // Led Amerelo
#define LED3 23  // Led Vermelho 

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

void loop() {
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
  float distancia = duracao * 0.0343 / 2;

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.println(" cm");

  // Apaga todos os LEDs antes de decidir qual acender
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);

  if (distancia >= 18) {
    // Rio em nível normal
    digitalWrite(LED1, HIGH);
    digitalWrite(BUZZER_PIN, LOW);

  } else if (distancia >= 11) {
    // Rio subindo, atenção
    digitalWrite(LED2, HIGH);
    digitalWrite(BUZZER_PIN, LOW);

  } else if (distancia > 0 && distancia < 11) {
    // Nível crítico, no limite da ponte
    digitalWrite(LED3, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(500); // bipe curto
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500);
}
