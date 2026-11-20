# iRobot — Firmware ESP32 (Robô Diferencial de 2 Rodas + Caster)

Projeto baseado no hardware do **WindFollowBlind**, adaptado para um chassi
diferencial de 2 motores, com sensores ambientais e navegação autônoma com desvio
de obstáculos:

- **2 motores DC com redutor**, cada um acionando **uma roda** (esquerda/direita).
- **1 roda caster livre** (giratória) na outra ponta do chassi, apenas de apoio.
- **Driver L298N** com PWM nos pinos ENA/ENB.
- **ESP32 DevKit** com Access Point, mDNS e interface web embarcada.
- **Sem botões físicos** — controle por interface web, API REST ou serial.
- **Sensores I2C**: AHT20 (temperatura/umidade), BMP280 (pressão) e VL53L0X
  (distância), usados para avisos ambientais e para o modo autônomo.

---

## 1. Hardware

### Motores (L298N)

| Sinal | GPIO | Função |
|-------|------|--------|
| Motor Esquerdo IN1 | 26 | Sentido 1 da roda esquerda |
| Motor Esquerdo IN2 | 25 | Sentido 2 da roda esquerda |
| Motor Esquerdo ENA (PWM) | 27 | Habilitação + velocidade |
| Motor Direito IN3 | 32 | Sentido 1 da roda direita |
| Motor Direito IN4 | 33 | Sentido 2 da roda direita |
| Motor Direito ENB (PWM) | 14 | Habilitação + velocidade |
| Roda caster | — | Livre, sem atuador nem sensor |

Regras elétricas:

- Fonte separada para os motores (nunca pelos 3V3/5V do ESP32).
- GND comum entre ESP32, L298N e fonte dos motores.
- Jumpers ENA/ENB do módulo L298N **removidos** para o PWM funcionar.
- `GPIO27` e `GPIO14` são colocados em LOW na primeira linha do `setup()`: com os
  enables flutuando no boot os motores podem girar e derrubar a alimentação.

### Alimentação sem USB

Ligação validada em bancada:

- Bateria **7–12 V** nos terminais `+12V`/`GND` do L298N, jumper `5V-EN` no lugar.
- Saída **`5V` do L298N → `VIN` do ESP32**.
- **`GND` do L298N → `GND` do ESP32** (mesma referência, obrigatório).
- Não ligar o USB junto com essa alimentação no `VIN` (backfeed).

Diagnóstico de bancada (Protótipo-67): com o pack 2S (7,4 V) descarregado o
ESP32 subia em USB mas o Access Point não aparecia na bateria; após recarregar o
pack o robô funcionou. Recomendações para eliminar o problema de vez:

- buck 5 V dedicado para o ESP32 em vez da saída 5 V do L298N;
- 1000 µF + 100 nF entre VIN e GND;
- pull-down de 10 kΩ em GPIO27 e GPIO14;
- pack 3S (11,1 V), que fica dentro da faixa confortável do L298N.

---

## 2. Sensores I2C

| Sensor | Endereço | Grandeza | Observações |
|--------|----------|----------|-------------|
| AHT20 | `0x38` | temperatura e umidade | leitura via `Adafruit_AHTX0` |
| BMP280 | `0x76` ou `0x77` | pressão e temperatura | modo normal, oversampling X2/X16, filtro X16, standby 500 ms |
| VL53L0X | `0x29` | distância (mm) | timing budget 20 ms, alcance útil até ~1,2 m |

Barramento: `SDA = GPIO21`, `SCL = GPIO22`, 100 kHz. Alimentar os módulos em
**3,3 V** (o ESP32 não tolera 5 V nos pinos I2C).

Cadência de leitura:

- Ambiente (AHT20 + BMP280): a cada **2 s**.
- VL53L0X: a cada **400 ms** em repouso e a cada **80 ms** com o modo autônomo
  ligado (`setFastDistance(true)`).

Comportamento:

