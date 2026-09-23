// C++ code - Controle de Motor L298N usando a biblioteca Bounce2
//

#include <Bounce2.h>

// Configurações de velocidade
int velocidade = 85;
int nivelVelocidade = 1;

// Pinos dos botões
const int pinoLigar = 7;
const int pinoSentido = 6;
const int pinoVelocidade = 5;

// Instâncias da biblioteca Bounce2 para cada botão
Bounce debouncerLigar = Bounce();
Bounce debouncerSentido = Bounce();
Bounce debouncerVelocidade = Bounce();

// Variáveis de controle do motor
int liga = 0;
int pin2 = HIGH;
int pin3 = LOW;

// Protótipos das funções
void ligaMotor();
void desligaMotor();
void alteraSentido();
void alteraVelocidade();

void setup()
{
  // Pinos de controle da ponte H (L298N)
  pinMode(9, OUTPUT); // ENA (PWM)
  pinMode(2, OUTPUT); // IN1
  pinMode(3, OUTPUT); // IN2

  // Configuração dos botões no Bounce2 (associa pino e define resistor interno)
  debouncerLigar.attach(pinoLigar, INPUT_PULLUP);
  debouncerLigar.interval(25); // Tempo de filtro em milissegundos

  debouncerSentido.attach(pinoSentido, INPUT_PULLUP);
  debouncerSentido.interval(25);

  debouncerVelocidade.attach(pinoVelocidade, INPUT_PULLUP);
  debouncerVelocidade.interval(25);
}

void loop()
{  
  // Atualiza a leitura dos botões a cada ciclo
  debouncerLigar.update();
  debouncerSentido.update();
  debouncerVelocidade.update();

  // -----------------------------
  // Botão Ligar / Desligar
  // -----------------------------
  // fell() retorna true exatamente UMA vez no instante em que o botão é pressionado (HIGH -> LOW)
  if (debouncerLigar.fell())
  {
    liga = !liga;
    if (liga == 0) {
      desligaMotor();
    }
  }

  // Controle de acionamento contínuo
  if (liga == 1)
  { 
    ligaMotor();
  }

  // -----------------------------
  // Botão Alterar Sentido
  // -----------------------------
  if (debouncerSentido.fell())
  {
    desligaMotor();
    alteraSentido();
  }

  // -----------------------------
  // Botão Alterar Velocidade
  // -----------------------------
  if (debouncerVelocidade.fell())
  {
    nivelVelocidade++; 
    alteraVelocidade();
  }
}

// =====================================================
// Funções do Motor
// =====================================================
void ligaMotor()
{
  analogWrite(9, velocidade);
  digitalWrite(2, pin2);
  digitalWrite(3, pin3);
}

void desligaMotor()
{
  analogWrite(9, 0); // Corta a alimentação PWM
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  delay(300); // Pausa para desaceleração mecânica do motor
}

void alteraSentido()
{
  pin2 = !pin2;
  pin3 = !pin3;
}

void alteraVelocidade()
{
  if (nivelVelocidade > 3) {
    nivelVelocidade = 1;
  }
  
  if (nivelVelocidade == 1) { 
    velocidade = 85;
  } else if (nivelVelocidade == 2) { 
    velocidade = 170;
  } else if (nivelVelocidade == 3) { 
    velocidade = 255;
  }
}
