# Projeto SmartCam Seguidora por Movimento

**Especificação funcional e técnica — versão consolidada**

- **Plataforma principal:** LILYGO T-SIMCAM IR-CUT com ESP32-S3
- **Firmware-base:** SmartCam OS — `https://github.com/devtiagoabreu/smartcam-os`
- **Acionamento:** driver L298N + um motorredutor CC
- **Movimento inicial:** horizontal (PAN)
- **Estratégia de visão inicial:** detecção de movimento, sem reconhecimento obrigatório de pessoas

> **Status:** proposta de arquitetura e plano de implementação. A câmera e a conectividade Wi-Fi já foram testadas pelo usuário com o SmartCam OS. O acionamento do motor ainda não funcionou e a ligação/pinagem do L298N precisa ser confirmada. Este documento não afirma que a integração do motor já esteja implementada ou validada.

---

## 1. Visão geral

O objetivo é transformar a câmera baseada na T-SIMCAM em uma câmera que acompanha movimentos no campo de visão, girando horizontalmente para manter a região de movimento próxima ao centro da imagem.

A decisão principal é **aproveitar o SmartCam OS como plataforma de software**, em vez de iniciar um firmware independente. Devem ser preservados, sempre que possível, os recursos já testados:

- Interface web.
- Configuração e conexão Wi-Fi.
- Conectividade e recursos de rede existentes.
- Captura e visualização/streaming da câmera.
- Estrutura de configuração e inicialização do SmartCam OS.

O desenvolvimento será concentrado em dois pontos:

1. Implementar ou adaptar um módulo leve de detecção de movimento.
2. Implementar e validar o controle do motorredutor por meio do L298N.

A detecção de pessoas, rostos ou objetos por inteligência artificial não será requisito da primeira versão.

## 2. O que já foi testado e o que falta

| Item | Situação informada | Próxima ação |
|---|---|---|
| SmartCam OS na T-SIMCAM | Firmware foi carregado e executado | Preservar a base existente |
| Wi-Fi / internet | Funcionou | Evitar alterações desnecessárias |
| Câmera | Funcionou | Preservar captura e streaming |
| Tentativa de detecção de pessoas | O firmware tentava detectar pessoas | Avaliar como desativar/substituir no modo inicial |
| Motorredutor | Não funcionou com o firmware testado | Diagnosticar hardware, GPIO e software |
| Driver L298N | É o driver informado pelo usuário | Confirmar ligações, alimentação e pinos |
| Movimento horizontal | É o objetivo inicial | Implementar PAN em um eixo |

**Importante:** ainda não foi confirmada a pinagem usada na ligação entre a T-SIMCAM e o L298N. Também não foram confirmados tensão, corrente de partida, rotação e torque do motorredutor.

## 3. Escopo da primeira versão (MVP)

A primeira versão deve ser simples, estável e fácil de diagnosticar.

### Incluído

- Manter SmartCam OS como firmware-base.
- Manter Wi-Fi, interface web e visualização da câmera.
- Criar um modo de rastreamento por movimento.
- Detectar regiões que mudam na imagem.
- Estimar o centro da região de movimento escolhida.
- Comandar o motor para girar à esquerda ou à direita.
- Oferecer controle manual do motor pela interface web.
- Permitir ativar e desativar o rastreamento automático.
- Expor parâmetros básicos de sensibilidade e velocidade.
- Incluir limites de segurança para o acionamento.

### Fora do escopo inicial

- Reconhecimento de identidade.
- Rastreamento de uma pessoa específica.
- Classificação de pessoa, animal, veículo ou objeto.
- Rastreamento vertical (TILT).
- Rastreamento simultâneo de vários alvos.
- Uso obrigatório de serviços em nuvem ou processamento remoto.
- Alteração ampla da interface e dos serviços de conectividade que já funcionam.

Esses recursos poderão ser avaliados depois que o controle horizontal estiver estável.

## 4. Arquitetura de software desejada

