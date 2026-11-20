# Diário de desenvolvimento (DEVLOG)

Registro cronológico do que foi feito em cada commit, com as dificuldades
encontradas e como foram resolvidas. Cada entrada é inserida automaticamente no
topo da seção **Histórico** por `tools/save.ps1`, que commita e faz push.

Formato de cada entrada: data/hora, mensagem do commit, o que mudou, dificuldades
e soluções, arquivos tocados.

---

## Histórico

### 2026-11-20 20:01 - fix(irobot): set default speed to 70 percent

**Area:** irobot

**Feito:**
- Velocidade inicial manual e autonoma ajustada para 70% (179 PWM) no firmware, HTML/JS e WebAssets
- Constantes e defaults de autoSpeed/pwmDefault alinhados, incluindo re autonoma
- Push pendente do commit anterior mantido para envio neste save

**Dificuldades e solucoes:**
- nenhuma

**Arquivos:**
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`

### 2026-11-20 19:48 - fix(irobot): add configurable auto speed and repair scan flow

**Area:** irobot

**Feito:**
- Reparo do reconhecimento 360 ao ligar autonomo com espera de leitura ToF fresca, giros/assentamento e alinhamento nao bloqueantes escalados pela velocidade
- Velocidade do modo autonomo configuravel (autoSpeed em NVS/config/API e sliders em Home, DL e Config) aplicada em frente, re, giro e scan
- WebAssets.h regenerado e README atualizado

**Dificuldades e solucoes:**
- Compilacao nao validada localmente porque os arduino-cli encontrados falham com erro de extracao PyInstaller (libcrypto-3.dll/VCRUNTIME140.dll) -> codigo revisado estaticamente, pronto para compilar/gravar com toolchain funcional

**Arquivos:**
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/README.md`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/irobot.ino`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`

### 2026-11-18 17:38 - feat(msa): lista redes wifi no modo cliente

**Area:** msa

**Feito:**
- Campo de senha deixou de vazar para tras do card de limites ao remover min-height zero do card
- Novo endpoint GET /api/wifi/scan varre ate 24 SSIDs unicos ordenados por sinal
- Aba Config lista redes com RSSI canal e criptografia e permite digitar outro SSID
- WebAssets e cache do service worker atualizados

**Dificuldades e solucoes:**
- Card com min-height zero encolhia abaixo do conteudo e o input ficava atras do card vizinho -> removi o min-height mantendo min-width zero
- Varias redes repetidas e fracas podiam poluir a lista -> dedupliquei por SSID e ordenei por RSSI
- Cliente pode querer rede oculta ou fora da varredura -> adicionei opcao Digitar SSID

**Arquivos:**
- `firmware/msa/DashboardService.cpp`
- `firmware/msa/DashboardService.h`
- `firmware/msa/README.md`
- `firmware/msa/WebAssets.h`
- `firmware/msa/web/css/style.css`
- `firmware/msa/web/index.html`
- `firmware/msa/web/js/app.js`
- `firmware/msa/web/sw.js`

### 2026-11-18 17:23 - fix(msa): ajusta interface mobile first

**Area:** msa

**Feito:**
- Unidade base mobile first de 9 a 14px com reducao controlada em paisagem
- Layout em uma coluna no celular e breakpoints min-width para tablet e desktop
- Fontes minimas 16px nos campos, alvos de toque de 44px e areas seguras do iOS
- Cache do service worker e WebAssets regenerados e regravados em COM12

**Dificuldades e solucoes:**
- O clamp antigo min(1vw,2vh) caia para 5px no retrato -> troquei por 2.6vw com piso de 9px e excecao em landscape
- CSS usava breakpoints max-width e escondia grids no mobile -> reescrevi mobile first com min-width
- Service worker podia servir css antigo -> bump de style.css?v=3 e cache msa-v3

**Arquivos:**
- `firmware/msa/WebAssets.h`
- `firmware/msa/web/css/style.css`
- `firmware/msa/web/index.html`
- `firmware/msa/web/sw.js`

### 2026-11-18 15:28 - feat(msa): adiciona OLED e interface responsiva

**Area:** msa, raiz

