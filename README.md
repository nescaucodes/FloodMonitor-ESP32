<div align="center">

  <img src="./iconbridge.png" width="150"/>
  <h1>Flood Monitor with ESP32</h1>
  <h3>Sistema de Monitoramento de Enchentes</h3>
  <p>Projeto open-source de utilidade pública para monitoramento de rios e canais em tempo real.</p>

  <img src="https://img.shields.io/badge/Platform-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white" alt="Arduino">
  <img src="https://img.shields.io/badge/Chip-ESP32-E7352C?style=for-the-badge&logo=espressif&logoColor=white" alt="ESP32">
  <img src="https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++">
  </div>

---

## Sobre o Projeto
**Flood Monitor with ESP32** é um projeto escolar focado em *Smart Cities* (Cidades Inteligentes) e internet das coisas (IoT). Consiste em um sistema automatizado de prevenção de desastres naturais através do monitoramento em tempo real do nível de rios/canais com alerta local e remoto em toda cidade.

**⚠️Nota de Compatibilidade:** É um **projeto puramente baseado na plataforma Arduino**. Todo o firmware foi desenvolvido em C++ utilizando o framework oficial da Arduino, facilitando a portabilidade e modificação do código em outros módulos ESP32.

  
## Funcionalidades:
  ### Telemetria com Sensor Ultrassônico
  Utilizando o sensor **HC-SR04** (som de alta frequência), é possível calcular a distância em centímetros e armazenar os valores de nível atual, pico máximo e mínimo do rio/canal.
  
  ### Dispositivos Locais de Alerta (Hardware)
  Dois dispositivos atuam como sinalizador de emergência na área de risco em um totém físico:
  - **🚨 Alerta Visual:** LED's indicam visualmente o nível de criticidade (Verde/Seguro, Amarelo/Atenção, Vermelho/Transbordo).
  - **🔈 Alerta Sonoro:** Ativação de um buzzer/sirene ativa quando a água chega a marca crítica (transbordo) de inundação.

  ### Servidor de Dados por WI-FI + Bluetooth
  O sistema foi desenhado para garantir que a informação chegue por todos os meios possíveis, recebendo alertas diretamente no celular pela rede WI-FI e também Bluetooth.
  - **📶 WI-FI WebServer:** O Arduino cria um *Access Point* e cria uma API em um server local. O site utiliza esses dados em formato JSON atráves de requisiçõe assíncronas.
  - **📱 Bluetooth Low Energy (BLE):** Atuando como canal de backup, transmite dados via protocolo [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).


  ### Dashboard Web
  Uma interface de usuário que roda no navegador e se conecta diretamente a WI-FI do Arduino para mostrar o monitoramento de forma intuitiva e gráfica.
  - **Monitor Principal:** Mostra o nível do rio em tempo real.
  - **Status:** Indica se todos os equipamentos estão funcionando.
  - **Gráficos:** Faz uma média do nível daquela semana, contando com nível mínimo, máximo e atual.

---
  ### Compilar a partir do código-fonte
  # Requisitos
  - Arduino IDE
  - Visual Studio Code

  ### Bibliotecas / API's
  [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino)).

  
