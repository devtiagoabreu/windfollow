#ifndef WIND_FOLLOW_WEB_ASSETS_H
#define WIND_FOLLOW_WEB_ASSETS_H

// Arquivo gerado por tools/generate_web_assets.ps1 - nao edite manualmente.
// Fonte: firmware/windfollow/web/{index.html, css/style.css, js/app.js}

static const char WIND_FOLLOW_INDEX_HTML[] PROGMEM = R"WFWEB(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <meta name="theme-color" content="#282a36">
    <meta name="apple-mobile-web-app-capable" content="yes">
    <meta name="apple-mobile-web-app-status-bar-style" content="black">
    <meta name="apple-mobile-web-app-title" content="WindFollow">
    <link rel="manifest" href="/manifest.json">
    <link rel="icon" type="image/svg+xml" href="data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNTEyIiBoZWlnaHQ9IjUxMiIgdmlld0JveD0iMCAwIDUxMiA1MTIiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxkZWZzPgo8bGluZWFyR3JhZGllbnQgaWQ9ImdyYWQiIHgxPSIwJSIgeTE9IjAlIiB4Mj0oMTAwJSAqIC0yNXB4KSIgeTI9IjEwMCUiPgo8c3RvcCBvZmZzZXQ9IjAiIHN0b3AtY29sb3I9IiNiZDkzdjkiLz4KPHN0b3Agb2Zmc2V0PSIxIiBzdG9wLWNvbG9yPSIjNjI3MmE0Ii8+CjwvbGluZWFyR3JhZGllbnQ+CjwvZGVmcz4KPHBhdGggZD0iTTI1NiA1MTJDMzk3LjQ1NiA1MTIgNTEyIDM5Ny40NTYgNTEyIDI1NUM1MTIgMTE0LjU0NCAzOTcuNDU2IDAgMjU2IDBDMTE0LjU0NCAwIDAgMTE0LjU0NCAwIDI1NkMwIDM5Ny40NTYgMTE0LjU0NiA1MTIgMjU2IDUxMloiIGZpbGw9InVybCgjZ3JhZCkiLz4KPGcgZmlsdGVyPSJ1cmwoI2ZpbHRlc18xXyIpIHN0cm9rZT0iIzZyNzI3NCIgc3Ryb2tlLXdpZHRoPSIyNSIgc3Ryb2tlLWxpbmVjYXA9InJvdW5kIiBzdHJva2UtbGluZWpvaW49InJvdW5kIj4KPGcgZmlsdGVyPSJ1cmwoI2ZpbHRlc18yXyIpPgo8cGF0aCBkPSJNMjU2IDEwNC41NDRDMTc5LjQ1NiAxMDQuNTQ0IDEyMS41NDQgMTYyLjQ1NiAxMjEuNTQ0IDIzOS4wMDBDMTIxLjU0NCAzMTUuNTQ0IDE3OS40NTYgMzcyLjQ1NiAyNTYgMzcyLjQ1NkMzMzIuNTQ0IDM3Mi40NTYgMzkwLjQ1NiAzMTUuNTQ0IDM5MC40NTYgMjM5LjAwMEMzOTAuNDU2IDE2Mi40NTYgMzMyLjU0NiAxMDQuNTQ0IDI1NiAxMDQuNTQ0WiIgZmlsdGVyPSJ1cmwoI2ZpbHRlc18zXykiLz4KPC9nPgo8L2c+Cjwvc3ZnPg==">
    <title>WindFollow — Controle Remoto</title>
    <link rel="stylesheet" href="/css/style.css">
