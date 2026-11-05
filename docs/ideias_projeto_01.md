# Projeto: Câmera Seguidora por Movimento

**Estudo de viabilidade e proposta inicial de arquitetura**

> **Objetivo:** desenvolver uma câmera que perceba movimento na imagem e gire mecanicamente para acompanhar a região onde o movimento acontece, aproveitando, sempre que possível, a placa LILYGO T-SIMCAM ESP32-S3, o driver L298N e o motorredutor já disponíveis.

## 1. Recomendação inicial

**Começar com detecção de movimento, e não com reconhecimento de pessoas.**

É uma primeira versão mais simples e tende a exigir bem menos processamento: o sistema compara quadros consecutivos ou compara a imagem atual com um fundo aprendido, identifica as regiões que mudaram e calcula onde está concentrada a mudança.

Isso não significa que a câmera saberá que o alvo é uma pessoa. Ela poderá reagir a qualquer mudança visual relevante: uma pessoa, um animal, um veículo, uma porta abrindo ou um objeto passando. Para o primeiro protótipo, essa limitação é aceitável e permite validar câmera, controle do motor e mecânica antes de adicionar inteligência mais pesada.

## 2. Comparação das abordagens

| Abordagem | Processamento | O que identifica | Adequação à primeira versão |
|---|---:|---|---|
| Diferença entre quadros | Baixo | Pixels que mudaram entre duas imagens | Muito boa para validar o conceito |
| Movimento contra fundo | Baixo a médio | Regiões que se afastam do cenário aprendido | Boa, mas exige ajuste de sensibilidade |
| Fluxo óptico | Médio a alto | Direção e intensidade do movimento dos pixels | Pode ser uma evolução, se necessário |
| Detecção de pessoa | Alto | Pessoa, conforme o modelo utilizado | Deixar para uma segunda etapa |
| Detecção de rosto | Médio a alto | Rosto visível | Não resolve pessoas de costas ou corpo inteiro |

## 3. Como funcionaria a detecção de movimento

Fluxo simplificado do processamento:

1. Capturar um quadro da câmera.
2. Reduzir a resolução de análise e, se possível, trabalhar em tons de cinza.
3. Comparar o quadro atual com o quadro anterior ou com uma imagem de fundo.
4. Aplicar um limiar para ignorar pequenas variações causadas por ruído do sensor.
5. Agrupar pixels alterados em regiões e descartar regiões muito pequenas.
6. Calcular o centro da região de movimento mais relevante.
7. Comparar esse centro com o centro da imagem.
8. Comandar o motor para reduzir o erro e trazer a região detectada para o centro.

Uma **zona morta** (*dead zone*) ao redor do centro evita que o motor fique corrigindo continuamente por pequenas oscilações. Também é recomendável exigir movimento por alguns quadros consecutivos antes de iniciar o acompanhamento.

## 4. Atenção: a câmera também vai se mover

**Este é o principal desafio técnico da solução baseada apenas em movimento.**

Quando a câmera está parada, a comparação entre quadros costuma destacar o objeto que se move. Mas, quando o motor gira a câmera, praticamente todo o cenário muda de posição na imagem. Um detector simples pode interpretar esse deslocamento global como movimento em toda a cena.

Para reduzir esse problema, a primeira versão deve usar uma estratégia conservadora:

- Mover em pequenos passos, em vez de girar continuamente.
- Após cada passo, aguardar a câmera estabilizar e descartar alguns quadros.
- Só então analisar novamente a imagem e corrigir a posição.
- Usar uma região de interesse e filtrar alterações espalhadas por quase toda a imagem.
- Ajustar a sensibilidade para ignorar ruído, mudanças leves de iluminação e vibração.

Se isso não for suficiente, uma evolução possível é estimar o movimento global da cena (**compensação de movimento da câmera**) e separar esse movimento do movimento independente do objeto. Essa etapa é mais complexa e deve ser considerada somente depois dos testes iniciais.

## 5. Arquitetura proposta

```text
CÂMERA (LILYGO T-SIMCAM / ESP32-S3)
             |
             v
Captura de quadros em baixa resolução
             |
             v
Detecção de movimento (diferença entre quadros / fundo)
             |
             v
Filtro de ruído + região de movimento + centroide
             |
             v
Erro horizontal em relação ao centro da imagem
             |
             v
Controle proporcional com zona morta e limite de velocidade
             |
             v
L298N (ponte H) -> motorredutor -> movimento PAN
             |
             +----> Nova captura após estabilização
```

