# Controle de Motor de Passo 28BYJ-48

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**, utilizando um Arduino para controlar um motor de passo **28BYJ-48**.

O sistema permite controlar o **sentido de rotação** e selecionar entre os modos de acionamento **passo completo** e **meio passo** através de botões.

## ⚙️ Componentes

* 1x Arduino
* 1x Motor de passo **28BYJ-48**
* 1x Driver **ULN2003**
* 3x Push-buttons
* Jumpers
* Protoboard
* Fonte de alimentação adequada para o motor

## 🎛️ Controles

O sistema possui três botões:

| Botão              | Função                                                   |
| ------------------ | -------------------------------------------------------- |
| **Meio Passo**     | Executa o motor utilizando a sequência de meio passo     |
| **Passo Completo** | Executa o motor utilizando a sequência de passo completo |
| **Sentido**        | Inverte o sentido de rotação                             |

Os botões utilizam os resistores **Pull-Up internos do Arduino**, configurados através de `INPUT_PULLUP`.

### Pinos dos botões

| Função         | Pino |
| -------------- | ---: |
| Meio Passo     |    7 |
| Passo Completo |    6 |
| Sentido        |    5 |

### Pinos do motor

| Entrada do driver | Pino Arduino |
| ----------------- | -----------: |
| IN1               |            8 |
| IN2               |            9 |
| IN3               |           10 |
| IN4               |           11 |

## 🔄 Modos de acionamento

### Passo Completo

No modo de **passo completo**, duas bobinas do motor permanecem acionadas simultaneamente.

A sequência utilizada é:

```text
1100
0110
0011
1001
```

Esse método proporciona **maior torque**, porém possui menor resolução de movimento.

O código considera:

```cpp
2048 passos ≈ 1 volta
```

### Meio Passo

No modo de **meio passo**, o acionamento alterna entre uma e duas bobinas.

A sequência utilizada é:

```text
1000
1100
0100
0110
0010
0011
0001
1001
```

Esse método aumenta a resolução do movimento e proporciona um movimento mais suave.

O código considera:

```cpp
4096 passos ≈ 1 volta
```

## 🔃 Controle de sentido

O sentido de rotação é controlado pela variável:

```cpp
bool sentido;
```

Quando o botão de sentido é pressionado, a função:

```cpp
inverteSentido();
```

altera o valor da variável:

```cpp
sentido = !sentido;
```

Assim, o próximo movimento será realizado no sentido oposto.

## 🧹 Debounce dos botões

Para evitar múltiplas leituras causadas pelo ruído mecânico dos botões, foi utilizada a biblioteca **Bounce2**.

Cada botão possui uma instância independente:

```cpp
Bounce debouncerMeioPasso;
Bounce debouncerPassoCompleto;
Bounce debouncerSentido;
```

O tempo de filtragem utilizado é de:

```cpp
25 ms
```

O acionamento é detectado através de:

```cpp
debouncer.fell()
```

que identifica a transição de **HIGH para LOW** quando o botão é pressionado.

## 🧩 Estrutura do código

O programa foi dividido em funções para facilitar a organização e manutenção:

### `setup()`

Realiza a configuração inicial dos pinos do Arduino e dos botões.

### `loop()`

Monitora os botões e executa as ações correspondentes.

### `inverteSentido()`

Altera o sentido de rotação do motor.

### `moverPassoCompleto()`

Executa o movimento utilizando a sequência de passo completo.

### `moverMeioPasso()`

Executa o movimento utilizando a sequência de meio passo.

### `escreverPinos()`

Envia os estados `HIGH` e `LOW` para as quatro entradas responsáveis pelo acionamento das bobinas.

### `desligarMotor()`

Desliga todas as bobinas após o término do movimento.

## ⏱️ Controle de velocidade

A velocidade do motor é determinada pelo intervalo entre os passos.

No código, esse intervalo é definido pelo parâmetro:

```cpp
int atrasoMs
```

Atualmente, o valor utilizado é:

```cpp
3 ms
```

Quanto **menor** o atraso, maior será a velocidade do motor. Entretanto, atrasos muito pequenos podem fazer com que o motor perca passos ou não consiga acompanhar a sequência de acionamento.

## 📁 Arquivos

```text
motor-passo/
│
├── motor-passo.ino
├── README.md
└── esquematico/
    └── ...
```

O arquivo `.ino` contém o código responsável pelo controle do motor.

Caso o projeto possua um esquemático, ele pode ser encontrado na pasta `esquematico`.

## 🛠️ Tecnologias utilizadas

* **Arduino**
* **C/C++**
* **Biblioteca Bounce2**
* **Motor de passo 28BYJ-48**
* **Driver ULN2003**

## 👥 Autores

**José Lucas**
**Marisol Sales**

Projeto desenvolvido para a disciplina d