</head>
<body>
    <div id="app">
        <!-- ===== TELA INICIAL: CONTROLE REMOTO (mobile first) ===== -->
        <header class="remote-header">
            <div class="remote-brand">
                <svg class="wind-logo" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                    <path d="M12 2L15 9H9L12 2Z" fill="#bd93f9"/>
                    <path d="M12 22L15 15H9L12 22Z" fill="#ff79c6"/>
                    <path d="M3 12C3 6.477 6.477 3 12 3S21 6.477 21 12S6.477 21 12 21S3 17.523 3 12Z" fill="none" stroke="#bd93f9" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
                    <path d="M12 3L12 21" stroke="#ff79c6" stroke-width="2" stroke-linecap="round"/>
                    <path d="M3 12L21 12" stroke="#bd93f9" stroke-width="2" stroke-linecap="round"/>
                </svg>
                <span class="brand-name">Wind<span class="brand-accent">Follow</span></span>
                <span class="brand-sub">Controle Remoto • L298N</span>
            </div>
            <div class="remote-header-actions">
                <label class="theme-switch">
                    <input type="checkbox" id="theme-toggle" onchange="sim.toggleTheme()">
                    <span class="theme-slider"></span>
                </label>
                <div id="connection-status" class="status-indicator status-off">Desconectado</div>
            </div>
        </header>

        <main id="content">
            <!-- Aba: Controle Remoto (inicial) -->
            <section id="tab-remote" class="tab-panel active">
                <div class="remote-card">
                    <div class="remote-status-grid">
                        <div class="remote-status-item">
                            <span class="remote-label">Wi-Fi</span>
                            <span class="remote-value" id="remote-status-wifi">AP</span>
                        </div>
                        <div class="remote-status-item">
                            <span class="remote-label">Motor 1</span>
                            <span class="remote-value" id="remote-status-m1">Parado</span>
                        </div>
                        <div class="remote-status-item">
                            <span class="remote-label">Motor 2</span>
                            <span class="remote-value" id="remote-status-m2">Parado</span>
                        </div>
                    </div>

                    <div class="remote-controls">
                        <!-- Motor 1 -->
                        <div class="remote-motor">
                            <div class="remote-motor-header">
                                <span class="remote-motor-title">Motor 1</span>
                                <span class="remote-motor-pill" id="remote-m1-pill">Parado</span>
                            </div>
                            <div class="remote-motor-btns">
                                <button class="remote-btn remote-btn-lg remote-btn-left" onclick="sim.motorMove('1','esquerda')">
                                    <span class="btn-arrow">◀</span>
                                    <span class="btn-label">Esquerda</span>
                                </button>
                                <button class="remote-btn remote-btn-lg remote-btn-stop" onclick="sim.motorStop('1')">
                                    <span class="btn-icon">⏹</span>
                                    <span class="btn-label">Parar</span>
                                </button>
                                <button class="remote-btn remote-btn-lg remote-btn-right" onclick="sim.motorMove('1','direita')">
                                    <span class="btn-label">Direita</span>
                                    <span class="btn-arrow">▶</span>
                                </button>
                            </div>
                        </div>

                        <!-- Motor 2 -->
                        <div class="remote-motor">
                            <div class="remote-motor-header">
                                <span class="remote-motor-title">Motor 2</span>
                                <span class="remote-motor-pill" id="remote-m2-pill">Parado</span>
                            </div>
                            <div class="remote-motor-btns">
                                <button class="remote-btn remote-btn-lg remote-btn-left" onclick="sim.motorMove('2','esquerda')">
                                    <span class="btn-arrow">◀</span>
                                    <span class="btn-label">Esquerda</span>
                                </button>
                                <button class="remote-btn remote-btn-lg remote-btn-stop" onclick="sim.motorStop('2')">
                                    <span class="btn-icon">⏹</span>
                                    <span class="btn-label">Parar</span>
                                </button>
                                <button class="remote-btn remote-btn-lg remote-btn-right" onclick="sim.motorMove('2','direita')">
                                    <span class="btn-label">Direita</span>
                                    <span class="btn-arrow">▶</span>
                                </button>
                            </div>
                        </div>

                        <!-- Parar Todos -->
                        <div class="remote-motor remote-motor-all">
                            <div class="remote-motor-header">
                                <span class="remote-motor-title">Todos</span>
                                <span class="remote-motor-pill remote-motor-pill-all">Parados</span>
                            </div>
                            <div class="remote-motor-btns">
                                <button class="remote-btn remote-btn-lg remote-btn-stop remote-btn-stop-all" onclick="sim.motorStop()">
                                    <span class="btn-icon">⏹</span>
                                    <span class="btn-label">Parar Todos</span>
                                </button>
                            </div>
                        </div>
                    </div>

                    <div class="remote-hint">
                        <p>🕷️ Mantenha pressionado para girar continuamente. Solte ou toque <b>Parar</span> para interromper.</p>
                        <p class="hint-detail">Motores giram continuamente até receber comando de parada.</p>
                    </div>
                </div>
            </section>

            <!-- Aba: Configuração de Dispositivos -->
            <section id="tab-devices" class="tab-panel">
                <div class="card">
                    <h2>Configuracao de Hardware (L298N + 2 Motores)</h2>
                    <p class="hint">Esses valores sao enviados para o ESP32 e salvos na memoria (NVS/Preferences).</p>
                    <form id="form-settings">
                        <div class="form-group">
                            <label>GPIO IN1 (Motor 1 — sentido 1)</label>
                            <input type="number" id="gpio_in1" min="0" max="48" placeholder="Ex: 26">
                            <span class="helper">Sinal da ESP32 ao L298N (Motor 1).</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO IN2 (Motor 1 — sentido 2)</label>
                            <input type="number" id="gpio_in2" min="0" max="48" placeholder="Ex: 25">
                            <span class="helper">Define o sentido contrario do Motor 1.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO ENA (Motor 1 — PWM)</label>
                            <input type="number" id="gpio_ena" min="0" max="48" placeholder="Ex: -1 (jumper)">
                            <span class="helper">-1 = jumper ENA instalado (sem PWM). Para PWM, informe o GPIO.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO IN3 (Motor 2 — sentido 1)</label>
                            <input type="number" id="gpio_in3" min="0" max="48" placeholder="Ex: 32">
                            <span class="helper">Sinal da ESP32 ao L298N (Motor 2).</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO IN4 (Motor 2 — sentido 2)</label>
                            <input type="number" id="gpio_in4" min="0" max="48" placeholder="Ex: 33">
                            <span class="helper">Define o sentido contrario do Motor 2.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO ENB (Motor 2 — PWM)</label>
                            <input type="number" id="gpio_enb" min="0" max="48" placeholder="Ex: -1 (jumper)">
                            <span class="helper">-1 = jumper ENB instalado (sem PWM).</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO Botao 1 (Motor 1)</label>
                            <input type="number" id="gpio_btn1" min="0" max="48" placeholder="Ex: 27">
                            <span class="helper">Botao NA entre este GPIO e GND. Avança a sequencia do Motor 1.</span>
                        </div>
                        <div class="form-group">
                            <label>GPIO Botao 2 (Motor 2)</label>
                            <input type="number" id="gpio_btn2" min="0" max="48" placeholder="Ex: 14">
                            <span class="helper">Botao NA entre este GPIO e GND. Avança a sequencia do Motor 2.</span>
                        </div>
                        <div class="form-group">
                            <label>Duracao do pulso (ms)</label>
                            <input type="number" id="step_duration_ms" min="50" max="2000" value="150">
                            <span class="helper">Tempo de acionamento do motor a cada comando/botao.</span>
                        </div>
                        <div class="form-group">
                            <label>Motor 1 habilitado</label>
                            <select id="motor1_enabled">
                                <option value="true">Sim</option>
                                <option value="false">Nao</option>
                            </select>
                        </div>
                        <div class="form-group">
                            <label>Motor 2 habilitado</label>
                            <select id="motor2_enabled">
                                <option value="true">Sim</option>
                                <option value="false">Nao</option>
                            </select>
                        </div>
                        <div class="form-actions">
                            <button type="button" class="btn btn-primary" id="btn-save">Salvar configuracao</button>
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
                                <span class="col">IN1 (M1)</span>
                                <span class="col">GPIO configurado</span>
                                <span class="col">Sentido 1 do Motor 1</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">IN2 (M1)</span>
                                <span class="col">GPIO configurado</span>
                                <span class="col">Sentido 2 do Motor 1</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">ENA (M1)</span>
                                <span class="col">GPIO PWM ou jumper</span>
                                <span class="col">Velocidade do Motor 1</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">IN3 (M2)</span>
                                <span class="col">GPIO configurado</span>
                                <span class="col">Sentido 1 do Motor 2</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">IN4 (M2)</span>
                                <span class="col">GPIO configurado</span>
                                <span class="col">Sentido 2 do Motor 2</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">ENB (M2)</span>
                                <span class="col">GPIO PWM ou jumper</span>
                                <span class="col">Velocidade do Motor 2</span>
                            </div>
                            <div class="motor-row">
                                <span class="col">GND</span>
                                <span class="col">GND ESP32 + GND fonte motor</span>
                                <span class="col">Referencia comum obrigatoria</span>
                            </div>
                        </div>
                        <div class="motor-warnings">
                            <p><strong>⚠ Alimentação:</strong> Nunca alimente o motor pelas GPIOs ou pelos 3,3V da ESP32. Use fonte separada.</p>
                            <p><strong>⚠ GND comum:</strong> Ligue o GND da fonte do motor, do L298N e da ESP32 juntos.</p>
                            <p><strong>⚠ Jumper ENA/ENB:</strong> Se deixado instalado, o motor roda em velocidade maxima. Remova para controlar por PWM.</p>
                        </div>
                    </div>
                </div>
                <div class="card info-card">
                    <h2>Padrao de fabrica (conforme docs/documentacao_hardware_esp32_l298n.md)</h2>
                    <div class="occupied-list">
                        <span class="badge">M1: IN1=26, IN2=25, BT1=27</span>
                        <span class="badge">M2: IN3=32, IN4=33, BT2=14</span>
                    </div>
                    <p class="hint">Botoes NA entre o GPIO e GND. Sequencia por botao: esquerda (contínuo) -> parado -> direita (contínuo) -> parado -> repete.</p>
                </div>
            </section>

            <!-- Aba: Configuração Wi-Fi -->
            <section id="tab-wifi" class="tab-panel">
                <div class="card setup-card">
                    <h2>Configuracao de Rede Wi-Fi</h2>

                    <div class="wifi-mode-card">
                        <div class="mode-badge" id="wifi-mode-badge">Modo: Access Point (AP)</div>
                        <div class="mode-info">
                            <strong>SSID atual:</strong> <span id="wifi-current-ssid">WindFollowBlind-Setup</span><br>
                            <strong>IP do ESP32:</strong> <span id="wifi-local-ip">192.168.4.1</span><br>
                            <strong>mDNS local:</strong> <span id="wifi-mdns">http://windfollowblind.local/</span>
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
                        <p><strong>http://windfollowblind.local/</strong></p>
                        <p>Caso o mDNS não resolva, use o IP exibido acima.</p>
                    </div>
                </div>
            </section>
        </main>

        <!-- Navegação inferior (mobile first) -->
        <nav class="bottom-nav">
            <button class="nav-item active" data-tab="remote" onclick="sim.switchTab('remote', this)">
                <span class="nav-icon">🎮</span>
                <span class="nav-label">Controle</span>
            </button>
            <button class="nav-item" data-tab="devices" onclick="sim.switchTab('devices', this)">
                <span class="nav-icon">⚙️</span>
                <span class="nav-label">Dispositivos</span>
            </button>
            <button class="nav-item" data-tab="wifi" onclick="sim.switchTab('wifi', this)">
                <span class="nav-icon">📶</span>
                <span class="nav-label">Wi-Fi</span>
            </button>
        </nav>
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

