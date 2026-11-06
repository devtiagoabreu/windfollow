# Future Upgrade — Hardware (WindFollow)

Documento de referência para evolução do hardware do projeto **WindFollow** (câmera seguidora de movimento).  
Baseado em soluções de código aberto analisadas: MatsRobot/Face-Tracking-using-ESP32-CAM, mmm1712/edge-ai-object-tracking-camera, Jarvis-X/Blob-detection-and-Tracking, s60sc/ESP32-CAM_Motion e tomasmarkey/boat-tracker.

---

## 1. Hardware atual (baseline)

| Item | Especificação | Observação |
|------|---------------|------------|
| Placa | LILYGO T-SIMCAM IR-CUT, PCB S3_CAM_SIM V1.3 | ESP32-S3 dual-core 240 MHz |
| MCU | ESP32-S3 | 16 MB Flash + 8 MB PSRAM |
| Câmera | OV5640 (confirmado no hardware) | 320×240 no modo de rastreamento |
| Driver | L298N (ponte H dupla, 2 A/canal) | Alimenta motorredutor DC |
| Motor | Motorredutor CC de veículo elétrico infantil | Tensão/corrente não identificadas |
| GPIO do L298N | IN1=45, IN2=46, ENA=21 | Modem 4G desligado, GPIO48 em LOW |

### Limitações conhecidas (baseline)
- **Motor DC sem feedback de posição**: não dá para saber em que ângulo a câmera ficou; o controle é por tempo/PWM aberto. Por isso o rastreamento para dentro de uma "zona morta", não "exatamente no centro".
- **GPIO 45/46 são strapping pins**: não podem ser puxados para HIGH durante o boot.
- **Inércia mecânica do motorredutor**: overshoot ao cortar o PWM; desaceleração natural apenas com PWM proporcional.
- **Sem eixo Y (tilt)**: só seguimento horizontal (pan).

---

## 2. Comparativo de atuadores (conforme repos open source)

Todos os repos de rastreamento ativo analisados usam **servo** (posição absoluta conhecida), não motor DC aberto:

| Atuador | Precisão | Feedback | Complexidade | Custo | Uso nos repos |
|---------|----------|----------|--------------|-------|---------------|
| Servo SG90/MG90S | Boa | Potenciômetro interno | Baixa | Baixo | MatsRobot, mmm1712 |
| Servo smart (Feetech STS3215) | Muito boa | Encoder + barramento serial | Média | Médio | tomasmarkey/boat-tracker |
| Motor DC + L298N (atual) | Baixa | Nenhum (aberto) | Média | Baixo | WindFollow (atual) |
| Motor DC + encoder externo | Boa | Encoder separado | Alta | Médio/Alto | Não encontrado nos repos |

### Recomendação
Se a prioridade é **precisão de rastreamento** (parar próximo do centro sem caça), migre para **servo** ou **servo smart**.  
Se a prioridade é **baixo custo e reuso do hardware atual**, mantenha o L298N + DC com controle proporcional e zona morta (já implementado no firmware).

---

## 3. Controladores de motor

| Controlador | Corrente contínua | Tensão | Interface | Aquecimento | Tamanho | Recomendação |
|-------------|-------------------|--------|-----------|-------------|---------|--------------|
| L298N (atual) | 2 A | 5–35 V | Paralelo (IN1/IN2/ENA) | Alto (dissipador necessário) | Grande | Manter para testes de baixo custo |
| TB6612FNG | 1,2 A (3 A pico) | 4,5–13,5 V | Paralelo + PWM | Médio | Médio | Boa alternativa compacta |
| DRV8833 | 1,2 A (2 A pico) | 2,7–10,8 V | Paralelo + PWM | Baixo | Pequeno | Ideal para espaços reduzidos e 3,3 V |

**Nota**: o L298N tem queda de tensão interna alta (~1,4 V × 2 pontes), ineficiente para motores de baixa tensão. TB6612FNG ou DRV8833 são mais eficientes e menores.

---

## 4. Driver PWM dedicado (PCA9685)

Todos os repos de rastreamento usam um controlador PWM dedicado via I2C:

| Item | PCA9685 | Alternativa (pwm nativo do ESP32-S3) |
|------|---------|--------------------------------------|
| Canal | 16 canais | Limitado pelos GPIOs disponíveis |
| Resolução | 12 bits (4096 passos) | 8–16 bits (depende do pino/timer) |
| Interface | I2C (400 kHz) | Direto nos GPIOs |
| Vantagem | Libera CPU e GPIOs da câmera | Sem hardware extra |
| Uso nos repos | MatsRobot, mmm1712 | Não usado |

**Recomendação**: adicione um **PCA9685** se migrar para servos ou precisar de controle de PWM fino em múltiplos canais (pan + tilt + iluminação IR-CUT). Comunica por I2C nos GPIOs livres (ex. 4/5, se não conflitarem com a câmera).

---

## 5. Placas de visão dedicadas (detecção fora do ESP32)

