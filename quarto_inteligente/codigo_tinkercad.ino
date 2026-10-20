// ================================================================
//  Quarto Inteligente - Jose Lucas de Lima Moura e Marisol Sales
//  Janela (servo) e ventilador (motor CC via L293D) controlados
//  por temperatura e chuva. Modo automatico e manual + LCD 16x2.
//  Simulado no Tinkercad (Arduino Uno).
// ================================================================
// Bibliotecas: servo da janela e LCD 16x2 (I2C) do Tinkercad
#include <Servo.h>
#include <Adafruit_LiquidCrystal.h>

// ---------------- Pinos (iguais ao circuito) ----------------
const int PIN_TEMP   = A0;   // TMP36 (Vout)
const int PIN_CHUVA  = A1;   // Potenciometro = sensor de chuva
// LCD I2C: SDA -> A4 | SCL -> A5  (obrigatorio no Arduino Uno)

const int BTN_TELA   = 2;    // Botao troca a tela (interrupcao externa INT0)
const int PIN_IN1    = 3;    // L293D 1A (PWM: controla a velocidade)
const int PIN_IN2    = 4;    // L293D 2A
const int PIN_EN     = 9;    // L293D 1-2EN (liga/desliga a ponte)
const int PIN_SERVO  = 10;   // Servo da janela (SIG)
const int BTN_MODO   = 11;   // Botao troca o modo (AUTO / MANUAL)
const int BTN_VENT   = 12;   // Botao ventilador (modo manual)
const int BTN_JANELA = 13;   // Botao janela (modo manual)

// ---------------- Ajustes ----------------
const float TEMP_QUENTE  = 28.0;  // graus C a partir do qual esta "quente"
const int   LIMIAR_CHUVA = 500;   // leitura do potenciometro abaixo disso = chovendo
const int   ANG_FECHADA  = 0;     // angulo do servo com a janela fechada
const int   ANG_ABERTA   = 90;    // angulo do servo com a janela aberta
const int   VEL_VENT     = 200;   // velocidade do ventilador (0 a 255)
const int   LEITURA_MAX  = 1023;  // maior valor do analogRead (ADC de 10 bits)
const int   PWM_MAX      = 255;   // maior valor do analogWrite

// ---------------- Tempos (millis no lugar de delay) ----------------
const unsigned long TEMPO_ABERTURA_MS = 1500;  // tela "Quarto Inteligente" no inicio
const unsigned long INTERVALO_MS      = 100;   // de quanto em quanto tempo o sistema atualiza
const unsigned long INTERVALO_SERIAL_MS = 5000;  // de quanto em quanto tempo envia o status
const unsigned long DEBOUNCE_MS         = 200;   // ignora repiques do botao dentro desse tempo

// ---------------- Telas do LCD ----------------
const int TELA_RESUMO   = 0;   // temperatura, chuva, modo, janela e ventilador
const int TELA_DETALHES = 1;   // leitura crua da chuva e PWM do ventilador
const int NUM_TELAS     = 2;

Servo janela;
Adafruit_LiquidCrystal lcd(0);

bool modoAuto     = true;
bool janelaAberta = false;
bool ventLigado   = false;

int  telaAtual = TELA_RESUMO;        // tela mostrada no LCD
bool mostrandoAbertura = true;       // true enquanto a tela de abertura esta no LCD
unsigned long ultimaAtualizacao = 0; // instante (millis) da ultima atualizacao
unsigned long ultimoEnvioSerial = 0; // instante (millis) do ultimo status enviado
// Flag levantada pela interrupcao do botao da tela (volatile: muda dentro da ISR)
volatile bool botaoTelaApertado = false;


const int botoes[3] = {BTN_MODO, BTN_JANELA, BTN_VENT};
int estadoAnt[3] = {HIGH, HIGH, HIGH};