- Temperatura exibida é a do AHT20; se ele faltar, cai para a do BMP280
  (`bmpTemperature` continua sempre disponível em separado).
- VL53L0X devolve `~8191` quando não há alvo no alcance → o firmware converte
  para `distanceMm = 0` (caminho livre). Timeout de leitura vira `lastRawMm =
  9999`.
- `distanceAgeMs()` informa a idade da última medição; o modo autônomo ignora
  leituras com mais de 500 ms para não decidir com dado velho.

---

## 3. Modo autônomo (desvio de obstáculos)

Liga/desliga pelo botão **Autônomo** do painel, pela página `/teste` ou pelo
comando serial `a`. Exige o VL53L0X presente (a API responde 409 se faltar).

Máquina de estados: `OFF → FORWARD → BACKUP → TURN → SETTLE → FORWARD`.

1. **FORWARD**: anda a 140 PWM até o ToF ler menos de 500 mm.
2. **BACKUP**: para e dá ré a 130 PWM por 500 ms (800 ms a cada 3 obstáculos).
3. **TURN**: gira **no próprio eixo** (uma roda em ré, outra em frente) a 160 PWM;
   a leitura do ToF continua durante o giro, a cada 80 ms, e o giro **não tem
   tempo fixo**: segue até 2 leituras seguidas de ≥ 1 m, com mínimo de 450 ms.
   Se em 2400 ms não achar 1 m livre, dá uma ré curta de 350 ms e continua
   girando do mesmo lado; depois de 2 segmentos troca de lado.
4. **SETTLE**: para 250 ms, confirma 1 m livre e só então volta para FORWARD.

Detecção de enroscado: cada obstáculo e cada giro sem saída contam como
tentativa; 100 tentativas seguidas sem 2 s de avanço livre desligam o autônomo,
geram `stuck`/`stuckSeq` e disparam banner + vibração + bipe no app.

Constantes em `HardwareConfig.h` (bloco `AUTO_*`):

| Constante | Valor | Papel |
|-----------|-------|-------|
| `AUTO_STOP_MM` | 500 | distância para parar e desviar |
| `AUTO_CLEAR_MM` | 1000 | folga mínima para retomar o reto |
| `AUTO_CLEAR_SAMPLES` | 2 | leituras seguidas para considerar livre |
| `AUTO_CLEAR_MAX_AGE_MS` | 500 | idade máxima da leitura aceita |
| `AUTO_BACKUP_MS` | 500 | ré padrão antes de girar |
| `AUTO_BACKUP_LONG_MS` | 800 | ré maior a cada 3 obstáculos |
| `AUTO_BACKUP_NUDGE_MS` | 350 | ré curta quando o giro não acha saída |
| `AUTO_BACKUP_SPEED` | 130 | PWM da ré |
| `AUTO_TURN_MIN_MS` | 450 | tempo mínimo de giro antes de avaliar |
| `AUTO_TURN_MAX_MS` | 2400 | tempo máximo por segmento de giro |
| `AUTO_TURN_SPEED` | 160 | PWM do pivô |
| `AUTO_FORWARD_SPEED` | 140 | PWM do avanço reto |
| `AUTO_SETTLE_MS` | 250 | pausa de confirmação |
| `AUTO_FREE_FORWARD_MS` | 2000 | avanço livre que zera o contador |
| `AUTO_STUCK_ATTEMPTS` | 100 | tentativas até considerar enroscado |

Detalhes de projeto, calibração e evidências de teste estão em
[`docs/IROBOT_NAVEGACAO_E_SENSORES.md`](../../docs/IROBOT_NAVEGACAO_E_SENSORES.md).

---

## 4. Avisos ambientais

O `EnvironmentAdvisor` transforma as leituras em avisos exibidos no painel, na
página `/teste`, no serial e no JSON de `/api/sensors`:

| Aviso | Regra | Mensagem ao usuário |
|-------|-------|---------------------|
| Vai chover | pressão caiu ≥ 1,0 hPa em 3 h e umidade ≥ 65 %, **ou** queda ≥ 0,6 hPa em 30 min com umidade ≥ 70 % | “Pode chover… leve guarda-chuva” |
| Chuva agora | sensor FC-37/YL-83 molhado (opcional, GPIO34) | “Chuva agora” |
| Calor | temperatura > 26 °C | “Ligue o ar-condicionado” |
| Frio | temperatura < 17 °C | “Desligue o ar-condicionado” |
| Ar seco | umidade < 30 % | “Beba água” |
| Incêndio | temperatura ≥ 45 °C, subida ≥ 4 °C em 1 min ou chama no KY-026 (opcional, GPIO39) | banner vermelho + vibração + sirene |

Regras de implementação:

- Histórico de pressão: 1 amostra por minuto, 180 amostras (3 h). A previsão de
  chuva só é emitida depois de 30 min de histórico; antes disso o app mostra
  “coletando dados”.
- Janela de incêndio: 12 amostras de temperatura a cada 5 s (1 min); compara a
  atual com a menor da janela.
- Histerese para não piscar no limite: 0,5 °C nos avisos de temperatura e 3 % no
  de umidade. O alerta de incêndio só destrava quando a temperatura cai 5 °C
  abaixo do limite e a subida fica abaixo de 1 °C/min.
- Limiares em `HardwareConfig.h` (bloco `ENV_*`).

### Sensores opcionais (ainda não ligados)

Descomente em `HardwareConfig.h`:

```c
#define IROBOT_RAIN_SENSOR_PIN   34   // FC-37/YL-83 AO (ADC1, entrada)
#define IROBOT_FLAME_SENSOR_PIN  39   // KY-026 AO (ADC1, entrada)
#define IROBOT_RAIN_WET_ADC      1800 // menor = mais molhado
#define IROBOT_FLAME_ADC         1000 // menor = chama presente
```

Use apenas pinos ADC1 (GPIO 32–39): o ADC2 não funciona com o Wi-Fi ligado. Para
detecção de incêndio mais confiável, o próximo passo é um MQ-2 (fumaça/gás) no
mesmo esquema de leitura analógica.

---

## 5. Interface web

`http://irobot.local` ou `http://192.168.4.1` (AP `Irobot-Setup` / senha
`irobot1234`). Versão minimalista: `/teste`.

- **Joystick / comandos rápidos**: frente, trás, esquerda, direita, parar.
- **Slider de velocidade geral** e sliders independentes por roda.
- **Configuração de PWM** (mínimo/máximo/padrão) salva em NVS.
- **Ambiente**: temperatura, umidade, pressão e distância, atualizados a cada 1 s.
- **Avisos**: lista com cor por severidade (crítico = incêndio/chuva agora,
  atenção = calor/chuva/ar seco, informativo = frio) e linha de tendência de
  pressão (30 min e 3 h).
- **Autônomo**: liga/desliga o desvio de obstáculos e permite ajustar a velocidade do modo autônomo.
- **Banners**: “Robô enroscado” (autônomo desligado após 100 tentativas) e
  “Possível incêndio” (vermelho). Ambos vibram e apitam; se a permissão de
  notificação estiver concedida, também geram notificação do navegador.
- **Telemetria**: status, motores, conexão e último erro de API.

Editar em `web/` e regenerar `WebAssets.h` (ver seção 8) — nunca editar o
`WebAssets.h` à mão.

---

## 6. API REST

| Endpoint | Método | Descrição |
|----------|--------|-----------|
| `/api/motor/status` | GET | direção, velocidades e se está movendo |
| `/api/motor/move` | POST | `direction` (`frente`/`tras`/`esquerda`/`direita`/`stop`) + `speed`, ou `leftSpeed`/`rightSpeed` |
| `/api/motor/stop` | POST | para tudo |
| `/api/config` | GET/POST | PWM mínimo/máximo/padrão e habilitação dos motores |
| `/api/sensors` | GET | ambiente + distance + estado do autônomo + `alerts` |
| `/api/auto` | GET | `enabled`, `state`, `tof`, `stuck`, `stuckSeq`, `avoidCount` |
| `/api/auto` | POST | `{"enabled": true/false}`; 409 `{"error":"VL53L0X ausente"}` sem ToF |
| `/api/system/info` | GET | uptime, heap, PSRAM |