**Feito:**
- DisplayService SSD1306 com deteccao automatica 0x3C/0x3D e leituras/avisos no OLED
- Interface web reescrita no padrao irobot com PWA, tela cheia, ticker e layout responsivo
- Configuracao Wi-Fi igual irobot com AP personalizavel, STA e reinicio ao salvar
- Compilacao, gravacao em COM12 e serial validando AHT20 0x38, BMP280 0x77 e OLED 0x3C

**Dificuldades e solucoes:**
- Adafruit SSD1306 nao estava instalada e o display podia responder em 0x3C ou 0x3D -> instalei a lib 2.5.17 e fiz scan I2C antes do begin com periphBegin=false para nao reiniciar o Wire
- AP do irobot so salvava SSID/senha sem aplicar -> persisti apSsid/apPass no NVS e carrego no boot antes do softAP
- Relatorio de exploracao veio truncado -> revisei diretamente web/js/app.js, index.html e style.css do irobot para replicar a tecnologia

**Arquivos:**
- `AGENTS.md`
- `README.md`
- `firmware/msa/DashboardService.cpp`
- `firmware/msa/DashboardService.h`
- `firmware/msa/HardwareConfig.h`
- `firmware/msa/README.md`
- `firmware/msa/WebAssets.h`
- `firmware/msa/msa.ino`
- `firmware/msa/web/css/style.css`
- `firmware/msa/web/index.html`
- `firmware/msa/web/js/app.js`
- `firmware/msa/web/manifest.json`
- `firmware/msa/web/sw.js`
- `firmware/msa/DisplayService.cpp`
- `firmware/msa/DisplayService.h`

### 2026-11-18 14:31 - feat(msa): cria firmware de sensores ambientais

**Area:** msa, raiz, tooling

**Feito:**
- novo firmware msa com AHT20 e BMP280 no I2C, leitura validada e cadencia configuravel
- dashboard web, API REST, alertas com tendencia, NVS, Wi-Fi AP/STA e mDNS
- perfil msa no flash.ps1 e correcao do detectOnly com stderr do esptool
- documentacao de hardware e API e README/AGENTS atualizados
- compilacao verificada com arduino-cli

**Dificuldades e solucoes:**
- detectOnly abortava porque o esptool escreve no stderr com ErrorActionPreference Stop -> alterei apenas o escopo da chamada
- tres portas seriais conectadas -> identifiquei o ESP32 pelo esptool e mantive deteccao dinamica
- WebAssets.h e gerado -> gerei apos editar web/ antes de compilar

**Arquivos:**
- `AGENTS.md`
- `README.md`
- `tools/flash.ps1`
- `firmware/msa/`

### 2026-11-18 08:43 - fix(irobot): corrige ordem dos botoes do D-pad

**Area:** irobot

**Feito:**
- Home e DL com Tras abaixo de Frente e Direita abaixo de Esquerda
- preview sincronizado
- WebAssets regenerado
- compilacao validada em 85% de flash

**Dificuldades e solucoes:**
- ordem vinha do fluxo automatico do grid com display contents -> reordenado o DOM dos botoes e medido com Chrome headless