/* ===== WindFollowBlind: Controle Remoto Mobile First (Drácula) ===== */

.remote-header {
  position: relative;
  padding: 1.5rem 1rem 1rem;
  background: linear-gradient(135deg, var(--color-bg-elevated), var(--color-bg));
  border-bottom: 1px solid var(--color-border);
  display: flex;
  flex-direction: column;
  gap: 0.75rem;
}

.remote-brand {
  display: flex;
  align-items: center;
  gap: 0.75rem;
}

.wind-logo {
  width: 48px;
  height: 48px;
  filter: drop-shadow(0 2px 4px rgba(189, 147, 249, 0.4));
}

.brand-name {
  font-size: 1.5rem;
  font-weight: 700;
  color: var(--color-text);
  letter-spacing: -0.5px;
}

.brand-accent {
  color: var(--color-primary);
}

.brand-sub {
  font-size: 0.75rem;
  color: var(--color-text-muted);
  margin-top: 0.1rem;
}

.remote-header-actions {
  display: flex;
  align-items: center;
  gap: 0.75rem;
  margin-left: auto;
}

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
  background-color: var(--color-bg-hover);
  transition: 0.3s;
  border-radius: 24px;
}

.theme-slider:before {
  position: absolute;
  content: "";
  height: 18px;
  width: 18px;
  left: 3px;
  bottom: 3px;
  background-color: white;
  transition: 0.3s;
  border-radius: 50%;
}

