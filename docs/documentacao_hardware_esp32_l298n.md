# Documentação do hardware --- ESP32 + L298N + 2 motores DC

## 1. Objetivo

O conjunto utiliza um ESP32, um driver L298N, dois motores DC com
redução e dois botões de pressão normalmente abertos (NA). Cada botão
controla seu próprio motor por uma sequência de quatro estados:

1.  Primeiro toque: gira para o primeiro sentido (esquerda).
2.  Segundo toque: para.
3.  Terceiro toque: gira para o sentido contrário (direita).
4.  Quarto toque: para.
5.  A sequência recomeça.

Os motores são independentes: cada botão avança somente a sequência do
motor correspondente. Os dois motores podem funcionar simultaneamente.

> "Esquerda" e "direita" indicam os sentidos desejados do mecanismo. O
> sentido físico real depende da orientação dos motores e da polaridade
> de suas ligações.

## 2. Componentes

  -----------------------------------------------------------------------
  Componente                          Função
  ----------------------------------- -----------------------------------
  ESP32 DevKit                        Microcontrolador que executa a
                                      lógica de controle; possui Wi-Fi e
                                      Bluetooth.

  Placa de expansão com bornes        Facilita a conexão dos GPIOs por
                                      terminais de parafuso.

  Driver L298N                        Ponte H dupla para controlar
                                      direção dos dois motores DC.

  2 motores DC com caixa de redução   Produzem o movimento mecânico.

  2 botões NA de dois fios            Fecham contato elétrico somente
                                      enquanto pressionados.
  -----------------------------------------------------------------------

Pelas fotografias, os motores parecem pertencer à família TT, mas
modelo, tensão nominal e corrente não podem ser confirmados apenas pela
aparência.

## 3. Ligações ESP32 → L298N

Mapeamento adotado conforme as cores informadas e a correção dos GPIOs:

  Cor do fio     GPIO ESP32 Terminal L298N   Função
  ------------ ------------ ---------------- --------------------
  Verde             GPIO 26 IN1              Direção do motor 1
  Roxo              GPIO 25 IN2              Direção do motor 1
  Azul              GPIO 32 IN3              Direção do motor 2
  Amarelo           GPIO 33 IN4              Direção do motor 2

### Saídas para os motores

  Canal do L298N   Saídas        Motor
  ---------------- ------------- ---------
  Canal A          OUT1 e OUT2   Motor 1
  Canal B          OUT3 e OUT4   Motor 2

A correspondência dos fios coloridos com IN1--IN4 deve ser conferida
diretamente nos bornes do driver antes de energizar. As fotos não
permitem confirmar com total segurança cada posição.

## 4. Ligações dos botões

Os botões são normalmente abertos, possuem dois fios e não têm
polaridade.

  Botão     Primeiro terminal   Segundo terminal   Função
  --------- ------------------- ------------------ -------------------------------
  Botão 1   GPIO 27             GND                Avança a sequência do motor 1
  Botão 2   GPIO 14             GND                Avança a sequência do motor 2

Configurar as entradas como `INPUT_PULLUP`, utilizando o resistor
interno do ESP32:

-   Botão solto: HIGH.
-   Botão pressionado: contato fechado entre GPIO e GND; leitura LOW.
-   Não é necessário resistor externo para essa configuração.

Os fios de cada botão podem ser ligados em qualquer ordem.

## 5. Sequência de funcionamento

### Botão 1 --- Motor 1

    Toque Estado                 Comando
  ------- ---------------------- --------------------
        1 Gira para esquerda     IN1 HIGH / IN2 LOW
        2 Para                   IN1 LOW / IN2 LOW
        3 Gira para direita      IN1 LOW / IN2 HIGH
        4 Para                   IN1 LOW / IN2 LOW
        5 Reinicia a sequência   Primeiro sentido

### Botão 2 --- Motor 2

    Toque Estado                 Comando
  ------- ---------------------- --------------------
        1 Gira para esquerda     IN3 HIGH / IN4 LOW
        2 Para                   IN3 LOW / IN4 LOW
        3 Gira para direita      IN3 LOW / IN4 HIGH
        4 Para                   IN3 LOW / IN4 LOW
        5 Reinicia a sequência   Primeiro sentido

