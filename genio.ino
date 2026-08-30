// C++ code
//

int buttonPin0 = 1; // Mudado para o pino 6 (o pino 1 interfere na Serial)
int buttonPin1 = 2; // Botão 1
int buttonPin2 = 3; // Botão 2
int buttonPin3 = 4; // Botão 3
int buttonPin4 = 5; // Botão 4

int entradaUsuario = -1;

// Protótipo da função
int configuraBotao(int b1, int b2, int b3, int b4);

// Sequencia de Leds/Botoes:
// 1 - Vermelho;
// 2 - Verde;
// 3 - Azul;
// 4 - Amarelo.

int sequencia[13];

void setup()
{
    Serial.begin(9600);
    iniciaJogo();
    limiteLED = 3;
    Serial.begin(9600); // Inicializa a comunicação Serial
  
    pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
    pinMode(buttonPin4, INPUT_PULLUP);
}

void loop()
{
    // Lê o botão pressionado no momento
    int botaoLido = configuraBotao(buttonPin1, buttonPin2, buttonPin3, buttonPin4);
  
}

void iniciaJogo()
{
  Serial.begin(9600);
  
  randomSeed(analogRead(A0));
  
  for(int i=0 ;i<13; i++){
  	sequencia[i] = random(1,5);
  }
  
  for(int i=0; i<13; i++){
  	Serial.print(sequencia[i]);
    Serial.print(" ");
  }
}

int configuraBotao(int b1, int b2, int b3, int b4) {
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
