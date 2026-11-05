Documentação Técnica – Módulo L298N (Ponte H Dupla)
1. Identificação
Modelo: L298N
Fabricante do CI: STMicroelectronics
Tipo: Driver de motores com ponte H dupla.
Aplicação: Controle de motores DC e motores de passo bipolares.
2. Datasheet oficial
Documento: L298 – Dual Full-Bridge Driver
Fabricante: STMicroelectronics
Código: DS0218

Baixar datasheet oficial em PDF

3. Funções da placa

O módulo permite:

Controlar dois motores DC.
Inverter o sentido de rotação.
Controlar velocidade por PWM.
Controlar motores de passo bipolares.
Realizar frenagem elétrica.
4. Pinagem
Pino	Função
OUT1	Saída do Motor A
OUT2	Saída do Motor A
OUT3	Saída do Motor B
OUT4	Saída do Motor B
12V	Alimentação dos motores
GND	Terra comum
5V	Alimentação lógica ou saída regulada
ENA	Habilitação do Motor A
ENB	Habilitação do Motor B
IN1	Controle de direção A
IN2	Controle de direção A
IN3	Controle de direção B
IN4	Controle de direção B
5. Especificações elétricas do CI
Característica	Valor
Tensão máxima do estágio de potência	46 V
Alimentação lógica	4,5–7 V
Corrente DC máxima por canal	2 A
Corrente de pico não repetitivo	3 A
Corrente de pico repetitivo	2,5 A
Temperatura máxima de junção	150 °C
Proteção térmica	Sim

Atenção: os valores acima são do circuito integrado L298. A capacidade real do módulo depende da montagem, dissipação térmica, diodos e componentes utilizados.

O L298 apresenta queda de tensão interna, reduzindo a tensão efetivamente entregue ao motor.

6. Jumpers
Jumper 5V-EN

Habilita o regulador de 5 V da placa, quando disponível.

Jumper ENA

Habilita permanentemente o canal A. Removendo-o, pode-se aplicar PWM externamente.

Jumper ENB

Habilita permanentemente o canal B. Removendo-o, pode-se aplicar PWM externamente.

A configuração deve ser conferida na versão específica do módulo.

7. Tabela de funcionamento
ENA	IN1	IN2	Resultado
HIGH	HIGH	LOW	Sentido 1
HIGH	LOW	HIGH	Sentido 2
HIGH	LOW	LOW	Parada livre
HIGH	HIGH	HIGH	Frenagem

Para o segundo motor, utilizar ENB, IN3 e IN4.

8. Controle de velocidade

O controle de velocidade é realizado através de PWM nas entradas ENA e ENB.

ENA controla a velocidade do Motor A.
ENB controla a velocidade do Motor B.
9. Cuidados de instalação
Verificar a tensão nominal dos motores.
Utilizar fonte compatível com a corrente dos motores.
Manter o GND comum entre fonte e controlador.
Não aplicar tensão de alimentação diretamente nas entradas IN1–IN4.
Verificar a temperatura do dissipador.
Não ultrapassar os limites elétricos dos componentes.
Conferir a configuração do jumper 5V-EN antes de alimentar a placa.
10. Documentação complementar
Datasheet oficial STMicroelectronics
Manual do módulo em português – Eletrodex
11. Observação sobre versões do módulo

Existem diferentes fabricantes e versões da placa vermelha L298N.

O datasheet do CI não garante que todas as placas possuam os mesmos reguladores, capacitores, diodos ou características de dissipação.

Para determinar as especificações exatas do módulo, é necessário verificar fotografias da parte superior e inferior da placa e identificar seus componentes.

Documento: Documentação técnica do módulo L298N
Formato: Markdown (.md)
Referência principal: STMicroelectronics – L298 Datasheet


LILYGO T-SIMCAM IR-CUT ESP32-S3
Documentação Técnica Completa

Identificação da placa: S3_CAM_SIM
Revisão do PCB: V1.3
Data impressa no PCB: 23/06/2022
Fabricante: LILYGO
Modelo comercial: T-SIMCAM IR-CUT
Câmeras compatíveis anunciadas: OV2640 / OV5640
Expansão: T-PCIE Module
Categoria: Plataforma de câmera IoT com ESP32-S3.

1. IDENTIFICAÇÃO FÍSICA

