# 🧠 Genius

Projeto de um jogo de memória inspirado no clássico **Genius**, desenvolvido com **Arduino**.

O jogador deve memorizar e reproduzir uma sequência de cores apresentada pelos LEDs. A cada rodada concluída corretamente, a sequência aumenta de tamanho, tornando o jogo progressivamente mais difícil.

O projeto utiliza **botões, LEDs, buzzer e temporização**, além de diferentes técnicas de tratamento dos botões para as versões de simulação e implementação na placa.

---

## 🎮 Funcionamento

O jogo funciona da seguinte maneira:

1. O jogador pressiona o botão de início.
2. O Arduino gera uma sequência aleatória de cores.
3. Os LEDs apresentam a sequência ao jogador.
4. O jogador deve reproduzir a sequência utilizando os quatro botões.
5. A cada resposta correta:

   * O LED correspondente é acionado.
   * O buzzer reproduz uma nota correspondente à cor.
6. Ao completar uma sequência:

   * O jogador avança de nível.
   * Uma nova sequência, com um elemento a mais, é apresentada.
7. O jogador perde caso:

   * Pressione um botão incorreto; ou
   * Permaneça mais de 5 segundos sem responder.
8. Ao completar todos os níveis, uma música de vitória é executada.

---

## 🔌 Componentes

* Arduino
* 5 botões
* 4 LEDs:

  * 🔴 Vermelho
  * 🟢 Verde
  * 🔵 Azul
  * 🟡 Amarelo
* Buzzer
* Resistores
* Jumpers
* Protoboard

---

## 📁 Estrutura do repositório

O repositório possui duas versões do código, destinadas a diferentes ambientes de execução:

```text
Genius/
│
├── genio.ino
├── Genius_arduino.ino
└── Esquemático elétrico
```

### `genio.ino`

Código utilizado para a **simulação do projeto no Tinkercad**.

Essa versão foi desenvolvida para funcionar no ambiente de simulação, permitindo testar o funcionamento do jogo antes da montagem física do circuito.

### `Genius_arduino.ino`

Código utilizado para execução na **placa Arduino física**.

Essa versão utiliza a biblioteca **Bounce2** para realizar o tratamento de debounce dos quatro botões utilizados durante o jogo.

O botão responsável por iniciar o jogo utiliza uma **interrupção externa**, permitindo detectar o acionamento do botão de início de forma independente do fluxo normal do `loop()`.

---

## 📚 Bibliotecas

A versão destinada à placa Arduino utiliza a biblioteca:

**Bounce2**

A biblioteca é utilizada para tratar o efeito de *bouncing* dos botões mecânicos.

O debounce evita que uma única pressão física seja interpretada pelo Arduino como várias pressões consecutivas.

Na implementação do projeto:

* **Botão de início:** utiliza interrupção.
* **Botões de resposta:** utilizam a biblioteca Bounce2.

---

## 🎵 Sistema de áudio

O projeto utiliza um buzzer para fornecer feedback sonoro ao jogador.

Cada botão possui uma frequência diferente:

| Botão | Cor      | Nota     |
| ----- | -------- | -------- |
| 1     | Vermelho | Dó       |
| 2     | Verde    | Mi       |
| 3     | Azul     | Sol      |
| 4     | Amarelo  | Dó agudo |

Além dos sons individuais dos botões, o projeto possui músicas para diferentes eventos:

* 🎮 Início do jogo
* ❌ Derrota
* ⬆️ Passagem de nível
* 🏆 Vitória

---

## ⏱️ Sistema de tempo

O jogador possui **5 segundos** para realizar cada entrada.

O temporizador é implementado utilizando a função `millis()`, evitando a necessidade de manter o programa constantemente contando o tempo.

A cada botão pressionado corretamente, o temporizador é reiniciado.

Caso o jogador ultrapasse os 5 segundos sem realizar uma entrada, o jogo é encerrado e a música de derrota é executada.

---

## 💡 Sistema de níveis

A sequência é armazenada em um vetor com **13 posições**.

Os valores armazenados representam as quatro cores:

```text
1 → Vermelho
2 → Verde
3 → Azul
4 → Amarelo
```

O jogo começa exigindo uma sequência de **4 elementos**.

A cada nível concluído, um novo elemento é acrescentado:

```text
Nível 1 → 4 elementos
Nível 2 → 5 elementos
Nível 3 → 6 elementos
...
Nível final → 13 elementos
```

O jogador vence ao completar a sequência máxima.

---

## 🔀 Geração da sequência

A sequência é gerada aleatoriamente utilizando a função:

```cpp
random(1, 5);
```

A função gera valores entre `1` e `4`, correspondentes às quatro cores disponíveis.

Para evitar que a sequência seja sempre igual após cada reinicialização, o projeto utiliza:

```cpp
randomSeed(analogRead(A0));
```

---

## 🧩 Esquemático elétrico

O repositório também disponibiliza o **esquemático elétrico do projeto**, desenvolvido no **Tinkercad**.

O esquemático permite visualizar as conexões entre:

* Arduino
* Botões
* LEDs
* Buzzer
* Resistores

Ele pode ser utilizado como referência para a montagem física do circuito.

---

## 🖥️ Simulação

A versão `genio.ino` pode ser utilizada para reproduzir o projeto no **Tinkercad**, permitindo testar a lógica e o funcionamento do circuito virtualmente.

A simulação é especialmente útil para verificar:

* Sequência de LEDs;
* Leitura dos botões;
* Sistema de níveis;
* Temporizador;
* Sons do buzzer;
* Condições de vitória e derrota.

---

## 🛠️ Tecnologias utilizadas

* **Arduino / C++**
* **Tinkercad Circuits**
* **Biblioteca Bounce2**
* LEDs
* Push buttons
* Buzzer
* Interrupções externas
* `millis()` para temporização
* Geração de números aleatórios

---

## 👨‍💻 Projeto

Este projeto foi desenvolvido como uma aplicação prática de programação em **C/C++ para sistemas embarcados**, explorando conceitos de:

* Entrada e saída digital;
* Interrupções;
* Debounce;
* Estruturas condicionais;
* Funções;
* Vetores;
* Geração de números aleatórios;
* Temporização;
* Controle de LEDs;
* Geração de tons com buzzer.

O projeto também utiliza uma abordagem diferente para o tratamento do botão de início e dos botões utilizados durante a partida, permitindo explorar tanto **interrupções** quanto **debounce por software** em um mesmo sistema.
