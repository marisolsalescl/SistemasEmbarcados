// ================================================================
//  Quarto Inteligente - Jose Lucas de Lima Moura e Marisol Sales
//  Janela (servo) e ventilador (motor CC via L298N) controlados
//  por temperatura e chuva. Modo automatico e manual + LCD 16x2.
//  Versao para a placa real (Arduino Uno).
// ================================================================
// Bibliotecas: servo da janela, LCD 16x2 com modulo I2C (PCF8574) e sensor DHT
// Instale pelo Gerenciador de Bibliotecas:
//   "LiquidCrystal I2C" (Frank de Brabander)
//   "DHT sensor library" (Adafruit) - aceite instalar junto a "Adafruit Unified Sensor"
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ---------------- Pinos (iguais ao circuito) ----------------
const int PIN_DHT    = 8;    // DHT (pino de dados)
const int PIN_CHUVA  = A1;   // Potenciometro = sensor de chuva
// LCD I2C: SDA -> A4 | SCL -> A5  (obrigatorio no Arduino Uno)

const int BTN_TELA   = 2;    // Botao troca a tela (interrupcao externa INT0)
const int PIN_IN1    = 3;    // L298N IN1 (PWM: controla a velocidade)
const int PIN_IN2    = 4;    // L298N IN2
const int PIN_LED    = 5;    // LED indicador de temperatura (PWM: controla o brilho)
const int PIN_EN     = 9;    // L298N ENA (se o jumper do ENA estiver no lugar, nao precisa ligar)
const int PIN_SERVO  = 10;   // Servo da janela (SIG)
const int BTN_MODO   = 11;   // Botao troca o modo (AUTO / MANUAL)
const int BTN_VENT   = 12;   // Botao ventilador (modo manual)
const int BTN_JANELA = 13;   // Botao janela (modo manual)

// ---------------- Ajustes ----------------
const float TEMP_QUENTE  = 22.0;  // graus C a partir do qual esta "quente"
const int   LIMIAR_CHUVA = 500;   // leitura do potenciometro abaixo disso = chovendo
const float HISTERESE_TEMP  = 1.0; // so volta a "agradavel" abaixo de TEMP_QUENTE - 1.0
const int   HISTERESE_CHUVA = 30;  // so volta a "seco" acima de LIMIAR_CHUVA + 30
const int   ANG_FECHADA  = 0;     // angulo do servo com a janela fechada
const int   ANG_ABERTA   = 90;    // angulo do servo com a janela aberta
const int   VEL_VENT     = 255;   // velocidade do ventilador (0 a 255). O L298N derruba ~2V,
                                  // entao com 5V no driver o motor precisa do maximo para girar
const int   LEITURA_MAX  = 1023;  // maior valor do analogRead (ADC de 10 bits)
const int   PWM_MAX      = 255;   // maior valor do analogWrite
const float TEMP_LED_MIN = 22.0;  // a partir daqui o LED comeca a acender
const float TEMP_LED_MAX = 24.0;  // a partir daqui o LED fica no brilho maximo

// ---------------- Sensor DHT ----------------
#define DHT_TIPO DHT11               // DHT11 (azul) ou DHT22 (branco)

// ---------------- LCD ----------------
const byte LCD_ENDERECO = 0x27;      // se o LCD nao mostrar nada, tente 0x3F

// ---------------- Tempos (millis no lugar de delay) ----------------
const unsigned long TEMPO_ABERTURA_MS = 3000;  // tela "Quarto Inteligente" no inicio
const unsigned long INTERVALO_MS      = 100;   // de quanto em quanto tempo o sistema atualiza
const unsigned long INTERVALO_SERIAL_MS = 5000;  // de quanto em quanto tempo envia o status
const unsigned long DEBOUNCE_MS         = 200;   // ignora repiques do botao dentro desse tempo
const unsigned long INTERVALO_DHT_MS    = 2000;  // o DHT so faz uma leitura nova a cada ~2 s

// ---------------- Telas do LCD ----------------
const int TELA_RESUMO   = 0;   // temperatura, chuva, modo, janela e ventilador
const int TELA_DETALHES = 1;   // leitura crua da chuva e PWM do ventilador
const int NUM_TELAS     = 2;

