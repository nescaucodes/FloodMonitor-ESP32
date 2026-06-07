<img src="./iconbridge.png" width="200"/>
# Monitor de Enchentes com ESP32
  ## Flood Monitor with ESP32
  Visualize localmente em tempo real as medidas e variações do nível do rio atráves de sensores HC-SR04 por Arduino ESP32
  
## Funcionalidades:
  ### Sensor ultrassônico
  Com o uso do sensor HC-SR04 (som de alta frequência), é possível calcular a distância minima, atual e máximo rio naquele momento.
  
  ### Totem / Alerta Visual
  Com uma placa solar leds, ele indica visualmente e sonoramente o perigo para a população local, atráves de alertas luminosos via LEDs e sonoro via um buzzer.

  ### Totem / Alerta Sonoro
  Em situações críticas, um buzzer emite sonoramente um aviso para moradores e regiões próximas.

  ### Acompanhamento
  Moradores que correm risco, recebem alertas diretamente no celular pela rede WI-FI e também Bluetooth.

  ### Site 24/7
  O monitoramento também pode ser feito pelo site.
  - **Monitor Principal**: Mostra o nível do rio em tempo real.
  - **Status**: Indica se todos os equipamentos estão funcionando.
  - **Gráficos**: Faz uma média do nível daquela semana, contando com nível mínimo, máximo e atual.
