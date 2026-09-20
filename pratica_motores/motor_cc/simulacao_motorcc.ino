// C++ code
//

// -----------------------------
// Configuracoes de velocidade
// -----------------------------
// Velocidade inicial do motor (PWM 0 a 255)
int velocidade = 85;

// Nível atual da velocidade: 1 2 ou 3
int nivelVelocidade = 1;

// -----------------------------
// Pinos dos botoes
// -----------------------------
int botaoLigar = 7; 		  // Botao para ligar/desligar o motor
int botaoSentido = 6; 		// Botao para alterar o sentido de rotacao
int botaoVelocidade = 5; 	// Botao para alterar a velocidade

// -----------------------------
// Variaveis de controle
// -----------------------------
// Indica se o motor deve estar ligado (1) ou desligado (0)
int liga = 0;

// Indica se o motor esta atualmente ligado
int ligado = 0;

// -----------------------------
// Controle do sentido do motor
// -----------------------------
// Estado inicial dos pinos responsaveis pelo sentido
int pin2 = HIGH;
int pin3 = LOW;

// -----------------------------
// Prototipos das funcoes
// -----------------------------
void ligaMotor();
void desligaMotor();
void alteraSentido();
void alteraVelocidade();

void setup()
{
  // Configura os pinos do motor como saída
  pinMode(9, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  // Configura os botoes como entrada com resistor de pull-up interno
  pinMode(botaoLigar, INPUT_PULLUP);
  pinMode(botaoSentido, INPUT_PULLUP);
  pinMode(botaoVelocidade, INPUT_PULLUP);
}

void loop()
{  
  
  // -----------------------------
  // Botão de ligar/desligar
  // -----------------------------
  if (digitalRead(botaoLigar) == LOW)
  {
    // Inverte o estado do motor
    liga = !liga;
    delay(1000); 
  }
  
  // -----------------------------
  // Controle de funcionamento
  // -----------------------------
  if(liga == 1)
  { 
	ligaMotor();
  } else if (liga == 0 && ligado == 1)
  {
    desligaMotor();
    ligado = 0;
  }
  
  // -----------------------------
  // Alteracao do sentido
  // -----------------------------
  if (digitalRead(botaoSentido) == LOW)
  {
    desligaMotor();
    alteraSentido();
  }
  
  // -----------------------------
  // Alteracao da velocidade
  // -----------------------------
  if (digitalRead(botaoVelocidade) == LOW)
  {
   	nivelVelocidade += 1; 
    alteraVelocidade();
    delay(1000); 

  }
}

// =====================================================
// Funcao para ligar o motor
// =====================================================
void ligaMotor ()
{
  analogWrite(9, velocidade);
  digitalWrite(2, pin2);
  digitalWrite(3, pin3);
  ligado = 1;
}

// =====================================================
// Funcao para desligar o motor
// =====================================================
void desligaMotor ()
{
  analogWrite(9, velocidade);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  delay(5000);
}

// =====================================================
// Funcao para alterar o sentido do motor
// =====================================================
void alteraSentido ()
{
  pin2 = !pin2;
  pin3 = !pin3;
}

// =====================================================
// Funcao para alterar a velocidade
// =====================================================
void alteraVelocidade()
{
  if (nivelVelocidade == 4){
    nivelVelocidade = 1;
  }
  
  if (nivelVelocidade == 1){ 		    // velocidade 01
  	velocidade = 85;
  } else if (nivelVelocidade == 2){ // velocidade 02
    velocidade = 170;
  } else if (nivelVelocidade == 3){ // velocidade 03
    velocidade = 255;
  }
}
