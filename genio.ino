// C++ code
//

// Sequencia de Leds/Botoes:
// 1 - Vermelho;
// 2 - Verde;
// 3 - Azul;
// 4 - Amarelo.

int sequencia[13];

void setup()
{
  iniciaJogo();
  limiteLED = 3;
}

void loop()
{
  
}

void iniciaJogo()
{
  Serial.begin(9600);
  
  randomSeed(analogRead(A0));
  
  for(int i=0 ;i<13; i++){
  	sequencia[i] = random(1,5);
  }
  
  for(int i=0; i<13; i++){
  	Serial.print(sequencia[i]);
    Serial.print(" ");
  }
}
