let MostrouErro = false;

async function buscarDados() {
  try {
    // Espera o fetch(vai até o endereço e busca a informação) ir até o ESP32 e trazer a resposta
    const resposta = await fetch ('/dados'); 

    // Espera converter o texto recebido JSON em JS normal
    const dados = await resposta.json();

    document.getElementById('nivel-atual').innerHTML = dados.distancia + " cm";
    MostrouErro = false;

  } catch (erro) {
    if (!MostrouErro) {
      console.log("Erro de conexão: ESP32 desconectado", erro);
      document.getElementById('nivel-atual').innerHTML = "Erro de conexão...";
      MostrouErro = true; 
    }
  }
}

// Executa a função a cada 2 segundos
setInterval(buscarDados, 2000);

buscarDados();


// Testes com gráficos
  const ctx = document.getElementById('grafico1');

  // Testes com variáveis
  let labelsX = ["Dia 1", "Dia 2", "Dia 3", "Dia 4", "Dia 5", "Dia 6", "Dia 7"];

  let valores = [10, 20, 30, 40, 50, 60, 70];

  new Chart(ctx, {
    type: 'bar',
    data: {
      labels: labelsX,
      datasets: [{
        label: `Centímetros`,
        data: valores,
        borderWidth: 1
      }]
    },
    options: {
      scales: {
        y: {
          beginAtZero: true
        }
      }
    }
  });