**Arquivos:**
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/preview/index.html`
- `firmware/irobot/web/index.html`

### 2026-11-18 08:35 - docs: consolida licoes aprendidas do iRobot e Windfollow

**Area:** docs, raiz

**Feito:**
- novo documento docs/LICOES_APRENDIDAS.md com hardware navegacao UI responsiva PWA geracao de assets preview workflow e checklist
- registro de erros e correcoes como virgula no WebAssets D-pad cortado knob fixo build versionado e limitacao PWA
- README aponta para o novo documento
- aprendizados tambem salvos no grafo de memoria persistente

**Dificuldades e solucoes:**
- conhecimento estava disperso entre conversas commits e DEVLOG -> consolidado em um documento unico e no grafo de memoria

**Arquivos:**
- `README.md`
- `docs/LICOES_APRENDIDAS.md`

### 2026-11-17 22:28 - feat(irobot): layout Home responsivo em retangulo perfeito

**Area:** irobot, raiz

**Feito:**
- grid preenche a viewport exatamente sem scroll em 667x375 844x390 e 960x540
- fontes botoes cards e joystick escalam por unidade de viewport
- comandos de autonomo e velocidade foram para a faixa inferior junto de sensores e status
- densidade compacta em paisagem baixa evita corte no D-pad
- raio do knob do joystick agora e medido do DOM
- preview sincronizado e README atualizado

**Dificuldades e solucoes:**
- texto do D-pad cortava em 844x390 -> medi retangulos com navegador headless -> media query compacta aplicada
- raio fixo do knob quebrava com joystick escalado -> raio agora lido do elemento renderizado

**Arquivos:**
- `README.md`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/preview/app.js`
- `firmware/irobot/preview/index.html`
- `firmware/irobot/preview/style.css`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`

### 2026-11-17 21:51 - feat(irobot): ajusta UI Home, icones PWA e preview local

**Area:** irobot, raiz

**Feito:**
- avisos ambientais agora aparecem no centro do cabecalho ao lado do titulo iRobot
- sensores e status foram movidos para baixo dos botoes de direcao na Home
- favicon e icones PWA PNG 32/192/512 servidos pelo firmware e embutidos no WebAssets
- manifest atualizado com display fullscreen e service worker v7
- pasta preview com mock de API e cenarios alerts/fire/stuck para validar a tela no PC
- artefatos de build removidos do git e build/ adicionado ao gitignore
- README atualizado com instrucoes do preview

**Dificuldades e solucoes:**
- gerador de assets escrevia bytes PNG sem virgula entre linhas -> compile falhou -> join com virgula aplicado e compile ok 85% flash
- PWA em AP HTTP nao oferece instalacao real no navegador -> documentado HTTPS/localhost e manifest com PNGs corretos

**Arquivos:**
- `.gitignore`
- `README.md`
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/DashboardService.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/build/esp32.esp32.esp32/build.options.json`
- `firmware/irobot/build/esp32.esp32.esp32/flash_args`
- `firmware/irobot/build/esp32.esp32.esp32/partitions.csv`
- `firmware/irobot/build/esp32.esp32.esp32/sdkconfig`
- `firmware/irobot/tools/generate_web_assets.ps1`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/manifest.json`
- `firmware/irobot/web/sw.js`
- `firmware/irobot/preview/`
- `firmware/irobot/tools/generate_icons.ps1`
- `firmware/irobot/web/favicon.png`
- `firmware/irobot/web/icon-192.png`
- `firmware/irobot/web/icon-512.png`

### 2026-11-17 21:12 - docs: README profissional com todos os projetos, hardware, API e fluxo de trabalho

**Area:** raiz

**Feito:**
- README reescrito com badges, autor Tiago de Abreu @devtiagoabreu, visao geral dos 3 projetos
- documentado hardware, GPIOs, rede, modulos, comandos seriais e navegacao autonoma do iRobot
- adicionada secao de ferramentas flash/assets/save e simuladores
- secao PWA com aviso HTTPS

**Dificuldades e solucoes:**
- nenhuma

**Arquivos:**
- `README.md`

### 2026-11-17 21:04 - feat(irobot): nova UI web com abas Home/DL/Config, PWA e letreiro de ambiente

**Area:** irobot

**Feito:**
- index.html reescrito como SPA com abas Home (paisagem joystick esquerda botoes centro sensores/status inferior badge conectado), DL (tela antiga mantida) e Config (wifi AP/client e PWM)
- app.js unificado com joystick duplo, D-pad, autonomo, PWA service worker e fullscreen
- manifest.json favicon.svg sw.js adicionados e servidos
- DashboardService rotas manifest favicon sw api/wifi restart
- irobot.ino conecta STA quando salvo

**Dificuldades e solucoes:**
- PWA em HTTP no AP nao instala em navegadores (exige HTTPS ou localhost): mantido manifest e service worker para quando servir via HTTPS e orientacao fullscreen
- layouts responsivos e joystick duplo exigiram rewrite do app.js

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/DashboardService.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/build/esp32.esp32.esp32/build.options.json`
- `firmware/irobot/irobot.ino`
- `firmware/irobot/tools/generate_web_assets.ps1`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/favicon.svg`
- `firmware/irobot/web/manifest.json`
- `firmware/irobot/web/sw.js`

### 2026-11-17 20:40 - feat(irobot): reconhecimento de 360 graus e alinhamento na direcao mais livre no modo autonomo

**Area:** irobot

**Feito:**
- novo estado SCAN varre 360 graus parando e medindo a cada 60 graus
- leituras sem alvo gravam distancia maxima
- apos scan o robo gira para a direcao mais livre e segue em frente
- heading estimado por tempo de giro
- estados SCAN_MEASURE SCAN_TURN ALIGN_BEST adicionados

**Dificuldades e solucoes:**
- nenhuma

**Arquivos:**
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/build/`