// Retorna true so no instante em que o botao e apertado
bool apertou(int i) {
  int atual = digitalRead(botoes[i]);
  bool borda = (estadoAnt[i] == HIGH && atual == LOW);
  estadoAnt[i] = atual;
  return borda;
}

// Interrupcao do botao da tela (pino 2): roda sozinha quando o botao e apertado.
// Fica curta de proposito: so levanta a flag, quem age e o loop.
void isrBotaoTela() {
  static unsigned long ultimoAperto = 0;
  unsigned long agora = millis();
  if (agora - ultimoAperto > DEBOUNCE_MS) {
    botaoTelaApertado = true;
    ultimoAperto = agora;
  }
}

float lerTemperatura() {
  float tensao = analogRead(PIN_TEMP) * 5.0 / 1023.0;
  return (tensao - 0.5) * 100.0;           // formula do TMP36
}

void controlarVentilador(bool ligar) {
  digitalWrite(PIN_IN2, LOW);
  if (ligar) {
    digitalWrite(PIN_EN, HIGH);
    analogWrite(PIN_IN1, VEL_VENT);
  } else {
    digitalWrite(PIN_EN, LOW);
    analogWrite(PIN_IN1, 0);
  }
}

// Tela 1 - resumo: temperatura, chuva, modo e estado dos atuadores
void mostrarTelaResumo(float temp, bool chovendo) {
  // Linha 1: T:30.2C  Chuva
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C ");
  lcd.print(chovendo ? " Chuva" : " Seco ");
  lcd.print("  ");

  // Linha 2: AUTO J:ABR V:ON
  lcd.setCursor(0, 1);
  lcd.print(modoAuto ? "AUTO " : "MAN  ");
  lcd.print("J:");
  lcd.print(janelaAberta ? "ABR " : "FEC ");
  lcd.print("V:");
  lcd.print(ventLigado ? "ON " : "OFF");
}

// Tela 2 - detalhes: intensidade da chuva e potencia do ventilador, em %
void mostrarTelaDetalhes(int chuvaV) {
  // Leitura menor = mais molhado, por isso o map e invertido (0 -> 100%)
  int chuvaPct = map(chuvaV, 0, LEITURA_MAX, 100, 0);
  int ventPct  = map(ventLigado ? VEL_VENT : 0, 0, PWM_MAX, 0, 100);

  // Linha 1: Chuva: 52%
  lcd.setCursor(0, 0);
  lcd.print("Chuva: ");
  lcd.print(chuvaPct);
  lcd.print("%    ");                 // apaga sobras quando o numero diminui

  // Linha 2: Vent: 78%
  lcd.setCursor(0, 1);
  lcd.print("Vent: ");
  lcd.print(ventPct);
  lcd.print("%    ");
}

// Escolhe qual tela mostrar no LCD
void atualizarDisplay(float temp, int chuvaV, bool chovendo) {
  // Tela de abertura: fica no LCD ate dar TEMPO_ABERTURA_MS, sem travar o sistema
  if (mostrandoAbertura) {
    if (millis() < TEMPO_ABERTURA_MS) return;
    lcd.clear();
    mostrandoAbertura = false;
  }

  if (telaAtual == TELA_RESUMO) {
    mostrarTelaResumo(temp, chovendo);
  } else {
    mostrarTelaDetalhes(chuvaV);
  }
}

// Le os comandos que chegam pela Serial (uma letra por comando)
// J e V passam para o modo manual; senao o automatico desfaria o comando
void lerComandosSerial() {
  while (Serial.available() > 0) {
    char comando = Serial.read();

    switch (comando) {
      case 'A': case 'a':
        modoAuto = true;
        Serial.println("> Modo AUTOMATICO");
        break;
      case 'M': case 'm':
        modoAuto = false;
        Serial.println("> Modo MANUAL");
        break;
      case 'J': case 'j':
        modoAuto = false;
        janelaAberta = !janelaAberta;
        Serial.println(janelaAberta ? "> Janela ABERTA" : "> Janela FECHADA");
        break;
      case 'V': case 'v':
        modoAuto = false;
        ventLigado = !ventLigado;
        Serial.println(ventLigado ? "> Ventilador LIGADO" : "> Ventilador DESLIGADO");
        break;
      case '\n': case '\r':           // fim de linha do monitor serial: ignora
        break;
      default:
        Serial.println("> Comando invalido. Use A, M, J ou V");
        break;
    }
  }
}