```text
                 SMARTCAM OS
        (base existente a preservar)
                       |
        +--------------+----------------+
        |              |                |
     Interface       Wi-Fi          Câmera / stream
        |              |                |
        +--------------+----------------+
                       |
                Configurações
                       |
              Módulo de rastreamento
                       |
          Detecção de movimento na imagem
                       |
         Filtro / região / centro do movimento
                       |
          Erro horizontal em relação ao centro
                       |
              Controlador PAN
                       |
             Camada de motor (driver)
                       |
                 GPIOs ESP32-S3
                       |
                    L298N
                       |
                Motorredutor CC
```

### Princípio de integração

O módulo de rastreamento deve ser separado da interface, da conectividade e do driver do motor. Dessa forma, será possível substituir o algoritmo de visão sem reescrever a interface web ou o Wi-Fi.

O módulo de motor também deve ser independente: a interface e o rastreador enviam comandos abstratos, como `esquerda`, `direita`, `parar` e `velocidade`; uma camada de hardware converte esses comandos em sinais elétricos do L298N.

Os nomes acima descrevem a arquitetura desejada, não nomes de funções já existentes no SmartCam OS. A implementação deve seguir a organização real do repositório depois de sua inspeção.

## 5. Estratégia de visão: detectar movimento, não pessoas

### 5.1 Por que começar por movimento

A detecção de pessoas normalmente depende de um modelo de visão computacional e pode exigir mais memória e processamento. Já a detecção básica de movimento pode ser feita com operações de imagem mais simples.

O sistema não precisa saber o que está se movendo para executar o primeiro teste. Precisa apenas encontrar uma região de mudança suficientemente grande e calcular sua posição horizontal.

A contrapartida é que qualquer movimento pode ser interpretado como alvo: pessoas, animais, veículos, folhas, sombras, reflexos ou objetos.

### 5.2 Algoritmo inicial sugerido

1. Capturar um quadro da câmera.
2. Reduzir a imagem para uma resolução de análise menor, se necessário.
3. Converter para tons de cinza, se isso for compatível com o fluxo de imagem disponível.
4. Comparar o quadro atual com o anterior ou com um modelo simples de fundo.
5. Aplicar limiar para reduzir ruído de sensor e pequenas variações.
6. Agrupar pixels alterados em regiões (componentes/contornos).
7. Descartar regiões abaixo de uma área mínima.
8. Escolher uma região prioritária — inicialmente, a maior região válida.
9. Calcular seu centro horizontal.
10. Comparar o centro detectado com o centro da imagem e produzir um erro horizontal.

Exemplo conceitual:

```text
erro_x = centro_da_regiao - centro_da_imagem

se abs(erro_x) <= zona_morta:
    parar_motor
senão se erro_x < 0:
    girar_para_esquerda
senão:
    girar_para_direita
```

O sentido físico pode ser invertido dependendo da montagem. Deve ser calibrado no teste manual.

### 5.3 Limitações

- Não identifica se o movimento pertence a uma pessoa.
- Pode reagir a alterações de iluminação, sombras, reflexos e ruído.
- Um alvo parado deixa de gerar diferença entre quadros e pode deixar de ser acompanhado.
- Com vários movimentos, é necessário escolher uma região prioritária.
- Se o alvo sair completamente da imagem, o sistema pode perdê-lo.

### 5.4 Problema especial: a câmera se movimenta

Quando a própria câmera gira, o cenário inteiro se desloca na imagem. Uma comparação simples entre quadros pode interpretar esse deslocamento global como movimento em toda a cena.

Para o MVP, usar uma estratégia de controle por pequenos passos:

1. Detectar movimento enquanto a câmera está parada.
2. Calcular a direção e comandar um pulso curto no motor.
3. Parar o motor.
4. Aguardar a estrutura estabilizar.
5. Descartar alguns quadros afetados por vibração e movimento global.
6. Capturar novamente e recalcular a posição do alvo.
7. Repetir até o alvo entrar na zona morta.

Se os testes mostrarem que o deslocamento do cenário gera falsos movimentos persistentes, uma evolução será estimar e compensar o movimento global da câmera. Essa etapa não deve ser adicionada antes de validar o método por pequenos passos.

### 5.5 Parâmetros ajustáveis

A interface web deverá permitir ajustar, no mínimo:

| Parâmetro | Função |
|---|---|
| Rastreamento automático | Liga/desliga o acompanhamento |
| Sensibilidade | Ajusta o limiar de mudança detectável |
| Área mínima | Ignora pequenas regiões e ruído |
| Zona morta | Evita correções quando o alvo está perto do centro |
| Duração do passo | Tempo de acionamento em cada correção |
| Intervalo de estabilização | Tempo de espera após o motor parar |
| Velocidade do motor | PWM, se suportado pela ligação e pelo driver |
| Região de interesse | Opcional; limita a área analisada |

Os valores padrão devem ser conservadores. Não definir números definitivos antes dos testes com a câmera e o conjunto mecânico.

## 6. Controle mecânico: um eixo horizontal (PAN)

O primeiro protótipo terá apenas um motorredutor e movimentação horizontal.

- Motor para um sentido: giro para a esquerda.
- Motor para o sentido oposto: giro para a direita.
- Motor parado: manter a posição atual.

Um único motor não permite inclinação vertical. Para PAN + TILT será necessário um segundo atuador e uma estrutura de dois eixos.

### 6.1 Sem sensor de posição

Até o momento não foi informado que o motor tenha encoder ou sensor de posição. Sem esse sensor, o firmware não conhece diretamente o ângulo do conjunto.

O MVP pode usar pulsos temporizados e realimentação visual: gira um pouco, para, captura novamente e corrige. Isso é menos preciso e pode sofrer com folga mecânica, inércia e variação de carga.

Recomenda-se prever batentes mecânicos e, idealmente, fins de curso. O firmware também deve impor um tempo máximo de acionamento por comando para evitar giro contínuo acidental.

## 7. Eletrônica: LILYGO T-SIMCAM, L298N e motorredutor

### 7.1 LILYGO T-SIMCAM IR-CUT ESP32-S3