### 2026-11-15 23:59 - docs(irobot): documentar sensores, avisos ambientais e navegacao autonoma

**Area:** docs, irobot, raiz

**Feito:**
- README do firmware irobot reescrito como referencia completa (hardware e alimentacao, tabela dos sensores I2C com enderecos e cadencia, maquina de estados do autonomo com tabela de constantes, regras dos avisos ambientais, API com exemplo real de /api/sensors, comandos seriais incluindo a e m, compilacao e gravacao, tabela de diagnostico de problemas, arquivos, prototipos, limites e proximos passos de hardware) e novo documento docs/IROBOT_NAVEGACAO_E_SENSORES.md com diagramas mermaid do fluxo de dados e da FSM, justificativa do pivo em vez de arco, tratamento de leituras invalidas do VL53L0X, criterios de calibracao, evidencias dos testes seriais, logica detalhada de previsao de chuva por barometro e de incendio, mitigacoes de brownout e checklist de teste, alem de links no README raiz

**Dificuldades e solucoes:**
- Manter a documentacao fiel ao codigo exigiu conferir cada valor no HardwareConfig e nas classes novas (cadencia 80/400 ms, janela de 3 h com 180 amostras, limiares e histereses) e registrar o que ainda e apenas planejado, como o monitor de bateria e os sensores de chuva e chama, para nao prometer recurso que o firmware nao tem

**Arquivos:**
- `README.md`
- `firmware/irobot/README.md`
- `docs/IROBOT_NAVEGACAO_E_SENSORES.md`

### 2026-11-15 23:28 - feat(irobot): avisos ambientais (chuva, calor, frio, ar seco, incendio)

**Area:** irobot

**Feito:**
- Novo EnvironmentAdvisor com historico de pressao do BMP280 (1 amostra por minuto, 3 h) para prever chuva pela regra de barometro e janela de 1 min da temperatura para detectar subida rapida, gerando avisos de chuva provavel, calor acima de 26 C, frio abaixo de 17 C, umidade abaixo de 30 e possivel incendio com histerese, expostos no objeto alerts de /api/sensors e no serial pelo comando m, com card Avisos e banner vermelho de incendio no painel e linha de avisos na pagina de teste mais suporte opcional a sensor de chuva FC-37 no GPIO34 e chama KY-026 no GPIO39 por defines, e o sensor ToF continua sendo lido durante o giro com log a cada 500 ms e leitura vencida nao conta mais como caminho livre

**Dificuldades e solucoes:**
- A previsao de chuva depende de 30 min de historico de pressao, entao no arranque o app mostra coletando dados em vez de arriscar um falso negativo, e o giro sem encoder precisou de um limite de idade da amostra (500 ms) para nao decidir caminho livre com leitura velha enquanto o robo roda

**Arquivos:**
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/DashboardService.h`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/README.md`
- `firmware/irobot/SensorService.cpp`
- `firmware/irobot/SensorService.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/irobot.ino`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/teste.html`
- `firmware/irobot/EnvironmentAdvisor.cpp`
- `firmware/irobot/EnvironmentAdvisor.h`

### 2026-11-15 23:15 - feat(irobot): desvio autonomo com re, pivo e espera por 1 m livre

**Area:** irobot

**Feito:**
- AutoNavigator para a 50 cm do obstaculo (era 25 cm), recua 500 ms antes de girar (800 ms a cada 3 obstaculos) e gira no proprio eixo com uma roda em re e outra em frente, em vez do arco que derrapava
- o giro nao tem mais tempo fixo, continua ate o ToF ler 1 m livre em 2 leituras seguidas, com minimo de 450 ms e maximo de 2,4 s por segmento
- se nao achar saida recua 350 ms e continua girando do mesmo lado e, apos dois segmentos, troca de lado
- so retoma o reto depois de assentar e confirmar 1 m
- cada giro sem saida tambem conta como tentativa para o detector de enroscamento (100 tentativas) e moveDifferential agora atualiza a direcao para a telemetria

**Dificuldades e solucoes:**
- O FSM antigo alternava lados a cada 650 ms com um arco de roda unica, o que num chassi com roda traseira fixa virava derrapagem e nunca dava tempo de medir
- reescrevi como maquina de estados inspirada nos projetos de obstacle avoidance do GitHub (stop-backup-pivot-turnate-clear) e observei no serial que, numa bancada cercada a 15 cm, ele agora alterna re e giro e troca de lado em vez de ficar piscando entre esquerda e direita

**Arquivos:**
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/MotorEngine.cpp`
- `firmware/irobot/SensorService.h`

