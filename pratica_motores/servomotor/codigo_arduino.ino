#include <Servo.h> // Biblioteca do servomotor

Servo servo_9;       // Cria o objeto do servo
int pinoPoten = A5;  // Pino analógico do potenciômetro
int valorPoten = 0;  // Armazena a leitura do potenciômetro (0 a 1023)
int angulo = 0;      // Armazena o ângulo calculado (0° a 180°)

void setup()
{
  // Conecta o servo ao pino 9 com limites de pulso para 0° e 180°
  servo_9.attach(9, 500, 2500); 
}

void loop()
{
  valorPoten = analogRead(pinoPoten);         // Lê o valor do potenciômetro
  angulo = map(valorPoten, 0, 1023, 0, 180);  // Converte 0-1023 para 0-180°
  servo_9.write(angulo);                     // Move o servo para o ângulo
  delay(15);                                  // Pausa para estabilização
}