A placa apresentada nas fotografias possui as seguintes identificações:

Identificação	Informação
Fabricante	LILYGO
Produto	T-SIMCAM IR-CUT
PCB	S3_CAM_SIM
Revisão	V1.3
Data do PCB	23-6-22
Câmeras anunciadas	OV2640 / OV5640
Expansão	T-PCIE
Interface de programação	USB Type-C

A embalagem original apresenta a inscrição:

T-SIMCAM IR-CUT

OV2640/OV5640 Camera

Support T-PCIE Module

É importante diferenciar esta versão das demais placas T-SIMCAM, T-Camera-S3 e T-CameraPlus-S3.

Os esquemas e mapas de GPIOs dessas outras placas não devem ser utilizados automaticamente nesta revisão.

2. DESCRIÇÃO GERAL

A LILYGO T-SIMCAM IR-CUT é uma plataforma de desenvolvimento baseada no ESP32-S3, projetada para aplicações de captura de imagens, monitoramento remoto e comunicação IoT.

O diferencial desta versão é a utilização de um conjunto óptico com filtro infravermelho IR-CUT.

A plataforma também possui suporte à expansão de comunicação celular através do módulo T-PCIE.

Principais funcionalidades:

Captura de imagens.
Processamento local.
Comunicação Wi-Fi.
Comunicação Bluetooth.
Armazenamento de imagens.
Comunicação celular mediante expansão compatível.
Operação com câmera IR-CUT.
Possibilidade de desenvolvimento de sistemas de monitoramento remoto.
3. ARQUITETURA DO SISTEMA

A arquitetura geral pode ser representada da seguinte maneira:

text
             LILYGO T-SIMCAM
                 ESP32-S3
                    |
        +-----------+-----------+
        |           |           |
      Câmera      Memória     Comunicação
      IR-CUT      microSD      Wi-Fi
        |           |           |
        +-----------+-----------+
                    |
                ESP32-S3
                    |
             Processamento
                    |
             Expansão T-PCIE
                    |
                Modem 4G
                    |
                 Internet
                    |
                 Servidor
                    |
              Aplicação web


A disponibilidade de cada recurso depende dos componentes efetivamente instalados e da revisão do hardware.

4. MICROCONTROLADOR ESP32-S3

O ESP32-S3 é o processador principal da plataforma.

Fabricante: Espressif Systems.

Características da família ESP32-S3:

Característica	Especificação
Arquitetura	Xtensa LX7
Núcleos	2
Frequência máxima	240 MHz
Wi-Fi	802.11 b/g/n
Bluetooth	Bluetooth 5 LE
Interfaces	SPI, I2C, UART, I2S
GPIO	Multiplexados
USB	USB integrado ao controlador

A configuração exata de Flash e PSRAM deve ser confirmada para a revisão da placa.

A família T-SIMCAM é comercializada com configurações de 16 MB Flash e 8 MB PSRAM.

Datasheet oficial

https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf

Manual técnico

https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf

5. SISTEMA DE CÂMERA

A embalagem identifica duas possibilidades de sensor:

OV2640.
OV5640.

A presença dos dois modelos na embalagem indica opções de configuração do produto, não necessariamente dois sensores instalados simultaneamente.

É necessário identificar qual sensor está presente no módulo físico.

5.1 OV2640

Fabricante: OmniVision.

Características:

Característica	Especificação
Tipo	CMOS
Resolução	2 megapixels
Resolução máxima	1600 × 1200
Interface	DVP
Configuração	SCCB
Formatos	JPEG, RGB565 e YUV
Aplicação	Captura de imagens

Datasheet:

https://www.uctronics.com/download/cam_module/OV2640DS.pdf

5.2 OV5640

Fabricante: OmniVision.

Características gerais:

Característica	Especificação
Tipo	CMOS
Resolução	5 megapixels
Resolução máxima	2592 × 1944
Interface	Dependente da configuração do módulo
Configuração	SCCB
Aplicação	Captura de imagens de maior resolução

A compatibilidade efetiva com a T-SIMCAM depende do módulo, do driver e do firmware utilizados.

Documentação técnica:

https://www.ovt.com/

6. SISTEMA IR-CUT

O principal diferencial desta versão é o sistema IR-CUT.

IR-CUT significa Infrared Cut Filter.

