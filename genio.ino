int buttonPin0 = 2;
int buttonPin1 = 3;
int buttonPin2 = 4;
int buttonPin3 = 5;
int buttonPin4 = 6;

const int ledVermelho = 9;
const int ledVerde = 10;
const int ledAzul = 8;
const int ledAmarelo = 7;
const int ledDelay = 500;

const int buzzer = 11;

int jogo = 0; 

int sequencia[13];
int passoAtual = 0;
int limiteSequencia = 4; // Quantidade de passos para a rodada
int limiteMax = 13; // Quantidade de passos para a rodada

int configuraBotao(int b1, int b2, int b3, int b4);
void iniciaJogo();
void piscaLED();
void acendeLed(int cor);
bool entradaJogador(int botaoPressionado, int sequenciaAtual);

volatile bool iniciar = false;

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(A0));

  	pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
    pinMode(buttonPin4, INPUT_PULLUP);

	pinMode(buzzer, OUTPUT);

    pinMode(ledVermelho, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledAzul, OUTPUT);
    pinMode(ledAmarelo, OUTPUT);

  	attachInterrupt(
      digitalPinToInterrupt(buttonPin0),
      interrupcaoInicio,
      FALLING
    );
}

void loop()
{
  	if(iniciar)
    {
      iniciaJogo();
      iniciar = false;
    }
  
  	if(jogo)
    {

      int botaoLido = configuraBotao(
          buttonPin1,
          buttonPin2,
          buttonPin3,
          buttonPin4
      );

      if (botaoLido != -1) {
          tocaSomBotao(botaoLido);

          Serial.print("\nBotao pressionado: ");
          Serial.println(botaoLido);

          acendeLed(botaoLido);

          if (entradaJogador(botaoLido, sequencia[passoAtual])) {

              Serial.println("CORRETO!");

              passoAtual++;

              // Jogador terminou a sequência atual
              if (passoAtual == limiteSequencia) {

                  Serial.println("Sequencia completa!");

                  limiteSequencia++;
                  passoAtual = 0;

                  musicaNivel();

                  // Mostra a nova sequência
                  piscaLED();

                  // Chegou ao limite máximo
                  if (limiteSequencia > limiteMax) {
                    Serial.println("Vitoria!");
                      musicaVitoria();
                      iniciaJogo();
                  }
              }

          }
          else {

              Serial.println("ERRADO! Inicie o jogo novamente.");

              musicaDerrota();
            
              delay(1000);
			
              jogo = 0;
          }

          delay(300); // Debounce simples
      }
    }
}

void iniciaJogo() {
  	jogo = 1;
	musicaInicio();
    limiteSequencia = 4;
    passoAtual = 0;
    Serial.print("\nSequencia inteira\n");
    for(int i = 0; i < limiteMax; i++){
        sequencia[i] = random(1, 5);
      Serial.print(sequencia[i]);

    }

    piscaLED();
}

int configuraBotao(int b1, int b2, int b3, int b4) {
    if (digitalRead(b1) == LOW) return 1;
    if (digitalRead(b2) == LOW) return 2;
    if (digitalRead(b3) == LOW) return 3;
    if (digitalRead(b4) == LOW) return 4;
    return -1;
}

void piscaLED() {
    Serial.print("\nSequencia nivel:");

    for (int i = 0; i < limiteSequencia; i++) {
        delay(300);
        acendeLed(sequencia[i]);
      Serial.print(sequencia[i]);
    }
}

void acendeLed(int cor) {
    int led = 0;
    switch (cor) {
        case 1: led = ledVermelho; break;
        case 2: led = ledVerde; break;
        case 3: led = ledAzul; break;
        case 4: led = ledAmarelo; break;
    }

    if (led != 0) {
        digitalWrite(led, HIGH);
        delay(ledDelay);
        digitalWrite(led, LOW);
    }
}

bool entradaJogador(int botaoPressionado, int sequenciaAtual) {
  if(botaoPressionado == sequenciaAtual){
    acendeLed(sequenciaAtual);
    return true;
  }else {
    return false;
  }
}

void tocaNota(int frequencia, int duracao)
{
  tone(buzzer, frequencia);
  delay(duracao);
  noTone(buzzer);
  delay(30);
}

// =========================
// MÚSICA DE INÍCIO
// =========================

void musicaInicio()
{
  // Fanfarra inspirada no Super Mario

  tocaNota(659, 120);  // Mi5
  tocaNota(659, 120);  // Mi5
  tocaNota(659, 120);  // Mi5

  tocaNota(523, 120);  // Dó5
  tocaNota(659, 180);  // Mi5
  tocaNota(784, 250);  // Sol5

  tocaNota(392, 250);  // Sol4
}


// =========================
// MÚSICA DE DERROTA
// =========================

void musicaDerrota()
{
  // Sequência descendente

  tocaNota(330, 180);  // Mi4
  tocaNota(311, 180);  // Ré#4
  tocaNota(294, 180);  // Ré4
  tocaNota(262, 300);  // Dó4

  delay(100);

  tocaNota(196, 500);  // Sol3
}


// =========================
// MÚSICA DE PASSAGEM DE NÍVEL
// =========================

void musicaNivel()
{
  // Jingle curto e ascendente

  tocaNota(523, 120);  // Dó5
  tocaNota(659, 120);  // Mi5
  tocaNota(784, 120);  // Sol5

  delay(50);

  tocaNota(1047, 300); // Dó6
}


// =========================
// MÚSICA DE VITÓRIA
// =========================

void musicaVitoria()
{
  // Fanfarra final

  tocaNota(523, 120);  // Dó5
  tocaNota(523, 120);  // Dó5
  tocaNota(523, 120);  // Dó5

  tocaNota(659, 180);  // Mi5
  tocaNota(784, 180);  // Sol5
  tocaNota(1047, 350); // Dó6

  delay(80);

  tocaNota(784, 150);  // Sol5
  tocaNota(1047, 500); // Dó6
}

// =========================
// SOM DOS BOTÕES
// =========================

void tocaSomBotao(int botao)
{
  switch (botao)
  {
    case 1:
      tone(buzzer, 262);  // Dó
      break;

    case 2:
      tone(buzzer, 330);  // Mi
      break;

    case 3:
      tone(buzzer, 392);  // Sol
      break;

    case 4:
      tone(buzzer, 523);  // Dó agudo
      break;
  }

  delay(150);
  noTone(buzzer);
}

void interrupcaoInicio()
{
  iniciar = true;
}
