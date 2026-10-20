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

const int PIN_IN2    = 2;    // L293D 2A
const int PIN_IN1    = 3;    // L293D 1A (PWM: controla a velocidade)
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

Servo janela;
Adafruit_LiquidCrystal lcd(0);

bool modoAuto     = true;
bool janelaAberta = false;
bool ventLigado   = false;

const int botoes[3] = {BTN_MODO, BTN_JANELA, BTN_VENT};
int estadoAnt[3] = {HIGH, HIGH, HIGH};

// Retorna true so no instante em que o botao e apertado
bool apertou(int i) {
  int atual = digitalRead(botoes[i]);
  bool borda = (estadoAnt[i] == HIGH && atual == LOW);
  estadoAnt[i] = atual;
  return borda;
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

// Mostra temperatura, chuva, modo e estado dos atuadores no LCD
void atualizarDisplay(float temp, bool chovendo) {
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

void setup() {
  for (int i = 0; i < 3; i++) pinMode(botoes[i], INPUT_PULLUP);
  pinMode(PIN_EN, OUTPUT);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  controlarVentilador(false);

  janela.attach(PIN_SERVO);
  janela.write(ANG_FECHADA);

  lcd.begin(16, 2);
  lcd.setBacklight(1);
  lcd.print("Quarto");
  lcd.setCursor(0, 1);
  lcd.print("Inteligente");
  delay(1500);
  lcd.clear();

  Serial.begin(9600);
}

void loop() {
  // -------- Leituras --------
  float temp     = lerTemperatura();
  int   chuvaV   = analogRead(PIN_CHUVA);
  bool  chovendo = chuvaV < LIMIAR_CHUVA;
  bool  quente   = temp >= TEMP_QUENTE;

  // -------- Botoes --------
  if (apertou(0)) modoAuto = !modoAuto;
  bool btnJanela = apertou(1);
  bool btnVent   = apertou(2);

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
  atualizarDisplay(temp, chovendo);

  Serial.print("T=");      Serial.print(temp);
  Serial.print(" Chuva="); Serial.print(chuvaV);
  Serial.print(" Modo=");  Serial.println(modoAuto ? "AUTO" : "MANUAL");

  delay(100);
}