### 2026-11-15 22:12 - fix(irobot): boot sem acionar motores e Wi-Fi com menor potencia de TX

**Area:** irobot

**Feito:**
- GPIO27 e GPIO14 agora sao colocados em LOW na primeira linha do setup, antes de serial e Wi-Fi, evitando que ENA/ENB flutuando no boot girem os motores e derrubem a alimentacao
- AP reduzido para 11 dBm para diminuir o pico de corrente
- boot avisa quando o reset anterior foi brownout e explica o que revisar
- diagnostico confirmou que no USB o AP sobe normal (Irobot-Setup) e o problema é alimentacao/bateria

**Dificuldades e solucoes:**
- O sintoma nao reproduziu com USB (AP sobe e fica estavel 15 min com heap em 207 kB e sensor lendo distancia), mas o usuario confirmou que na bateria a rede nem aparece -> sem banco de testes com bateria, apliquei mitigacoes de firmware (enable em LOW no boot, TX 11 dBm) e documentei a causa provavel: sag do 5V do L298N e ENA/ENB flutuando durante o boot

**Arquivos:**
- `firmware/irobot/irobot.ino`

### 2026-11-15 21:50 - chore(irobot): diagnostico de reset, heap e clientes WiFi

**Area:** irobot

**Feito:**
- Boot agora imprime o motivo do reset anterior (incluindo BROWNOUT) via esp_reset_reason
- heartbeat de 10 s mostra heap livre, menor heap, numero de clientes conectados ao AP, estado do autonomo e distancia
- Wi-Fi do AP com setSleep(false) para nao economizar energia
- teste de 40 s com motores em 150 PWM sem reset e heap estavel em 207 kB

**Dificuldades e solucoes:**
- O sintoma relatado (AP some e nao volta) nao reproduziu em bancada com motores ligados -> adicionei instrumentacao de reset e heap e deixei uma captura serial longa rodando para pegar a causa no momento da falha

**Arquivos:**
- `firmware/irobot/irobot.ino`

### 2026-11-15 21:32 - chore(irobot): gravacao no COM9, diagnostico de distancia bruta e mDNS sem duplicata

**Area:** irobot

**Feito:**
- Gravado no ESP32 D0WDQ6 do COM9 (1.074.303 bytes, 81% flash)
- comandos seriais a e m validados no hardware com autonomo entrando em frente e desligando corretamente
- AHT20 e BMP280 respondendo (26,8 C 70,4% 945,5 hPa)
- serial m agora mostra a leitura bruta do VL53L0X (8191 significa nenhum alvo no alcance)
- removida a segunda chamada de MDNS.begin duplicada no DashboardService que gerava o erro service already exists

**Dificuldades e solucoes:**
- A distancia aparecia 0 e nao dava para saber se o sensor estava quebrado ou apenas sem alvo -> expus lastRawMm no SensorService e no comando m, confirmando 8191 (sem alvo) em vez de falha de I2C, e reflash para validar no funcionamento real

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/SensorService.cpp`
- `firmware/irobot/SensorService.h`
- `firmware/irobot/irobot.ino`

### 2026-11-15 20:36 - feat(irobot): autonomo continuo com deteccao de enroscamento e alerta no app

**Area:** irobot

**Feito:**
- AutoNavigator deixou de parar apos 3 giros: agora roda ate o usuario desligar ou ate acumular 100 obstaculos seguidos sem completar 2 s de avanco livre (AUTO_STUCK_ATTEMPTS e AUTO_FREE_FORWARD_MS no HardwareConfig)
- ao enroscar desliga o modo, zera o PWM e publica stuck e stuckSeq em /api/auto e /api/sensors
- painel ganhou banner de alerta com vibracao e bipe e polling de 1 s
- /teste tambem alerta
- comandos seriais continuam a e m e o log mostra a contagem de obstaculos

**Dificuldades e solucoes:**
- Nenhuma porta COM presente nas duas tentativas de gravacao -> firmware compilado (1.074.447 bytes, 81% flash) e commitado, restando gravar com tools/flash.ps1 -Sketch irobot assim que o ESP32 for reconectado
- notificacao nativa do browser exige HTTPS e no AP ela so funciona ate onde o navegador permite, entao o alerta principal ficou sendo o banner com vibracao e som

**Arquivos:**
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/teste.html`

