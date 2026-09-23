# Controle de Servomotor com Arduino

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**, utilizando Arduino para realizar o controle de um **servomotor**.

O sistema permite controlar a **posição angular** do servomotor, entre **0° e 180°**, utilizando um **potenciômetro**.

## 📋 Atividade

Etapa 2 — Servomotor: conectar um servomotor ao Arduino e programar os seguintes movimentos:

* Posicionamento em **0°, 90° e 180°**.
* **Movimento gradual** entre os ângulos.
* **Controle da posição** por meio de um potenciômetro.

## ⚙️ Funcionalidades

* **Leitura do potenciômetro:** lê um valor analógico entre `0` e `1023`.
* **Conversão para ângulo:** converte a leitura para um ângulo entre `0°` e `180°`.
* **Posicionamento do servo:** move o servomotor para o ângulo calculado.

### Relação entre potenciômetro e ângulo

| Leitura do potenciômetro | Ângulo do servo |
| -----------------------: | --------------: |
|                        0 |              0° |
|                      512 |            ~90° |
|                     1023 |            180° |

Como a leitura é feita continuamente, girar o potenciômetro gradualmente move o servo de forma gradual entre as posições.

## 🔌 Ligações

### Potenciômetro

| Terminal do potenciômetro | Ligação |
| ------------------------- | ------: |
| Extremidade 1             |  5V     |
| Cursor (pino central)     |  A5     |
| Extremidade 2             |  GND    |

### Servomotor

| Fio do servo  | Ligação                          |
| ------------- | -------------------------------: |
| SIG (sinal)   | Pino 9                           |
| PWR (+)       | Fonte de energia externa (+)     |
| GND (-)       | Fonte de energia externa (-)     |

O servomotor é alimentado por uma **fonte externa**, evitando sobrecarregar o regulador de 5V do Arduino. O **GND da fonte deve estar ligado ao GND do Arduino**, para que o sinal de controle tenha a mesma referência.

O esquemático completo está disponível em [esquematico.pdf](esquematico.pdf).

## 🧠 Funcionamento

O programa utiliza a biblioteca `Servo.h` e as seguintes variáveis:

* `servo_9`: objeto que representa o servomotor.
* `pinoPoten`: pino analógico onde o potenciômetro está ligado (`A5`).
* `valorPoten`: armazena a leitura do potenciômetro (`0` a `1023`).
* `angulo`: armazena o ângulo calculado (`0°` a `180°`).

No `setup()`, o servo é conectado ao pino 9 com os limites de pulso para 0° e 180°:

```cpp
servo_9.attach(9, 500, 2500);
```

```text
500 µs  → 0°
2500 µs → 180°
```

## 🎚️ Controle da posição

A cada ciclo do `loop()`, o programa lê o potenciômetro, converte o valor para ângulo e envia para o servo:

```cpp
valorPoten = analogRead(pinoPoten);
angulo = map(valorPoten, 0, 1023, 0, 180);
servo_9.write(angulo);
```

A função `map()` realiza a conversão proporcional da faixa `0–1023` (conversor analógico-digital de 10 bits) para a faixa `0–180` (ângulos do servo).

## ⏱️ Temporização

Ao final de cada ciclo é utilizado:

```cpp
delay(15);
```

Esse intervalo dá tempo para o servomotor alcançar a nova posição antes da próxima leitura, deixando o movimento mais estável.

## 🛠️ Tecnologias utilizadas

* Arduino
* C/C++
* Biblioteca Servo
* PWM
* Potenciômetro
* Servomotor
* Tinkercad (simulação e esquemático)

## 👨‍💻 Autores

**José Lucas**
**Marisol Sales**

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**.
