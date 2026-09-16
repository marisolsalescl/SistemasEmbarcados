// ============================================================
// CONFIGURAÇÃO DOS BOTÕES
// ============================================================

// Botao 0 utilizado para iniciar o jogo por interrupcao 
int buttonPin0 = 2;

// Botoes 1 a 4 utilizadas para resposta do jogador
int buttonPin1 = 3;
int buttonPin2 = 4;
int buttonPin3 = 5;
int buttonPin4 = 6;

// ============================================================
// CONFIGURAÇÃO DOS LEDs
// ============================================================
const int ledVermelho = 9;
const int ledVerde = 10;
const int ledAzul = 8;
const int ledAmarelo = 7;

// Tempo que cada LED permanece aceso
const int ledDelay = 500;

// ============================================================
// CONFIGURAÇÃO DO BUZZER
// ============================================================
const int buzzer = 11;

// ============================================================
// VARIÁVEIS DE CONTROLE DO JOGO
// ============================================================

// Mostra se o jogo esta em andamento
int jogo = 0;

// ============================================================
// TEMPORIZADOR
// ============================================================

// Guarda o momento em que o temporizador foi iniciado
unsigned long inicioTemporizador;

// Tempo para o jogador responder (5s)
const unsigned long tempoLimite = 5000;

// ============================================================
// SEQUÊNCIA DO JOGO
// ============================================================

// Armazena a ordem para acender os LEDS
int sequencia[13];

// Indica a posicao da sequencia que o jogador estar
int passoAtual = 0;

// Quantidade de passos por rodada
int limiteSequencia = 4; 

// Quantidade maxima de passos do jogo
int limiteMax = 13; 

// ============================================================
// PROTÓTIPOS DAS FUNÇÕES
// ============================================================

int configuraBotao(int b1, int b2, int b3, int b4);
void iniciaJogo();
void piscaLED();
void acendeLed(int cor);
bool entradaJogador(int botaoPressionado, int sequenciaAtual);

// ============================================================
// INTERRUPCAO
// ============================================================

// Variavel alterada na interrupcao
volatile bool iniciar = false;

// ============================================================
// SETUP
// ============================================================

void setup() 
{
    // Inicia comunicacao serial
    Serial.begin(9600);
  
    // Inicia o gerador de numeros aleatorios
    randomSeed(analogRead(A0));
 
    // --------------------------------------------------------
    // CONFIGURAÇÃO DOS BOTÕES
    // --------------------------------------------------------
  	
  	// Utilizado o resistor interno do Arduino (PULLUP)
  	pinMode(buttonPin0, INPUT_PULLUP);
    pinMode(buttonPin1, INPUT_PULLUP);
    pinMode(buttonPin2, INPUT_PULLUP);
    pinMode(buttonPin3, INPUT_PULLUP);
    pinMode(buttonPin4, INPUT_PULLUP);

    // --------------------------------------------------------
    // CONFIGURAÇÃO DO BUZZER
    // --------------------------------------------------------
	pinMode(buzzer, OUTPUT);

  
    // --------------------------------------------------------
    // CONFIGURAÇÃO DOS LEDs
    // --------------------------------------------------------
    pinMode(ledVermelho, OUTPUT);
    pinMode(ledVerde, OUTPUT);
    pinMode(ledAzul, OUTPUT);
    pinMode(ledAmarelo, OUTPUT);

    // --------------------------------------------------------
    // CONFIGURAÇÃO DA INTERRUPCAO
    // --------------------------------------------------------
  	
  	// A interrupcao acontece quando mudar o estado HIGH para LOW
  	// Quando o botao 0 e pressionado
  	attachInterrupt(
      digitalPinToInterrupt(buttonPin0),
      interrupcaoInicio,
      FALLING
    );
}

// ============================================================
// LOOP PRINCIPAL
// ============================================================

