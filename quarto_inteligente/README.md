# 🏠 Quarto Inteligente

Projeto desenvolvido para a disciplina de **Sistemas Embarcados e Prototipação**, sob orientação do **Prof. Dr. Emmanuel Andrade**, utilizando **Arduino Uno** para automatizar a janela e o ventilador de um quarto de acordo com a **temperatura** e a **chuva**.

A janela é movida por um **servomotor** e o ventilador é um **motor CC** acionado por uma ponte H. O sistema funciona em modo **automático** ou **manual**, mostra as informações em um **LCD 16x2 (I2C)** com duas telas e pode ser controlado e monitorado pela **porta Serial**.

O projeto tem duas versões:

| Versão | Arquivo | Onde roda |
| ------ | ------- | --------- |
| Simulação | `codigo_tinkercad.ino` | Tinkercad Circuits |
| Placa real | `Codigo_placa.ino` | Arduino Uno montado em protoboard |

A lógica, a pinagem e a interface são as mesmas nas duas. A versão da placa troca alguns componentes e bibliotecas (veja [Da simulação para a placa real](#-da-simulação-para-a-placa-real)).

---

## ⚙️ Funcionamento

### Modo automático

O Arduino lê a temperatura e o sensor de chuva e decide sozinho o que fazer:

| Situação | Janela | Ventilador |
| -------- | ------ | ---------- |
| 🌡️ Quente e sem chuva | Aberta (servo 90°) | Desligado |
| 🌡️ Quente e chovendo | Fechada (servo 0°) | Ligado |
| 😌 Temperatura agradável | Fechada (servo 0°) | Desligado |

A ideia é ventilar o quarto pela janela sempre que possível. Quando está chovendo, a janela é fechada e o ventilador assume.

Os limites usados em cada versão:

| Ajuste | Tinkercad | Placa real |
| ------ | --------- | ---------- |
| "Quente" a partir de | 28 °C | 22 °C |
| Volta a "agradável" abaixo de | 28 °C | 21 °C (histerese de 1 °C) |
| "Chovendo" quando a leitura do potenciômetro fica abaixo de | 500 | 500 |
| Volta a "seco" acima de | 500 | 530 (histerese de 30) |

### Histerese (placa real)

Sensores reais oscilam. Com um único limite, uma temperatura variando entre 21,9 °C e 22,0 °C faria a janela abrir e fechar sem parar. Por isso a versão da placa guarda o estado anterior em `quente` e `chovendo`, e só muda quando a leitura passa do limite com folga:

```cpp
if (isnan(temp))                              quente = false;
else if (temp >= TEMP_QUENTE)                 quente = true;
else if (temp < TEMP_QUENTE - HISTERESE_TEMP) quente = false;

if (chuvaV < LIMIAR_CHUVA)                        chovendo = true;
else if (chuvaV > LIMIAR_CHUVA + HISTERESE_CHUVA) chovendo = false;
```

Se o sensor de temperatura não responder (`NAN`), o sistema considera que não está quente.

### Modo manual

Os botões (ou os comandos da Serial) abrem/fecham a janela e ligam/desligam o ventilador diretamente.

### LED indicador de temperatura

Um LED no pino 5 tem o brilho controlado por **PWM**, proporcional à temperatura (regra de três de uma faixa de temperatura para 0 a 255):

| Versão | Apagado em | Brilho máximo em |
| ------ | ---------- | ---------------- |
| Tinkercad | 20 °C | 35 °C |
| Placa real | 22 °C | 24 °C |

```cpp
float t = constrain(temp, TEMP_LED_MIN, TEMP_LED_MAX);
int brilho = (t - TEMP_LED_MIN) * PWM_MAX / (TEMP_LED_MAX - TEMP_LED_MIN);
analogWrite(PIN_LED, brilho);
```

Na placa a faixa é curta de propósito, para o LED mudar com pequenas variações. Como o DHT11 mede de 1 em 1 °C, o LED tem na prática três níveis: apagado (22 °C), metade (23 °C) e máximo (24 °C).

---

## 🔌 Componentes

| Função | Tinkercad | Placa real |
| ------ | --------- | ---------- |
| Microcontrolador | Arduino Uno | Arduino Uno |
| Temperatura | **TMP36** (analógico) | **DHT11** (digital) |
| Chuva | Potenciômetro | Potenciômetro 10 kΩ |
| Display | LCD 16x2 I2C | LCD 16x2 com módulo I2C **PCF8574** |
| Janela | Servomotor | Servo **SG90** |
| Ventilador | Motor CC | Motor CC |
| Ponte H | **L293D** | Módulo **L298N** |
| Alimentação externa | Fonte de bancada | Módulo de fonte **MB102** na protoboard |
| Botões | 4 push-buttons | 4 push-buttons |
| LED | LED + resistor 220 Ω | LED vermelho + resistor 220 Ω |

---

## 📍 Pinagem

| Pino | Componente | Tipo |
| ---- | ---------- | ---- |
| A0 | TMP36 (Vout), **só no Tinkercad** | Entrada analógica |
| A1 | Potenciômetro (sensor de chuva) | Entrada analógica |
| A4 / A5 | LCD I2C (SDA / SCL) | I2C |
| 2 | Botão troca de tela | Entrada digital (**interrupção INT0**) |
| 3 | Ponte H, entrada 1 (velocidade do ventilador) | Saída **PWM** |
| 4 | Ponte H, entrada 2 | Saída digital |
| 5 | LED indicador de temperatura | Saída **PWM** |
| 8 | DHT11 (dados), **só na placa** | Entrada digital |
| 9 | Ponte H, enable (EN do L293D / ENA do L298N) | Saída digital |
| 10 | Servo da janela (SIG) | Saída (sinal do servo) |
| 11 | Botão modo AUTO / MANUAL | Entrada digital |
| 12 | Botão ventilador (modo manual) | Entrada digital |
| 13 | Botão janela (modo manual) | Entrada digital |

Todos os botões usam `INPUT_PULLUP`, ou seja, ficam em `HIGH` soltos e vão para `LOW` quando apertados. Cada botão liga o pino ao GND.

### Ligações de alimentação (placa real)

* **Potenciômetro:** pontas no 5V e no GND, pino do meio no A1;
* **DHT11, LCD:** VCC no 5V e GND no GND;
* **L298N:** borne +12V no 5V da fonte MB102 e GND no GND da fonte. Se o jumper do ENA estiver no lugar, o fio do pino 9 é opcional;
* **Servo:** vermelho no 5V da fonte MB102, marrom no GND e laranja no pino 10;
* **LED:** pino 5 → resistor 220 Ω → LED → GND.

> ⚠️ O GND do Arduino, da fonte MB102, do L298N e do servo precisam estar **ligados entre si**. Sem o GND comum, o motor e o servo não respondem corretamente.

---

## 🎛️ Controles

### Botões

| Botão | Função |
| ----- | ------ |
| Modo (pino 11) | Alterna entre modo **AUTO** e **MANUAL** |
| Janela (pino 13) | Abre/fecha a janela (só no modo manual) |
| Ventilador (pino 12) | Liga/desliga o ventilador (só no modo manual) |
| Tela (pino 2) | Troca a tela do LCD (via **interrupção**) |

### Comandos pela Serial (9600 baud)

Basta digitar a letra no Monitor Serial (maiúscula ou minúscula):

| Comando | Ação |
| ------- | ---- |
| `A` | Modo automático |
| `M` | Modo manual |
| `J` | Abre/fecha a janela |
| `V` | Liga/desliga o ventilador |

Os comandos `J` e `V` colocam o sistema no **modo manual** automaticamente; caso contrário, o modo automático desfaria o comando no ciclo seguinte.

O Arduino responde a cada comando (ex.: `> Ventilador LIGADO`) e envia o status do sistema a cada **5 segundos**:

```text
T=23.00 Chuva=320 Modo=AUTO Janela=FECHADA Vent=ON
```

---

## 🖥️ Telas do LCD

Ao ligar, o LCD mostra **"Quarto Inteligente"** (por 1,5 s no Tinkercad e 3 s na placa). Depois disso, o botão do pino 2 alterna entre as telas:

### Tela 1 – Resumo

```text
T:23.0C  Chuva
AUTO J:FEC V:ON
```

* Temperatura e se está chovendo ou seco. Na placa, se o DHT11 não responder, aparece `T:ERRO`;
* Modo (`AUTO` / `MAN`), janela (`ABR` / `FEC`) e ventilador (`ON` / `OFF`).

### Tela 2 – Detalhes

```text
Chuva: 69%
Vent: 100%
```

* **Chuva:** intensidade da chuva em porcentagem. A leitura é invertida, pois quanto mais molhado, menor a tensão (leitura `1023` → 0%, leitura `0` → 100%);
* **Vent:** potência do ventilador, calculada a partir do valor de PWM (0 a 255 → 0 a 100%).

---

## 🧠 Detalhes da implementação

### Temporização sem `delay()`

O programa não usa `delay()`. O `loop()` roda livremente e cada tarefa verifica, com `millis()`, se já está na hora de executar:

* Leituras, lógica, atuadores e LCD: a cada **100 ms**;
* Leitura nova do DHT11 (placa): a cada **2 s**, o limite do sensor;
* Status pela Serial: a cada **5 s**;
* Tela de abertura: some sozinha após **1,5 s** (Tinkercad) ou **3 s** (placa).

Assim o sistema nunca fica travado e continua respondendo aos botões e à Serial.

### Fluxo do `loop()`

1. Se ainda não passaram 100 ms desde a última atualização, retorna;
2. Lê a temperatura e o potenciômetro;
3. Aplica a histerese em `quente` e `chovendo` (placa);
4. Lê os botões, a flag da interrupção da tela e os comandos da Serial;
5. Decide o estado da janela e do ventilador (automático ou manual);
6. Aciona o servo, o motor e o LED;
7. Atualiza o LCD;
8. A cada 5 s, envia o status pela Serial.

A decisão (passo 5) só altera as variáveis `janelaAberta` e `ventLigado`; quem mexe no hardware é o passo 6.

### Interrupção externa

O botão de troca de tela está no **pino 2**, um dos dois pinos do Uno com interrupção externa (INT0). A interrupção é configurada com:

```cpp
attachInterrupt(digitalPinToInterrupt(BTN_TELA), isrBotaoTela, FALLING);
```

A rotina de interrupção (`isrBotaoTela`) é propositalmente curta: apenas aplica um **debounce** de 200 ms e levanta uma flag `volatile`. Quem troca a tela e limpa o LCD é o `loop()`.

### Saídas PWM

* **Pino 3:** velocidade do motor do ventilador (`analogWrite` na entrada 1 da ponte H);
* **Pino 5:** brilho do LED indicador de temperatura.

> O pino 9 (enable da ponte H) é usado como saída digital, pois a biblioteca `Servo` usa o Timer1 e desativa o `analogWrite()` nos pinos 9 e 10.

Na placa, o ventilador usa **PWM 255** (velocidade máxima): o L298N perde cerca de 2 V internamente e, alimentado com 5 V, só entrega uns 3 V ao motor. Com menos que isso, o motor pode não girar.

### Sensor de temperatura

**Tinkercad (TMP36):** a tensão é convertida em graus Celsius pela fórmula do sensor (10 mV/°C com offset de 0,5 V):

```cpp
float tensao = analogRead(PIN_TEMP) * 5.0 / 1023.0;
return (tensao - 0.5) * 100.0;
```

**Placa (DHT11):** o sensor é digital e só faz uma leitura nova a cada ~2 s. A função consulta o sensor nesse intervalo e, no meio-tempo, devolve o último valor lido:

```cpp
float lerTemperatura() {
  if (!jaLeuDHT || millis() - ultimaLeituraDHT >= INTERVALO_DHT_MS) {
    ultimaLeituraDHT = millis();
    jaLeuDHT = true;
    tempAtual = dht.readTemperature();   // NAN se a leitura falhar
  }
  return tempAtual;
}
```

### Organização em funções

| Grupo | Função | Responsabilidade |
| ----- | ------ | ---------------- |
| Entradas | `lerTemperatura()` | Lê o sensor de temperatura e devolve °C |
| Entradas | `apertou()` | Detecta o instante em que um botão é apertado |
| Entradas | `isrBotaoTela()` | Rotina de interrupção do botão de tela |
| Saídas | `controlarVentilador()` | Liga/desliga o motor pela ponte H |
| Saídas | `atualizarLedTemperatura()` | Ajusta o brilho do LED conforme a temperatura |
| Display | `mostrarTelaResumo()` | Desenha a tela 1 do LCD |
| Display | `mostrarTelaDetalhes()` | Desenha a tela 2 do LCD |
| Display | `atualizarDisplay()` | Cuida da abertura e escolhe qual tela mostrar |
| Serial | `lerComandosSerial()` | Lê e executa os comandos recebidos pela Serial |
| Serial | `enviarStatus()` | Envia leituras e estados pela Serial |

O servo é controlado direto no `loop()`, com `janela.write(janelaAberta ? ANG_ABERTA : ANG_FECHADA)`.

---

## 🔁 Da simulação para a placa real

Nem tudo que existe no Tinkercad se comporta igual ao componente físico. As mudanças feitas no `Codigo_placa.ino`:

| Parte | Tinkercad | Placa real | Ajuste no código |
| ----- | --------- | ---------- | ---------------- |
| Temperatura | TMP36 no A0 | DHT11 no pino 8 | Biblioteca DHT, leitura a cada 2 s, tratamento de falha (`NAN` → `ERRO`) |
| LCD | `Adafruit_LiquidCrystal` (chip MCP23008) | Módulo I2C PCF8574 | Biblioteca `LiquidCrystal_I2C`, endereço `0x27`, `lcd.init()` e `lcd.backlight()` |
| Ponte H | L293D, ventilador com PWM 200 | L298N, ventilador com PWM 255 | Mesma lógica de pinos; PWM no máximo por causa da queda de tensão |
| Alimentação | Fonte de bancada | Fonte MB102 | GND comum ao Arduino |
| Leituras | Limite único | Com ruído | Histerese na temperatura e na chuva |
| Limites | Quente ≥ 28 °C, LED de 20 a 35 °C | Quente ≥ 22 °C, LED de 22 a 24 °C | Ajustados para a temperatura ambiente da sala |

### Problemas comuns na montagem

| Sintoma | Causa provável | Solução |
| ------- | -------------- | ------- |
| LCD aceso, mas sem texto | Contraste ou endereço I2C | Girar o trimpot azul atrás do módulo; se não resolver, trocar `LCD_ENDERECO` para `0x3F` |
| LCD sem texto e luz não pisca no teste | SDA/SCL trocados | Conferir SDA → A4 e SCL → A5 |
| `T:ERRO` no LCD | DHT11 sem resposta | Conferir o fio de dados no pino 8 e a alimentação do sensor |
| Motor não gira | Tensão baixa no L298N | Manter `VEL_VENT = 255` e conferir o GND comum |
| Servo ou motor instáveis, Arduino reiniciando | Alimentação pelo Arduino | Alimentar servo e L298N pela fonte MB102, com GND comum |

---

## ✅ Requisitos atendidos

| Requisito | Como foi atendido |
| --------- | ----------------- |
| Arduino Uno | Simulado no Tinkercad e montado em protoboard com Arduino Uno |
| Código comentado e em funções | Ver tabela de funções acima |
| Pelo menos uma interrupção | Botão de tela no pino 2 (INT0) |
| Boas práticas / sem `delay()` | Constantes nomeadas e temporização com `millis()` |
| ≥ 5 I/O digitais | 4 botões + IN2, EN e servo (e o DHT11 na placa) |
| ≥ 2 entradas analógicas | Tinkercad: A0 (TMP36) e A1 (chuva). Placa: A1 (chuva); o DHT11 é digital |
| ≥ 2 saídas analógicas (PWM) | Pino 3 (ventilador) e pino 5 (LED) |
| Serial nos dois sentidos | Recebe comandos `A`, `M`, `J`, `V` e envia o status |
| Dispositivo SPI ou I2C | LCD 16x2 via I2C |
| LCD com pelo menos duas telas | Telas Resumo e Detalhes |
| Usuário troca de tela | Botão do pino 2 |

---

## 📁 Estrutura

```text
quarto_inteligente/
│
├── codigo_tinkercad.ino
├── Codigo_placa.ino
├── Esquematico - Quarto Inteligente.pdf
└── README.md
```

### `codigo_tinkercad.ino`

Código da simulação, desenvolvido e testado no **Tinkercad Circuits**.

### `Codigo_placa.ino`

Código adaptado para a montagem física, com DHT11, módulo LCD PCF8574, L298N e histerese.

### `Esquematico - Quarto Inteligente.pdf`

Esquemático elétrico com as conexões entre o Arduino, sensores, botões, LCD, servo, ponte H e motor.

---

## 📚 Bibliotecas

### Tinkercad

```cpp
#include <Servo.h>                  // servomotor da janela
#include <Adafruit_LiquidCrystal.h> // LCD 16x2 I2C do Tinkercad
```

### Placa real

```cpp
#include <Wire.h>              // comunicacao I2C
#include <Servo.h>             // servomotor da janela
#include <LiquidCrystal_I2C.h> // LCD 16x2 com modulo PCF8574
#include <DHT.h>               // sensor de temperatura DHT11
```

Instale pelo Gerenciador de Bibliotecas da Arduino IDE:

* **LiquidCrystal I2C** (Frank de Brabander);
* **DHT sensor library** (Adafruit), aceitando instalar junto a **Adafruit Unified Sensor**.

---

## 👨‍💻 Autores

* **José Lucas de Lima Moura**
* **Marisol Sales**

Orientação: **Prof. Dr. Emmanuel Andrade**
