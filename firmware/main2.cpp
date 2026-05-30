// ==========================================
// CONFIGURAÇÃO DOS PINOS (Mapeamento de Hardware)
// ==========================================
const int PIN_TRIGGER = 12;
const int PIN_ECHO    = 14;

const int LED_VERDE    = 18;
const int LED_AMARELO  = 19;
const int LED_VERMELHO = 21;

const int PIN_BUZZER   = 25;

// ==========================================
// CONFIGURAÇÕES DE PWM PARA O BUZZER (Som Gradual)
// ==========================================
const int RESOLUCAO_PWM = 8; // 8 bits
int frequenciaBuzzer = 1000;  // Frequência inicial (Hz)
int direcaoSom = 1;          // 1 para subir o tom, -1 para descer

// ==========================================
// DEFINIÇÃO DOS ESTADOS DO SISTEMA
// ==========================================
enum Estados { SECO, ALERTA, CRITICO };
Estados estadoAtual = SECO;
Estados estadoAnterior = SECO;

// ==========================================
// VARIÁVEIS DE CONTROLE DE TEMPO (millis)
// ==========================================
unsigned long tempoAnteriorLeitura = 0;
const long intervaloLeitura = 200; // Lê o sensor a cada 200ms

unsigned long tempoAnteriorBuzzer = 0;
const long intervaloBuzzer = 15;  // Velocidade da transição do som gradual (sirene)

void setup() {
  // Inicializa a comunicação Serial (Taxa padrão para ESP32)
  Serial.begin(115200);
  
  // Configuração dos pinos do Sensor Ultrassônico
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  // Configuração dos pinos dos LEDs
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);

  // Configuração do canal PWM do Buzzer (Nova API do ESP32)
  ledcAttach(PIN_BUZZER, frequenciaBuzzer, RESOLUCAO_PWM);
  ledcWriteTone(PIN_BUZZER, 0); // Começa desligado
}

void loop() {
  unsigned long tempoAtual = millis();

  // 1. Leitura do Sensor em intervalo não-bloqueante
  if (tempoAtual - tempoAnteriorLeitura >= intervaloLeitura) {
    tempoAnteriorLeitura = tempoAtual;
    
    float distancia = lerSensorUltrassonico();
    atualizarEstado(distancia);
  }

  // 2. Executa as ações baseadas no estado atual
  controlarAtuadores(tempoAtual);

  // 3. Simulação do Envio de Mensagem Bluetooth
  enviarDadosBluetooth();
}

// ==========================================
// FUNÇÕES AUXILIARES
// ==========================================

float lerSensorUltrassonico() {
  // Garante o Trigger em nível baixo antes do pulso
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(2);
  
  // Envia um pulso de 10 microssegundos para disparar o sensor
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);
  
  // Mede o tempo de resposta do pino Echo
  long duracao = pulseIn(PIN_ECHO, HIGH, 30000); // Timeout adicionado para evitar travamentos
  
  // Calcula a distância em centímetros
  float distanciaCm = duracao * 0.034 / 2;
  
  // Filtro básico para leituras inválidas fora do alcance do sensor (0 ou > 200cm)
  if (distanciaCm <= 0 || distanciaCm > 200.0) {
    return 200.0; // Assume reservatório vazio se a leitura falhar
  }
  
  return distanciaCm;
}

void atualizarEstado(float distancia) {
  // CONFIGURAÇÃO BASEADA EM UM SENSOR A 200CM DO FUNDO:
  // Lembre-se: Mais perto de 200cm = Vazio | Mais perto de 0cm = Cheio
  
  if (distancia > 130.0) {
    // Distância entre 130cm e 200cm -> Nível Baixo / Seco
    estadoAtual = SECO;
  } 
  else if (distancia <= 130.0 && distancia > 40.0) {
    // Distância entre 40cm e 130cm -> Nível Médio
    estadoAtual = ALERTA;
  } 
  else if (distancia <= 40.0) {
    // Distância menor que 40cm -> Nível Alto / Crítico (Risco de transbordar)
    estadoAtual = CRITICO;
  }
}

void controlarAtuadores(unsigned long tempoAtual) {
  switch (estadoAtual) {
    
    case SECO:
      digitalWrite(LED_VERDE, HIGH);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, LOW);
      ledcWriteTone(PIN_BUZZER, 0); // Desliga o som
      break;
      
    case ALERTA:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, HIGH);
      digitalWrite(LED_VERMELHO, LOW);
      ledcWriteTone(PIN_BUZZER, 0); // Desliga o som
      break;
      
    case CRITICO:
      digitalWrite(LED_VERDE, LOW);
      digitalWrite(LED_AMARELO, LOW);
      digitalWrite(LED_VERMELHO, HIGH);
      
      // Lógica do som gradual (Sirene) usando millis()
      if (tempoAtual - tempoAnteriorBuzzer >= intervaloBuzzer) {
        tempoAnteriorBuzzer = tempoAtual;
        
        // Altera a frequência gradualmente
        frequenciaBuzzer += (50 * direcaoSom);
        
        // Define os limites da frequência (de 800Hz a 3000Hz)
        if (frequenciaBuzzer >= 3000 || frequenciaBuzzer <= 800) {
          direcaoSom *= -1; // Inverte a direção (sobe/desce)
        }
        
        ledcWriteTone(PIN_BUZZER, frequenciaBuzzer);
      }
      break;
  }
}

void enviarDadosBluetooth() {
  if (estadoAtual != estadoAnterior) {
    
    Serial.println("\n--- MUDANÇA DE ESTADO DETECTADA ---");
    
    switch (estadoAtual) {
      case SECO:
        Serial.println("[BT_SEND]: STATUS=NIVEL_BAIXO; LED=VERDE");
        break;
      case ALERTA:
        Serial.println("[BT_SEND]: STATUS=NIVEL_MEDIO; LED=AMARELO");
        break;
      case CRITICO:
        Serial.println("[BT_SEND]: STATUS=NIVEL_CRITICO_CHEIO; LED=VERMELHO; BUZZER=LIGADO");
        break;
    }
    
    Serial.println("-----------------------------------\n");
    
    estadoAnterior = estadoAtual;
  }
}