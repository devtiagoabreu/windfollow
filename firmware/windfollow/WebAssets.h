#ifndef WIND_FOLLOW_WEB_ASSETS_H
#define WIND_FOLLOW_WEB_ASSETS_H

// Arquivo gerado por tools/generate_web_assets.ps1 - nao edite manualmente.
// Fonte: firmware/windfollow/web/{index.html, css/style.css, js/app.js}

static const char WIND_FOLLOW_INDEX_HTML[] PROGMEM = R"WFWEB(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WindFollow - ESP32</title>
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>
    <div id="app">
        <header>
            <h1>WindFollow</h1>
            <div class="header-actions">
                <label class="theme-switch">
                    <input type="checkbox" id="theme-toggle" onchange="sim.toggleTheme()">
                    <span class="theme-slider"></span>
                </label>
                <div id="connection-status" class="status-indicator status-off">Conectando...</div>
            </div>
        </header>

        <!-- Abas principais -->
        <nav class="tabs" id="main-tabs">
            <button class="tab active" data-tab="dashboard" onclick="sim.switchTab('dashboard', this)">
                <span class="tab-icon">📺</span> Painel de Controle
            </button>
            <button class="tab" data-tab="devices" onclick="sim.switchTab('devices', this)">
                <span class="tab-icon">⚙️</span> Configuração de Dispositivos
            </button>
            <button class="tab" data-tab="wifi" onclick="sim.switchTab('wifi', this)">
                <span class="tab-icon">📶</span> Configuração Wi-Fi
            </button>
        </nav>

        <main id="content">
            <!-- Aba: Painel de Controle -->
            <section id="tab-dashboard" class="tab-panel active">
                <div class="card">
                    <h2>Camera</h2>
                    <div class="video-placeholder">
                        <img id="cam-stream" alt="Camera WindFollow">
                        <div class="video-overlay">
                            <div class="crosshair"></div>
                            <div class="motion-box" id="motion-box" style="display:none;"></div>
                            <div class="target-dot" id="target-dot" style="display:none;"></div>
                        </div>
                        <div class="video-controls">
                            <button class="btn btn-primary btn-small" id="btn-camera" onclick="sim.toggleCamera()">Ativar camera</button>
                            <span class="camera-status" id="camera-status">Camera desativada</span>
                        </div>
                    </div>
                    <div class="status-grid">
                        <div class="status-item">
                            <span class="label">Wi-Fi</span>
                            <span class="value" id="status-wifi">Modo AP</span>
                        </div>
                        <div class="status-item">
                            <span class="label">Modo</span>
                            <span class="value" id="status-mode">Idle</span>
                        </div>
                        <div class="status-item">
                            <span class="label">Motor</span>
                            <span class="value" id="status-motor">Parado</span>
                        </div>
                        <div class="status-item">
                            <span class="label">Rastreamento</span>
                            <span class="value" id="status-tracking">Desligado</span>
                        </div>
                        <div class="status-item">
                            <span class="label">Movimento</span>
                            <span class="value" id="status-motion">Não</span>
                        </div>
                        <div class="status-item">
                            <span class="label">Coordenadas</span>
                            <span class="value" id="status-coords">—</span>
                        </div>
                    </div>

                    <!-- Indicador visual de direção -->
                    <div class="direction-card">
                        <h3>Direção do Rastreamento</h3>
                        <div class="direction-visual">
                            <div class="direction-center"></div>
                            <div class="direction-arrow" id="direction-arrow"></div>
                            <div class="direction-labels">
                                <span class="dir-label dir-left">Esquerda</span>
                                <span class="dir-label dir-center">Centro</span>
                                <span class="dir-label dir-right">Direita</span>
                            </div>
                        </div>
                        <div class="direction-zone">
                            <div class="zone-bar" id="zone-bar"></div>
                        </div>
                        <div class="direction-info">
                            <span class="info-label">Posição X:</span>
                            <span class="info-value" id="info-pos-x">—</span>
                            <span class="info-label">Erro:</span>
                            <span class="info-value" id="info-error">—</span>
                        </div>
                    </div>
                </div>
                <div class="card controls-card">
                    <h2>Controles Manuais</h2>
                    <div class="btn-group">
                        <button class="btn btn-primary" onclick="sim.moveLeft()">Esquerda</button>
                        <button class="btn btn-danger" onclick="sim.stop()">Parar</button>
                        <button class="btn btn-primary" onclick="sim.moveRight()">Direita</button>
                        <button class="btn btn-outline" onclick="sim.centerTarget()">Centro</button>
                    </div>
                    <div class="control-row">
                        <label class="toggle">
                            <input type="checkbox" id="toggle-tracking" onchange="sim.toggleTracking(this.checked)">
                            <span class="toggle-slider"></span>
                            <span class="toggle-label nowrap">Rastreamento automático</span>
                        </label>
                        <span class="control-status" id="tracking-status">Desligado</span>
                    </div>
                    <div class="camera-hint">
                        <p>📷 Mova um objeto na frente da câmera para testar o rastreamento por movimento.</p>
                        <p class="hint-detail">Caixa verde aparece quando movimento é detectado. Coordenadas e direção são mostradas no painel.</p>
                    </div>
                </div>
            </section>

            <!-- Aba: Configuração de Dispositivos -->
            <section id="tab-devices" class="tab-panel">
                <div class="card">
                    <h2>Configuracao de Hardware (L298N)</h2>
                    <p class="hint">Esses valores sao enviados para o ESP32 e salvos na memoria (NVS/Preferences).</p>
                    <form id="form-settings">
                        <div class="form-group">
                            <label>GPIO IN1 (L298N) — sentido 1</label>
                            <input type="number" id="gpio_in1" min="0" max="48" placeholder="Ex: 21">
                            <span class="helper">Sinal da ESP32 ao L298N que define o sentido de rotação. IN1=HIGH + IN2=LOW gira em um sentido.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO IN2 (L298N) — sentido 2</label>
                            <input type="number" id="gpio_in2" min="0" max="48" placeholder="Ex: 22">
                            <span class="helper">Trabalha com o IN1 para inverter o sentido. IN1=LOW + IN2=HIGH gira no sentido contrário. Ambos LOW = motor parado.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO ENA (L298N) — velocidade (PWM)</label>
                            <input type="number" id="gpio_ena" min="0" max="48" placeholder="Ex: 23">
                            <span class="helper">Controla a velocidade via PWM (0% desligado, 100% máximo). Jumper ENA do módulo deve ser removido para a ESP32 controlar por PWM.</span>
                        </div>
                        <div class="form-group">
                            <label>Tensão do motor (V)</label>
                            <input type="number" id="motor_voltage" min="0" step="0.1" placeholder="Ex: 12">
                            <span class="helper">Apenas para exibição e registro. Não é medida pela ESP32.</span>
                        </div>
                        <div class="form-group">
                            <label>Zona morta (pixels)</label>
                            <input type="number" id="dead_zone" min="0" max="200" value="30">
                            <span class="helper">Tolerância central antes de corrigir. Evita que o motor fique corrigindo continuamente por pequenas oscilações.</span>
                        </div>
                        <div class="form-group">
                            <label>Sensibilidade</label>
                            <input type="number" id="sensitivity" min="1" max="1000" value="30">
                            <span class="helper">Limiar de diferença de quadros. Menor = mais sensível a movimentos pequenos.</span>
                        </div>
                        <div class="form-group">
                            <label>Área mínima (pixels)</label>
                            <input type="number" id="min_area" min="1" max="5000" value="100">
                            <span class="helper">Ignora regiões muito pequenas (ruído do sensor).</span>
                        </div>
                        <div class="form-group">
                            <label>Duração do pulso (ms)</label>
                            <input type="number" id="step_duration_ms" min="50" max="2000" value="150">
                            <span class="helper">Tempo de acionamento do motor a cada correção.</span>
                        </div>
                        <div class="form-group">
                            <label>Estabilização (ms)</label>
                            <input type="number" id="stabilization_ms" min="0" max="5000" value="300">
                            <span class="helper">Tempo de espera após o motor parar, antes de capturar o próximo quadro.</span>
                        </div>
                        <div class="form-actions">
                            <button type="button" class="btn btn-primary" id="btn-save">Salvar configuração</button>
                            <button type="button" class="btn btn-outline" onclick="sim.validate()">Validar pinos</button>
                            <button type="button" class="btn btn-outline" onclick="sim.reset()">Reset defaults</button>
                        </div>
                        <div id="msg-result" class="msg-result"></div>
                    </form>
                    <div class="info-card motor-info">
                        <h2>Ligação L298N ↔ ESP32</h2>
                        <div class="motor-table">
                            <div class="motor-row">
                                <span class="col">L298N</span>
                                <span class="col">ESP32</span>
                                <span class="col">Função</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">IN1</span>
                                <span class="col">GPIO configurado (saída)</span>
                                <span class="col">Define sentido de rotação 1</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">IN2</span>
                                <span class="col">Outro GPIO (saída)</span>
                                <span class="col">Define sentido de rotação 2</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">ENA</span>
                                <span class="col">GPIO com PWM</span>
                                <span class="col">Controla velocidade</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">GND</span>
                                <span class="col">GND da ESP32 + GND da fonte do motor</span>
                                <span class="col">Referência comum obrigatória</span>
                            </div>
                        </div>
                        <div class="motor-warnings">
                            <p><strong>⚠ Atenção:</strong> Não escolha os GPIOs antes de conferir o esquema da revisão da sua T-SIMCAM V1.3. Pinos ocupados pela câmera/modem não podem ser usados.</p>
                            <p><strong>⚠ Alimentação:</strong> Nunca alimente o motor pelas GPIOs ou pelos 3,3V da ESP32. Use uma fonte separada adequada ao motor.</p>
                            <p><strong>⚠ GND comum:</strong> Ligue o GND da fonte do motor, do L298N e da ESP32 juntos (referência comum).</p>
                            <p><strong>⚠ Jumper ENA:</strong> Normalmente precisa ser removido para que a ESP32 controle o pino ENA por PWM.</p>
                        </div>
                    </div>
                </div>
                <div class="card info-card">
                    <h2>Pinos ocupados (T-SIMCAM V1.3)</h2>
                    <p>Nao use estes GPIOs para o L298N:</p>
                    <div class="occupied-list">
                        <span class="badge">4</span><span class="badge">5</span><span class="badge">8</span><span class="badge">9</span><span class="badge">10</span><span class="badge">17</span><span class="badge">18</span>
                        <span class="badge">25</span><span class="badge">26</span><span class="badge">36</span><span class="badge">39</span><span class="badge">40</span><span class="badge">41</span><span class="badge">42</span><span class="badge">43</span><span class="badge">44</span><span class="badge">45</span><span class="badge">46</span><span class="badge">48</span>
                    </div>
                    <p class="hint">Antes de energizar o circuito real, confirme os GPIOs livres no esquema da revisao V1.3 da sua placa.</p>
                </div>
            </section>

            <!-- Aba: Configuração Wi-Fi -->
            <section id="tab-wifi" class="tab-panel">
                <div class="card setup-card">
                    <h2>Configuracao de Rede Wi-Fi</h2>

                    <div class="wifi-mode-card">
                        <div class="mode-badge" id="wifi-mode-badge">Modo: Access Point (AP)</div>
                        <div class="mode-info">
                            <strong>SSID atual:</strong> <span id="wifi-current-ssid">WindFollow-Setup</span><br>
                            <strong>IP do ESP32:</strong> <span id="wifi-local-ip">192.168.4.1</span><br>
                            <strong>mDNS local:</strong> <span id="wifi-mdns">http://windfollow.local/</span>
                        </div>
                    </div>

                    <div class="form-group">
                        <label>Redes Wi-Fi disponíveis</label>
                        <div style="display:flex; gap:8px; align-items:center;">
                            <select id="wifi-networks" class="network-select" style="flex:1;">
                                <option value="">Procurando redes...</option>
                            </select>
                            <button class="btn btn-outline btn-small" onclick="sim.scanWifi()">Atualizar</button>
                        </div>
                    </div>

                    <div class="form-group">
                        <label>Ou digite o nome da rede (SSID)</label>
                        <input type="text" id="wifi-ssid-manual" placeholder="Nome da rede">
                    </div>

                    <div class="form-group">
                        <label>Senha do Wi-Fi</label>
                        <input type="password" id="wifi-password" placeholder="Senha">
                    </div>

                    <div class="form-actions">
                        <button type="button" class="btn btn-primary" onclick="sim.connectWifi()">Conectar e salvar</button>
                    </div>
                    <div id="wifi-msg" class="msg-result"></div>

                    <div class="setup-hint">
                        <p>Depois de conectar, acesse o ESP32 pela rede local em:</p>
                        <p><strong>http://windfollow.local/</strong></p>
                        <p>Caso o mDNS não resolva, use o IP exibido acima.</p>
                    </div>
                </div>
            </section>
        </main>
    </div>
    <script src="/js/app.js"></script>