É um mecanismo óptico que utiliza um filtro móvel para controlar a passagem da radiação infravermelha.

6.1 Funcionamento diurno

Durante o dia:

O filtro infravermelho fica posicionado diante do sensor.
Reduz a interferência da radiação infravermelha.
Melhora a reprodução das cores.
Produz imagens mais naturais.
6.2 Funcionamento noturno

Durante a noite:

O filtro infravermelho é retirado do caminho óptico.
O sensor consegue aproveitar melhor a iluminação infravermelha.
A câmera pode trabalhar com iluminação IR externa.
6.3 Mecanismo físico

Nas fotografias da sua placa aparece um conjunto óptico com dois fios, vermelho e azul.

Esses fios são compatíveis visualmente com a ligação de um atuador do mecanismo IR-CUT.

A confirmação da função elétrica e da polaridade depende do esquema do módulo da câmera.

Não conectar diretamente esses fios a GPIOs do ESP32-S3 sem identificar o circuito de acionamento.

O mecanismo pode exigir um circuito driver específico.

6.4 Aplicações
Câmeras de segurança.
Monitoramento noturno.
Observação de animais.
Monitoramento rural.
Sistemas de vigilância.
Câmeras IoT externas.

Importante: o filtro IR-CUT não é uma fonte de iluminação infravermelha. Para imagens noturnas, pode ser necessário utilizar LEDs IR externos.

7. EXPANSÃO T-PCIE

A embalagem informa:

Support T-PCIE Module

Essa interface permite utilizar uma expansão de comunicação celular compatível.

Uma possibilidade é a utilização de módulos da família SIM7600.

7.1 Funções
Comunicação LTE/4G.
Envio de fotografias.
Comunicação TCP/IP.
Comunicação HTTP.
Comunicação MQTT.
Integração com servidores remotos.
7.2 Arquitetura de comunicação
text
CÂMERA
   |
   v
ESP32-S3
   |
   v
PROCESSAMENTO DA IMAGEM
   |
   v
MÓDULO T-PCIE
   |
   v
MODEM CELULAR
   |
   v
REDE 4G
   |
   v
INTERNET
   |
   v
SERVIDOR REMOTO

7.3 Cuidados

Nem todo módulo mPCIe é eletricamente compatível com a placa.

É necessário verificar:

Pinagem do conector.
Tensão de alimentação.
Consumo de corrente.
Comunicação UART.
Sinal de acionamento PWRKEY.
Compatibilidade do modem.
Compatibilidade das bandas LTE.

O modem pode ser adquirido separadamente.

8. INTERFACES E CONECTORES

Pelas fotografias, é possível identificar visualmente:

Componente	Função
USB Type-C	Alimentação e programação
Conector FPC	Conexão do módulo de câmera
Slot microSD	Armazenamento
Botão BOOT	Modo de programação
Botão RESET/RST	Reinicialização
Conector de expansão	Interface para acessórios
Conector de antena	Comunicação por radiofrequência, conforme circuito
Conector de alimentação	Alimentação de periféricos ou bateria, conforme identificação

A função de cada conector deve ser confirmada pelo esquema elétrico da revisão V1.3.

9. GPIO E PINAGEM

A pinagem é uma das partes mais importantes desta documentação.

A placa possui a identificação:

S3_CAM_SIM V1.3

A documentação geral da T-SIMCAM apresenta um mapa de pinos, mas ele não deve ser considerado automaticamente válido para todos os circuitos IR-CUT e revisões.

9.1 Pinagem correta da S3_CAM_SIM V1.3 (IR-CUT)

A tabela abaixo é a do exemplo de fábrica da LilyGO para a T-SIMCAM
(`examples/t_sim_cam_factory/config.h`). Ela é a referência válida para o PCB
S3_CAM_SIM V1.3 e é a usada por `firmware/windfollow/CameraService.cpp`.

Câmera DVP
Sinal	GPIO
HREF	7
VSYNC	6
PCLK	13
XCLK	14
D0	11
D1	9
D2	8
D3	10
D4	12
D5	17
D6	16
D7	15
SDA	4
SCL	5
PWDN	-1 (não usado)
RESET	-1 (não usado)
IR-CUT	18

