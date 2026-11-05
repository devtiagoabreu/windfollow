# WindFollow

Câmera seguidora por movimento para LILYGO T-SIMCAM ESP32-S3 + L298N + motorredutor CC.

Baseado na documentação em `docs/`:
- `hardware.md` — L298N, T-SIMCAM V1.3, motorredutor
- `ideias_projeto_01.md` — estudo de viabilidade (detecção de movimento, PAN, zona morta)
- `ideias_projeto_02.md` — especificação consolidada (SmartCam OS como base, 7 fases)

## Hardware (do docs/hardware.md)

- Placa: LILYGO T-SIMCAM IR-CUT, PCB `S3_CAM_SIM V1.3`, 23/06/2022
- MCU: ESP32-S3 dual-core LX7, 240 MHz, 16 MB Flash + 8 MB PSRAM (a confirmar)
- Câmera: OV2640 ou OV5640 (a confirmar qual está instalado)
- Driver: L298N (ponte H dupla, 2 A/canal)
- Motor: motorredutor CC de veículo elétrico infantil (tensão/corrente não identificadas)

## GPIO ocupados (T-SIMCAM V1.3) — não usar para L298N

Câmera DVP: 4, 5, 8, 9, 25, 26, 36, 39, 41, 42, 43, 44, 45, 46, 48  
Modem celular: 10, 17, 18

## Simulador (visualização no computador)

Abra `simulator/index.html` no navegador para ver a interface web sem ESP32.
Inclui fluxo de setup Wi-Fi, acesso via mDNS `windfollow.local` e detecção de movimento pela webcam.

## Firmware (para ESP32 via Arduino IDE)

Pasta `firmware/`. Arquivos criados até agora:

| Arquivo | Função |
|---------|--------|
| `HardwareConfig.h` | Struct de configuração (GPIOs, tensão, zona morta, etc.) |
| `ConfigManager.h/.cpp` | Leitura/gravação em Preferences (NVS), validação de GPIO |
| `L298NDriver.h/.cpp` | Controle do L298N (esquerda/direita/parar/PWM) |
| `MotionEngine.h/.cpp` | Gerenciamento de movimento com duração de pulso |
| `MotionDetector.h/.cpp` | Detecção de diferença de quadros + centroide |

Ainda a criar: `windfollow.ino`, `ApiServer.h/.cpp`, `MotionTrackingEngine.h/.cpp`.

## Dados pendentes antes de energizar

- Sensor de câmera instalado (OV2640 vs OV5640)
- Tensão nominal, corrente e corrente de partida do motorredutor
- GPIOs livres para L298N (confirmar no esquema V1.3)
- Fonte de alimentação separada para o motor
- Limites mecânicos / fins de curso
