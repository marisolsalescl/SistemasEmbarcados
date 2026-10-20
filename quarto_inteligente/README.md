# 🏠 Quarto Inteligente

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**, utilizando **Arduino Uno** para automatizar a janela e o ventilador de um quarto de acordo com a **temperatura** e a **chuva**.

A janela é movida por um **servomotor** e o ventilador é um **motor CC** acionado por uma ponte H **L293D**. O sistema funciona em modo **automático** ou **manual**, mostra as informações em um **LCD 16x2 (I2C)** com duas telas e pode ser controlado e monitorado pela **porta Serial**.

---

## ⚙️ Funcionamento

### Modo automático

O Arduino lê a temperatura e o sensor de chuva e decide sozinho o que fazer:

| Situação | Janela | Ventilador |
| -------- | ------ | ---------- |
| 🌡️ Quente (≥ 28 °C) e sem chuva | Aberta | Desligado |
| 🌡️ Quente (≥ 28 °C) e chovendo | Fechada | Ligado |
| 😌 Temperatura agradável (< 28 °C) | Fechada | Desligado |

A ideia é ventilar o quarto pela janela sempre que possível. Quando está chovendo, a janela é fechada e o ventilador assume.

### Modo manual

Os botões (ou os comandos da Serial) abrem/fecham a janela e ligam/desligam o ventilador diretamente.

### LED indicador de temperatura

Um LED no pino 5 tem o brilho controlado por **PWM**: apagado em 20 °C, aumentando gradualmente até o brilho máximo em 35 °C.

---

## 🔌 Componentes

* Arduino Uno
* Display LCD 16x2 com módulo I2C
* Sensor de temperatura **TMP36**
* Potenciômetro (simula o sensor de chuva)
* Servomotor (janela)
* Motor CC (ventilador)
* Ponte H **L293D**
* Fonte de alimentação externa (alimenta o motor pelo L293D)
* 4 botões push-button
* LED + resistor de 220 Ω
* Protoboards e jumpers

---

## 📍 Pinagem

| Pino | Componente | Tipo |
| ---- | ---------- | ---- |
| A0 | TMP36 (Vout) | Entrada analógica |
| A1 | Potenciômetro (sensor de chuva) | Entrada analógica |
| A4 / A5 | LCD I2C (SDA / SCL) | I2C |
| 2 | Botão troca de tela | Entrada digital (**interrupção INT0**) |
| 3 | L293D 1A (velocidade do ventilador) | Saída **PWM** |
| 4 | L293D 2A | Saída digital |
| 5 | LED indicador de temperatura | Saída **PWM** |
| 9 | L293D 1-2EN (habilita a ponte) | Saída digital |
| 10 | Servo da janela (SIG) | Saída (sinal do servo) |
| 11 | Botão modo AUTO / MANUAL | Entrada digital |
| 12 | Botão ventilador (modo manual) | Entrada digital |
| 13 | Botão janela (modo manual) | Entrada digital |