Periféricos da placa
Sinal	GPIO
Cartão SD MISO	40
Cartão SD MOSI	38
Cartão SD SCLK	39
Cartão SD CS	47
PCIE PWR	48
PCIE TX	45
PCIE RX	46
LED do modem	21
Microfone	41 / 42 / 2
UART do modem	43 / 44
Power latch	1 (HIGH no boot)
Botão	0

ATENÇÃO: a tabela da wiki geral da T-SIMCam (HREF 26, VSYNC 25, PCLK 36,
D0-D7 39-48) NÃO vale para esta revisão. Esses GPIOs pertencem ao flash/PSRAM
(26-37) e à UART/PCIE do modem (39-48). Usá-los na câmera provoca panic
`LoadProhibited` no `esp_camera_init` e boot loop.

Documentação:

https://github.com/Xinyuan-LilyGO/LilyGo-Camera-Series/blob/master/examples/t_sim_cam_factory/config.h

https://wiki.lilygo.cc/products/t-sim-series/t-simcam/

9.2 Ligação padrão do L298N (modem 4G desligado)

Este projeto não usa o modem 4G. Com o GPIO48 (PCIE_PWR) mantido em LOW, os
pinos 45 e 46 (UART do modem) ficam livres para o L298N:

Função	GPIO	Ligação
IN1	45	Entrada de sentido 1 do L298N
IN2	46	Entrada de sentido 2 do L298N
ENA	21	PWM de velocidade (jumper ENA removido)

Cuidados:

- GPIO45 e GPIO46 são strapping pins. O L298N não pode puxá-los para HIGH
  durante o boot; as entradas do L298N são de alta impedância, então a ligação
  direta funciona, mas confira com um multímetro antes de energizar.
- O GPIO21 também aciona o LED do modem; como o modem fica desligado, ele é
  usado como PWM do ENA.
- GND do L298N, da fonte do motor e da placa devem ser comuns.
- Não alimente o motor pela placa; use fonte separada.

Esses valores são o padrão de fábrica do firmware e são gravados no NVS na
primeira inicialização (ou ao restaurar os defaults no painel).

10. ALIMENTAÇÃO ELÉTRICA

A documentação da família T-SIMCAM informa alimentação por USB-C e conector para bateria Li-Po de 3,7 V.

Alimentação	Utilização
USB-C	Alimentação e programação
Bateria Li-Po	Alimentação portátil
Alimentação do modem	Conforme módulo utilizado
Cuidados
Verificar a polaridade dos conectores.
Não aplicar 5 V diretamente nos GPIOs.
Não alimentar atuadores IR-CUT diretamente pelo GPIO.
Verificar a corrente necessária pelo modem.
Considerar os picos de consumo durante a transmissão celular.
Conferir os limites dos reguladores.
Utilizar bateria compatível com o circuito de carga.

Os limites de corrente dos reguladores e do circuito de carga da revisão V1.3 precisam ser confirmados pelo esquema elétrico.

11. PROGRAMAÇÃO

A família T-SIMCAM possui suporte para:

Arduino IDE.
PlatformIO.
Visual Studio Code.
ESP32 Arduino Core.
11.1 Configuração Arduino IDE

Configuração inicial indicada pela documentação oficial da T-SIMCam:

Parâmetro	Configuração
Board	ESP32S3 Dev Module
USB CDC On Boot	Enabled
CPU Frequency	240 MHz
Flash Size	16 MB
PSRAM	OPI PSRAM
Upload Speed	921600
Partition Scheme	Conforme Flash instalada
USB Mode	CDC and JTAG

Essas configurações devem ser verificadas com o firmware e a revisão física da placa.

11.2 Bibliotecas
Biblioteca	Aplicação
esp32-camera	Controle da câmera
TinyGSM	Comunicação com modem
PubSubClient	MQTT
WiFi	Comunicação Wi-Fi
SD / SD_MMC	Armazenamento
ESP32 I2S	Áudio digital
12. PROJETOS POSSÍVEIS
12.1 Câmera de monitoramento rural
Captura periódica de fotografias.
Armazenamento local.
Envio por rede celular.
Monitoramento remoto.
12.2 Câmera de segurança diurna e noturna
Alternância do filtro IR-CUT.
Captura de imagens.
Iluminação IR externa.
Transmissão de eventos.
12.3 Monitoramento de máquinas
Registro fotográfico.
Identificação visual de condições.
Comunicação com servidor.
Histórico de imagens.
12.4 Estação autônoma
Alimentação por bateria.
Possível integração solar.
Captura programada.
Comunicação celular.
13. ARQUITETURA DE SOFTWARE SUGERIDA
text
             INICIALIZAÇÃO
                   |
                   v
             CONFIGURAÇÃO
                   |
                   v
             INICIALIZA CÂMERA
                   |
                   v
             IDENTIFICA SENSOR
                   |
                   v
             CAPTURA IMAGEM
                   |
                   v
             PROCESSA JPEG
                   |
                   v
             ARMAZENA IMAGEM
                   |
                   v
             CONECTA À REDE
                   |
                   v
             TRANSMITE DADOS
                   |
                   v
             AGUARDA PRÓXIMO CICLO


