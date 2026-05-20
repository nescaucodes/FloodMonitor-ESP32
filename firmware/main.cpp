int trigPin = 32;
int echoPin = 33;

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(ledPin, OUTPUT);
}

void loop() {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distancia = duration * 0.034 / 2;

  Serial.print("Distancia(cm): ");
  Serial.print(distancia);
  Serial.println(" cm");

  if(distancia < 15) {
    Serial.println("Alerta");
  }

  delay(5000);

/*
  digitalWrite(ledPinR, HIGH);
  delay(5000);
  digitalWrite(ledPinR, LOW);

  digitalWrite(ledPinG, HIGH);
  delay(5000);
  digitalWrite(ledPinG, LOW);
*/  
/*
  digitalWrite(ledPin, HIGH);
  delay(5000);

  digitalWrite(ledPin, LOW);
  delay(5000);
*/ 
}

/*
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(ledPinR, redValue);
  analogWrite(ledPinG, greenValue);
  analogWrite(ledPinB, blueValue);
}
*/