</body>
</html>
)WFWEB";

static const char WIND_FOLLOW_STYLE_CSS[] PROGMEM = R"WFWEB(
/* ============================================================================
   DESIGN SYSTEM — WIND FOLLOW
   Tema escuro (Dracula) por padrão. Tema claro disponível via atributo data-theme="light".
   ============================================================================ */

:root {
  /* --- Dracula (escuro) — padrão --- */
  --color-bg: #282a36;
  --color-bg-elevated: #343746;
  --color-bg-hover: #44475a;
  --color-bg-input: #21222c;
  --color-text: #f8f8f2;
  --color-text-muted: #bd93f9;
  --color-border: #44475a;
  --color-border-focus: #bd93f9;

  --color-primary: #8be9fd;     /* cyan */
  --color-primary-hover: #7bc7e6;
  --color-success: #50fa7b;     /* green */
  --color-danger: #ff5555;      /* red */
  --color-warning: #f1fa8c;     /* yellow */
  --color-info: #ff79c6;        /* pink */

  --color-status-ok-bg: rgba(80, 250, 123, 0.15);
  --color-status-ok-text: #50fa7b;

  --radius: 6px;
  --shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
  --transition: all 0.2s ease;

  font-synthesis: none;
  text-rendering: optimizeLegibility;
}

