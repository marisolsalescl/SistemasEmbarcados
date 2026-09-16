#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

int temperaturaNTC = 30;
int luminosidadeLDR = 10;
int temperaturaDHT = 40;
int luminosidadeDHT = 15;

int buttonTela = 2;
int buttonPause = 3;

int telaAtual = 1;
int estBotaoAnterior = HIGH;

void setup() {
  pinMode(buttonTela, INPUT_PULLUP);
  pinMode(buttonPause, INPUT_PULLUP);
  temperaturaNTC = 
  lcd.begin(16, 2);
  
  desenharTela1();
}

void loop() {
  int estBotaoAtual = digitalRead(buttonTela);
  
  if(estBotaoAtual == LOW && estBotaoAnterior == HIGH){
    if(telaAtual == 1){
    	telaAtual = 2;
    } 
    else {
      telaAtual = 1;
    }
    
    lcd.clear();
    if(telaAtual == 1){
    	desenharTela1();
    } 
    if(telaAtual == 2) {
      desenharTela2();
    }
  }
  
  
  delay(100); 
}

void desenharTela1() {
  lcd.setCursor(0, 0);
  lcd.print("Temp NTC: "); 
  lcd.print(temperaturaNTC);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("Lumi LDR: ");
  lcd.print(luminosidadeLDR);
}

void desenharTela2() {
  lcd.setCursor(0, 0);
  lcd.print("Temp DHT: "); 
  lcd.print(temperaturaDHT);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("Lumi DHT: ");
  lcd.print(luminosidadeDHT);
}