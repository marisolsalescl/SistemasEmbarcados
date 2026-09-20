#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <math.h>

#define DHTPIN 6
#define DHTTYPE DHT11

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);


// ====================
// NTC
// ====================

#define PIN_NTC A1

const float R_FIXO = 10000.0;      // Resistor fixo: 10kΩ
const float R0_NTC = 10000.0;      // NTC: 10kΩ
const float T0_KELVIN = 298.15;    // 25°C em Kelvin
const float BETA = 3950.0;         // Beta do NTC

float temperaturaNTC = 30;


// ====================
// LDR
// ====================

const int pinoLDR = A0;

int valorLDR;          // leitura bruta do ADC
int luminosidadeLDR;   // porcentagem (0 a 100)


// ====================
// DHT11
// ====================

int temperaturaDHT = 40;
int umidadeDHT = 15;


// ====================
// BOTÕES
// ====================

int buttonTela = 3;
int buttonPause = 2;


// ====================
// CONTROLE DAS TELAS
// ====================

int telaAtual = 1;
int telaAnterior = 0;
int estBotaoAnterior = HIGH;


// ====================
// PAUSE
// ====================

volatile int pause = 0;
int pause_anterior = 0;


// ====================
// SETUP
// ====================

void setup() {

  Serial.begin(9600);

  dht.begin();

  pinMode(buttonTela, INPUT_PULLUP);
  pinMode(buttonPause, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  attachInterrupt(
    digitalPinToInterrupt(buttonPause),
    interrupcaoInicio,
    FALLING
  );
}


// ====================
// LOOP
// ====================

void loop() {

  // ====================
  // LEITURA DO LDR
  // ====================

  valorLDR = analogRead(pinoLDR);

  luminosidadeLDR = map(valorLDR, 0, 679, 0, 100);
  luminosidadeLDR = constrain(luminosidadeLDR, 0, 100);

  Serial.print("LDR ADC: ");
  Serial.print(valorLDR);
  Serial.print(" | Luminosidade: ");
  Serial.print(luminosidadeLDR);
  Serial.println("%");


  // ====================
  // LEITURA DO NTC
  // ====================

  temperaturaNTC = lerTemperaturaNTC();

  Serial.print("ADC NTC: ");
  Serial.println(analogRead(PIN_NTC));

  Serial.print("Temperatura NTC: ");
  Serial.print(temperaturaNTC);
  Serial.println(" C");


  // ====================
  // LEITURA DO DHT11
  // ====================

  temperaturaDHT = dht.readTemperature();
  umidadeDHT = dht.readHumidity();


  // ====================
  // BOTÃO DA TELA
  // ====================

  int estBotaoAtual = digitalRead(buttonTela);

  if (estBotaoAtual == LOW && estBotaoAnterior == HIGH) {

    if (telaAtual == 1) {
      telaAtual = 2;
    }
    else {
      telaAtual = 1;
    }

    delay(50);
  }

  estBotaoAnterior = estBotaoAtual;


  // ====================
  // TROCA DE TELA
  // ====================

  if (telaAtual != telaAnterior) {

    if (telaAtual == 1) {
      desenharLayoutTela1();
    }

    if (telaAtual == 2) {
      desenharLayoutTela2();
    }

    telaAnterior = telaAtual;
  }


  // ====================
  // ATUALIZAÇÃO DOS VALORES
  // ====================

  if (pause == 0) {

    if (telaAtual == 1) {
      atualizarValoresTela1();
    }
    else if (telaAtual == 2) {
      atualizarValoresTela2();
    }
  }


  // ====================
  // PISCA DURANTE PAUSE
  // ====================

  if (pause == 1) {
    piscaTela();
  }

  if (pause != pause_anterior) {

    lcd.backlight();

    pause_anterior = pause;
  }

  delay(1000);
}


// ==================================================
// FUNÇÃO DO NTC
// ==================================================

float lerTemperaturaNTC() {

  int adc = analogRead(PIN_NTC);

  if (adc == 0) {
    adc = 1;
  }

  float resistenciaNtc =
    R_FIXO * (450.0 / adc - 1.0);

  float tempKelvin =
    1.0 / (
      (1.0 / T0_KELVIN) +
      (log(resistenciaNtc / R0_NTC) / BETA)
    );

  return tempKelvin - 273.15;
}


// ==================================================
// TELA 1
// ==================================================

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


// ==================================================
// TELA 2
// ==================================================

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


// ==================================================
// INTERRUPÇÃO
// ==================================================

void interrupcaoInicio() {

  pause = !pause;
}


// ==================================================
// PISCA LCD
// ==================================================

void piscaTela() {

  lcd.backlight();

  delay(500);

  lcd.noBacklight();

  delay(500);
}