Para aplicações noturnas, acrescentar uma rotina de controle do filtro IR-CUT, utilizando o circuito driver apropriado.

14. DOCUMENTAÇÃO OFICIAL E DATASHEETS
14.1 LILYGO T-SIMCAM

Wiki oficial:

https://wiki.lilygo.cc/products/t-sim-series/t-simcam/

Página do produto:

https://lilygo.cc/products/t-simcam

14.2 Repositório de hardware e software

https://github.com/Xinyuan-LILYGO/LilyGo-Camera-Series

14.3 Esquemas elétricos

https://github.com/Xinyuan-LILYGO/LilyGo-Camera-Series/tree/master/schematic/T_SIMCAM

A disponibilidade de um esquema específico para a variante IR-CUT V1.3 deve ser confirmada.

14.4 ESP32-S3

Datasheet:

https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf

Technical Reference Manual:

https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf

14.5 OV2640

https://www.uctronics.com/download/cam_module/OV2640DS.pdf

14.6 OV5640

https://www.ovt.com/

Consultar a documentação técnica do sensor OV5640.

15. IDENTIFICAÇÃO DO SENSOR INSTALADO

A embalagem informa compatibilidade com OV2640 e OV5640.

O sensor efetivamente instalado precisa ser identificado.

Uma possibilidade é consultar o identificador do sensor através da interface SCCB.

O driver de câmera ESP32 também pode auxiliar na identificação durante a inicialização.

A identificação deve ser realizada antes de selecionar o driver e os parâmetros de captura.

16. PONTOS PENDENTES DE ENGENHARIA

Os seguintes itens precisam ser confirmados para completar o esquema técnico específico da placa.

Item	Situação
Modelo ESP32-S3	Família identificada
Revisão PCB V1.3	Identificada visualmente
Data PCB	23/06/2022
Sensor OV2640 ou OV5640	A confirmar
Pinagem da câmera	A confirmar no esquema
GPIO do IR-CUT	A confirmar
Driver do IR-CUT	A identificar
Tensão do atuador IR-CUT	A medir/documentar
Regulador principal	A identificar
Circuito de carga	A identificar
Pinagem do T-PCIE	A confirmar
GPIOs da expansão celular	A confirmar
Corrente máxima de alimentação	A confirmar
Pinagem completa	A confirmar

Não utilizar valores estimados como especificações elétricas definitivas.

17. PROCEDIMENTO RECOMENDADO PARA ENGENHARIA REVERSA

Para documentar completamente a placa:

Fotografar a frente e o verso em alta resolução.
Identificar as marcações dos circuitos integrados.
Identificar o regulador de tensão.
Identificar o circuito de alimentação.
Identificar o sensor de câmera.
Localizar o driver do IR-CUT.
Conferir as trilhas do conector FPC.
Localizar os sinais de alimentação e comunicação.
Conferir o conector de expansão.
Comparar os componentes com o esquema elétrico.

Com isso será possível produzir um esquema elétrico específico da revisão S3_CAM_SIM V1.3.

18. CONCLUSÃO

A LILYGO T-SIMCAM IR-CUT é uma plataforma baseada no ESP32-S3 para desenvolvimento de câmeras inteligentes e sistemas IoT.

Sua característica mais importante é a combinação de câmera com filtro infravermelho removível e possibilidade de expansão para comunicação celular.

A revisão física identificada é:

S3_CAM_SIM V1.3 – 23/06/2022

A documentação deve ser mantida específica dessa revisão para evitar erros de pinagem, alimentação e acionamento do sistema IR-CUT.