Cada botão mantém sua própria sequência. Pressionar um botão não deve
alterar o estado do outro motor.

A lógica deve detectar cada novo aperto, e não avançar repetidamente
enquanto o botão permanecer pressionado. Recomenda-se antirrepique
(debounce) por software, por exemplo, de aproximadamente 50 ms.

## 6. Habilitação do L298N

-   **ENA** habilita o canal A (motor 1).
-   **ENB** habilita o canal B (motor 2).

Para o controle simples de direção descrito, ambos os canais precisam
estar habilitados. Isso pode ser feito mantendo os jumpers ENA e ENB
instalados, conforme a configuração do módulo. Se os jumpers forem
removidos, será necessário fornecer sinais apropriados aos terminais de
habilitação.

O controle de velocidade por PWM não faz parte desta configuração.

## 7. Alimentação e referência elétrica

-   Alimentar os motores com tensão compatível com suas especificações.
-   Alimentar o ESP32 por uma fonte adequada à placa.
-   Interligar o GND do ESP32 ao GND do L298N (terra comum).
-   Nunca alimentar motores diretamente por GPIOs do ESP32.
-   O borne marcado `+12V` é a entrada de alimentação do módulo; a
    tensão utilizada deve respeitar as especificações do driver e dos
    motores.
-   Conferir a configuração do jumper de 5 V do L298N antes de conectar
    qualquer alimentação ao ESP32.

As fotografias não permitem confirmar a tensão da fonte, a corrente
disponível ou a configuração exata dos jumpers.

## 8. Cuidados antes de energizar

1.  Desligar a alimentação antes de alterar ligações.
2.  Confirmar verde em IN1, roxo em IN2, azul em IN3 e amarelo em IN4.
3.  Confirmar os GPIOs: verde 26, roxo 25, azul 32 e amarelo 33.
4.  Confirmar botão 1 entre GPIO 27 e GND e botão 2 entre GPIO 14 e GND.
5.  Confirmar GND comum entre ESP32 e L298N.
6.  Confirmar que ENA e ENB estão habilitados.
7.  Fazer o primeiro teste sem carga mecânica, se possível.
8.  Se um motor girar no sentido oposto ao desejado, inverter a lógica
    daquele motor ou seus dois fios de saída, sempre com a alimentação
    desligada.
9.  Verificar aquecimento do L298N e a corrente de partida/bloqueio dos
    motores. Não considerar a corrente anunciada comercialmente como
    garantia de operação contínua.
10. Se houver limites físicos de deslocamento, instalar fins de curso ou
    proteção equivalente. A sequência de botões, sozinha, não impede que
    o motor force um batente.

## 9. Resumo das conexões

  Item          Conexão
  ------------- -------------------------------------
  Verde         ESP32 GPIO 26 → L298N IN1
  Roxo          ESP32 GPIO 25 → L298N IN2
  Azul          ESP32 GPIO 32 → L298N IN3
  Amarelo       ESP32 GPIO 33 → L298N IN4
  Botão 1       GPIO 27 ↔ GND
  Botão 2       GPIO 14 ↔ GND
  Motor 1       L298N OUT1 / OUT2
  Motor 2       L298N OUT3 / OUT4
  Habilitação   ENA e ENB habilitados
  Referência    GND do ESP32 ligado ao GND do L298N

## 10. Dados ainda não confirmados visualmente

-   Correspondência física exata dos fios coloridos nos terminais
    IN1--IN4.
-   Qual motor físico está conectado a OUT1/OUT2 e qual a OUT3/OUT4.
-   Tensão e corrente nominais dos motores.
-   Tensão e capacidade de corrente da fonte.
-   Estado dos jumpers ENA, ENB e do jumper de alimentação de 5 V.
-   Existência de fins de curso ou outras proteções mecânicas.

------------------------------------------------------------------------

**Resumo:** ESP32 controla dois motores por L298N. Dois botões NA
independentes avançam cada motor pela sequência esquerda → parado →
direita → parado, repetidamente.