Todos os botões usam `INPUT_PULLUP`, ou seja, ficam em `HIGH` soltos e vão para `LOW` quando apertados.

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
T=30.12 Chuva=320 Modo=AUTO Janela=FECHADA Vent=ON
```

---

## 🖥️ Telas do LCD

Ao ligar, o LCD mostra **"Quarto Inteligente"** por 1,5 s. Depois disso, o botão do pino 2 alterna entre as telas:

### Tela 1 – Resumo

```text
T:30.1C  Chuva
AUTO J:FEC V:ON
```

* Temperatura e se está chovendo ou seco;
* Modo (`AUTO` / `MAN`), janela (`ABR` / `FEC`) e ventilador (`ON` / `OFF`).

### Tela 2 – Detalhes

```text
Chuva: 69%
Vent: 78%
```

* **Chuva:** intensidade da chuva em porcentagem. A leitura do sensor é invertida, pois quanto mais molhado, menor a tensão (leitura `1023` → 0%, leitura `0` → 100%);
* **Vent:** potência do ventilador, calculada a partir do valor de PWM (0 a 255 → 0 a 100%).

---

## 🧠 Detalhes da implementação

### Temporização sem `delay()`

O programa não usa `delay()`. O `loop()` roda livremente e cada tarefa verifica, com `millis()`, se já está na hora de executar:

* Leituras, lógica, atuadores e LCD: a cada **100 ms**;
* Status pela Serial: a cada **5 s**;
* Tela de abertura: some sozinha após **1,5 s**.

Assim o sistema nunca fica travado e continua respondendo aos botões e à Serial.

### Interrupção externa

O botão de troca de tela está no **pino 2**, um dos dois pinos do Uno com interrupção externa (INT0). A interrupção é configurada com:

```cpp
attachInterrupt(digitalPinToInterrupt(BTN_TELA), isrBotaoTela, FALLING);
```

A rotina de interrupção (`isrBotaoTela`) é propositalmente curta: apenas aplica um **debounce** de 200 ms e levanta uma flag `volatile`. Quem troca a tela e limpa o LCD é o `loop()`.

### Saídas PWM

* **Pino 3:** velocidade do motor do ventilador (`analogWrite` no L293D);
* **Pino 5:** brilho do LED indicador de temperatura.

> O pino 9 (EN do L293D) é usado como saída digital, pois a biblioteca `Servo` usa o Timer1 e desativa o `analogWrite()` nos pinos 9 e 10.

### Sensor de temperatura

A tensão do TMP36 é convertida em graus Celsius pela fórmula do sensor (10 mV/°C com offset de 0,5 V):

```cpp
float tensao = analogRead(PIN_TEMP) * 5.0 / 1023.0;
return (tensao - 0.5) * 100.0;
```

### Organização em funções

| Função | Responsabilidade |
| ------ | ---------------- |
| `lerTemperatura()` | Lê o TMP36 e converte para °C |
| `apertou()` | Detecta o instante em que um botão é apertado |
| `isrBotaoTela()` | Rotina de interrupção do botão de tela |
| `controlarVentilador()` | Liga/desliga o motor pelo L293D |
| `atualizarLedTemperatura()` | Ajusta o brilho do LED conforme a temperatura |
| `mostrarTelaResumo()` | Desenha a tela 1 do LCD |
| `mostrarTelaDetalhes()` | Desenha a tela 2 do LCD |
| `atualizarDisplay()` | Cuida da abertura e escolhe qual tela mostrar |
| `lerComandosSerial()` | Lê e executa os comandos recebidos pela Serial |
| `enviarStatus()` | Envia leituras e estados pela Serial |

---

## ✅ Requisitos atendidos

| Requisito | Como foi atendido |
| --------- | ----------------- |
| Arduino Uno | Projeto simulado no Tinkercad com Arduino Uno |
| Código comentado e em funções | Ver tabela de funções acima |
| Pelo menos uma interrupção | Botão de tela no pino 2 (INT0) |
| Boas práticas / sem `delay()` | Constantes nomeadas e temporização com `millis()` |
| ≥ 5 I/O digitais | 4 botões + IN2, EN e servo |
| ≥ 2 entradas analógicas | A0 (TMP36) e A1 (sensor de chuva) |
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
├── Esquematico - Quarto Inteligente.pdf
└── README.md
```

### `codigo_tinkercad.ino`

Código do projeto, desenvolvido e testado no **Tinkercad Circuits**.

### `Esquematico - Quarto Inteligente.pdf`

Esquemático elétrico com as conexões entre o Arduino, sensores, botões, LCD, servo, L293D e motor.

---

## 📚 Bibliotecas

```cpp
#include <Servo.h>                  // servomotor da janela
#include <Adafruit_LiquidCrystal.h> // LCD 16x2 I2C do Tinkercad
```

---

## 👨‍💻 Autores

* **José Lucas de Lima Moura**
* **Marisol Sales**