Exemplo de `/api/sensors`:

```json
{
  "aht": true, "bmp": true, "tof": true,
  "temperature": 24.1, "humidity": 86.7, "pressure": 945.9,
  "bmpTemperature": 24.3, "distanceMm": 146,
  "auto": true, "autoState": "girando", "stuck": false,
  "stuckSeq": 0, "avoidCount": 3, "uptime": 48213,
  "alerts": {
    "rain": false, "rainSensor": false, "hot": false, "cold": false,
    "dry": false, "fire": false, "trendReady": false,
    "trend3h": 0.0, "trend30m": 0.0, "tempRise": 0.07
  }
}
```

---

## 7. Comandos seriais (115200 baud)

| Comando | Efeito |
|---------|--------|
| `f` / `t` / `e` / `d` / `s` | frente / trás / esquerda / direita / parar |
| `1`–`9` | velocidade de frente (10 %–90 %) |
| `i` | status |
| `r` | reiniciar |
| `a` | liga/desliga o modo autônomo |
| `m` | ambiente completo, distância bruta, estado do autônomo e avisos |

No boot o firmware imprime o motivo do reset (`esp_reset_reason`), com aviso
explícito em caso de BROWNOUT. A cada 10 s sai uma linha de saúde:

```text
[saude] heap=207548 minHeap=202372 clientesWiFi=1 auto=girando dist=163
```

Durante o giro autônomo, a cada 500 ms:

```text
[giro] dist=147 mm (bruto 147) livre=nao
```

---

## 8. Compilação e gravação

A porta COM muda a cada replug do cabo; o script detecta a porta e confere o
chip (ESP32 clássico) na hora da gravação:

```text
powershell -ExecutionPolicy Bypass -File tools/flash.ps1 -Sketch irobot
```

Só listar portas/chips (sem gravar):

```text
powershell -ExecutionPolicy Bypass -File tools/flash.ps1 -Sketch irobot -DetectOnly
```

Após editar `web/` (rodar dentro de `firmware/irobot`):

```text
powershell -ExecutionPolicy Bypass -File tools/generate_web_assets.ps1
```

O script aborta se houver 0 ou mais de 1 porta serial (aí use `-Port COMx`) e
bloqueia a gravação se o chip real não casar com o firmware.

---

## 9. Diagnóstico rápido

| Sintoma | Causa provável | Ação |
|---------|----------------|------|
| AP não aparece na bateria, mas funciona no USB | pack fraco / queda de tensão nos motores | recarregar pack, buck dedicado, 1000 µF no VIN |
| Boot com aviso de BROWNOUT | mesma causa acima | idem |
| `dist` sempre 0 apontando para o vazio | normal: sem alvo no alcance (~8191 bruto) | — |
| `Distancia bruta (mm): 8191` | nenhum obstáculo dentro do alcance | — |
| `Distancia bruta (mm): 9999` | timeout de leitura do VL53L0X | conferir I2C/alimentação |
| “VL53L0X nao encontrado no I2C” | fiação ou endereço | conferir SDA/SCL/3V3 |
| Autônomo recusado | ToF ausente | corrigir sensor antes de ligar |
| `No serial data received` no esptool | placa travada | replugar o cabo / segurar BOOT |
| Motores girando no boot | enables flutuando | pull-down 10 kΩ em GPIO27/14 |
| Tendência de pressão zerada | menos de 30 min ligado | aguardar o histórico |

---

## 10. Arquivos