void loop()
{
  	// Verifica se o botao de inicio foi pressionado
  	if(iniciar)
    {
      iniciaJogo();
      
      // Depois de inicar a variavel e zerada 
      // para evitar reinicio automatico
      iniciar = false;
    }
  	
  	// Executa o jogo
  	if(jogo)
    {
      
	  // Verifica qual botao foi pressionado
      int botaoLido = configuraBotao(
          buttonPin1,
          buttonPin2,
          buttonPin3,
          buttonPin4
      );
		
      // Quando o jogador pressiona o botao
      if (botaoLido != -1) {
        
          // Reinicia temporizador
          inicioTemporizador = millis();
          
          tocaSomBotao(botaoLido);

          // Mostra botao pressionado no monitor
          Serial.print("\nBotao pressionado: ");
          Serial.println(botaoLido);

          acendeLed(botaoLido);

          // Verifique se o botao pressionado e o correto
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
                  if (limiteSequencia < limiteMax) 
                  	piscaLED();

                  // Chegou ao limite máximo (Jogador Venceu)
                  if (limiteSequencia >= limiteMax) {
                    Serial.println("Vitoria!");
                    musicaVitoria();
                    jogo = 0;
                  }
              }

          }
        	
          // Caso o jogador erre a sequencia
          else {

              Serial.println("ERRADO! Inicie o jogo novamente.");

              musicaDerrota();
            
              delay(1000);
			
              // Encerra o jogo
              jogo = 0;
          }
        
          // Evita que o mesmo botao seja pressionado varias vezes
          delay(300); 
      }
      	
      // Verfica o tempo que o jogador tem para apertar o botao
      if(millis() - inicioTemporizador >= tempoLimite)
      {
         // Apos 5s sem interacao o jogador perde
         Serial.println("\n5 segundos passaram! Voce Perdeu!");
         musicaDerrota();
       	 jogo = 0;
      }
    }
}

// ============================================================
// INICIA O JOGO
// ============================================================

void iniciaJogo() {
  
	// Jogo em andamento
  	jogo = 1;
	
  	musicaInicio();
  	
  	// Primeiro nivel exigem 4 acertos
    limiteSequencia = 4;
  	
  	// O primeiro elemento e verificado
    passoAtual = 0;
  
  	// Gera sequencia aleatorio e mostra no serial
    Serial.print("\nSequencia inteira\n");
    for(int i = 0; i < limiteMax; i++){
      	// O preenchimento ocorre com numeros de 1 a 4
        sequencia[i] = random(1, 5);
      Serial.print(sequencia[i]);

    }
	
  	// Mostra a sequencia incial (4 pisques) para o jogador
    piscaLED();
}

// ============================================================
// IDENTIFICA QUAL BOTÃO FOI PRESSIONADO
// ============================================================

int configuraBotao(int b1, int b2, int b3, int b4) {
  	
  	// O INPUT_PULLUP faz com que o botao pressionado apresente LOW
    if (digitalRead(b1) == LOW) return 1;
    if (digitalRead(b2) == LOW) return 2;
    if (digitalRead(b3) == LOW) return 3;
    if (digitalRead(b4) == LOW) return 4;
    
  	// Caso nenhum seja pressionado
  	return -1;
}

// ============================================================
// MOSTRA A SEQUÊNCIA AO JOGADOR
// ============================================================

void piscaLED() {
    Serial.print("\nSequencia nivel:");
    
  	// Percorre a sequenci ate o limite
    for (int i = 0; i < limiteSequencia; i++) {
        delay(300);
      	
      	// Pisca o LED correspondente
        acendeLed(sequencia[i]);
      Serial.print(sequencia[i]);
    }
  	
  	// Inicia o temporizador apos fim da sequencia
  	inicioTemporizador = millis();
}

// ============================================================
// ACENDE O LED CORRESPONDENTE À COR
// ============================================================

void acendeLed(int cor) {
    int led = 0;
    switch (cor) {
        case 1: led = ledVermelho; break;
        case 2: led = ledVerde; break;
        case 3: led = ledAzul; break;
        case 4: led = ledAmarelo; break;
    }
	
  	// Quando encontrado o LED acende e apaga
    if (led != 0) {
        digitalWrite(led, HIGH);
        delay(ledDelay);
        digitalWrite(led, LOW);
    }
}

// ============================================================
// VERIFICA A RESPOSTA DO JOGADOR
// ============================================================

bool entradaJogador(int botaoPressionado, int sequenciaAtual) {
  
  // Compara o botao com o valor na sequencia
  if(botaoPressionado == sequenciaAtual){
    
    // Se acertar acende o led
    acendeLed(sequenciaAtual);
    return true;
  }else {
    
    // Caso nao acerte retorna falso
    return false;
  }
}

// ============================================================
// TOCA UMA NOTA NO BUZZER
// ============================================================

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

// ============================================================
// ROTINA DA INTERRUPÇÃO
// ============================================================

void interrupcaoInicio()
{
  
  // Inicia sempre que o botao 0 e pressionado
  iniciar = true;
}