.theme-switch input:checked + .theme-slider {
  background-color: var(--color-primary);
}

.theme-switch input:checked + .theme-slider:before {
  transform: translateX(20px);
}

.status-indicator {
  display: inline-flex;
  align-items: center;
  gap: 0.5rem;
  padding: 0.4rem 0.8rem;
  border-radius: 20px;
  font-size: 0.8rem;
  font-weight: 600;
}

.status-indicator.status-off {
  background: rgba(255, 85, 85, 0.15);
  color: var(--color-danger);
  border-color: var(--color-danger);
}

.status-indicator.status-ok {
  background: rgba(80, 250, 123, 0.15);
  color: var(--color-success);
  border-color: var(--color-success);
}

/* Conteúdo principal */
main#content {
  padding: 1rem;
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

/* Cards remotos */
.remote-card {
  background: var(--color-bg-elevated);
  border-radius: 16px;
  padding: 1.5rem;
  box-shadow: var(--shadow);
  border: 1px solid var(--color-border);
}

.remote-status-grid {
  display: grid;
  grid-template-columns: repeat(3, 1fr);
  gap: 0.75rem;
  margin-bottom: 1.5rem;
}

.remote-status-item {
  background: var(--color-bg);
  border: 1px solid var(--color-border);
  border-radius: 12px;
  padding: 0.75rem;
  text-align: center;
}

.remote-label {
  display: block;
  font-size: 0.7rem;
  color: var(--color-text-muted);
  margin-bottom: 0.25rem;
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.remote-value {
  font-size: 1rem;
  font-weight: 700;
  color: var(--color-text);
}

.remote-value.moving { color: var(--color-success); }
.remote-value.stopped { color: var(--color-text-muted); }

/* Controles dos motores */
.remote-controls {
  display: flex;
  flex-direction: column;
  gap: 1rem;
}

.remote-motor {
  background: var(--color-bg);
  border: 1px solid var(--color-border);
  border-radius: 16px;
  padding: 1.25rem;
}

.remote-motor-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 1rem;
}

.remote-motor-title {
  font-size: 1.1rem;
  font-weight: 600;
  color: var(--color-text);
}

.remote-motor-pill {
  font-size: 0.75rem;
  font-weight: 600;
  padding: 0.25rem 0.75rem;
  border-radius: 20px;
  background: var(--color-bg-hover);
  color: var(--color-text-muted);
  border: 1px solid var(--color-border);
}