[data-theme="light"] {
  /* --- Tema claro --- */
  --color-bg: #f3f4f6;
  --color-bg-elevated: #ffffff;
  --color-bg-hover: #e5e7eb;
  --color-bg-input: #ffffff;
  --color-text: #111827;
  --color-text-muted: #6b7280;
  --color-border: #d1d5db;
  --color-border-focus: #2563eb;

  --color-primary: #2563eb;
  --color-primary-hover: #1d4ed8;
  --color-success: #16a34a;
  --color-danger: #dc2626;
  --color-warning: #ca8a04;
  --color-info: #1d4ed8;

  --color-status-ok-bg: rgba(22, 163, 74, 0.15);
  --color-status-ok-text: #15803d;

  --radius: 8px;
  --shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
}

/* ============================================================================
   RESET & BASE
   ============================================================================ */
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

html {
  scroll-behavior: smooth;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Fira Code', 'Consolas', monospace;
  background: var(--color-bg);
  color: var(--color-text);
  line-height: 1.6;
  transition: background-color 0.3s ease, color 0.3s ease;
}

.simulator-badge {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  background: var(--color-warning);
  color: #1a1a1a;
  text-align: center;
  padding: 6px 12px;
  font-size: 12px;
  font-weight: 700;
  z-index: 1000;
  letter-spacing: 0.5px;
}

#app {
  max-width: 900px;
  margin: 0 auto;
  padding: 20px;
}

header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 0;
  border-bottom: 1px solid var(--color-border);
  margin-bottom: 24px;
}

header h1 {
  font-size: 24px;
  font-weight: 700;
  color: var(--color-text);
  display: flex;
  align-items: center;
  gap: 10px;
}

header h1::before {
  content: '🌙';
  font-size: 20px;
}

[data-theme="light"] header h1::before {
  content: '☀️';
}

.header-actions {
  display: flex;
  align-items: center;
  gap: 16px;
}

/* ============================================================================
   TEMA SWITCH
   ============================================================================ */
.theme-switch {
  position: relative;
  display: inline-block;
  width: 44px;
  height: 24px;
}

.theme-switch input {
  opacity: 0;
  width: 0;
  height: 0;
}

.theme-slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: var(--color-bg-hover);
  transition: 0.3s;
  border-radius: 24px;
  border: 1px solid var(--color-border);
}

.theme-slider:before {
  position: absolute;
  content: '';
  height: 18px;
  width: 18px;
  left: 3px;
  bottom: 3px;
  background: var(--color-text);
  transition: 0.3s;
  border-radius: 50%;
}

.theme-switch input:checked + .theme-slider {
  background: var(--color-primary);
}

.theme-switch input:checked + .theme-slider:before {
  transform: translateX(20px);
}

/* ============================================================================
   STATUS INDICATOR
   ============================================================================ */
.status-indicator {
  display: flex;
  align-items: center;
  gap: 6px;
  padding: 6px 12px;
  border-radius: 20px;
  font-size: 12px;
  font-weight: 600;
  background: var(--color-status-ok-bg);
  color: var(--color-status-ok-text);
  border: 1px solid var(--color-success);
}

/* ============================================================================
   TABS (ABAS)
   ============================================================================ */
.tabs {
  display: flex;
  gap: 4px;
  margin-bottom: 24px;
  border-bottom: 2px solid var(--color-border);
}

.tab {
  padding: 12px 20px;
  border: none;
  border-bottom: 2px solid transparent;
  background: var(--color-bg-elevated);
  color: var(--color-text-muted);
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: var(--transition);
  border-radius: var(--radius) var(--radius) 0 0;
  margin-bottom: -2px;
}

.tab:hover {
  background: var(--color-bg-hover);
  color: var(--color-text);
}

.tab.active {
  background: var(--color-bg-elevated);
  color: var(--color-primary);
  border-bottom: 2px solid var(--color-primary);
  border-color: var(--color-border) var(--color-border) var(--color-primary);
}

.tab-icon {
  margin-right: 6px;
}

/* ============================================================================
   PANELS
   ============================================================================ */
.tab-panel {
  display: none;
}

.tab-panel.active {
  display: block;
}

