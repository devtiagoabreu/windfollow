# Simulador WindFollow

Pasta para visualizar a interface web do projeto no computador, sem precisar do ESP32.

## Como usar

Abra o arquivo `simulator/index.html` diretamente no navegador (duplo clique). Não precisa de servidor local.

## Fluxo de uso

### 1. Configuracao inicial (Wi-Fi)
Na primeira vez, o simulador mostra a pagina **Rede Wi-Fi** (setup):
- O ESP32 esta simulado em modo Access Point (AP): SSID `WindFollow-Setup`, IP `192.168.4.1`
- Clique em **Atualizar lista** para escanear redes disponíveis
- Selecione a rede ou digite o SSID manualmente
- Informe a senha e clique em **Conectar e salvar**
- Depois de conectado, o simulador libera o Dashboard e a pagina de Configuracao

### 2. Acesso pelo mDNS local
No ESP32 real, depois de configurar o Wi-Fi, voce acessa por:
- `http://windfollow.local/setup` (mDNS)
- Ou pelo IP exibido no display (ex: `http://192.168.1.45`)

No simulador isso é apenas informativo (o navegador ja esta rodando localmente).

### 3. Dashboard
- Ative a camera para detecção de movimento real via webcam
- Controles manuais: Esquerda / Parar / Direita
- Toggle: Rastreamento automático

### 4. Configuracao de hardware
Formulario com GPIOs IN1/IN2/ENA, tensao do motor, zona morta, sensibilidade, area minima, duracao do pulso, estabilizacao.
- Valida GPIOs ocupados da T-SIMCAM V1.3
- Persiste no `localStorage` (simula Preferences/NVS do ESP32)

## Observacoes

- O simulador **não se conecta a hardware real**. É apenas para validar o visual e o fluxo.
- Dados salvos no `localStorage` do navegador. Limpe o site ou use modo anônimo para resetar.
- No ESP32 real, a mesma interface sera servida pelo WebServer do firmware e os dados salvos em `Preferences (NVS)` com `ESP.restart()` após salvar GPIOs.
