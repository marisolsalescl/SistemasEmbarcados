# Sistema de Monitoramento com Arduino

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**, utilizando Arduino para aquisição de dados de sensores e apresentação das informações em um display LCD 16x2 com comunicação I2C.

## 📋 Descrição

O sistema realiza a leitura de três sensores:

- 🌡️ **Termistor NTC**: medição de temperatura;
- 💡 **LDR**: medição de luminosidade;
- 🌡️💧 **DHT11**: medição de temperatura e umidade.

Os dados são apresentados em um display LCD 16x2, dividido em duas telas.

### Tela 1

- Temperatura medida pelo NTC;
- Luminosidade medida pelo LDR.

### Tela 2

- Temperatura medida pelo DHT11;
- Umidade medida pelo DHT11.

A troca entre as telas é realizada por meio de um botão.

O sistema também possui um modo **Pause**, acionado por outro botão. O Pause utiliza uma interrupção externa e faz o backlight do LCD piscar enquanto o sistema está pausado.

## 🔧 Componentes

- Arduino Uno;
- Display LCD 16x2 com módulo I2C;
- Termistor NTC 10 kΩ;
- LDR;
- DHT11;
- 2 botões push-button;
- Resistor de 10 kΩ;
- Jumpers.

## 📐 Esquemático

O projeto possui um **esquemático elétrico** com as conexões entre o Arduino, display, botões e sensores.

### Observação sobre o esquemático

No esquemático, o **Termistor NTC e o sensor DHT11 foram substituídos por conectores** que representam os respectivos pinos de conexão dos sensores.

Essa representação foi utilizada para facilitar a elaboração do esquemático. Os conectores representam os pontos onde os sensores devem ser conectados, mantendo as respectivas conexões elétricas utilizadas no projeto.

## 🎛️ Controles

### Botão de troca de tela

O botão conectado ao **pino 3** alterna entre as duas telas do display.

### Botão Pause

O botão conectado ao **pino 2** utiliza uma **interrupção externa** para ativar ou desativar o modo Pause.

Durante o Pause:

- O backlight do LCD pisca;
- A atualização visual dos valores é interrompida;
- Ao sair do Pause, o backlight do LCD é restaurado.

## ⚙️ Funcionamento

As leituras dos sensores são realizadas a cada **1 segundo**, utilizando `millis()` em vez de `delay()`. Dessa forma, o programa continua respondendo aos botões enquanto aguarda o próximo intervalo de leitura.

O botão de troca de tela utiliza a biblioteca **Bounce2** para realizar o debounce.

O botão de Pause utiliza uma interrupção externa no pino 2.

A temperatura do NTC é calculada utilizando a equação baseada no parâmetro **Beta** do termistor.

## 📚 Bibliotecas utilizadas

```cpp
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <math.h>
#include <Bounce2.h>