main {
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.card {
  background: var(--color-bg-elevated);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  padding: 20px;
  box-shadow: var(--shadow);
  transition: var(--transition);
}

.card h2 {
  font-size: 18px;
  font-weight: 600;
  margin-bottom: 16px;
  color: var(--color-text);
  display: flex;
  align-items: center;
  gap: 8px;
}

.hint {
  font-size: 13px;
  color: var(--color-text-muted);
  margin-bottom: 16px;
}

/* ============================================================================
   VIDEO / CAMERA
   ============================================================================ */
.video-placeholder {
  width: 100%;
  max-width: 640px;
  height: 360px;
  background: #000;
  border-radius: var(--radius);
  position: relative;
  overflow: hidden;
  margin-bottom: 16px;
}

.video-placeholder video {
  width: 100%;
  height: 100%;
  object-fit: cover;
  display: block;
}

.video-overlay {
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  pointer-events: none;
}

.video-controls {
  position: absolute;
  bottom: 10px;
  left: 10px;
  display: flex;
  align-items: center;
  gap: 10px;
  background: rgba(0, 0, 0, 0.5);
  padding: 6px 10px;
  border-radius: 20px;
  pointer-events: auto;
  z-index: 10;
  border: 1px solid rgba(255,255,255,0.1);
}

.camera-status {
  color: #fff;
  font-size: 12px;
  font-weight: 600;
}

.camera-on {
  color: var(--color-success);
}

.crosshair {
  position: absolute;
  top: 50%;
  left: 50%;
  width: 20px;
  height: 20px;
  border: 2px solid var(--color-danger);
  border-radius: 50%;
  transform: translate(-50%, -50%);
  pointer-events: none;
}

.motion-box {
  position: absolute;
  border: 2px solid var(--color-success);
  pointer-events: none;
  box-shadow: 0 0 8px rgba(80, 250, 123, 0.5);
  background: rgba(80, 250, 123, 0.15);
}

.target-dot {
  position: absolute;
  width: 14px;
  height: 14px;
  background: var(--color-info);
  border: 3px solid #fff;
  border-radius: 50%;
  pointer-events: none;
  z-index: 15;
  box-shadow: 0 0 10px var(--color-info);
  transform: translate(-50%, -50%);
}

/* ============================================================================
   MOBILE FIRST - Responsividade
   ============================================================================ */
@media (max-width: 640px) {
  #app {
    padding: 10px;
  }

  header {
    flex-direction: column;
    gap: 12px;
    align-items: stretch;
    text-align: center;
  }

  .header-actions {
    justify-content: center;
    gap: 12px;
    flex-wrap: wrap;
  }

  header h1 {
    font-size: 20px;
  }

  .tabs {
    flex-wrap: wrap;
  }

  .tab {
    flex: 1;
    text-align: center;
    padding: 10px 6px;
    font-size: 13px;
  }

  .tab-icon {
    margin-right: 4px;
  }

  .card {
    padding: 16px;
  }

  .video-placeholder {
    max-width: 100%;
    height: 240px;
  }

  .btn-group {
    flex-direction: column;
    gap: 8px;
  }

  .btn {
    width: 100%;
    text-align: center;
  }

  .control-row {
    flex-direction: column;
    align-items: flex-start;
    gap: 10px;
  }

  .control-status {
    margin-left: 0;
    align-self: flex-start;
  }

  .form-actions {
    flex-direction: column;
  }

  .form-actions .btn {
    width: 100%;
  }

  .direction-card {
    margin-top: 16px;
    padding: 16px;
  }

  .direction-visual {
    height: 120px;
  }

  .direction-info {
    flex-direction: column;
    gap: 8px;
  }

  .info-value {
    min-width: auto;
  }

  .status-grid {
    grid-template-columns: 1fr;
  }

  .network-select {
    width: 100%;
  }

  .wifi-mode-card {
    flex-direction: column;
    gap: 10px;
    align-items: flex-start;
  }

  .mode-badge {
    align-self: flex-start;
  }
}

@media (min-width: 641px) and (max-width: 1024px) {
  .tab {
    padding: 10px 14px;
    font-size: 14px;
  }
  .card {
    padding: 18px;
  }
}

.motion-detected {
  color: var(--color-success);
  font-weight: 700;
}

.direction-left {
  color: var(--color-info);
  font-weight: 700;
}

.direction-right {
  color: var(--color-warning);
  font-weight: 700;
}

/* ============================================================================
   STATUS GRID
   ============================================================================ */
.status-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(140px, 1fr));
  gap: 12px;
}

.status-item {
  background: var(--color-bg-hover);
  padding: 12px;
  border-radius: var(--radius);
  border: 1px solid var(--color-border);
}

