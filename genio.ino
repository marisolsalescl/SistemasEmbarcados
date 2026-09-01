// C++ code
//

int buttonPin0 = 1; // Mudado para o pino 6 (o pino 1 interfere na Serial)
int buttonPin1 = 2; // Botão 1
int buttonPin2 = 3; // Botão 2
int buttonPin3 = 4; // Botão 3
int buttonPin4 = 5; // Botão 4

int entradaUsuario = -1;

const int ledVermelho = 8;
const int ledVerde = 9;
const int ledAzul = 7;
const int ledAmarelo = 6;
const int ledDelay = 750;

int sequencia[13];

// Configuracao LEDs
int limiteLED;

// Protótipo da função
int configuraBotao(int b1, int b2, int b3, int b4);

void iniciaJogo();

void piscaLED();

void acendeLed(int cor);

void setup()
{
    Serial.begin(9600); // Inicializa a comunicação Serial
    randomSeed(analogRead(A0));

    // Configura botoes
    pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
    pinMode(buttonPin4, INPUT_PULLUP);

     // Inicia LEDs
    pinMode(ledVermelho, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledAzul, OUTPUT);
    pinMode(ledAmarelo, OUTPUT);

    iniciaJogo();

}

void loop()
{
    // Lê o botão pressionado no momento
    int botaoLido = configuraBotao(buttonPin1, buttonPin2, buttonPin3, buttonPin4);
  
}

void iniciaJogo()
{
  limiteLED = 4;
    
  for(int i=0 ;i<13; i++){
  	sequencia[i] = random(1,5);
  }
  
  for(int i=0; i<13; i++){
  	Serial.print(sequencia[i]);
    Serial.print(" ");
  }

   piscaLED();
}

int configuraBotao(int b1, int b2, int b3, int b4) 
{
  int clique = -1; // -1 significa nenhum botão pressionado

  if (digitalRead(b1) == LOW) {
    clique = 1;
  }
  else if (digitalRead(b2) == LOW) {
    clique = 2;
  }
  else if (digitalRead(b3) == LOW) {
    clique = 3;
  }
  else if (digitalRead(b4) == LOW) {
    clique = 4;
  }

  return clique;
}

void piscaLED()
{
  for (int i = 0; i < limiteLED; i++)
  {
    delay(ledDelay);

    acendeLed(sequencia[i]);
  }
}

void acendeLed(int cor)
{
  int led;

  switch (cor)
  {
    case 1:
      led = ledVermelho;
      break;

    case 2:
      led = ledVerde;
      break;

    case 3:
      led = ledAzul;
      break;

    case 4:
      led = ledAmarelo;
      break;
  }


  digitalWrite(led, HIGH);

  delay(ledDelay);

  digitalWrite(led, LOW);
}
bool entradaJogador(int botaoPressionado, int sequencia) {
  int botaoPressionado = configuraBotao();

  if (botaoPressionado == sequencia) {
    // acender o led
    return true; // Entrada correta
  } 
  else {
    return false; // Entrada incorreta
  }
}