A arquitetura inicial recomendada é de um único eixo horizontal (**PAN**). Com um motor, o sistema consegue girar para a esquerda e para a direita. Para inclinação vertical (**TILT**), será necessário um segundo atuador e uma estrutura mecânica de dois eixos.

## 6. Componentes e compatibilidade

### 6.1 LILYGO T-SIMCAM IR-CUT ESP32-S3

- Pode ser a unidade de captura e processamento da primeira versão.
- A documentação fornecida indica a família ESP32-S3 e suporte a sensores de câmera como OV2640/OV5640; é necessário confirmar qual sensor está instalado na placa.
- A pinagem exata da revisão da placa deve ser conferida antes de escolher GPIOs para o L298N. Não assumir que tabelas genéricas de outras revisões se aplicam.
- Começar com resolução reduzida e taxa de quadros moderada para poupar memória e processamento.
- A conexão celular não é necessária para o rastreamento local básico; pode ser considerada depois para acesso remoto.

### 6.2 Driver L298N

- Controla o sentido de rotação e permite variar a velocidade por PWM.
- O material fornecido informa limite nominal de até **2 A por canal** para o CI L298; o limite real do módulo depende da placa, dissipação e condições de uso.
- O L298N apresenta queda de tensão e pode aquecer. Verificar se a tensão que chega ao motor é suficiente.
- Confirmar corrente nominal e, principalmente, corrente de partida/bloqueio do motor antes de ligar.
- A alimentação do motor deve vir de uma fonte apropriada. Não alimentar o motor pelas GPIOs ou pela saída de alimentação da placa.
- Unir o GND da fonte do motor, do driver e da placa de controle (referência comum), respeitando o esquema elétrico.

> **Atenção:** não conectar o motor ao L298N até medir ou obter sua tensão e corrente. Se a corrente de partida exceder a capacidade do driver, será necessário outro driver.

### 6.3 Motorredutor

- O motor disponível parece ser um motor CC com caixa de redução, mas modelo, tensão, corrente, rotação e torque ainda não estão confirmados.
- Verificar se a rotação é suficientemente lenta para apontar sem ultrapassar o alvo.
- Verificar torque para movimentar câmera, suporte e cabos sem esforço excessivo.
- Prever batentes ou fins de curso para impedir que a estrutura gire além do limite mecânico.
- Sem encoder ou sensor de posição, o sistema não conhece diretamente o ângulo do motor. O controle será feito por realimentação visual e pequenos acionamentos temporizados.
- Folga mecânica e inércia podem causar oscilação; começar com movimentos curtos e velocidade baixa.

## 7. Limitações da detecção simples

- Não distingue pessoa de outros objetos em movimento.
- Pode reagir a sombras, reflexos, mudanças bruscas de iluminação, chuva, folhas e ruído da imagem.
- Um objeto parado deixa de gerar movimento e pode deixar de ser acompanhado.
- Se várias coisas se moverem ao mesmo tempo, será necessário escolher uma região prioritária (por exemplo, a maior região ou a mais próxima do centro).
- Se o alvo sair completamente do campo de visão, o sistema não saberá automaticamente para que lado procurar sem uma estratégia adicional.

Uma melhoria simples, ainda sem IA, é manter temporariamente a última direção e posição do alvo, procurar novamente por alguns segundos e só depois voltar a uma posição de espera.

## 8. Plano de desenvolvimento por etapas

1. **Etapa 1 — Validar a câmera:** capturar e exibir imagens estáveis; confirmar sensor, resolução, taxa de quadros e memória disponível.
2. **Etapa 2 — Detectar movimento sem motor:** executar diferença entre quadros; mostrar ou registrar a região detectada e seu centro. Testar com pessoas, objetos, sombras e mudanças de luz.
3. **Etapa 3 — Testar o motor separadamente:** confirmar tensão, corrente e sentido de rotação; testar o L298N com fonte adequada e sem a câmera montada.
4. **Etapa 4 — Integrar em um eixo:** montar PAN horizontal; comandar passos curtos de motor conforme o erro entre o centro do movimento e o centro da imagem.
5. **Etapa 5 — Reduzir oscilações:** ajustar zona morta, duração dos pulsos, intervalo de estabilização, filtros e limites mecânicos.
6. **Etapa 6 — Testar cenários reais:** testar um alvo por vez, múltiplos movimentos, iluminação variável e movimento da própria câmera.
7. **Etapa 7 — Evoluir se necessário:** considerar compensação de movimento global, sensor de posição/encoder ou um modelo de detecção de pessoa.