HISTÓRICO DO DOCUMENTO
Campo	Informação
Documento	T-SIMCAM_IR-CUT_ESP32-S3
Formato	Markdown
Codificação	UTF-8
Fabricante	LILYGO
PCB	S3_CAM_SIM
Revisão	V1.3
Data do PCB	23/06/2022
Base documental	Fotografias físicas e documentação LILYGO
Status	Documentação técnica em consolidação


# Documentação Técnica --- Conjunto Motorredutor de Carro Elétrico Infantil

**Documento:** DOC-MR-001\
**Revisão:** 1.0\
**Status:** Identificação preliminar --- dados de placa ainda não
confirmados\
**Idioma:** Português (Brasil)

------------------------------------------------------------------------

## 1. Objetivo

Este documento registra a identificação preliminar, a arquitetura, o
funcionamento, os cuidados de instalação e os procedimentos básicos de
inspeção de um conjunto motorredutor utilizado em veículos elétricos
infantis.

A documentação foi elaborada a partir de uma fotografia do conjunto.
Portanto, ela **não substitui a identificação por etiqueta, medição
elétrica ou consulta ao fabricante**.

## 2. Identificação visual preliminar

O conjunto fotografado aparenta ser um **motorredutor de corrente
contínua (CC/DC)**, típico de sistemas de tração de carrinhos elétricos
infantis.

Elementos visualmente observáveis:

  -----------------------------------------------------------------------
  Elemento                Identificação           Observação
                          preliminar              
  ----------------------- ----------------------- -----------------------
  Motor                   Motor CC com escovas,   O tamanho/formato não
                          possivelmente da        permite confirmar
                          família RS              RS390, RS550 ou RS555

  Redutor                 Caixa de redução        Relação de redução não
                          acoplada ao motor       identificada

  Saída mecânica          Eixo/encaixe de saída   Geometria e dimensões
                          na extremidade da caixa precisam ser medidas

  Chicote                 Condutores elétricos    Polaridade e pinagem
                          saindo do conjunto      precisam ser
                                                  verificadas

  Módulo eletrônico ao    Possível                A função e o modelo não
  lado                    controlador/driver      podem ser confirmados
                                                  pela foto
  -----------------------------------------------------------------------

**Identificação provisória:** motorredutor DC para tração de veículo
elétrico infantil.\
**Modelo exato:** pendente de confirmação.

> Não considerar RS550, RS555 ou qualquer tensão nominal como
> especificação confirmada deste exemplar.

## 3. Princípio de funcionamento

O motor CC converte energia elétrica em movimento rotativo. A caixa
redutora transmite esse movimento à saída mecânica, reduzindo a rotação
e aumentando o torque disponível (desconsideradas as perdas mecânicas).

Em uma aplicação de tração, o controlador comanda a alimentação do
motor. A inversão da polaridade aplicada a um motor CC com escovas
normalmente inverte o sentido de rotação. O controlador do veículo pode
também implementar aceleração progressiva, frenagem elétrica, proteção
de corrente e seleção de marcha.

### 3.1 Diagrama funcional simplificado

``` text
Bateria
   |
   v
Controlador eletrônico
   |
   v
Motor CC
   |
   v
Caixa redutora
   |
   v
Eixo / acoplamento de saída
   |
   v
Roda ou mecanismo acionado
```

O diagrama é conceitual. A placa mostrada na fotografia ainda não foi
identificada e pode não ser o controlador principal do veículo.

## 4. Características técnicas

### 4.1 Dados conhecidos pela inspeção visual

  Parâmetro                       Situação
  ------------------------------- ---------------------------
  Categoria                       Motorredutor elétrico
  Tipo provável de alimentação    Corrente contínua (DC)
  Aplicação provável              Veículo elétrico infantil
  Redução mecânica                Presente
  Tensão nominal                  **Não identificada**
  Potência nominal                **Não identificada**
  Corrente nominal / de partida   **Não identificada**
  Rotação do motor                **Não identificada**
  Rotação de saída                **Não identificada**
  Torque de saída                 **Não identificado**
  Relação de redução              **Não identificada**
  Tipo e dimensões do eixo        **A medir**
  Modelo e fabricante             **Não identificados**

### 4.2 Famílias que podem aparecer nesse tipo de aplicação

Motores das famílias RS380, RS390, RS550 e RS555 são encontrados em
veículos infantis, mas a família não pode ser determinada com segurança
apenas pela fotografia.

