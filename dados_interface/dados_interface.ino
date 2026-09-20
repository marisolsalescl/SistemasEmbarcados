#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <math.h>
#include <Bounce2.h> // Adicionada biblioteca para os botões

// ====================
// DHT11
// ====================
#define DHTPIN 6
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// ====================
// NTC
// ====================
#define PIN_NTC A1

const float R_FIXO = 10000.0;    // Resistor fixo: 10kΩ
const float R0_NTC = 10000.0;    // NTC: 10kΩ
const float T0_KELVIN = 298.15;  // 25°C em Kelvin
const float BETA = 3950.0;       // Beta do NTC

float temperaturaNTC = 0;

// ====================
// LDR
// ====================
const int pinoLDR = A0;
int valorLDR;
int luminosidadeLDR;

// ====================
// LEITURAS DHT
// ====================
int temperaturaDHT = 0;
int umidadeDHT = 0;

// ====================
// BOTÕES
// ====================
const int buttonTela = 3;   // Botão para trocar de tela
const int buttonPause = 2;  // Botão de interrupção (Pause) - Pino 2 suporta interrupção no Uno/Nano

// Instâncias do Bounce2
Bounce debouncerTela = Bounce();

// ====================
// CONTROLE DAS TELAS
// ====================
int telaAtual = 1;
int telaAnterior = 0;

// ====================
// PAUSE E TEMPO
// ====================
volatile bool pause = false;
bool pauseAnterior = false;

unsigned long tempoAnteriorLeitura = 0;
const unsigned long INTERVALO_LEITURA = 1000; // Atualiza sensores a cada 1 segundo sem travar os botões

// Protótipos das funções
float lerTemperaturaNTC();
void desenharLayoutTela1();
void atualizarValoresTela1();
void desenharLayoutTela2();
void atualizarValoresTela2();
void interrupcaoPause();
void piscaTela();

// ====================
// SETUP
// ====================
void setup() {
  Serial.begin(9600);
  dht.begin();

  // Configuração do botão de troca de tela com Bounce2
  debouncerTela.attach(buttonTela, INPUT_PULLUP);
  debouncerTela.interval(25); // Debounce de 25ms

  // Configuração do botão de Pause (Interrupção Externa)
  pinMode(buttonPause, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPause), interrupcaoPause, FALLING);

  lcd.init();
  lcd.backlight();
}

// ====================
// LOOP
// ====================
void loop() {
  // 1. Atualiza a leitura do botão de tela continuamente
  debouncerTela.update();

  if (debouncerTela.fell()) { // Detecta quando o botão de tela foi pressionado
    if (telaAtual == 1) {
      telaAtual = 2;
    } else {
      telaAtual = 1;
    }
  }

  // 2. Executa a leitura dos sensores e atualização sem usar delay()
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnteriorLeitura >= INTERVALO_LEITURA) {
    tempoAnteriorLeitura = tempoAtual;

    // Leitura LDR
    valorLDR = analogRead(pinoLDR);
    luminosidadeLDR = map(valorLDR, 0, 679, 0, 100);
    luminosidadeLDR = constrain(luminosidadeLDR, 0, 100);

    // Leitura NTC
    temperaturaNTC = lerTemperaturaNTC();

    // Leitura DHT
    temperaturaDHT = dht.readTemperature();
    umidadeDHT = dht.readHumidity();

    // Exibição Serial
    Serial.print("NTC: "); Serial.print(temperaturaNTC);
    Serial.print(" C | LDR: "); Serial.print(luminosidadeLDR);
    Serial.print("% | DHT Temp: "); Serial.print(temperaturaDHT);
    Serial.print(" C | Umid: "); Serial.println(umidadeDHT);
  }

  // 3. Atualização da Interface LCD
  if (telaAtual != telaAnterior) {
    if (telaAtual == 1) desenharLayoutTela1();
    if (telaAtual == 2) desenharLayoutTela2();
    telaAnterior = telaAtual;
  }

  // 4. Lógica de Atualização/Pause
  if (!pause) {
    if (telaAtual == 1) {
      atualizarValoresTela1();
    } else if (telaAtual == 2) {
      atualizarValoresTela2();
    }
  } else {
    piscaTela();
  }

  // Restaura o backlight quando sai do modo Pause
  if (pause != pauseAnterior) {
    if (!pause) lcd.backlight();
    pauseAnterior = pause;
  }
}

// ==================================================
// FUNÇÕES AUXILIARES
// ==================================================
float lerTemperaturaNTC() {
  int adc = analogRead(PIN_NTC);
  if (adc == 0) adc = 1;

  float resistenciaNtc = R_FIXO * (250.0 / adc - 1.0);
  float tempKelvin = 1.0 / ((1.0 / T0_KELVIN) + (log(resistenciaNtc / R0_NTC) / BETA));

  return tempKelvin - 273.15;
}

void desenharLayoutTela1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp NTC: ");
  lcd.setCursor(0, 1);
  lcd.print("Lumi LDR: ");
}

void atualizarValoresTela1() {
  lcd.setCursor(10, 0);
  lcd.print(temperaturaNTC, 1);
  lcd.print("C  ");

  lcd.setCursor(10, 1);
  lcd.print(luminosidadeLDR);
  lcd.print("%  ");
}

void desenharLayoutTela2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp DHT: ");
  lcd.setCursor(0, 1);
  lcd.print("Umid DHT: ");
}

void atualizarValoresTela2() {
  lcd.setCursor(10, 0);
  lcd.print(temperaturaDHT);
  lcd.print("C  ");

  lcd.setCursor(10, 1);
  lcd.print(umidadeDHT);
  lcd.print("%  ");
}

// ISR (Interrupt Service Routine)
void interrupcaoPause() {
  static unsigned long ultimoTempoInterrupcao = 0;
  unsigned long tempoInterrupcao = millis();

  // Debounce básico para a interrupção (200ms)
  if (tempoInterrupcao - ultimoTempoInterrupcao > 200) {
    pause = !pause;
  }
  ultimoTempoInterrupcao = tempoInterrupcao;
}

void piscaTela() {
  static unsigned long ultimoPisca = 0;
  static bool estadoBacklight = true;

  // Pisca o LCD sem travar a execução do programa com delay()
  if (millis() - ultimoPisca >= 500) {
    ultimoPisca = millis();
    estadoBacklight = !estadoBacklight;
    if (estadoBacklight) {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
  }
}