| Arquivo | Função |
|---------|--------|
| `irobot.ino` | setup/loop, AP + mDNS, comandos seriais, logs de saúde |
| `HardwareConfig.h` | GPIOs, limites PWM, constantes `AUTO_*` e `ENV_*` |
| `L298NDualDriver.h/.cpp` | controle do L298N com PWM |
| `MotorEngine.h/.cpp` | comandos de direção, diferencial e pivô |
| `ConfigManager.h/.cpp` | leitura/gravação em NVS |
| `SensorService.h/.cpp` | AHT20 + BMP280 + VL53L0X, cadência e diagnóstico |
| `AutoNavigator.h/.cpp` | máquina de estados do desvio de obstáculos |
| `EnvironmentAdvisor.h/.cpp` | tendência de pressão, avisos e detecção de incêndio |
| `DashboardService.h/.cpp` | servidor HTTP e API REST |
| `WebAssets.h` | HTML/CSS/JS embarcados (gerado) |
| `web/` | fontes da interface web |
| `tools/generate_web_assets.ps1` | gera `WebAssets.h` a partir de `web/` |

---

## 11. Protótipos

### Protótipo-67

- Montagem de bancada do projeto irobot.
- Alimentado com **2 baterias 3,7 V em série (7,4 V)** no L298N, que também
  alimentou o ESP32 pela saída 5 V (`VIN`).
- Deu conta de ESP32 + drivers + motores, mas ficou **no limite** (7,4 V é o
  mínimo do L298N, que derruba ~2 V: sobram ~5 V nos motores).
- Funcionalidades testadas: web em `/` e `/teste`, comandos, PWM, AP Wi-Fi,
  sensores I2C, modos autônomo e avisos.
- Próximo passo de potência: 3 células (11,1 V) ou pack 12 V com buck para o
  ESP32, mantendo o GND comum.

---

## 12. Limites conhecidos

- Sem encoders → malha aberta: não sabe distância/ângulo exatos. O pivô do modo
  autônomo é calibrado por tempo (`AUTO_TURN_MAX_MS`) e confirmado pelo ToF.
- Motores TT → baixo torque; adequado a piso plano e robô leve.
- A roda caster absorve deriva lateral, mas exige piso liso.
- Duas rodas motrizes → menor tração; manter o centro de gravidade perto do eixo.
- No controle manual, `e`/`d` giram em torno da roda parada; só o modo autônomo
  usa pivô verdadeiro (`moveDifferential` com sinais opostos).
- O aviso de chuva é uma tendência barométrica local (6–12 h), não previsão
  meteorológica oficial; a precisão aumenta com o histórico e com o sensor de
  chuva físico.
- A detecção de incêndio é heurística (temperatura/subida de temperatura) e só
  fica confiável com sensor de chama e/ou fumaça instalados.

---

## 13. Próximos passos de hardware

- **Monitor de bateria** (planejado): divisor R1 = 100 kΩ / R2 = 33 kΩ (fator
  4,03×, máximo 13,3 V) do positivo da bateria para `GPIO35` (ADC1), com 100 nF
  para GND; consumo ~95 µA. Para pack 4S usar R2 = 22 kΩ. Expor `batteryV` em
  `/api/sensors` e alertar abaixo de ~6,9 V (2S).
- **Sensor de chuva FC-37/YL-83** no GPIO34 e **chama KY-026** no GPIO39,
  habilitando os defines da seção 4.
- **MQ-2** (fumaça/gás) para reforçar a detecção de incêndio.

---

## 14. Histórico desta entrega

| Commit | Conteúdo |
|--------|----------|
| `26f5150` | sensores AHT20/BMP280/VL53L0X + modo autônomo inicial |
| `b26d4d3` | autônomo contínuo e detecção de enroscado |
| `0f50703` | gravação na porta detectada, `lastRawMm`, mDNS |
| `1f9532a` | diagnóstico de reset, heap e Wi-Fi |
| `a163a2f` | boot sem acionar motores, TX 11 dBm |
| `5e958ad` | desvio com ré, pivô e espera por 1 m livre |
| `f8687a1` | avisos ambientais (chuva, calor, frio, ar seco, incêndio) + leitura durante o giro |