// Envia pela Serial as leituras e o estado do sistema
void enviarStatus(float temp, int chuvaV) {
  Serial.print("T=");        Serial.print(temp);
  Serial.print(" Chuva=");   Serial.print(chuvaV);
  Serial.print(" Modo=");    Serial.print(modoAuto ? "AUTO" : "MANUAL");
  Serial.print(" Janela=");  Serial.print(janelaAberta ? "ABERTA" : "FECHADA");
  Serial.print(" Vent=");    Serial.println(ventLigado ? "ON" : "OFF");
}

void setup() {
  for (int i = 0; i < 3; i++) pinMode(botoes[i], INPUT_PULLUP);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  controlarVentilador(false);
  
  pinMode(BTN_TELA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_TELA), isrBotaoTela, FALLING);

  janela.attach(PIN_SERVO);
  janela.write(ANG_FECHADA);

  lcd.begin(16, 2);
  lcd.setBacklight(1);
  lcd.print("Quarto");
  lcd.setCursor(0, 1);
  lcd.print("Inteligente");

  Serial.begin(9600);
  
  Serial.println("Comandos: A=auto  M=manual  J=janela  V=ventilador");
}

void loop() {
  
  // So atualiza a cada INTERVALO_MS; no resto do tempo o loop segue livre
  unsigned long agora = millis();
  if (agora - ultimaAtualizacao < INTERVALO_MS) return;
  ultimaAtualizacao = agora;
  
  // -------- Leituras --------
  float temp     = lerTemperatura();
  int   chuvaV   = analogRead(PIN_CHUVA);
  bool  chovendo = chuvaV < LIMIAR_CHUVA;
  bool  quente   = temp >= TEMP_QUENTE;

  // -------- Botoes --------
  if (apertou(0)) modoAuto = !modoAuto;
  bool btnJanela = apertou(1);
  bool btnVent   = apertou(2);
  
  // Botao da tela: a interrupcao avisou que foi apertado -> proxima tela
  if (botaoTelaApertado) {
    botaoTelaApertado = false;
    telaAtual = (telaAtual + 1) % NUM_TELAS;
    lcd.clear();                       // limpa o texto da tela anterior
  }

  // -------- Comandos da Serial --------
  lerComandosSerial();
  
  // -------- Logica --------
  if (modoAuto) {
    if (quente && !chovendo) {         // quente e sem chuva: abre a janela
      janelaAberta = true;
      ventLigado   = false;
    } else if (quente && chovendo) {   // quente e chovendo: fecha e liga o ventilador
      janelaAberta = false;
      ventLigado   = true;
    } else {                           // temperatura agradavel: tudo desligado
      janelaAberta = false;
      ventLigado   = false;
    }
  } else {                             // modo manual: os botoes mandam
    if (btnJanela) janelaAberta = !janelaAberta;
    if (btnVent)   ventLigado   = !ventLigado;
  }

  janela.write(janelaAberta ? ANG_ABERTA : ANG_FECHADA);
  controlarVentilador(ventLigado);
 
  // -------- Display --------
  atualizarDisplay(temp, chuvaV, chovendo);
  
  // -------- Serial (status a cada INTERVALO_SERIAL_MS) --------
  if (agora - ultimoEnvioSerial >= INTERVALO_SERIAL_MS) {
    ultimoEnvioSerial = agora;
    enviarStatus(temp, chuvaV);
  }
}
