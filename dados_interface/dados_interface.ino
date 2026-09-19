#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd(0);

int temperaturaNTC = 30;
int luminosidadeLDR;
int temperaturaDHT = 40;
int luminosidadeDHT = 15;

int buttonTela = 3;
int buttonPause = 2;

int telaAtual = 1;
int telaAnterior = 0;

int estBotaoAnterior = HIGH;

int pause = 0;
int pause_anterior = 0;

void setup() {
  pinMode(buttonTela, INPUT_PULLUP);
  pinMode(buttonPause, INPUT_PULLUP);
  temperaturaNTC = 
  lcd.begin(16, 2);
  
  attachInterrupt(
    digitalPinToInterrupt(buttonPause),
    interrupcaoInicio,
    FALLING
  );

}

void loop() {
  
  int LDR = analogRead(A0);
  luminosidadeLDR = map(LDR, 0, 679, 0, 100);
  
  int estBotaoAtual = digitalRead(buttonTela);

  if(estBotaoAtual == LOW && estBotaoAnterior == HIGH){
    if(telaAtual == 1){
    	telaAtual = 2;
    } 
    else {
      telaAtual = 1;
    }
    delay(50);
  }
  estBotaoAnterior = estBotaoAtual;
   
  if(telaAtual != telaAnterior){
    if(telaAtual == 1){
      desenharLayoutTela1();
    } 
    if(telaAtual == 2) {
      desenharLayoutTela2();
    }
    telaAnterior = telaAtual;
  }
  
  if (pause == 0){
   	if (telaAtual == 1){
      atualizarValoresTela1();
    } else if (telaAtual == 2){
      atualizarValoresTela2();
    }
  }
  
  delay(100);
}

void desenharLayoutTela1() {
  lcd.setCursor(0, 0);
  lcd.print("Temp NTC: "); 

  lcd.setCursor(0, 1);
  lcd.print("Lumi LDR: ");

}

void desenharLayoutTela2() {
  lcd.setCursor(0, 0);
  lcd.print("Temp DHT: "); 
  
  lcd.setCursor(0, 1);
  lcd.print("Lumi DHT: ");
}

void atualizarValoresTela1() {
  lcd.setCursor(10, 0);
  lcd.print(temperaturaNTC);
  lcd.print("C  ");

  lcd.setCursor(10, 1);
  lcd.print(luminosidadeLDR);
  lcd.print("%  ");
}

void atualizarValoresTela2() {
  lcd.setCursor(10, 0);
  lcd.print(temperaturaDHT);
  lcd.print("C  ");

  lcd.setCursor(10, 1);
  lcd.print(luminosidadeDHT);
  lcd.print("%  ");
}

void interrupcaoInicio()
{
  if (pause == 1 && pause_anterior == 0){
  	pause = false;
  } else {
    pause = true;
  }
}