.status-item .label {
  font-size: 11px;
  color: var(--color-text-muted);
  display: block;
  margin-bottom: 4px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.status-item .value {
  font-size: 14px;
  font-weight: 600;
  color: var(--color-text);
}

/* ============================================================================
   CONTROLES
   ============================================================================ */
.controls-card {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.btn-group {
  display: flex;
  gap: 12px;
  flex-wrap: wrap;
}

.btn {
  padding: 10px 20px;
  border: none;
  border-radius: var(--radius);
  font-size: 14px;
  font-weight: 600;
  cursor: pointer;
  transition: var(--transition);
}

.btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.btn-small {
  padding: 6px 12px;
  font-size: 12px;
}

.btn-primary {
  background: var(--color-primary);
  color: #1a1a1a;
}

.btn-primary:hover:not(:disabled) {
  background: var(--color-primary-hover);
  transform: translateY(-1px);
}

.btn-danger {
  background: var(--color-danger);
  color: #fff;
}

.btn-danger:hover:not(:disabled) {
  background: #ff3333;
  transform: translateY(-1px);
}

.btn-outline {
  background: transparent;
  color: var(--color-text);
  border: 1px solid var(--color-border);
}

.btn-outline:hover {
  background: var(--color-bg-hover);
  border-color: var(--color-primary);
}

/* Toggle switch */
.toggle-group {
  display: flex;
  align-items: center;
  gap: 12px;
}

.toggle {
  position: relative;
  display: inline-block;
  width: 44px;
  height: 24px;
}

.toggle input {
  opacity: 0;
  width: 0;
  height: 0;
}

.toggle-slider {
  position: absolute;
  cursor: pointer;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: var(--color-bg-hover);
  transition: 0.3s;
  border-radius: 24px;
  border: 1px solid var(--color-border);
}

.toggle-slider:before {
  position: absolute;
  content: '';
  height: 18px;
  width: 18px;
  left: 3px;
  bottom: 3px;
  background: var(--color-text);
  transition: 0.3s;
  border-radius: 50%;
}

.toggle input:checked + .toggle-slider {
  background: var(--color-primary);
}

.toggle input:checked + .toggle-slider:before {
  transform: translateX(20px);
}

.control-row {
  display: flex;
  align-items: center;
  gap: 12px;
  margin-bottom: 12px;
}

.control-label {
  font-size: 14px;
  color: var(--color-text);
}

.control-status {
  margin-left: auto;
  font-size: 13px;
  font-weight: 600;
  color: var(--color-primary);
}

.control-status.status-on {
  color: var(--color-success);
}

.control-status.status-off {
  color: var(--color-text-muted);
}

.hint-detail {
  font-size: 12px;
  color: var(--color-text-muted);
  margin-top: 4px;
}

.toggle-label {
  font-size: 14px;
  color: var(--color-text);
  margin-left: 6px;
  white-space: nowrap;
}

.camera-hint {
  margin-top: 16px;
  font-size: 13px;
  color: var(--color-text-muted);
  background: var(--color-bg-hover);
  padding: 12px 16px;
  border-radius: var(--radius);
  border: 1px solid var(--color-border);
}

/* ============================================================================
   FORMULÁRIOS
   ============================================================================ */
.form-group {
  margin-bottom: 16px;
}

.form-group label {
  display: block;
  font-size: 14px;
  font-weight: 600;
  margin-bottom: 6px;
  color: var(--color-text);
}

.form-group input,
.form-group select {
  width: 100%;
  padding: 10px 12px;
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  font-size: 14px;
  background: var(--color-bg-input);
  color: var(--color-text);
  transition: var(--transition);
}

.form-group input:focus,
.form-group select:focus {
  outline: none;
  border-color: var(--color-border-focus);
  box-shadow: 0 0 0 3px rgba(189, 147, 249, 0.1);
}

.helper {
  font-size: 12px;
  color: var(--color-text-muted);
  margin-top: 4px;
  display: block;
}

.form-actions {
  display: flex;
  gap: 10px;
  margin-top: 8px;
  flex-wrap: wrap;
}

/* ============================================================================
   MSG RESULT
   ============================================================================ */
.msg-result {
  margin-top: 12px;
  padding: 10px;
  border-radius: var(--radius);
  font-size: 13px;
  display: none;
  border: 1px solid transparent;
}

.msg-result.success {
  background: var(--color-status-ok-bg);
  color: var(--color-status-ok-text);
  border-color: var(--color-success);
}

.msg-result.error {
  background: rgba(255, 85, 85, 0.15);
  color: var(--color-danger);
  border-color: var(--color-danger);
}

.msg-result.info {
  background: rgba(255, 121, 198, 0.15);
  color: var(--color-info);
  border-color: var(--color-info);
}

/* ============================================================================
   WIFI SETUP CARD
   ============================================================================ */
.setup-card {
  max-width: 520px;
}

.wifi-mode-card {
  background: var(--color-bg-hover);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  padding: 14px;
  margin-bottom: 16px;
  display: flex;
  justify-content: space-between;
  align-items: flex-start;
  gap: 12px;
}

.mode-badge {
  background: var(--color-primary);
  color: #1a1a1a;
  font-size: 11px;
  font-weight: 700;
  padding: 4px 8px;
  border-radius: 12px;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.mode-info {
  font-size: 13px;
  color: var(--color-text-muted);
  line-height: 1.6;
}

.mode-info strong {
  color: var(--color-text);
}

.network-select {
  width: 100%;
  padding: 10px 12px;
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  font-size: 14px;
  background: var(--color-bg-input);
  color: var(--color-text);
  margin-top: 6px;
}

.setup-hint {
  margin-top: 16px;
  font-size: 13px;
  color: var(--color-text-muted);
  background: var(--color-bg-hover);
  padding: 12px;
  border-radius: var(--radius);
  border: 1px solid var(--color-border);
}

.setup-hint p {
  margin-bottom: 8px;
}

.setup-hint p:last-child {
  margin-bottom: 0;
}

.setup-hint strong {
  color: var(--color-text);
}

/* ============================================================================
   PINS OCCUPIED
   ============================================================================ */
.occupied-list {
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
  margin-bottom: 12px;
}

.occupied-list .badge {
  background: var(--color-bg-hover);
  color: var(--color-text-muted);
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 12px;
  font-weight: 600;
  border: 1px solid var(--color-border);
}

.info-card .hint {
  font-size: 12px;
  color: var(--color-text-muted);
  border-top: 1px solid var(--color-border);
  padding-top: 12px;
  margin-top: 12px;
}

/* ============================================================================
   DIREÇÃO VISUAL
   ============================================================================ */
.direction-card {
  background: var(--color-bg-elevated);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  padding: 20px;
  box-shadow: var(--shadow);
  margin-top: 20px;
}

.direction-card h3 {
  font-size: 16px;
  font-weight: 600;
  margin-bottom: 16px;
  color: var(--color-text);
  display: flex;
  align-items: center;
  gap: 8px;
}

.direction-visual {
  position: relative;
  width: 100%;
  height: 140px;
  background: var(--color-bg-hover);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  margin-bottom: 12px;
  overflow: hidden;
}

.direction-center {
  position: absolute;
  left: 50%;
  top: 50%;
  width: 10px;
  height: 10px;
  background: var(--color-primary);
  border-radius: 50%;
  transform: translate(-50%, -50%);
  z-index: 10;
  box-shadow: 0 0 10px var(--color-primary);
}

.direction-arrow {
  position: absolute;
  left: 50%;
  top: 50%;
  width: 0;
  height: 0;
  border-left: 8px solid transparent;
  border-right: 8px solid transparent;
  border-top: 20px solid var(--color-success);
  transform: translate(-50%, -50%);
  transition: left 0.1s ease;
  z-index: 20;
}

.direction-labels {
  position: absolute;
  bottom: 8px;
  left: 0;
  right: 0;
  display: flex;
  justify-content: space-between;
  font-size: 11px;
  font-weight: 600;
  color: var(--color-text-muted);
  pointer-events: none;
}

.dir-label {
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.direction-zone {
  height: 24px;
  background: var(--color-bg-input);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  margin-bottom: 16px;
  overflow: hidden;
  position: relative;
}

.zone-bar {
  position: absolute;
  left: 50%;
  top: 0;
  bottom: 0;
  width: 4px;
  background: var(--color-success);
  transform: translateX(-50%);
  transition: left 0.1s ease;
  z-index: 5;
}

.direction-info {
  display: flex;
  gap: 24px;
  font-size: 13px;
}

.info-label {
  color: var(--color-text-muted);
  font-size: 12px;
}

.info-value {
  font-family: 'Fira Code', 'Consolas', monospace;
  color: var(--color-text);
  font-weight: 600;
  min-width: 60px;
  display: inline-block;
}

/* ============================================================================
   MOTOR INFO CARD
   ============================================================================ */
.motor-info {
  margin-top: 24px;
}

.motor-table {
  display: flex;
  flex-direction: column;
  gap: 6px;
  margin-bottom: 16px;
}

.motor-row {
  display: grid;
  grid-template-columns: 120px 1fr 1fr;
  gap: 12px;
  padding: 8px 10px;
  background: var(--color-bg-input);
  border: 1px solid var(--color-border);
  border-radius: var(--radius);
  font-size: 13px;
  color: var(--color-text);
}

.motor-row .col {
  overflow: hidden;
  text-overflow: ellipsis;
}

.motor-row:first-child {
  background: var(--color-bg-hover);
  font-weight: 600;
  color: var(--color-primary);
  border-bottom: 2px solid var(--color-border);
}

.motor-warnings {
  background: rgba(255, 121, 198, 0.1);
  border: 1px solid var(--color-info);
  border-radius: var(--radius);
  padding: 14px;
}

.motor-warnings p {
  margin-bottom: 10px;
  font-size: 13px;
  color: var(--color-text-muted);
  line-height: 1.5;
}

.motor-warnings p:last-child {
  margin-bottom: 0;
}

.motor-warnings strong {
  color: var(--color-info);
}

/* ============================================================================
   SCROLLBAR
   ============================================================================ */
::-webkit-scrollbar {
  width: 8px;
  height: 8px;
}

::-webkit-scrollbar-track {
  background: var(--color-bg);
}

::-webkit-scrollbar-thumb {
  background: var(--color-bg-hover);
  border-radius: 4px;
}

::-webkit-scrollbar-thumb:hover {
  background: var(--color-border);
}

/* ===== ESP32: camera stream (JPEG via /camera/capture) ===== */
.video-placeholder {
  aspect-ratio: 4 / 3;
  height: auto;
}

.video-placeholder img {
  width: 100%;
  height: 100%;
  object-fit: contain;
  display: block;
  background: #000;
}

.status-indicator.status-off {
  background: rgba(255, 85, 85, 0.15);
  color: var(--color-danger);
  border-color: var(--color-danger);
}
)WFWEB";

static const char WIND_FOLLOW_APP_JS[] PROGMEM = R"WFWEB(
// WindFollow — interface do ESP32 (T-SIMCAM + L298N)
// A UI e a mesma do simulador; aqui os dados vem do firmware via API REST.

function varCss(name) {
    return getComputedStyle(document.documentElement).getPropertyValue(name).trim();
}

async function api(method, path, body) {
    const opts = { method, headers: {} };
    if (body !== undefined) {
        opts.headers['Content-Type'] = 'application/json';
        opts.body = JSON.stringify(body);
    }
    try {
        const r = await fetch(path, opts);
        const txt = await r.text();
        try {
            return JSON.parse(txt);
        } catch (e) {
            return { status: r.ok ? 'ok' : 'error', raw: txt };
        }
    } catch (e) {
        return { status: 'error', error: 'offline' };
    }
}

const sim = {
    config: {
        gpio_in1: -1,
        gpio_in2: -1,
        gpio_ena: -1,
        motor_voltage: 0,
        motor_enabled: false,
        dead_zone: 30,
        sensitivity: 30,
        min_area: 100,
        step_duration_ms: 150,
        stabilization_ms: 300,
        auto_tracking: false
    },

    occupiedGpios: [4, 5, 8, 9, 10, 17, 18, 25, 26, 36, 39, 40, 41, 42, 43, 44, 45, 46, 48],

    cameraOn: false,
    cameraReady: false,
    captureTimer: null,
    pollTimer: null,

    init() {
        this.setupTheme();
        this.setupForm();
        this.loadConfig();
        this.pollStatus();
        this.pollTimer = setInterval(() => this.pollStatus(), 700);
    },

    setupTheme() {
        const toggle = document.getElementById('theme-toggle');
        const saved = localStorage.getItem('windfollow_theme');
        if (saved === 'light') {
            document.documentElement.setAttribute('data-theme', 'light');
            toggle.checked = true;
        } else {
            document.documentElement.removeAttribute('data-theme');
            toggle.checked = false;
        }
    },

    toggleTheme() {
        const isLight = document.getElementById('theme-toggle').checked;
        if (isLight) {
            document.documentElement.setAttribute('data-theme', 'light');
            localStorage.setItem('windfollow_theme', 'light');
        } else {
            document.documentElement.removeAttribute('data-theme');
            localStorage.setItem('windfollow_theme', 'dark');
        }
    },

    switchTab(tabId, btn) {
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        if (btn) btn.classList.add('active');
        document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
        document.getElementById('tab-' + tabId).classList.add('active');
    },

    // ============ CONFIGURACAO ============
    async loadConfig() {
        try {
            const cfg = await api('GET', '/api/config');
            if (cfg && cfg.gpio_in1 !== undefined) {
                this.config = Object.assign(this.config, cfg);
            }
        } catch (e) {
            console.warn('Falha ao carregar config', e);
        }
        this.renderConfig();
    },

    renderConfig() {
        const setVal = (id, val) => {
            const el = document.getElementById(id);
            if (el) el.value = val >= 0 ? val : '';
        };
        setVal('gpio_in1', this.config.gpio_in1);
        setVal('gpio_in2', this.config.gpio_in2);
        setVal('gpio_ena', this.config.gpio_ena);
        const setNum = (id, val) => {
            const el = document.getElementById(id);
            if (el) el.value = val;
        };
        setNum('motor_voltage', this.config.motor_voltage);
        setNum('dead_zone', this.config.dead_zone);
        setNum('sensitivity', this.config.sensitivity);
        setNum('min_area', this.config.min_area);
        setNum('step_duration_ms', this.config.step_duration_ms);
        setNum('stabilization_ms', this.config.stabilization_ms);

        const toggle = document.getElementById('toggle-tracking');
        if (toggle) toggle.checked = !!this.config.auto_tracking;
        this.updateTrackingStatusUI();
    },

    setupForm() {
        const form = document.getElementById('form-settings');
        if (!form) return;
        form.querySelectorAll('input').forEach(input => {
            input.addEventListener('change', (e) => {
                const key = e.target.id;
                if (!key) return;
                let val;
                if (key === 'motor_voltage') val = parseFloat(e.target.value);
                else val = parseInt(e.target.value);
                if (key === 'gpio_in1' || key === 'gpio_in2' || key === 'gpio_ena') {
                    if (e.target.value === '' || isNaN(val)) val = -1;
                }
                if (!isNaN(val)) this.config[key] = val;
            });
        });
        document.getElementById('btn-save').addEventListener('click', () => this.save());
    },

    async save() {
        const btn = document.getElementById('btn-save');
        const msg = document.getElementById('msg-result');

        const error = this.validateConfig(this.config);
        if (error) {
            this.showMsg(msg, error, 'error');
            return;
        }

        btn.disabled = true;
        btn.textContent = 'Salvando...';

        const r = await api('POST', '/api/config', this.config);

        if (r && r.status === 'ok') {
            this.showMsg(msg, r.message || 'Configuracao salva.', 'success');
            btn.textContent = 'Salvo!';
            btn.classList.remove('btn-primary');
            btn.classList.add('btn-outline');
            if (r.restart) {
                this.showMsg(msg, r.message, 'info');
            }
        } else {
            this.showMsg(msg, (r && (r.error || r.message)) || 'Falha ao salvar.', 'error');
        }

        setTimeout(() => {
            btn.disabled = false;
            btn.textContent = 'Salvar configuração';
            btn.classList.remove('btn-outline');
            btn.classList.add('btn-primary');
        }, 2500);
        setTimeout(() => { msg.style.display = 'none'; }, 6000);
    },

    validateConfig(cfg) {
        const gpioCheck = (g, name) => {
            if (g < 0 || g > 48) return name + ' invalido';
            if (this.occupiedGpios.includes(g)) return name + ' ocupado pela camera/modem';
            return null;
        };
        const e1 = gpioCheck(cfg.gpio_in1, 'GPIO IN1');
        if (e1) return e1;
        const e2 = gpioCheck(cfg.gpio_in2, 'GPIO IN2');
        if (e2) return e2;
        const e3 = gpioCheck(cfg.gpio_ena, 'GPIO ENA');
        if (e3) return e3;
        if (cfg.gpio_in1 >= 0 && cfg.gpio_in2 >= 0 && cfg.gpio_in1 === cfg.gpio_in2) return 'IN1 e IN2 devem ser diferentes';
        if (cfg.gpio_in1 >= 0 && cfg.gpio_ena >= 0 && cfg.gpio_in1 === cfg.gpio_ena) return 'IN1 e ENA devem ser diferentes';
        if (cfg.gpio_in2 >= 0 && cfg.gpio_ena >= 0 && cfg.gpio_in2 === cfg.gpio_ena) return 'IN2 e ENA devem ser diferentes';
        if (!(cfg.motor_voltage > 0)) return 'Tensao do motor deve ser maior que 0';
        return null;
    },

    validate() {
        const msg = document.getElementById('msg-result');
        const error = this.validateConfig(this.config);
        if (error) this.showMsg(msg, error, 'error');
        else this.showMsg(msg, 'Pinos validos e disponiveis.', 'success');
        setTimeout(() => { msg.style.display = 'none'; }, 4000);
    },

    async reset() {
        this.config = {
            gpio_in1: -1, gpio_in2: -1, gpio_ena: -1,
            motor_voltage: 0, motor_enabled: false,
            dead_zone: 30, sensitivity: 30, min_area: 100,
            step_duration_ms: 150, stabilization_ms: 300,
            auto_tracking: false
        };
        this.renderConfig();
        const msg = document.getElementById('msg-result');
        this.showMsg(msg, 'Defaults restaurados. Enviando ao ESP32...', 'info');
        const r = await api('POST', '/api/config', this.config);
        if (r && r.status === 'ok') this.showMsg(msg, r.message || 'Defaults salvos.', 'success');
    },

    // ============ CAMERA ============
    async toggleCamera() {
        if (this.cameraOn) {
            await api('POST', '/api/camera/stop');
            this.setCameraUI(false);
        } else {
            const r = await api('POST', '/api/camera/start');
            if (!r || !r.ready) {
                const msg = document.getElementById('msg-result');
                this.showMsg(msg, (r && r.message) || 'Camera indisponivel no firmware.', 'error');
                return;
            }
            this.setCameraUI(true);
        }
    },

    setCameraUI(on) {
        this.cameraOn = on;
        const img = document.getElementById('cam-stream');
        const btn = document.getElementById('btn-camera');
        const st = document.getElementById('camera-status');

        if (on) {
            const tick = async () => {
                try {
                    const r = await fetch('/camera/capture?t=' + Date.now(), { cache: 'no-store' });
                    if (!r.ok) return;
                    const b64 = await r.text();
                    if (b64 && b64.length > 100) img.src = 'data:image/jpeg;base64,' + b64;
                } catch (e) { /* mantem o ultimo quadro */ }
            };
            tick();
            this.captureTimer = setInterval(tick, 350);
            btn.textContent = 'Desativar camera';
            st.textContent = 'Camera ativa';
            st.className = 'camera-status camera-on';
        } else {
            if (this.captureTimer) { clearInterval(this.captureTimer); this.captureTimer = null; }
            img.removeAttribute('src');
            btn.textContent = 'Ativar camera';
            st.textContent = 'Camera desativada';
            st.className = 'camera-status';
            document.getElementById('motion-box').style.display = 'none';
            document.getElementById('target-dot').style.display = 'none';
        }
    },

    // ============ CONTROLES ============
    async moveLeft() {
        await api('POST', '/api/motion/move', { direction: 'esquerda', duration_ms: this.config.step_duration_ms || 150 });
        this.pollStatus();
    },

    async moveRight() {
        await api('POST', '/api/motion/move', { direction: 'direita', duration_ms: this.config.step_duration_ms || 150 });
        this.pollStatus();
    },

    async stop() {
        await api('POST', '/api/motion/stop');
        this.pollStatus();
    },

    async centerTarget() {
        await api('POST', '/api/motion/stop');
        document.getElementById('target-dot').style.display = 'none';
        document.getElementById('motion-box').style.display = 'none';
        document.getElementById('status-motor').textContent = 'Centralizado';
        this.resetDirectionVisual();
    },

    async toggleTracking(enabled) {
        await api('POST', enabled ? '/api/tracking/start' : '/api/tracking/stop');
        this.config.auto_tracking = enabled;
        this.updateTrackingStatusUI();
        const msg = document.getElementById('msg-result');
        this.showMsg(msg, 'Rastreamento ' + (enabled ? 'ativado' : 'desativado'), 'info');
        setTimeout(() => { msg.style.display = 'none'; }, 1500);
        this.pollStatus();
    },

    updateTrackingStatusUI() {
        const statusEl = document.getElementById('tracking-status');
        if (!statusEl) return;
        statusEl.textContent = this.config.auto_tracking ? 'Ligado' : 'Desligado';
        statusEl.className = 'control-status ' + (this.config.auto_tracking ? 'status-on' : 'status-off');
    },

    // ============ WI-FI ============
    async scanWifi() {
        const select = document.getElementById('wifi-networks');
        const msg = document.getElementById('wifi-msg');
        select.innerHTML = '<option value="">Procurando redes...</option>';
        const r = await api('GET', '/api/wifi/scan');
        select.innerHTML = '<option value="">-- Selecione uma rede --</option>';
        const nets = (r && r.networks) || [];
        nets.forEach(n => {
            const opt = document.createElement('option');
            opt.value = n.ssid;
            opt.textContent = n.ssid + ' (' + n.rssi + ' dBm)';
            select.appendChild(opt);
        });
        this.showMsg(msg, nets.length + ' redes encontradas', 'info');
        setTimeout(() => { msg.style.display = 'none'; }, 2500);
    },

    async connectWifi() {
        const msg = document.getElementById('wifi-msg');
        const select = document.getElementById('wifi-networks');
        const manual = document.getElementById('wifi-ssid-manual');
        const pass = document.getElementById('wifi-password');
        const btn = document.querySelector('button[onclick="sim.connectWifi()"]');

        const ssid = (select.value || manual.value || '').trim();
        const password = pass.value || '';

        if (!ssid) {
            this.showMsg(msg, 'Informe o nome da rede (SSID)', 'error');
            return;
        }

        const orig = btn.textContent;
        btn.disabled = true;
        btn.textContent = 'Conectando...';

        const r = await api('POST', '/api/wifi/config', { ssid: ssid, password: password });

        if (r && r.status === 'ok') {
            this.showMsg(msg, r.message || ('Salvo: ' + ssid), 'success');
            setTimeout(() => this.pollStatus(), 3000);
        } else {
            this.showMsg(msg, (r && (r.error || r.message)) || 'Falha ao salvar rede.', 'error');
        }

        btn.disabled = false;
        btn.textContent = orig;
        setTimeout(() => { msg.style.display = 'none'; }, 6000);
    },

    updateWifiUI(w) {
        if (!w) return;
        const badge = document.getElementById('wifi-mode-badge');
        const ssidEl = document.getElementById('wifi-current-ssid');
        const ipEl = document.getElementById('wifi-local-ip');
        const mdnsEl = document.getElementById('wifi-mdns');
        const wifiStatus = document.getElementById('status-wifi');

        if (w.connected) {
            badge.textContent = 'Modo: Estacao (STA)';
            badge.style.background = '#16a34a';
            ssidEl.textContent = w.ssid || '—';
            ipEl.textContent = w.ip || '—';
            wifiStatus.textContent = 'Conectado (' + (w.ip || '?') + ')';
        } else {
            badge.textContent = 'Modo: Access Point (AP)';
            badge.style.background = '#2563eb';
            ssidEl.textContent = 'WindFollow-Setup';
            ipEl.textContent = w.apIp || '192.168.4.1';
            wifiStatus.textContent = 'Modo AP (sem Wi-Fi)';
        }
        mdnsEl.textContent = 'http://' + (w.mdns || 'windfollow.local') + '/';
    },

    // ============ STATUS / POLLING ============
    async pollStatus() {
        const s = await api('GET', '/api/tracking/status');
        if (!s || s.status === 'error') {
            const conn = document.getElementById('connection-status');
            conn.textContent = 'ESP32 offline';
            conn.className = 'status-indicator status-off';
            return;
        }

        const conn = document.getElementById('connection-status');
        conn.textContent = 'ESP32 online';
        conn.className = 'status-indicator status-ok';
        this.status = s;

        this.updateWifiUI(s.wifi);
        document.getElementById('status-mode').textContent = s.tracking ? 'Rastreamento' : 'Idle';
        document.getElementById('status-tracking').textContent = s.tracking ? 'Ativo' : 'Desligado';
        document.getElementById('status-motor').textContent =
            (s.motor && s.motor.moving)
                ? (s.motor.direction === 'esquerda' ? 'Movendo esquerda' : 'Movendo direita')
                : 'Parado';

        document.getElementById('toggle-tracking').checked = !!s.tracking;
        this.config.auto_tracking = !!s.tracking;
        this.updateTrackingStatusUI();

        if (s.camera) {
            this.cameraReady = !!s.camera.ready;
            if (!s.camera.ready) {
                document.getElementById('camera-status').textContent = 'Camera indisponivel';
            } else if (this.cameraOn) {
                document.getElementById('camera-status').textContent = 'Camera ativa (' + (s.camera.frames || 0) + ' frames)';
                document.getElementById('camera-status').className = 'camera-status camera-on';
            }
        }

        this.updateMotionVisual(s);
    },

    updateMotionVisual(s) {
        const box = document.getElementById('motion-box');
        const dot = document.getElementById('target-dot');
        const motionEl = document.getElementById('status-motion');
        const coordsEl = document.getElementById('status-coords');
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        const posX = document.getElementById('info-pos-x');
        const err = document.getElementById('info-error');

        const valid = s.motion && s.frameWidth > 0 && s.box && s.box.w > 0;
        if (!valid) {
            box.style.display = 'none';
            dot.style.display = 'none';
            motionEl.textContent = 'Não';
            motionEl.className = 'value';
            coordsEl.textContent = '—';
            this.resetDirectionVisual();
            return;
        }

        const fw = s.frameWidth;
        const fh = s.frameHeight;

        box.style.display = 'block';
        box.style.left = (s.box.x / fw * 100) + '%';
        box.style.top = (s.box.y / fh * 100) + '%';
        box.style.width = (s.box.w / fw * 100) + '%';
        box.style.height = (s.box.h / fh * 100) + '%';

        dot.style.display = 'block';
        dot.style.left = (s.centerX / fw * 100) + '%';
        dot.style.top = (s.centerY / fh * 100) + '%';

        motionEl.textContent = 'Sim';
        motionEl.className = 'value motion-detected';
        coordsEl.textContent = Math.round(s.centerX) + ', ' + Math.round(s.centerY);

        const percentX = (s.errorX / (fw / 2)) * 100;
        const left = 50 + percentX / 2;
        arrow.style.left = left + '%';
        zoneBar.style.left = left + '%';

        const dead = this.config.dead_zone;
        if (Math.abs(s.errorX) <= dead) {
            arrow.style.borderTopColor = varCss('--color-success');
            posX.style.color = varCss('--color-success');
            err.style.color = varCss('--color-success');
        } else if (s.errorX < 0) {
            arrow.style.borderTopColor = varCss('--color-info');
            posX.style.color = varCss('--color-info');
            err.style.color = varCss('--color-info');
        } else {
            arrow.style.borderTopColor = varCss('--color-warning');
            posX.style.color = varCss('--color-warning');
            err.style.color = varCss('--color-warning');
        }

        posX.textContent = Math.round(s.centerX);
        err.textContent = Math.round(s.errorX) + ' px';
    },

    resetDirectionVisual() {
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        const posX = document.getElementById('info-pos-x');
        const err = document.getElementById('info-error');
        arrow.style.left = '50%';
        arrow.style.borderTopColor = varCss('--color-text-muted');
        zoneBar.style.left = '50%';
        posX.textContent = '—';
        err.textContent = '—';
        posX.style.color = varCss('--color-text-muted');
        err.style.color = varCss('--color-text-muted');
    },

    showMsg(el, text, type) {
        if (!el) return;
        el.textContent = text;
        el.className = 'msg-result ' + type;
        el.style.display = 'block';
    }
};

document.addEventListener('DOMContentLoaded', () => sim.init());

window.addEventListener('beforeunload', () => {
    if (sim.captureTimer) clearInterval(sim.captureTimer);
    if (sim.pollTimer) clearInterval(sim.pollTimer);
});
)WFWEB";

#endif // WIND_FOLLOW_WEB_ASSETS_H