.remote-motor-pill.moving { background: var(--color-success); color: #282a36; }
.remote-motor-pill.stopped { background: var(--color-bg-hover); color: var(--color-text-muted); }

.remote-motor-btns {
  display: grid;
  grid-template-columns: 1fr auto 1fr;
  gap: 0.75rem;
  align-items: center;
}

.remote-btn {
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  gap: 0.5rem;
  padding: 1rem 0.5rem;
  border: none;
  border-radius: 14px;
  font-size: 0.85rem;
  font-weight: 600;
  cursor: pointer;
  transition: all 0.2s;
  touch-action: manipulation;
  min-height: 64px;
}

.remote-btn:active {
  transform: scale(0.95);
}

.remote-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

.remote-btn-left {
  background: var(--color-info);
  color: #282a36;
}

.remote-btn-left:active {
  background: #ff9fdb;
}

.remote-btn-right {
  background: var(--color-primary);
  color: #282a36;
}

.remote-btn-right:active {
  background: #7bc7e6;
}

.remote-btn-stop {
  background: var(--color-danger);
  color: #282a36;
}

.remote-btn-stop:active {
  background: #ff7777;
}

.remote-btn-stop-all {
  background: var(--color-danger);
  color: #282a36;
  grid-column: 1 / -1;
}

.remote-btn .btn-arrow {
  font-size: 1.2rem;
}

.remote-btn .btn-icon {
  font-size: 1.5rem;
}

.remote-btn .btn-label {
  font-size: 0.9rem;
}

.remote-motor-all .remote-motor-btns {
  grid-template-columns: 1fr;
}

/* Aviso */
.remote-hint {
  margin-top: 1rem;
  padding: 1rem;
  background: var(--color-bg-hover);
  border-radius: 12px;
  border: 1px solid var(--color-border);
}

.remote-hint p {
  margin: 0.25rem 0;
  font-size: 0.85rem;
  color: var(--color-text-muted);
}

.remote-hint .hint-detail {
  font-size: 0.75rem;
  color: var(--color-text-muted);
  opacity: 0.8;
}

/* Navegação inferior (mobile first) */
.bottom-nav {
  position: fixed;
  bottom: 0;
  left: 0;
  right: 0;
  display: flex;
  justify-content: space-around;
  padding: 0.75rem 0.5rem;
  background: var(--color-bg-elevated);
  border-top: 1px solid var(--color-border);
  z-index: 100;
}

.nav-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.25rem;
  background: none;
  border: none;
  cursor: pointer;
  padding: 0.5rem;
  color: var(--color-text-muted);
  font-size: 0.7rem;
  transition: color 0.2s;
  touch-action: manipulation;
}

.nav-item.active {
  color: var(--color-primary);
}

.nav-icon {
  font-size: 1.5rem;
}

.nav-label {
  font-size: 0.65rem;
  font-weight: 600;
}

/* Ajustes para telas maiores */
@media (min-width: 640px) {
  main#content {
    max-width: 500px;
    margin: 0 auto;
  }

  .remote-header {
    flex-direction: row;
    align-items: center;
    justify-content: space-between;
    padding: 1.5rem 2rem;
  }

  .remote-header-actions {
    margin-left: 0;
  }

  .bottom-nav {
    bottom: env(safe-area-inset-bottom);
    padding: 0.75rem max(env(safe-area-inset-right), 1rem);
  }
}

/* Scrollbar customizada */
::-webkit-scrollbar {
  width: 6px;
  height: 6px;
}

::-webkit-scrollbar-track {
  background: var(--color-bg-hover);
}

::-webkit-scrollbar-thumb {
  background: var(--color-border);
  border-radius: 3px;
}

::-webkit-scrollbar-thumb:hover {
  background: var(--color-text-muted);
}
)WFWEB";

static const char WIND_FOLLOW_APP_JS[] PROGMEM = R"WFWEB(
// WindFollowBlind Interface Web (sem camera)
// Controle de 2 motores DC via L298N + configuracao Wi-Fi.
// Tela inicial: controle remoto mobile first.