### 2026-11-15 19:35 - feat(irobot): sensores AHT20/BMP280/VL53L0X e modo autonomo

**Area:** irobot

**Feito:**
- SensorService le AHT20 + BMP280 (temp/umidade/pressao) e VL53L0X (distancia) no I2C SDA=21/SCL=22 com intervalos escalonados
- AutoNavigator implementa FSM frente/para/desvia/assenta/desiste com giros alternados a cada 650 ms e limite de 3 tentativas
- novas rotas GET /api/sensors, GET e POST /api/auto e comandos seriais a e m
- painel ganhou card Ambiente com distancia e botao Autonomo e a pagina /teste mostra linha compacta com toggle
- WebAssets.h regenerado e firmware compila em 81% de flash

**Dificuldades e solucoes:**
- arduino-cli reportou sucesso com 1.070.207 bytes
- a gravacao nao pode ser feita porque nenhuma porta COM estava presente no momento do teste -> deixei o firmware compilado e orientei reconectar o ESP32 e gravar com tools/flash.ps1 -Sketch irobot

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/DashboardService.h`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/irobot.ino`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/teste.html`
- `firmware/irobot/AutoNavigator.cpp`
- `firmware/irobot/AutoNavigator.h`
- `firmware/irobot/SensorService.cpp`
- `firmware/irobot/SensorService.h`

### 2026-11-15 19:08 - docs(irobot): registra o Prototipo-67 e o teste com duas baterias 3.7V

**Area:** irobot

**Feito:**
- README ganha secao Prototipos com o Prototipo-67
- teste com 2 baterias 3.7V em serie (7.4V) alimentou tudo, ficou um pouco fraco mas validou todas as funcionalidades

**Dificuldades e solucoes:**
- nenhuma

**Arquivos:**
- `firmware/irobot/README.md`

### 2026-11-15 18:53 - docs(irobot): registra alimentacao do ESP32 pela saida 5V do L298N

**Area:** irobot

**Feito:**
- README documenta bateria 7-12V no L298N com jumper 5V-EN, saida 5V para VIN do ESP32 e GND comum
- aviso de nao usar USB junto
- menciona a rota /teste

**Dificuldades e solucoes:**
- nenhuma

**Arquivos:**
- `firmware/irobot/README.md`

### 2026-11-15 17:12 - feat(irobot): painel antigo de volta em / e versao simples em /teste

**Area:** irobot

**Feito:**
- raiz volta a ter joystick a esquerda e sliders de velocidade a direita
- /teste serve a pagina so com 5 botoes
- gerador embute teste_html e links antigos ganham ?v=3 contra cache

**Dificuldades e solucoes:**
- esp32 parou de responder ao esptool apos matar o monitor serial -> reset fisico na placa e nova deteccao da porta resolveram

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/DashboardService.h`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/tools/generate_web_assets.ps1`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`
- `firmware/irobot/web/teste.html`

### 2026-11-15 16:50 - fix(irobot): gerador de WebAssets embutia aspas no inicio do app.js

**Area:** irobot

**Feito:**
- generate_web_assets.ps1 nao adiciona mais aspas extras apos R-raw
- JS servido voltou a ser valido e os botoes conseguem chamar a API
- WebAssets.h regerado e gravado em COM5