Há conjuntos comerciais anunciados em versões de 6 V, 12 V e 24 V, com
diferentes rotações e configurações. Essas opções **não são
intercambiáveis automaticamente**.

Referências externas para comparação: - [Exemplo comercial de
motorredutores RS550/RS555 para carrinhos
infantis](https://www.electric-b2c.com/products/children-electric-car-gearbox-with-motor-dc-12v-6v-kids-ride-on-electric-car-motor-gear-box-baby-car-reducer-gearbox-550-and-390) -
[Catálogo técnico de motores DC da família
RS](https://www.mglorymotor.com/uploads/file/20250114/dexin-motor-catalog-2024-7-19.pdf) -
[Exemplo de manual de veículo infantil com motor
RS550](https://leantoys.pl/data/include/cms/instrukcje-eng/JH-101_manual_1.pdf)

Os valores encontrados em anúncios ou em manuais de outros veículos
servem somente como referência de mercado. Não devem ser atribuídos a
este conjunto sem confirmação.

## 5. Controlador eletrônico e ligação

O módulo eletrônico que aparece ao lado do motor não foi identificado.
Antes de conectá-lo, é necessário registrar:

-   fabricante e código da placa;
-   tensão de alimentação permitida;
-   corrente contínua e corrente de pico;
-   identificação dos terminais;
-   função de cada conector;
-   existência de proteção contra sobrecorrente e subtensão;
-   compatibilidade com o motor e com a bateria.

### 5.1 Cuidados com a ligação

1.  Desconectar a bateria antes de mexer na fiação.
2.  Não aplicar tensão por tentativa ou por semelhança com outro modelo.
3.  Não presumir a polaridade dos fios pelas cores.
4.  Confirmar a tensão nominal do motor e do controlador.
5.  Usar fusível e cabos dimensionados para a corrente do sistema.
6.  Evitar manter o motor travado ou energizado sem movimento.
7.  Fixar o veículo ou o conjunto de forma segura durante testes,
    mantendo rodas suspensas quando apropriado.

**Atenção:** um erro de tensão ou ligação pode danificar o motor, o
controlador, a bateria e a fiação.

## 6. Procedimento de inspeção e identificação

### 6.1 Inspeção visual

-   [ ] Fotografar todos os lados do motor e da caixa redutora.
-   [ ] Procurar etiqueta, gravação ou código no corpo do motor.
-   [ ] Registrar inscrições na placa eletrônica.
-   [ ] Verificar fios rompidos, terminais oxidados e sinais de
    aquecimento.
-   [ ] Verificar trincas, folgas e danos na carcaça.
-   [ ] Verificar se o eixo de saída apresenta desgaste ou deformação.

### 6.2 Medições mecânicas

Registrar, preferencialmente em milímetros:

  Medida                                         Valor
  ---------------------------------------------- ----------
  Comprimento total do conjunto                  Pendente
  Diâmetro e comprimento do corpo do motor       Pendente
  Largura, altura e comprimento da caixa         Pendente
  Diâmetro do eixo de saída                      Pendente
  Comprimento útil do eixo                       Pendente
  Tipo de encaixe da saída                       Pendente
  Distância entre furos de fixação               Pendente
  Quantidade de dentes do pinhão, se acessível   Pendente

### 6.3 Verificações elétricas

Devem ser feitas por pessoa capacitada, com instrumentos adequados:

-   medir a tensão da bateria;
-   identificar a tensão nominal indicada no motor ou no equipamento
    original;
-   medir a resistência entre os terminais do motor, interpretando o
    resultado com cautela;
-   verificar continuidade dos cabos e conectores;
-   medir corrente em teste controlado, com instrumento e proteção
    apropriados.

A resistência medida em um motor com escovas pode variar conforme a
posição do rotor. Uma leitura isolada não determina a tensão nominal nem
a potência do motor.

## 7. Manutenção preventiva

  -----------------------------------------------------------------------
  Item                    Verificação             Ação recomendada
  ----------------------- ----------------------- -----------------------
  Cabos e conectores      Folga, oxidação ou      Limpar ou substituir
                          aquecimento             com a alimentação
                                                  desconectada

  Carcaça do motor        Trincas e deformações   Substituir se houver
                                                  dano estrutural

  Caixa redutora          Ruído, folga ou         Inspecionar engrenagens
                          travamento              e lubrificação conforme
                                                  fabricante

  Eixo de saída           Desgaste e empenamento  Substituir peças
                                                  danificadas

  Fixações                Parafusos frouxos       Reapertar conforme
                                                  especificação

  Temperatura             Aquecimento excessivo   Interromper o uso e
                                                  investigar sobrecarga
  -----------------------------------------------------------------------

Não aplicar óleo ou graxa sem conhecer o material das engrenagens e a
recomendação do fabricante. Alguns redutores utilizam plásticos que
podem ser danificados por lubrificantes incompatíveis.

## 8. Diagnóstico básico de falhas

  -----------------------------------------------------------------------
  Sintoma                 Causas possíveis        Verificação inicial
  ----------------------- ----------------------- -----------------------
  Não gira                Bateria descarregada,   Conferir bateria,
                          cabo interrompido,      conectores e
                          controlador ou motor    continuidade
                          com defeito             

  Faz ruído, mas não      Engrenagem danificada   Inspecionar redutor e
  movimenta a saída       ou acoplamento solto    eixo

  Gira com pouca força    Bateria fraca, queda de Medir tensão sob carga
                          tensão, sobrecarga ou   e verificar transmissão
                          desgaste                

  Aquece excessivamente   Sobrecarga, travamento, Interromper o teste e
                          tensão incorreta ou     verificar carga e
                          ventilação insuficiente especificações

  Funciona em apenas um   Comando, controlador,   Conferir circuito de
  sentido                 conexão ou chave de     comando
                          reversão                

  Funciona de forma       Mau contato, fio        Inspecionar chicote e
  intermitente            rompido ou proteção do  conectores
                          controlador atuando     
  -----------------------------------------------------------------------

Não insistir em testes quando houver cheiro de queimado, fumaça, ruído
de engrenagem quebrada ou aquecimento anormal.

## 9. Critérios para substituição

Um motorredutor substituto deve ser compatível em todos os itens
relevantes:

-   tensão nominal;
-   corrente de operação e de pico;
-   potência e capacidade de torque;
-   rotação de saída;
-   relação de redução;
-   sentido de rotação e possibilidade de reversão;
-   dimensões externas e pontos de fixação;
-   diâmetro, comprimento e formato do eixo;
-   tipo de conector e compatibilidade com o controlador.

**Não selecionar uma peça apenas porque o anúncio informa RS550 ou
RS555.** Mesmo dentro da mesma família, existem diferenças de tensão,
rotação, pinhão e caixa redutora.

## 10. Dados pendentes para fechar a ficha técnica

  -----------------------------------------------------------------------
  Informação necessária               Como obter
  ----------------------------------- -----------------------------------
  Modelo do motor                     Foto nítida da etiqueta ou gravação

  Tensão nominal                      Etiqueta, manual ou identificação
                                      do veículo

  Potência e corrente                 Datasheet ou medição controlada

  Rotação do motor                    Datasheet ou tacômetro

  Rotação de saída                    Tacômetro em teste seguro

  Relação de redução                  Dados do fabricante ou comparação
                                      entre rotações

  Torque                              Datasheet ou ensaio apropriado

  Pinagem do controlador              Código da placa e rastreamento dos
                                      terminais

  Dimensões                           Paquímetro e registro fotográfico
  -----------------------------------------------------------------------

## 11. Registro de revisão

  Revisão   Descrição                                 Status
  --------- ----------------------------------------- ------------
  1.0       Documento inicial baseado na fotografia   Preliminar

------------------------------------------------------------------------

## 12. Conclusão

O item fotografado é compatível visualmente com um conjunto motorredutor
DC utilizado em veículos elétricos infantis. A presença de motor, caixa
de redução, saída mecânica e chicote é consistente com esse tipo de
aplicação.

Entretanto, **modelo, tensão, potência, corrente, rotação, torque e
pinagem permanecem pendentes de confirmação**. A identificação
definitiva exige fotografias adicionais, leitura das inscrições e
medições.

**Próxima etapa recomendada:** fotografar a etiqueta/gravação do motor,
a parte traseira, os conectores e os dois lados da caixa redutora. Com
esses dados, esta documentação pode ser atualizada para uma ficha
técnica específica do equipamento.


Fim da documentação.