const sim = {
    // Estado Wi-Fi
    wifi: {
        configured: false,
        mode: 'AP',
        ssid: 'WindFollowBlind-Setup',
        password: '',
        localIp: '192.168.4.1',
        mdns: 'windfollowblind.local',
        connectedNetwork: null,
        signalStrength: 0
    },

    // Redes simuladas para scan
    availableNetworks: [
        { ssid: 'Casa_WiFi', rssi: -45 },
        { ssid: 'Vizinho_2G', rssi: -68 },
        { ssid: 'Empresa_Guest', rssi: -72 },
        { ssid: 'Meu_Internet', rssi: -55 },
        { ssid: 'WiFi_Livre', rssi: -80 }
    ],

    // Configuracao de hardware
    config: {
        gpio_in1: 26,
        gpio_in2: 25,
        gpio_ena: -1,
        gpio_in3: 32,
        gpio_in4: 33,
        gpio_enb: -1,
        gpio_btn1: 27,
        gpio_btn2: 14,
        step_duration_ms: 150,
        motor1_enabled: true,
        motor2_enabled: true
    },

    // Estado dos motores (atualizado via API /api/motor/status)
    motor: {
        moving1: false,
        moving2: false,
        direction1: 'parado',
        direction2: 'parado',
        step_duration_ms: 150
    },

    occupiedGpios: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27,
        28, 29, 30, 31, 32, 33, 34, 35, 36, 39],

    // ========================================================================
    // Inicializacao
    // ========================================================================
    init() {
        this.loadState();
        this.renderConfig();
        this.updateWifiUI();
        this.setupTabs();
        this.setupForm();
        this.updateStatus();

        // Atualizacao periodica do status
        setInterval(() => {
            this.updateStatus();
        }, 1000);
    },

    loadState() {
        const saved = localStorage.getItem('windflollowblind_state');
        if (saved) {
            try {
                const state = JSON.parse(saved);
                this.config = { ...this.config, ...(state.config || {}) };
                this.wifi = { ...this.wifi, ...(state.wifi || {}) };
            } catch (e) {
                console.warn('Erro ao carregar estado', e);
            }
        }
    },

    saveState() {
        const state = { config: this.config, wifi: this.wifi };
        localStorage.setItem('windflollowblind_state', JSON.stringify(state));
    },

    // ========================================================================
    // API
    // ========================================================================
    async api(method, path, body) {
        const opts = { method };
        if (body) {
            opts.headers = { 'Content-Type': 'application/json' };
            opts.body = JSON.stringify(body);
        }
        try {
            const res = await fetch(path, opts);
            if (!res.ok) {
                const errTxt = await res.text().catch(() => '');
                throw new Error(errTxt || res.status);
            }
            const ct = res.headers.get('content-type') || '';
            if (ct.includes('application/json')) {
                return await res.json();
            }
            return await res.text();
        } catch (err) {
            throw err;
        }
    },

    // ========================================================================
    // Tema / Tabs
    // ========================================================================
    toggleTheme() {
        const isLight = document.getElementById('theme-toggle').checked;
        if (isLight) {
            document.documentElement.setAttribute('data-theme', 'light');
            localStorage.setItem('windflollowblind_theme', 'light');
        } else {
            document.documentElement.removeAttribute('data-theme');
            localStorage.setItem('windflollowblind_theme', 'dark');
        }
    },

    setupTabs() {
        document.querySelectorAll('.nav-item').forEach(tab => {
            tab.addEventListener('click', (e) => {
                this.switchTab(e.currentTarget.dataset.tab, e.currentTarget);
            });
        });
        const themeToggle = document.getElementById('theme-toggle');
        const savedTheme = localStorage.getItem('windflollowblind_theme');
        if (savedTheme === 'light') {
            document.documentElement.setAttribute('data-theme', 'light');
            if (themeToggle) themeToggle.checked = true;
        } else {
            document.documentElement.removeAttribute('data-theme');
            if (themeToggle) themeToggle.checked = false;
        }
    },

    switchTab(tabId, btn) {
        document.querySelectorAll('.nav-item').forEach(t => t.classList.remove('active'));
        if (btn) btn.classList.add('active');
        document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
        document.getElementById('tab-' + tabId).classList.add('active');
    },

    // ========================================================================
    // Formulario de configuracao
    // ========================================================================
    setupForm() {
        const btnSave = document.getElementById('btn-save');
        if (btnSave) {
            btnSave.addEventListener('click', () => this.saveConfig());
        }
    },

    renderConfig() {
        const c = this.config;
        const map = {
            gpio_in1: 'gpio_in1',
            gpio_in2: 'gpio_in2',
            gpio_ena: 'gpio_ena',
            gpio_in3: 'gpio_in3',
            gpio_in4: 'gpio_in4',
            gpio_enb: 'gpio_enb',
            gpio_btn1: 'gpio_btn1',
            gpio_btn2: 'gpio_btn2',
            step_duration_ms: 'step_duration_ms'
        };
        for (const [key, id] of Object.entries(map)) {
            const el = document.getElementById(id);
            if (el) el.value = c[key] ?? '';
        }
        const m1 = document.getElementById('motor1_enabled');
        const m2 = document.getElementById('motor2_enabled');
        if (m1) m1.value = c.motor1_enabled ? 'true' : 'false';
        if (m2) m2.value = c.motor2_enabled ? 'true' : 'false';
    },

    async saveConfig() {
        const c = {};
        for (const key of ['gpio_in1','gpio_in2','gpio_ena','gpio_in3','gpio_in4','gpio_enb','gpio_btn1','gpio_btn2','step_duration_ms']) {
            const el = document.getElementById(key);
            c[key] = el ? parseInt(el.value, 10) || 0 : 0;
        }
        const m1 = document.getElementById('motor1_enabled');
        const m2 = document.getElementById('motor2_enabled');
        c.motor1_enabled = m1 ? m1.value === 'true' : true;
        c.motor2_enabled = m2 ? m2.value === 'true' : true;

        try {
            const res = await this.api('POST', '/api/config', c);
            this.config = { ...this.config, ...c };
            this.saveState();
            this.renderConfig();
            this.showMsg(document.getElementById('msg-result'),
                res.message || 'Configuracao salva.', 'success');
            if (res.restart) {
                setTimeout(() => alert('O ESP32 vai reiniciar para aplicar os pinos.'), 300);
            }
        } catch (err) {
            this.showMsg(document.getElementById('msg-result'), 'Erro ao salvar: ' + err, 'error');
        }
    },

    validate() {
        const c = this.config;
        const errors = [];
        const gpioFields = ['gpio_in1','gpio_in2','gpio_ena','gpio_in3','gpio_in4','gpio_enb','gpio_btn1','gpio_btn2'];
        const used = [];
        for (const k of gpioFields) {
            const v = c[k];
            if (v >= 0) {
                if (this.occupiedGpios.includes(v)) errors.push(k + ' (' + v + ') esta ocupado/reservado.');
                if (used.includes(v)) errors.push(k + ' (' + v + ') repetido.');
                used.push(v);
            }
        }
        const msg = document.getElementById('msg-result');
        if (errors.length) {
            this.showMsg(msg, errors.join(' '), 'error');
        } else {
            this.showMsg(msg, 'Pinos validos (seguranca de simulacao).', 'success');
        }
    },

    reset() {
        this.config = {
            gpio_in1: 26, gpio_in2: 25, gpio_ena: -1,
            gpio_in3: 32, gpio_in4: 33, gpio_enb: -1,
            gpio_btn1: 27, gpio_btn2: 14,
            step_duration_ms: 150,
            motor1_enabled: true, motor2_enabled: true
        };
        this.renderConfig();
        this.saveState();
        this.showMsg(document.getElementById('msg-result'), 'Defaults restaurados.', 'info');
    },

    // ========================================================================
    // Wi-Fi
    // ========================================================================
    updateWifiUI() {
        const badge = document.getElementById('wifi-mode-badge');
        if (badge) {
            badge.textContent = this.wifi.mode === 'AP' ? 'Modo: Access Point (AP)' : 'Modo: Estacao (STA)';
            badge.style.background = this.wifi.mode === 'AP' ? '#2563eb' : '#16a34a';
        }
        document.getElementById('wifi-current-ssid').textContent = this.wifi.ssid;
        document.getElementById('wifi-local-ip').textContent = this.wifi.localIp;
        document.getElementById('wifi-mdns').textContent = 'http://' + this.wifi.mdns + '/';
        const modeSel = document.getElementById('wifi-mode');
        if (modeSel) modeSel.value = this.wifi.mode;
        const ssidIn = document.getElementById('wifi-ssid');
        if (ssidIn) ssidIn.value = this.wifi.ssid;
        const passIn = document.getElementById('wifi-password');
        if (passIn) passIn.value = this.wifi.password;
    },

    async scanWifi() {
        const select = document.getElementById('wifi-networks');
        const msg = document.getElementById('wifi-msg');
        select.innerHTML = '<option value="">Procurando redes...</option>';
        try {
            const nets = await this.api('GET', '/api/wifi/scan');
            select.innerHTML = '<option value="">-- Selecione uma rede --</option>';
            (nets.networks || []).forEach(net => {
                const opt = document.createElement('option');
                opt.value = net.ssid;
                opt.textContent = `${net.ssid} (${net.rssi} dBm)`;
                select.appendChild(opt);
            });
            this.showMsg(msg, (nets.networks || []).length + ' redes encontradas', 'info');
        } catch (e) {
            select.innerHTML = '<option value="">Erro ao escanear</option>';
            this.showMsg(msg, 'Erro ao escanear redes: ' + e, 'error');
        }
        setTimeout(() => { if (msg) msg.style.display = 'none'; }, 2000);
    },

    async connectWifi() {
        const msg = document.getElementById('wifi-msg');
        const select = document.getElementById('wifi-networks');
        const manual = document.getElementById('wifi-ssid-manual');
        const pass = document.getElementById('wifi-password');
        const ssid = (select.value || manual.value || '').trim();
        const password = pass.value || '';
        if (!ssid) {
            this.showMsg(msg, 'Informe o nome da rede (SSID)', 'error');
            return;
        }
        const btn = document.querySelector('button[onclick="sim.connectWifi()"]');
        const origText = btn.textContent;
        btn.disabled = true;
        btn.textContent = 'Conectando...';
        try {
            await this.api('POST', '/api/wifi/config', { ssid, password });
            this.wifi.configured = true;
            this.wifi.mode = 'STA';
            this.wifi.connectedNetwork = ssid;
            this.wifi.ssid = ssid;
            this.wifi.password = password;
            this.saveState();
            this.updateWifiUI();
            this.showMsg(msg, `Conectado a "${ssid}"`, 'success');
            this.showMsg(document.getElementById('msg-result'),
                'Acesse o ESP32 em: http://windfollowblind.local/ ou http://' + this.wifi.localIp, 'info');
        } catch (err) {
            this.showMsg(msg, 'Falha ao conectar: ' + err, 'error');
        } finally {
            btn.disabled = false;
            btn.textContent = origText;
        }
        setTimeout(() => {
            if (msg) msg.style.display = 'none';
            const m2 = document.getElementById('msg-result');
            if (m2) m2.style.display = 'none';
        }, 4000);
    },

    // ========================================================================
    // Motores
    // ========================================================================
    async motorMove(motor, direction) {
        if (motor === '1' && !this.config.motor1_enabled) {
            this.showMsg(document.getElementById('msg-result'), 'Motor 1 desabilitado.', 'error');
            return;
        }
        if (motor === '2' && !this.config.motor2_enabled) {
            this.showMsg(document.getElementById('msg-result'), 'Motor 2 desabilitado.', 'error');
            return;
        }

        try {
            await this.api('POST', '/api/motor/move', { motor, direction });
            this.showMsg(document.getElementById('msg-result'),
                'Motor ' + motor + ' -> ' + direction + ' (movimento continuo)', 'info');
        } catch (err) {
            this.showMsg(document.getElementById('msg-result'), 'Erro no motor: ' + err, 'error');
        }
    },

    async motorStop(motor) {
        if (motor) {
            try {
                await this.api('POST', '/api/motor/move', { motor, direction: 'stop' });
            } catch (err) {
                this.showMsg(document.getElementById('msg-result'), 'Erro ao parar: ' + err, 'error');
            }
        } else {
            try {
                await this.api('POST', '/api/motor/stop', {});
            } catch (err) {
                this.showMsg(document.getElementById('msg-result'), 'Erro ao parar: ' + err, 'error');
            }
        }
    },

    async toggleMotor1(enabled) {
        this.config.motor1_enabled = enabled;
        await this.api('POST', '/api/config', this.config);
        this.saveState();
        if (!enabled) this.motorStop('1');
    },

    async toggleMotor2(enabled) {
        this.config.motor2_enabled = enabled;
        await this.api('POST', '/api/config', this.config);
        this.saveState();
        if (!enabled) this.motorStop('2');
    },

    // ========================================================================
    // Status / polling
    // ========================================================================
    async updateStatus() {
        try {
            const m = await this.api('GET', '/api/motor/status');
            this.motor = m;
            this.updateMotorUI();
        } catch (e) {
            // sem ESP conectado: mantem estado local
        }
        try {
            const w = await this.api('GET', '/api/wifi/status');
            this.wifi = { ...this.wifi, ...w };
            this.updateWifiUI();
        } catch (e) {
            // ignora
        }
    },

    updateMotorUI() {
        const m1 = document.getElementById('status-motor1');
        const m2 = document.getElementById('status-motor2');
        const d1 = document.getElementById('status-dir1');
        const d2 = document.getElementById('status-dir2');
        const p1 = document.getElementById('remote-m1-pill');
        const p2 = document.getElementById('remote-m2-pill');

        if (m1) m1.textContent = this.motor.moving1 ? 'Movendo' : 'Parado';
        if (m2) m2.textContent = this.motor.moving2 ? 'Movendo' : 'Parado';
        if (d1) d1.textContent = this.motor.direction1;
        if (d2) d2.textContent = this.motor.direction2;

        // Pills de status nos motores
        if (p1) {
            p1.textContent = this.motor.moving1 ? this.motor.direction1 : 'Parado';
            p1.className = 'remote-motor-pill ' + (this.motor.moving1 ? 'moving' : 'stopped');
        }
        if (p2) {
            p2.textContent = this.motor.moving2 ? this.motor.direction2 : 'Parado';
            p2.className = 'remote-motor-pill ' + (this.motor.moving2 ? 'moving' : 'stopped');
        }
    },

    // ========================================================================
    // Camera (desabilitada)
    // ========================================================================
    async toggleCamera() {
        this.showMsg(document.getElementById('msg-result'),
            'Esta versao nao possui camera.', 'info');
    },

    // ========================================================================
    // Mensagens
    // ========================================================================
    showMsg(el, text, type) {
        if (!el) return;
        el.textContent = text;
        el.className = 'msg-result ' + type;
        el.style.display = 'block';
        setTimeout(() => { el.style.display = 'none'; }, 3000);
    }
};

// Inicializa quando o DOM estiver pronto
document.addEventListener('DOMContentLoaded', () => {
    sim.init();
});
)WFWEB";

#endif // WIND_FOLLOW_WEB_ASSETS_H