| Placa | Processamento | Câmera | Interface com ESP32 | Vantagem | Desvantagem |
|-------|--------------|--------|---------------------|----------|-------------|
| ESP32-S3 (atual) | Onboard (MCU) | OV5640 integrada | — | Baixo custo, tudo em um | CPU limitada para CV pesada |
| OpenMV H7 / H7 Plus | Onboard (STM32) | Integrada | UART / I2C / Serial | `find_blobs`, rastreamento de cor nativo, muito rápido | Mais caro, menos flexível que câmera genérica |
| Grove Vision AI V2 | Onboard (Himax) | OV5647-62 | UART / I2C | Detecção de objeto local (YOLO-like), independente do laptop | Custo médio, ecossistema Seeed |
| Raspberry Pi + USB cam | Externo (Linux) | Qualquer USB | Serial / USB / rede | OpenCV completo, YOLO, rastreamento avançado | Maior, mais caro, mais energia |
| NVIDIA Jetson Nano | Externo (GPU) | Qualquer USB/CSI | Rede / USB | Redes neurais pesadas, rastreamento robusto | Muito maior, caro, alto consumo |

**Recomendação**: mantenha a detecção no ESP32-S3 para baixo custo. Se precisar de rastreamento de objetos específicos (não apenas "qualquer movimento"), avalie **Grove Vision AI V2** ou **OpenMV H7**.

---

## 6. Câmeras

| Sensor | Resolução típica | Interface | Uso nos repos | Observação |
|--------|------------------|-----------|---------------|------------|
| OV2640 | QQVGA–UXGA | DVP (8-bit) | MatsRobot (ESP32-CAM) | Comum, barato, boa integração |
| OV5640 (atual) | QQVGA–2592×1944 | DVP (8-bit) | WindFollow (atual) | Melhor que OV2640, já confirmado na T-SIMCAM |
| Módulo USB (qualquer) | VGA–4K | USB | tomasmarkey (Mac) | Fácil de trocar, mas não embarcada no ESP32 |

**Recomendação**: mantenha o OV5640 da T-SIMCAM. Só troque se precisar de resolução maior ou foco automático.

---

## 7. Roadmap de upgrade por fases

### Fase 1 — Baixo custo (manter T-SIMCAM + L298N)
- Ajuste fino de `dead_zone`, `kp_x`, `pwm_kp`, `pwm_min/pwm_max` no firmware (já implementado).
- Adicione **fonte separada** para o motor e **GND comum** (evita brownout da ESP32).
- Adicione **jumper ENA removido** para controle real de PWM.

### Fase 2 — Médio custo (melhor precisão de atuador)
- Substitua **L298N por TB6612FNG ou DRV8833** (menor, mais eficiente, menos aquecimento).
- Adicione **PCA9685** via I2C para controle de PWM de 12 bits (libera CPU/GPIO).
- Considere adicionar **eixo Y (tilt)** com segundo atuador.

### Fase 3 — Alto custo (rastreamento de objeto específico)
- Substitua ou complemente a detecção por **OpenMV H7** ou **Grove Vision AI V2** (rastreamento de cor/blob ou objeto local).
- Ou mova a visão para **Raspberry Pi + USB cam** com OpenCV/YOLO, enviando comandos de pan/tilt para o ESP32-S3 via serial/UART.

---

## 8. Esquema de ligação sugerido (upgrade)

```
ESP32-S3 (T-SIMCAM)
├── GPIO 4 / 5  → PCA9685 SDA / SCL  (I2C, se usado)
├── GPIO 45     → Driver IN1 (L298N/TB6612FNG/DRV8833)
├── GPIO 46     → Driver IN2
├── GPIO 21     → Driver ENA / PWM
├── GPIO 48     → LOW (PCIE_PWR, modem desligado)
├── GPIO 1      → HIGH (power latch da placa)
└── GPIO 0      → Botão (não usar para atuador)

Driver de motor
├── VMS / VM    → Fonte separada do motor (ex. 12 V)
├── VCC         → 5 V (lógica do driver)
├── GND         → GND comum com ESP32 e fonte do motor
├── OUT1/OUT2   → Motor DC (fios do motorredutor)
└── ENA         → PWM do ESP32 (GPIO 21)

Fontes
├── USB-C / Li-Po → ESP32-S3 (3,3 V / 5 V lógica)
└── Fonte separada → Motor (12 V ou conforme especificação do motorredutor)
```

**Cuidados críticos**:
- **Nunca alimente o motor pelos 3,3 V ou 5 V da ESP32**. Use fonte separada.
- **GND comum** entre ESP32, driver e fonte do motor.
- **GPIO 45/46 (strapping)**: não puxe para HIGH no boot.
- **GPIO 48**: mantenha em LOW para deixar o modem 4G desligado.
- **Jumper ENA**: remova se quiser controlar velocidade por PWM; caso contrário o motor fica sempre em velocidade máxima.

---

## 9. Resumo de decisão

| Cenário | Hardware recomendado |
|---------|---------------------|
| Baixo custo, só teste | Manter T-SIMCAM + L298N + ajustes de software |
| Boa precisão, baixo custo | TB6612FNG/DRV8833 + PCA9685 + servo |
| Dois eixos (pan + tilt) | Dois servos + PCA9685 + fonte 5 V separada |
| Rastreamento de objeto específico | Grove Vision AI V2 ou OpenMV H7 + ESP32-S3 via serial |
| Máxima performance (CV pesada) | Raspberry Pi / Jetson + câmera USB + ESP32-S3 como escravo de movimento |

---

*Documento baseado em análise de repositórios open source de câmeras seguidoras (MatsRobot, mmm1712, Jarvis-X, s60sc, tomasmarkey) e nas especificações do hardware atual do WindFollow.*