A documentação oficial consultada para a família T-SIMCam descreve ESP32-S3 dual-core LX7, Wi-Fi/Bluetooth, câmera OV2640, 16 MB de Flash e 8 MB de PSRAM. A placa também prevê modem celular opcional via mPCIe. Essas especificações devem ser conferidas contra a revisão física da unidade utilizada. [Documentação LilyGO](https://github.com/Xinyuan-LILYGO/documentation/blob/master/en/products/t-sim-series/t-simcam/index.md) | [Página do produto](https://lilygo.cc/products/t-simcam).

A placa já executou o SmartCam OS, conectou-se à internet e operou a câmera no teste informado.

Antes de implementar o acionamento:

- Identificar a revisão exata da placa.
- Identificar o sensor de câmera realmente instalado (a documentação da família menciona variantes como OV2640/OV5640; isso não confirma qual está na unidade).
- Conferir o esquema/pinout específico da revisão.
- Identificar GPIOs realmente disponíveis, sem conflito com câmera, modem, microSD, USB, IR-CUT ou outros periféricos.
- Não copiar pinagem genérica de outra placa ESP32 ou de outra revisão da T-SIMCAM.

Como referência, a documentação da T-SIMCam lista GPIOs 4, 5, 8, 9, 25, 26, 36, 39–48 para funções da câmera e GPIOs 17, 18 e 10 para UART/controle do modem. Portanto, vários pinos comuns de exemplos ESP32 podem já estar ocupados. Essa lista é uma referência para a revisão documentada, não uma autorização para escolher qualquer GPIO restante sem conferir o esquema elétrico da placa. [Pinout oficial](https://github.com/Xinyuan-LILYGO/documentation/blob/master/en/products/t-sim-series/t-simcam/index.md#pin-diagram).

### 7.2 Driver L298N

O L298N é uma ponte H dupla. Para um motor CC, um canal pode controlar o sentido de rotação e, com PWM no pino de habilitação, a velocidade.

Referência funcional típica de um canal:

| L298N | Função |
|---|---|
| OUT1 / OUT2 | Terminais do motor |
| IN1 / IN2 | Define o sentido |
| ENA | Habilita o canal A; pode receber PWM |
| GND | Referência comum |
| Alimentação do motor | Alimenta o estágio de potência conforme o módulo |

A pinagem acima descreve o funcionamento usual do módulo, não os GPIOs que devem ser usados na T-SIMCAM. Os GPIOs só podem ser definidos depois de conferir o esquema da placa e o código do SmartCam OS.

Cuidados:

- O material técnico fornecido pelo usuário informa limite nominal de até 2 A por canal para o CI L298; isso não garante que qualquer módulo suporte 2 A continuamente. Dissipação e montagem influenciam.
- O L298N tem queda de tensão e pode aquecer.
- Medir ou obter a tensão nominal e a corrente do motor.
- Considerar a corrente de partida/bloqueio, que pode ser muito maior que a corrente em vazio.
- Usar fonte separada e adequada para o motor, conforme a tensão/corrente exigidas.
- Não alimentar o motor por GPIO ou pela alimentação de 3,3 V da ESP32.
- Unir o GND da fonte do motor, do L298N e da placa de controle.
- Verificar jumpers ENA/ENB e a configuração do módulo antes de energizar.
- Não ligar ou desligar fios do motor com o circuito energizado.

**Não energizar o conjunto com base apenas nesta tabela.** A ligação final depende do módulo específico, da fonte e dos dados do motor.

### 7.3 Motorredutor

O motor disponível foi descrito anteriormente como um provável motor CC com caixa redutora, mas não há identificação confirmada de modelo, tensão, corrente, rotação ou torque.

Antes do teste:

- Ler etiqueta ou identificar o motor.
- Medir tensão e corrente em condições controladas, se não houver documentação.
- Verificar corrente de partida/bloqueio com método e instrumento adequados.
- Verificar se o torque suporta câmera, suporte e cabos.
- Verificar se a velocidade permite correções suaves.
- Confirmar que o L298N é apropriado; se não for, escolher outro driver.

## 8. Diagnóstico: por que o motor não funcionou

A falha ainda não foi diagnosticada. Não é possível concluir se é software, pinagem, alimentação, driver ou motor sem testes.

Seguir esta ordem:

### Etapa A — Inspeção sem energizar

1. Fotografar a ligação atual.
2. Identificar o modelo/revisão do módulo L298N.
3. Identificar os terminais do motor e da fonte.
4. Anotar todos os fios entre ESP32-S3 e L298N.
5. Conferir a tensão nominal do motor e da fonte.
6. Confirmar se o GND é comum.

### Etapa B — Teste independente do motor e driver

1. Desconectar o controle automático da câmera.
2. Confirmar que a fonte é compatível com o motor e o driver.
3. Testar o motor e o L298N com um programa mínimo ou teste manual controlado.
4. Verificar se o canal habilita e se IN1/IN2 mudam o sentido.
5. Testar PWM apenas depois de confirmar o giro básico.
6. Monitorar aquecimento e corrente.

### Etapa C — Teste dos GPIOs

1. Conferir o pinout da revisão exata da T-SIMCAM.
2. Conferir no código do SmartCam OS quais GPIOs já são usados.
3. Selecionar GPIOs livres e apropriados.
4. Testar cada saída com medição elétrica antes de conectar o driver.
5. Só então conectar ao L298N.

### Etapa D — Integração com SmartCam OS

1. Criar um teste de motor independente da detecção de movimento.
2. Disponibilizar comandos manuais de esquerda, direita e parar.
3. Confirmar que o motor responde.
4. Adicionar limites de tempo e parada segura.
5. Só depois ligar o comando automático vindo do rastreador.

## 9. Integração com o SmartCam OS

### 9.1 Princípio: preservar o que já funciona

O repositório `devtiagoabreu/smartcam-os` deve ser a base do desenvolvimento. A prioridade é preservar a interface, conectividade Wi-Fi, streaming e estrutura de configuração que já foram testados pelo usuário.

Não se deve criar uma interface paralela nem substituir serviços funcionais sem necessidade.

### 9.2 Separação em módulos

A implementação desejada deve manter responsabilidades separadas:

- **Interface/configuração:** controles web, parâmetros e estado do sistema.
- **Conectividade:** Wi-Fi, rede e serviços existentes.
- **Câmera:** captura e streaming já funcionais.
- **Visão:** detecção de movimento e seleção da região.
- **Rastreamento:** cálculo do erro horizontal e lógica de correção.
- **Motor:** abstração de esquerda/direita/parar/velocidade.
- **Segurança:** limites de tempo, parada e limites mecânicos.

Os nomes e caminhos concretos dos arquivos devem ser definidos após examinar o conteúdo real do repositório. Não presumir que já existam funções ou arquivos com os nomes desta especificação.

### 9.3 Comportamento esperado da interface

Adicionar uma área ou seção de controle de movimento, mantendo o estilo e a navegação do SmartCam OS.

Controles sugeridos:

- **Modo:** Desligado / Manual / Automático.
- **Esquerda:** acionamento manual enquanto pressionado ou por pulso limitado.
- **Parar:** interrompe imediatamente o motor.
- **Direita:** acionamento manual enquanto pressionado ou por pulso limitado.
- **Sensibilidade:** ajuste do detector.
- **Área mínima:** filtro de ruído.
- **Zona morta:** tolerância central.
- **Velocidade:** PWM, se aplicável.
- **Passo de giro:** duração máxima do pulso.
- **Estabilização:** espera após cada movimento.
- **Status:** câmera, Wi-Fi, modo de rastreamento e estado do motor.

A interface não deve indicar que o motor está funcionando apenas porque um comando foi enviado. Sempre que possível, deve mostrar o comando/estado solicitado e distinguir isso de uma confirmação física, que exigiria sensor de posição ou feedback elétrico.

## 10. Estratégia de implementação em fases

| Fase | Entrega | Critério de conclusão |
|---|---|---|
| 0. Levantamento | Revisão do repositório, placa, driver, motor e ligações | Pinagem e estrutura do código identificadas |
| 1. Preservação | SmartCam OS original continua inicializando | Wi-Fi, interface e câmera continuam funcionais |
| 2. Driver manual | Teste isolado de motor pelo L298N | Esquerda, direita e parar funcionam |
| 3. Interface manual | Controles manuais na interface existente | Usuário comanda o motor pela web |
| 4. Detector offline | Detecção de movimento sem acionar motor | Região e centro do movimento são calculados |
| 5. Rastreamento PAN | Integração do detector com motor por passos | Câmera acompanha um movimento simples |
| 6. Ajustes | Sensibilidade, zona morta, estabilização e limites | Movimento sem oscilações excessivas |
| 7. Robustez | Testes com luz variável e múltiplos movimentos | Falsos acionamentos dentro do aceitável |

**Regra de desenvolvimento:** concluir e testar cada fase antes de avançar. Se uma mudança quebrar Wi-Fi, interface ou câmera, reverter essa mudança antes de prosseguir.

## 11. Repositórios e documentação de referência

### Observação sobre a inspeção do SmartCam OS

O usuário informou que gravou o SmartCam OS na T-SIMCAM e validou Wi-Fi/internet e câmera. Esse relato é a evidência disponível sobre o funcionamento atual. Nesta etapa, o conteúdo interno do repositório `devtiagoabreu/smartcam-os` não pôde ser inspecionado diretamente; portanto, este documento não atribui ao código nomes de arquivos, funções, GPIOs ou protocolos que ainda não foram confirmados. Para implementar com precisão, será necessário analisar o ZIP/clone da versão efetivamente gravada ou obter acesso ao repositório.


| Referência | Link | Como usar |
|---|---|---|
| SmartCam OS | <https://github.com/devtiagoabreu/smartcam-os> | Firmware-base a preservar; precisa ser inspecionado para identificar estrutura, recursos, pinos e controle de motor reais. |
| LilyGo Camera Series | <https://github.com/Xinyuan-LILYGO/LilyGo-Camera-Series> | Referência da família de câmeras LilyGo. |
| Wiki T-SIMCAM | <https://wiki.lilygo.cc/products/t-sim-series/t-simcam/> | Identificação da placa, conexões e detalhes específicos. |
| ESP-WHO | <https://github.com/espressif/esp-who> | Referência opcional para futura evolução de visão computacional; não é requisito para o MVP de movimento. |
| Exemplo de câmera com detecção de movimento | <https://github.com/W5TSU/rc-ravens-LilyGo-T-SimCam> | Projeto comunitário para T-SIMCam que usa movimento como critério para captura/envio de imagens; não é um rastreador PAN pronto, mas pode servir como referência de detecção de movimento na placa. |
| ESP32-CAM Pan Tilt | <https://github.com/ErroujiOussama/ESP32_CAM_pan_tilt> | Referência de arquitetura de câmera móvel; utiliza servos e hardware diferente. |
| Edge AI Object Tracking Camera | <https://github.com/mmm1712/edge-ai-object-tracking-camera> | Referência de lógica de centralização/rastreamento, com hardware diferente. |
| ESP-WHO object tracking | <https://github.com/espressif/esp-who/tree/master/examples/object_tracking> | Referência de rastreamento mais avançado; o exemplo citado usa plataforma/hardware diferente e não deve ser tratado como firmware pronto para a T-SIMCAM. |

**Nota sobre o estado da pesquisa:** os projetos acima são referências para estudo. A compatibilidade de cada exemplo com a revisão específica da T-SIMCAM deve ser confirmada antes de reutilizar código. A documentação oficial consultada lista a pinagem da câmera e do modem; não foi possível concluir, apenas com essa tabela, quais GPIOs adicionais estão livres na unidade física.

Uma referência adicional de arquitetura de controle de motor em ESP32-S3 é [ESP32-S3 Motor Controller](https://github.com/LarsEsDoch/esp32-motor-controller). Ela demonstra controle PWM, painel web e parada de emergência, mas foi construída para outra placa e outro circuito de potência; seus GPIOs e ligações não devem ser copiados para a T-SIMCAM/L298N.

## 12. Dados pendentes para fechar o projeto

- [ ] ZIP ou acesso ao conteúdo atual do repositório SmartCam OS, incluindo a versão/commit que foi gravada na placa.
- [ ] Foto nítida da T-SIMCAM e identificação da revisão.
- [ ] Foto da ligação entre T-SIMCAM, L298N, fonte e motor.
- [ ] Foto/identificação do módulo L298N.
- [ ] Tensão nominal do motorredutor.
- [ ] Corrente nominal e corrente de partida/bloqueio do motor.
- [ ] Tensão e corrente disponíveis na fonte do motor.
- [ ] Confirmação dos GPIOs usados na ligação.
- [ ] Limites mecânicos de giro e existência de fins de curso.

## 13. Critérios de aceitação do MVP

O projeto será considerado funcional quando:

1. O SmartCam OS continuar iniciando normalmente.
2. A interface web continuar acessível.
3. O Wi-Fi e os recursos de rede existentes continuarem funcionando.
4. A câmera continuar capturando e transmitindo imagens.
5. O usuário conseguir comandar manualmente esquerda, direita e parar pela interface.
6. O motor não ultrapassar os limites de acionamento configurados.
7. O modo automático detectar movimento sem executar inferência de pessoa.
8. O motor corrigir a direção em pequenos passos e parar quando a região estiver dentro da zona morta.
9. A câmera não entrar em oscilação contínua em um teste simples com um alvo em movimento.

## 14. Conclusão e decisão técnica

A direção definida é **reaproveitar o SmartCam OS e transformá-lo em uma câmera seguidora por movimento**.

O firmware existente será responsável por interface, conectividade e câmera. O desenvolvimento novo ficará concentrado na detecção leve de movimento, na lógica de rastreamento horizontal e no acionamento do L298N.

A prioridade imediata não é escrever o algoritmo de rastreamento. É descobrir por que o motor não funcionou, identificando a ligação elétrica, a alimentação, os GPIOs e o código de controle existente. Com o motor funcionando em modo manual, a integração automática fica muito mais simples de testar e depurar.

**Não assumir pinagem, tensão ou corrente.** Esses dados precisam ser confirmados na placa, no driver e no motor reais antes de energizar o circuito.

---

*Documento consolidado a partir das informações e testes relatados pelo usuário e das referências técnicas listadas. Pontos ainda não verificados no código-fonte ou no hardware estão explicitamente marcados como pendentes.*
