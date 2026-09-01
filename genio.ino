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

int sequencia[13];
int passoAtual = 0;
int limiteSequencia;

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

void loop() { 
  	if(iniciar)
    {
      iniciaJogo();
      iniciar = false;
    }
  	
    int botaoLido = configuraBotao(buttonPin1, buttonPin2, buttonPin3, buttonPin4);

    if (botaoLido != -1) {
        Serial.print("Botao pressionado: ");
        Serial.println(botaoLido);

        acendeLed(botaoLido); 

        if (entradaJogador(botaoLido, sequencia[passoAtual])) {
            Serial.println("CORRETO!");
            passoAtual++;
          if(passoAtual == 4){
            iniciaJogo();
          }
            }
         else {
            Serial.println("ERRADO! Reiniciando");
            delay(1000);
            iniciaJogo(); 
        }

        delay(300); // Debounce simples
    }
}

void iniciaJogo() {
    limiteSequencia = 4;
    passoAtual = 0;
    Serial.print("\nSequencia\n");
    for(int i = 0; i < 13; i++){
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
    for (int i = 0; i < limiteSequencia; i++) {
        delay(300);
        acendeLed(sequencia[i]);
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
    acendeLed(sequenciaAtual);
    return false;
  }
}

void interrupcaoInicio()
{
  iniciar = true;
}
