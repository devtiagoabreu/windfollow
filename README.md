# WindFollow

Câmera seguidora por movimento para LILYGO T-SIMCAM ESP32-S3 + L298N + motorredutor CC.

Baseado na documentação em `docs/`:
- `hardware.md` — L298N, T-SIMCAM V1.3, motorredutor
- `ideias_projeto_01.md` — estudo de viabilidade (detecção de movimento, PAN, zona morta)
- `ideias_projeto_02.md` — especificação consolidada (SmartCam OS como base, 7 fases)

## Hardware (do docs/hardware.md)

- Placa: LILYGO T-SIMCAM IR-CUT, PCB `S3_CAM_SIM V1.3`, 23/06/2022
- MCU: ESP32-S3 dual-core LX7, 240 MHz, 16 MB Flash + 8 MB PSRAM
- Câmera: OV5640 (confirmado no hardware, 320x240 no modo de rastreamento)
- Driver: L298N (ponte H dupla, 2 A/canal)
- Motor: motorredutor CC de veículo elétrico infantil (tensão/corrente a confirmar)

## GPIOs da T-SIMCAM V1.3

Câmera DVP: XCLK 14, SIOD 4, SIOC 5, D0 11, D1 9, D2 8, D3 10, D4 12, D5 17, D6 16,
D7 15, VSYNC 6, HREF 7, PCLK 13, IR-CUT 18.  
Microfone: 42, 41, 2. Cartão SD: 40, 38, 39, 47. UART do modem: 43, 44.  
PCIE/modem: 48 (PCIE_PWR), 45 (TX), 46 (RX), LED 21. Power latch: 1. Botão: 0.

**Ligação padrão do L298N neste projeto (modem 4G desligado):**

| Função | GPIO |
|--------|------|
| IN1 | 45 |
| IN2 | 46 |
| ENA (PWM) | 21 |

O firmware mantém GPIO48 em LOW no boot para desligar o modem e liberar 45/46.
GPIO45/46 são strapping pins: o L298N não pode puxá-los para HIGH no boot.
Pinos utilizáveis: 21, 45 e 46. O restante pertence à câmera, flash/PSRAM, SD,
microfone, modem ou botão.

## Rede e interface

- AP: `WindFollow-Setup` / senha `windfollow123`, IP `192.168.4.1`
- mDNS: `http://windfollow.local`
- Configuração salva em NVS (namespace `windfollow`)
- Interface web embarcada com dashboard, câmera e rastreamento por movimento

## Simulador (visualização no computador)

Abra `simulator/index.html` no navegador para ver a interface web sem ESP32.
Inclui fluxo de setup Wi-Fi, acesso via mDNS `windfollow.local` e detecção de movimento pela webcam.

## Firmware (para ESP32 via Arduino IDE)

Pasta `firmware/windfollow/`. Módulos:

| Arquivo | Função |
|---------|--------|
| `HardwareConfig.h` | Struct de configuração (GPIOs, tensão, zona morta, etc.) |
| `ConfigManager.h/.cpp` | Leitura/gravação em Preferences (NVS), migração e validação de GPIO |
| `L298NDriver.h/.cpp` | Controle do L298N (esquerda/direita/parar/PWM) |
| `MotionEngine.h/.cpp` | Gerenciamento de movimento com duração de pulso |
| `MotionDetector.h/.cpp` | Detecção de diferença de quadros + centroide |
| `CameraService.h/.cpp` | Câmera OV5640, captura JPEG, inicialização sob demanda |
| `DashboardService.h/.cpp` | Servidor HTTP, API REST e assets embarcados |
| `WebAssets.h` | HTML/CSS/JS gerados por `tools/generate_web_assets.ps1` |
| `windfollow.ino` | Setup/loop, AP + mDNS, câmera, heartbeat e comandos seriais |

Compilação/gravação (ESP32-S3, 16 MB, PSRAM OPI, USB CDC):

```text
arduino-cli compile --fqbn "esp32:esp32:esp32s3:CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600,FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi" firmware/windfollow
arduino-cli upload -p COM3 --fqbn "esp32:esp32:esp32s3:CDCOnBoot=cdc,USBMode=hwcdc,UploadSpeed=921600,FlashSize=16M,PartitionScheme=huge_app,PSRAM=opi" firmware/windfollow
```

Comandos pela serial (115200): `c` inicia a câmera, `i` mostra status, `p` captura, `r` reinicia.

Antes de regenerar o firmware após editar `web/`, rode:

```text
powershell -ExecutionPolicy Bypass -File tools/generate_web_assets.ps1
```

## Dados pendentes antes de energizar

- Tensão nominal, corrente e corrente de partida do motorredutor
- Fonte de alimentação separada para o motor (GND comum)
- Limites mecânicos / fins de curso
