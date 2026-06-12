#define TRIG_PIN 26
#define ECHO_PIN 14
#define BUZZER_PIN 25
#define LED1 18
#define LED2 22
#define LED3 23

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

  // delay(500);

  // Se estiver a menos de 12 cm
  if (distancia > 0 && distancia < 11) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(5000); // duração do bip
    digitalWrite(BUZZER_PIN, LOW);

    delay(1900); // completa os 2 segundos

  } else {
    delay(100);
  }

  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);

  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);

  digitalWrite(LED3, HIGH);
  delay(500);
  digitalWrite(LED3, LOW);
}