Servo janela;
LiquidCrystal_I2C lcd(LCD_ENDERECO, 16, 2);
DHT dht(PIN_DHT, DHT_TIPO);

bool modoAuto     = true;
bool janelaAberta = false;
bool ventLigado   = false;
bool quente       = false;   // guardam o estado anterior para a histerese
bool chovendo     = false;

int  telaAtual = TELA_RESUMO;        // tela mostrada no LCD
bool mostrandoAbertura = true;       // true enquanto a tela de abertura esta no LCD
unsigned long ultimaAtualizacao = 0; // instante (millis) da ultima atualizacao
unsigned long ultimoEnvioSerial = 0; // instante (millis) do ultimo status enviado
unsigned long ultimaLeituraDHT  = 0; // instante (millis) da ultima leitura do DHT
bool  jaLeuDHT   = false;            // false ate a primeira leitura do DHT
float tempAtual  = NAN;              // ultima temperatura lida do DHT
// Flag levantada pela interrupcao do botao da tela (volatile: muda dentro da ISR)
volatile bool botaoTelaApertado = false;


const int botoes[3] = {BTN_MODO, BTN_JANELA, BTN_VENT};
int estadoAnt[3] = {HIGH, HIGH, HIGH};

// ================ LEITURA DAS ENTRADAS ===============
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

// Le a temperatura do DHT em graus C. O sensor so aceita uma leitura a cada
// INTERVALO_DHT_MS; no meio-tempo devolve o ultimo valor lido.
// Retorna NAN se a leitura falhar (sensor desligado ou mal conectado).
float lerTemperatura() {
  if (!jaLeuDHT || millis() - ultimaLeituraDHT >= INTERVALO_DHT_MS) {
    ultimaLeituraDHT = millis();
    jaLeuDHT = true;
    tempAtual = dht.readTemperature();
  }
  return tempAtual;
}

// ================ ACIONAMENTOS DE SAIDAS ===============
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

// LED indicador: quanto mais quente, mais forte o brilho (saida PWM)
void atualizarLedTemperatura(float temp) {
  if (isnan(temp)) temp = TEMP_LED_MIN;    // sensor com problema: LED apagado
  float t = constrain(temp, TEMP_LED_MIN, TEMP_LED_MAX);
  int brilho = (t - TEMP_LED_MIN) * PWM_MAX / (TEMP_LED_MAX - TEMP_LED_MIN);
  analogWrite(PIN_LED, brilho);
}

// ================ DISPLAY LCD ===============
// Tela 1 - resumo: temperatura, chuva, modo e estado dos atuadores
void mostrarTelaResumo(float temp, bool chovendo) {
  // Linha 1: T:30.2C  Chuva
  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (isnan(temp)) {
    lcd.print("ERRO  ");                 // DHT nao respondeu
  } else {
    lcd.print(temp, 1);
    lcd.print("C ");
  }
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

// ================ COMUNICAÇÃO SERIAL ===============
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
  pinMode(PIN_LED, OUTPUT);
  controlarVentilador(false);

  pinMode(BTN_TELA, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_TELA), isrBotaoTela, FALLING);

  dht.begin();

  janela.attach(PIN_SERVO);
  janela.write(ANG_FECHADA);

  lcd.init();
  lcd.backlight();
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
  float temp   = lerTemperatura();
  int   chuvaV = analogRead(PIN_CHUVA);

  // Histerese: so muda de estado quando passa do limite com folga,
  // senao o ruido da leitura faz a janela abrir e fechar sem parar
  if (isnan(temp))                              quente = false;
  else if (temp >= TEMP_QUENTE)                 quente = true;
  else if (temp < TEMP_QUENTE - HISTERESE_TEMP) quente = false;

  if (chuvaV < LIMIAR_CHUVA)                        chovendo = true;
  else if (chuvaV > LIMIAR_CHUVA + HISTERESE_CHUVA) chovendo = false;

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
  atualizarLedTemperatura(temp);

  // -------- Display --------
  atualizarDisplay(temp, chuvaV, chovendo);

  // -------- Serial (status a cada INTERVALO_SERIAL_MS) --------
  if (agora - ultimoEnvioSerial >= INTERVALO_SERIAL_MS) {
    ultimoEnvioSerial = agora;
    enviarStatus(temp, chuvaV);
  }
}