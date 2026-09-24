```cpp
#include <Bounce2.h>


// ============================================================
// Pinos dos botões
// ============================================================

const int pinoMeioPasso = 7;
const int pinoPassoCompleto = 6;
const int pinoSentido = 5;


// ============================================================
// Instâncias do Bounce2 para os botões
// ============================================================

// Cada botão possui um objeto Bounce2 para realizar o
// tratamento do efeito "bounce" (ruído mecânico do botão).

Bounce debouncerMeioPasso = Bounce();
Bounce debouncerPassoCompleto = Bounce();
Bounce debouncerSentido = Bounce();


// ============================================================
// Pinos de controle do motor de passo
// ============================================================

int IN1 = 8;
int IN2 = 9;
int IN3 = 10;
int IN4 = 11;


// Define o sentido inicial do motor
// true  = horário
// false = anti-horário

int sentido = true;


// ============================================================
// Sequência de Passo Completo
// ============================================================

// No passo completo, duas bobinas ficam acionadas
// simultaneamente, proporcionando maior torque.
//
// Cada linha representa um passo do motor.
// As colunas representam IN1, IN2, IN3 e IN4.

const int passoCompleto[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};


// ============================================================
// Sequência de Meio Passo
// ============================================================

// No meio passo, a sequência alterna entre uma e duas
// bobinas acionadas.
//
// Isso aumenta a resolução do movimento e deixa o motor
// capaz de realizar o dobro de passos por volta.

const int meioPasso[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};


// ============================================================
// Configurações do motor 28BYJ-48
// ============================================================

// O motor possui uma redução interna de aproximadamente 64:1.
//
// 2048 passos -> aproximadamente 1 volta em passo completo
// 4096 passos -> aproximadamente 1 volta em meio passo

const int PASSOS_POR_VOLTA_FULL = 2048;
const int PASSOS_POR_VOLTA_HALF = 4096;


// ============================================================
// Função de configuração
// ============================================================

void setup() {

  // Configura os pinos das bobinas como saída
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Garante que o motor comece desligado
  desligarMotor();

  delay(1000);


  // ----------------------------------------------------------
  // Configuração dos botões com Bounce2
  // ----------------------------------------------------------

  // INPUT_PULLUP utiliza o resistor de pull-up interno
  // do Arduino. Dessa forma:
  //
  // Botão solto  -> HIGH
  // Botão pressionado -> LOW

  debouncerMeioPasso.attach(pinoMeioPasso, INPUT_PULLUP);
  debouncerMeioPasso.interval(25);

  debouncerPassoCompleto.attach(pinoPassoCompleto, INPUT_PULLUP);
  debouncerPassoCompleto.interval(25);

  debouncerSentido.attach(pinoSentido, INPUT_PULLUP);
  debouncerSentido.interval(25);
}


// ============================================================
// Loop principal
// ============================================================

void loop() {

  // Atualiza a leitura dos três botões.
  // Essa função deve ser chamada continuamente para que
  // o Bounce2 consiga identificar corretamente os acionamentos.

  debouncerMeioPasso.update();
  debouncerPassoCompleto.update();
  debouncerSentido.update();


  // ----------------------------------------------------------
  // Botão de Meio Passo
  // ----------------------------------------------------------

  // fell() detecta a transição de HIGH para LOW,
  // ou seja, o momento em que o botão é pressionado.

  if (debouncerMeioPasso.fell()) {

    // Realiza uma volta completa utilizando a sequência
    // de meio passo.
    //
    // 2048 passos são utilizados aqui conforme o código original.
    // O terceiro parâmetro define o atraso entre os passos.

    moverMeioPasso(PASSOS_POR_VOLTA_FULL, sentido, 3);

    delay(2000);
  }


  // ----------------------------------------------------------
  // Botão de Passo Completo
  // ----------------------------------------------------------

  if (debouncerPassoCompleto.fell()) {

    // Realiza uma volta utilizando a sequência
    // de passo completo.

    moverPassoCompleto(PASSOS_POR_VOLTA_FULL, sentido, 3);

    delay(2000);
  }


  // ----------------------------------------------------------
  // Botão de Sentido
  // ----------------------------------------------------------

  if (debouncerSentido.fell()) {

    // Inverte o sentido de rotação do motor.

    inverteSentido();
  }
}


// ============================================================
// Função para inverter o sentido do motor
// ============================================================

void inverteSentido() {

  // Inverte o valor lógico do sentido:
  //
  // true  -> false
  // false -> true

  sentido = !sentido;
}


// ============================================================
// Movimento em Passo Completo
// ============================================================

void moverPassoCompleto(int passos, bool horario, int atrasoMs) {

  // Executa a quantidade de passos informada.

  for (int i = 0; i < passos; i++) {

    // Se o sentido for horário:
    //   0 -> 1 -> 2 -> 3
    //
    // Se o sentido for anti-horário:
    //   3 -> 2 -> 1 -> 0

    int passo = horario ? (i % 4) : (3 - (i % 4));

    // Envia a sequência correspondente para as bobinas.

    escreverPinos(passoCompleto[passo]);

    // Aguarda antes de executar o próximo passo.
    // Quanto maior o atraso, menor será a velocidade.

    delay(atrasoMs);
  }

  // Desliga as bobinas após terminar o movimento
  // para reduzir o consumo e evitar aquecimento.

  desligarMotor();
}


// ============================================================
// Movimento em Meio Passo
// ============================================================

void moverMeioPasso(int passos, bool horario, int atrasoMs) {

  // Executa a quantidade de passos informada.

  for (int i = 0; i < passos; i++) {

    // Se o sentido for horário:
    //   0 -> 1 -> 2 -> ... -> 7
    //
    // Se o sentido for anti-horário:
    //   7 -> 6 -> 5 -> ... -> 0

    int passo = horario ? (i % 8) : (7 - (i % 8));

    // Envia a sequência correspondente para as bobinas.

    escreverPinos(meioPasso[passo]);

    // Aguarda antes de executar o próximo passo.

    delay(atrasoMs);
  }

  // Desliga as bobinas ao final do movimento.

  desligarMotor();
}


// ============================================================
// Função para controlar as bobinas
// ============================================================

void escreverPinos(const int bobina[4]) {

  // Envia os valores da sequência para cada entrada
  // responsável por controlar uma das bobinas.

  digitalWrite(IN1, bobina[0]);
  digitalWrite(IN2, bobina[1]);
  digitalWrite(IN3, bobina[2]);
  digitalWrite(IN4, bobina[3]);
}


// ============================================================
// Função para desligar o motor
// ============================================================

void desligarMotor() {

  // Coloca todas as entradas em LOW,
  // desligando todas as bobinas do motor.

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
