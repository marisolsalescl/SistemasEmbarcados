# Controle de Motor DC com Arduino

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**, utilizando Arduino para realizar o controle de um **motor DC**.

O sistema permite controlar o funcionamento do motor, seu **sentido de rotação** e sua **velocidade**, utilizando três botões.

## ⚙️ Funcionalidades

O sistema possui três comandos principais:

* **Ligar/Desligar:** controla o funcionamento do motor.
* **Alterar sentido:** inverte o sentido de rotação do motor.
* **Alterar velocidade:** alterna entre três níveis de velocidade.

### Níveis de velocidade

| Nível | PWM | Descrição         |
| ----: | --: | ----------------- |
|     1 |  85 | Velocidade baixa  |
|     2 | 170 | Velocidade média  |
|     3 | 255 | Velocidade máxima |

Após o terceiro nível, o sistema retorna automaticamente para o primeiro.

## 🔌 Ligações

### Botões

| Componente           | Pino Arduino |
| -------------------- | -----------: |
| Botão Ligar/Desligar |            7 |
| Botão Sentido        |            6 |
| Botão Velocidade     |            5 |

Os botões utilizam `INPUT_PULLUP`, portanto:

* Botão não pressionado → `HIGH`
* Botão pressionado → `LOW`

### Controle do motor

| Função                       | Pino Arduino |
| ---------------------------- | -----------: |
| Controle de velocidade (PWM) |            9 |
| Controle de sentido 1        |            2 |
| Controle de sentido 2        |            3 |

Os pinos 2 e 3 são utilizados para definir o sentido de rotação, enquanto o pino 9 controla a velocidade por meio de PWM.

## 🧠 Funcionamento

O programa utiliza variáveis para armazenar o estado do sistema:

* `liga`: indica se o motor deve estar ligado ou desligado.
* `ligado`: indica se o motor está atualmente em funcionamento.
* `velocidade`: armazena o valor de PWM aplicado ao motor.
* `nivelVelocidade`: indica o nível atual de velocidade.
* `pin2` e `pin3`: armazenam os estados utilizados para controlar o sentido de rotação.

O programa também foi organizado utilizando funções específicas para cada operação:

```cpp
void ligaMotor();
void desligaMotor();
void alteraSentido();
void alteraVelocidade();
```

Essa divisão facilita a organização e a manutenção do código.

## 🔄 Alteração do sentido

O sentido de rotação é alterado invertendo os estados dos pinos 2 e 3:

```cpp
pin2 = !pin2;
pin3 = !pin3;
```

Inicialmente:

```text
Pino 2 → HIGH
Pino 3 → LOW
```

Ao pressionar o botão de sentido:

```text
Pino 2 → LOW
Pino 3 → HIGH
```

Dessa forma, o sentido de rotação é invertido.

## 🎚️ Controle de velocidade

A velocidade é controlada utilizando `analogWrite()` no pino 9.

O Arduino utiliza PWM com valores entre `0` e `255`:

```cpp
analogWrite(9, velocidade);
```

O projeto utiliza três valores:

```text
Nível 1 → 85
Nível 2 → 170
Nível 3 → 255
```

O botão de velocidade percorre esses três níveis de forma cíclica.

## ⏱️ Temporização

Durante o desenvolvimento do projeto, foram utilizados `delay()` para facilitar os testes dos botões e evitar múltiplas leituras causadas pelo acionamento.

Atualmente:

* `delay(1000)` → utilizado após os botões de ligar e velocidade.
* `delay(5000)` → utilizado durante o desligamento do motor.

> **Observação:** esses `delay()` são temporários e podem ser substituídos posteriormente por uma implementação de **debounce sem bloqueio**, utilizando `millis()` ou uma biblioteca específica.

## 🛠️ Tecnologias utilizadas

* Arduino
* C/C++
* PWM
* Botões (Push Buttons)
* Motor DC
* Controle de sentido de rotação

## 👨‍💻 Autores

**José Lucas**
**Marisol Sales**

Projeto desenvolvido para a disciplina de **Sistemas Embarcados**.
