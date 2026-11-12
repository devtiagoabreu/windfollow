# iRobot — Firmware ESP32 (Robô Diferencial de 4 Rodas)

Projeto baseado no hardware existente do **WindFollowBlind**, adaptado para um robô diferencial de 4 rodas:
- **2 motores DC com redutor**, cada um acionando o par de rodas de um lado (esquerda/direita).
- **Driver L298N** com controle de PWM nos pinos ENA/ENB.
- **ESP32 DevKit** com Access Point, mDNS e interface web embarcada.
- **Sem botões físicos** — controle exclusivamente via interface web / API REST.

## Hardware

| Sinal | GPIO | Função |
|-------|------|--------|
| Motor Esquerdo IN1 | 26 | Sentido 1 |
| Motor Esquerdo IN2 | 25 | Sentido 2 |
| Motor Esquerdo ENA (PWM) | 27 | Habilitação + velocidade |
| Motor Direito IN3 | 32 | Sentido 1 |
| Motor Direito IN4 | 33 | Sentido 2 |
| Motor Direito ENB (PWM) | 14 | Habilitação + velocidade |

**Regras elétricas:**
- Fonte separada para os motores (nunca pelos 3V3/5V do ESP32).
- GND comum entre ESP32, L298N e fonte dos motores.
- Jumpers ENA/ENB do módulo L298N devem ser **removidos** para que o PWM funcione.

## Interface Web

Acesse via `http://irobot.local` ou IP do Access Point (`192.168.4.1` por padrão).

### Controles
- **Frente / Tras / Esquerda / Direita / Parar**: movimento diferencial do robô.
- **Velocidade Esquerda / Direita**: sliders independentes (0–255) para curvas suaves.
- **Aplicar Velocidades**: envia controle diferencial direto para cada motor.
- **Configuracao**: PWM minimo/maximo/padrao, salvo em NVS.

### API REST

| Endpoint | Metodo | Descricao |
|----------|--------|-----------|
| `/api/motor/status` | GET | Status dos motores (direcao, velocidades, movendo) |
| `/api/motor/move` | POST | Move o robô (`direction`: frente/tras/esquerda/direita/stop, `speed`: 0-255) ou diferencial (`leftSpeed`, `rightSpeed`) |
| `/api/motor/stop` | POST | Para todos os motores |
| `/api/config` | GET/POST | Ler/salvar configuracao de PWM |
| `/api/system/info` | GET | Uptime, heap, PSRAM |

## Compilacao

```bash
arduino-cli compile --fqbn "esp32:esp32:esp32" --build-path "%TEMP%\irobot-build" firmware/irobot
arduino-cli upload -p COM6 --fqbn "esp32:esp32:esp32" --input-dir "%TEMP%\irobot-build"
```

## Comandos Seriais (115200 baud)

| Comando | Efeito |
|---------|--------|
| `f` | Frente |
| `t` | Tras |
| `e` | Esquerda |
| `d` | Direita |
| `s` | Stop |
| `1`–`9` | Velocidade frente (10%–90%) |
| `i` | Status |
| `r` | Reiniciar |

## Arquivos

| Arquivo | Funcao |
|---------|--------|
| `irobot.ino` | Setup/loop, AP + mDNS, comandos seriais |
| `HardwareConfig.h` | Struct de configuracao (GPIOs, limites PWM) |
| `L298NDualDriver.h/.cpp` | Controle do L298N com PWM |
| `MotorEngine.h/.cpp` | Comandos diferenciais do robô |
| `ConfigManager.h/.cpp` | Leitura/gravacao em NVS |
| `DashboardService.h/.cpp` | Servidor HTTP, API REST |
| `WebAssets.h` | HTML/CSS/JS embarcados |
| `web/` | Fontes da interface web |
| `tools/generate_web_assets.ps1` | Gera WebAssets.h a partir de web/ |

## Limites Conhecidos

- Sem encoders → controle em malha aberta (nao sabe distancia/angulo exato).
- Motores tipo TT → baixo torque, adequado para piso plano e robô leve.
- 4 rodas geram mais atrito lateral em curvas que 2 rodas + caster.