**Dificuldades e solucoes:**
- app.js chegava ao navegador com aspas solta no inicio (SyntaxError) e nenhum fetch saia do celular -> corrigido o delimitador do raw string e conferido byte a byte no header gerado

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/tools/generate_web_assets.ps1`

### 2026-11-15 16:17 - feat(irobot): tela simplificada so com botoes de direcao

**Area:** irobot

**Feito:**
- index.html com 5 botoes grandes (frente, tras, esquerda, direita, parar)
- app.js reescrito mostrando erro de API na propria tela
- css novo e WebAssets.h regerado

**Dificuldades e solucoes:**
- sem acesso ao AP da placa pela maquina (Windows bloqueia netsh wlan connect sem Localizacao) -> validacao feita por compilacao, gravacao e log serial de boot

**Arquivos:**
- `firmware/irobot/WebAssets.h`
- `firmware/irobot/web/css/style.css`
- `firmware/irobot/web/index.html`
- `firmware/irobot/web/js/app.js`

### 2026-11-15 16:06 - fix(irobot): tela nao enviava comandos por ler o corpo do POST do socket

**Area:** irobot

**Feito:**
- POST usa server.arg('plain') em move e config
- o corpo ja vem bufferizado pelo WebServer e readString devolvia vazio esperando 1s por requisicao

**Dificuldades e solucoes:**
- handlers liam client().readString() depois do parser consumir o corpo, causando 400 e travando o servidor -> trocado por arg('plain') e regravado no ESP32

**Arquivos:**
- `firmware/irobot/DashboardService.cpp`

### 2026-11-15 15:58 - fix(tooling): flash.ps1 falhava com uma unica porta serial

**Area:** tooling

**Feito:**
- ports agora e sempre array com @() e a gravacao segue sem -Port
- firmware irobot compilado e gravado com sucesso em COM8 (ESP32-D0WD-V3)

**Dificuldades e solucoes:**
- PowerShell desembrulha array de um elemento e .Count fica vazio -> Get-SerialPorts embrulhado em @() e campo Count validado

**Arquivos:**
- `tools/flash.ps1`

### 2026-11-15 15:53 - docs(devlog): corrige entrada quebrada pelo separador de itens

**Area:** docs, raiz

**Feito:**
- entrada do commit anterior reorganizada em bullets
- AGENTS.md avisa para nao usar ponto e virgula dentro do texto de um item

**Dificuldades e solucoes:**
- o proprio texto do item continha o separador e foi dividido ao meio -> texto reescrito sem o caractere

**Arquivos:**
- `AGENTS.md`
- `docs/DEVLOG.md`

### 2026-11-15 15:52 - fix(tooling): itens de Summary/Difficulty separados por ';' no save.ps1

**Area:** docs, raiz, tooling

**Feito:**
- save.ps1 divide -Summary e -Difficulty pelo ponto e virgula (powershell -File nao converte virgula em lista)
- entrada anterior do DEVLOG reorganizada em bullets
- AGENTS.md atualizado com o separador

**Dificuldades e solucoes:**
- powershell.exe -File entregou "a","b" como uma unica string e o DEVLOG saiu corrido -> split pelo separador e entrada corrigida

**Arquivos:**
- `AGENTS.md`
- `docs/DEVLOG.md`
- `tools/save.ps1`

### 2026-11-15 15:51 - feat(irobot): chassi 2 rodas + caster, deteccao automatica de placa/COM e DEVLOG automatico

**Area:** irobot, windfollowblind, tooling, docs

**Feito:**
- chassi documentado como 2 motores (1 roda cada) + roda caster livre
- novo `tools/flash.ps1`: detecta COM na hora, confere chip com esptool e grava
- novo `tools/save.ps1` + `docs/DEVLOG.md`: commit, push e diário automáticos
- `AGENTS.md`: fluxo de início de sessão e regras de porta/commit
- corrige `windfollowblind.ino` (não compilava) e typos de mDNS do blind
- remove COM fixa dos READMEs/documentação; README raiz ganha seção irobot

**Dificuldades e soluções:**
- arduino-cli marca clone CH340 como Unknown -> chip identificado via esptool flash-id
- `windflollowblind.ino` não batia com a pasta (main file missing) -> `git mv` preservando histórico
- pasta de build com `%TEMP%` literal dentro do repo -> removida; scripts usam TEMP
- PowerShell 5.1 lê .ps1 sem BOM como ANSI -> scripts mantidos em ASCII puro

**Arquivos:**
- `README.md`
- `docs/WIND_FOLLOW_BLIND_DOCUMENTACAO.md`
- `firmware/irobot/DashboardService.cpp`
- `firmware/irobot/HardwareConfig.h`
- `firmware/irobot/README.md`
- `firmware/windfollowblind/DashboardServiceBlind.cpp`
- `firmware/windfollowblind/windflollowblind.ino -> firmware/windfollowblind/windfollowblind.ino`
- `AGENTS.md`
- `docs/DEVLOG.md`
- `tools/flash.ps1`
- `tools/save.ps1`