## 9. Critérios para decidir se precisa detectar pessoas

A detecção de movimento é suficiente se o objetivo for:

- Girar a câmera na direção de qualquer movimento relevante.
- Fazer um protótipo de baixo custo e validar a mecânica.
- Operar em um ambiente relativamente controlado, com poucos movimentos concorrentes.

A detecção de pessoa passa a fazer sentido se o objetivo exigir:

- Ignorar animais, veículos, folhas, sombras e outros movimentos.
- Priorizar pessoas quando há vários objetos se movendo.
- Manter o acompanhamento de uma pessoa específica ou distinguir alvos.
- Evitar que a câmera reaja a qualquer alteração visual.

A detecção de pessoa não precisa ser descartada definitivamente. Pode ser adicionada como uma segunda camada: primeiro o algoritmo leve identifica movimento; somente em situações selecionadas um modelo mais pesado verifica se a região contém uma pessoa. A viabilidade dessa combinação dependerá da memória, velocidade e suporte de modelos na versão específica do firmware/ESP32-S3.

## 10. Repositórios e referências para estudo

| Projeto / documentação | Link | Utilidade e observações |
|---|---|---|
| ESP-WHO — Espressif | <https://github.com/espressif/esp-who> | Framework de visão computacional da Espressif; útil para estudar detecção facial/pessoas e exemplos compatíveis. Não assumir que todos os modelos rodam na T-SIMCAM. |
| ESP32-CAM Pan Tilt | <https://github.com/ErroujiOussama/ESP32_CAM_pan_tilt> | Referência de câmera com mecanismo pan/tilt e controle; utiliza servos, portanto a mecânica/driver é diferente do motorredutor com L298N. |
| Edge AI Object Tracking Camera | <https://github.com/mmm1712/edge-ai-object-tracking-camera> | Referência de lógica de rastreamento e centralização de alvo, com hardware diferente do disponível. |
| ESP-WHO object tracking | <https://github.com/espressif/esp-who/tree/master/examples/object_tracking> | Referência de arquitetura de rastreamento; o exemplo indicado usa ESP32-P4 e servos, não é uma implementação pronta para a T-SIMCAM ESP32-S3. |
| Documentação LilyGo Camera Series | <https://github.com/Xinyuan-LILYGO/LilyGo-Camera-Series> | Repositórios e documentação da família de câmeras LilyGo. |
| Wiki T-SIMCAM | <https://wiki.lilygo.cc/products/t-sim-series/t-simcam/> | Referência para identificar a placa, conectores e detalhes específicos do modelo. |

## 11. Informações que ainda precisam ser levantadas

- Identificação exata da revisão da T-SIMCAM e do sensor de câmera instalado.
- Tensão nominal, corrente em funcionamento e corrente de partida/bloqueio do motorredutor.
- Peso total da câmera e do suporte mecânico.
- Velocidade de giro desejada e ângulo máximo de PAN.
- Disponibilidade de encoder, potenciômetro ou fins de curso; caso não existam, definir limites e estratégia de acionamento temporizado.
- GPIOs realmente livres na revisão específica da placa.

## 12. Conclusão

**A melhor estratégia é construir primeiro uma câmera seguidora por movimento.**

Ela é mais simples, econômica em processamento e permite testar o conjunto óptico, eletrônico e mecânico sem começar por um modelo de inteligência artificial. O ponto crítico será lidar com o movimento da própria câmera: usar pequenos passos, estabilizar e analisar novamente é a abordagem inicial mais prudente.

Depois que o protótipo estiver funcionando, os testes mostrarão se a detecção genérica gera falsos acionamentos demais. Se gerar, a evolução pode ser uma filtragem mais robusta ou uma verificação de pessoa aplicada apenas às regiões onde houve movimento.

---

*Projeto conceitual — versão inicial. Dados elétricos, pinagem e compatibilidade final pendentes de validação.*
