let MostrouErro = false;

let ultimaLeitura = Date.now() - 10000;

const frames = ['Conectando.', 'Conectando..', 'Conectando...'];
let frameAtual = 0;
let animacao = null;

animacao = setInterval(() => {
        document.getElementById('nivel-atual').textContent = frames[frameAtual];
        frameAtual = (frameAtual + 1) % frames.length;
      }, 400);
      

async function buscarDados() {
  try {
    // Espera o fetch(vai até o endereço e busca a informação) ir até o ESP32 e trazer a resposta
    const resposta = await fetch ('http://192.168.4.1/dados'); 

    // Espera converter o texto recebido JSON em JS normal
    const dados = await resposta.json();

    document.getElementById('nivel-atual').innerHTML = dados.distancia + " cm";

    document.getElementById('metrica-atual').innerHTML = dados.distancia + " cm";

    ultimaLeitura = Date.now();// Já inicia desconectado

    MostrouErro = false;

    clearInterval(animacao);

  } catch (erro) {
    if (!MostrouErro) {
      console.log("Erro de conexão: ESP32 desconectado", erro);
      MostrouErro = true; 
    }
  }
}


// Executa a função a cada 2 segundos
setInterval(buscarDados, 2000);

buscarDados();

// Status Atual: Ligado... / Desligado...
setInterval(function() {
  const agora = Date.now()
  const diferenca = agora - ultimaLeitura;

  if (diferenca < 5000) {
    document.getElementById('status-texto2').textContent = 'Ligado...';
    document.getElementById('status-texto2').style.color = '#097969';
  } else {
    document.getElementById('status-texto2').textContent = 'Desligado...'
    document.getElementById('status-texto2').style.color = '	#800020';
  }

}, 1000) // Repete a cada 1 segundo


// Modo para trocar de tema
function toggleModo() {
  document.body.classList.toggle('lightmode');
  const img = document.querySelector('#btn-modo img');
    img.src = document.body.classList.contains('lightmode') ? '/images/html/modes/lightmode.png' : '/images/html/modes/darkmode.png';
}



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