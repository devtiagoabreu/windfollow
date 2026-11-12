#ifndef WEB_ASSETS_H
#define WEB_ASSETS_H

// Conteudos gerados automaticamente a partir da pasta web/
// Nao edite manualmente - rode generate_web_assets.ps1 apos alterar os arquivos web/
static const char* index_html = R"rawliteral("
<!DOCTYPE html>
<html lang="pt-BR">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
  <title>iRobot - Tela de Teste</title>
  <link rel="stylesheet" href="/style.css">
</head>
<body>

  <header class="app-header">
    <div class="header-left">
      <h1>iRobot</h1>
      <span class="subtitulo">Teste de Comando</span>
    </div>
    <div class="header-right">
      <div id="status-badge" class="badge offline">Desconectado</div>
      <label class="switch">
        <input type="checkbox" id="power-toggle" checked>
        <span class="slider-round"></span>
      </label>
    </div>
  </header>

  <main class="test-layout">
    <!-- Coluna esquerda: joystick -->
    <section class="joystick-column">
      <div class="joystick-card">
        <div class="joystick-title">Joystick Diferencial</div>
        <div class="joystick-base" id="joystick-base">
          <div class="joystick-center-dot"></div>
          <div class="joystick-knob" id="joystick-knob"></div>
          <div class="joystick-ring" id="joystick-ring"></div>
        </div>
        <div class="joystick-hint">Arraste para controlar (esq/dir = curva)</div>
        <div class="joy-readout">
          <span id="joy-dir">Parado</span>
          <span id="joy-int">Int: 0%</span>
        </div>
      </div>
    </section>

    <!-- Coluna direita: botoes e controles de teste -->
    <section class="controls-column">
      <!-- Botoes direcionais -->
      <div class="card">
        <div class="card-title">Comandos Rapidos</div>
        <div class="dpad">
          <button class="btn dpad-btn" id="btn-frente" data-dir="frente">Frente</button>
          <div class="dpad-row">
            <button class="btn dpad-btn" id="btn-esquerda" data-dir="esquerda">Esquerda</button>
            <button class="btn dpad-btn btn-stop" id="btn-stop">Parar</button>
            <button class="btn dpad-btn" id="btn-direita" data-dir="direita">Direita</button>
          </div>
          <button class="btn dpad-btn" id="btn-tras" data-dir="tras">Tras</button>
        </div>
      </div>

      <!-- Velocidade geral -->
      <div class="card">
        <div class="card-title">Velocidade</div>
        <div class="slider-row">
          <label class="slider-label">Geral</label>
          <input type="range" id="speed-slider" min="0" max="100" value="70" class="hslider">
          <span id="speed-value" class="speed-val">70%</span>
        </div>
      </div>

      <!-- Controle diferencial independente -->
      <div class="card">
        <div class="card-title">Motores Independentes</div>
        <div class="slider-row">
          <label class="slider-label">Esquerda</label>
          <input type="range" id="left-slider" min="-100" max="100" value="0" class="hslider">
          <span id="left-value" class="speed-val">0</span>
        </div>
        <div class="slider-row">
          <label class="slider-label">Direita</label>
          <input type="range" id="right-slider" min="-100" max="100" value="0" class="hslider">
          <span id="right-value" class="speed-val">0</span>
        </div>
        <button class="btn btn-outline btn-block" id="btn-apply-diff">Aplicar Diferencial</button>
      </div>

      <!-- Configuracao PWM -->
      <div class="card">
        <div class="card-title">Configuracao PWM</div>
        <div class="config-row">
          <label>Min</label>
          <input type="number" id="cfg-pwm-min" min="0" max="255" value="60" class="cfg-input">
        </div>
        <div class="config-row">
          <label>Max</label>
          <input type="number" id="cfg-pwm-max" min="0" max="255" value="255" class="cfg-input">
        </div>
        <div class="config-row">
          <label>Default</label>
          <input type="number" id="cfg-pwm-default" min="0" max="255" value="150" class="cfg-input">
        </div>
        <div class="config-row">
          <label>Motores</label>
          <label class="switch inline">
            <input type="checkbox" id="cfg-motors-enabled" checked>
            <span class="slider-round"></span>
          </label>
        </div>
        <button class="btn btn-outline btn-block" id="btn-save-config">Salvar Configuracao</button>
      </div>

      <!-- Sistema -->
      <div class="card">
        <div class="card-title">Sistema</div>
        <button class="btn btn-outline btn-block" id="btn-system-info">Ler Status do Sistema</button>
        <div id="system-info" class="sys-info"></div>
      </div>
    </section>
  </main>

  <!-- Painel de telemetria ao vivo -->
  <div class="telemetry" id="telemetry">
    <div class="tel-item"><span class="tel-label">Status:</span> <span id="tel-status" class="tel-value">Parado</span></div>
    <div class="tel-item"><span class="tel-label">Motor Esq:</span> <span id="tel-left" class="tel-value">0</span></div>
    <div class="tel-item"><span class="tel-label">Motor Dir:</span> <span id="tel-right" class="tel-value">0</span></div>
    <div class="tel-item"><span class="tel-label">Movendo:</span> <span id="tel-moving" class="tel-value">Nao</span></div>
    <div class="tel-item"><span class="tel-label">Conexao:</span> <span id="tel-conn" class="tel-value">Offline</span></div>
    <div class="tel-item"><span class="tel-label">Ultimo erro:</span> <span id="tel-api-error" class="tel-value offline">Nenhum</span></div>
  </div>

  <script src="/app.js"></script>
</body>
</html>

)rawliteral";
static const char* style_css = R"rawliteral("
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

:root {
  --bg: #0b0f19;
  --panel: #111827;
  --panel-hover: #1f2937;
  --accent: #38bdf8;
  --accent-dim: #0ea5e9;
  --text: #e2e8f0;
  --muted: #94a3b8;
  --danger: #ef4444;
  --danger-hover: #b91c1c;
  --success: #22c55e;
  --border: #1e293b;
  --ring: rgba(56, 189, 248, 0.35);
}

html, body {
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
  background: var(--bg);
  color: var(--text);
  min-height: 100vh;
  overflow-x: hidden;
  touch-action: none;
  -webkit-tap-highlight-color: transparent;
}

body {
  display: flex;
  flex-direction: column;
  height: 100vh;
  padding: 0;
}

/* ===== Header ===== */
.app-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0.75rem 1rem;
  border-bottom: 1px solid var(--border);
  background: #0f172a;
  flex-shrink: 0;
}

.header-left {
  display: flex;
  align-items: center;
  gap: 0.6rem;
}

.app-header h1 {
  font-size: 1.15rem;
  font-weight: 700;
  color: var(--accent);
}

.subtitulo {
  font-size: 0.75rem;
  color: var(--muted);
  background: rgba(255,255,255,0.05);
  padding: 0.12rem 0.5rem;
  border-radius: 4px;
  border: 1px solid rgba(255,255,255,0.08);
}

.header-right {
  display: flex;
  align-items: center;
  gap: 0.9rem;
}

.badge {
  padding: 0.2rem 0.6rem;
  border-radius: 9999px;
  font-size: 0.75rem;
  font-weight: 600;
  border: 1px solid;
}

.badge.offline { background: #1e293b; color: #94a3b8; border-color: #334155; }
.badge.online  { background: #064e3b; color: #bbf7d0; border-color: #15803d; }

.switch {
  position: relative;
  display: inline-block;
  width: 44px;
  height: 24px;
}

.switch input { opacity: 0; width: 0; height: 0; }

.slider-round {
  position: absolute;
  cursor: pointer;
  top: 0; left: 0; right: 0; bottom: 0;
  background: #334155;
  border-radius: 34px;
  transition: 0.2s;
}

.switch input:checked + .slider-round { background: #10b981; }

.slider-round:before {
  position: absolute; content: "";
  height: 18px; width: 18px;
  left: 3px; bottom: 3px;
  background: white;
  border-radius: 50%;
  transition: 0.2s;
}

.switch input:checked + .slider-round:before { transform: translateX(20px); }

.switch.inline { width: 44px; height: 24px; }

/* ===== Main layout ===== */
main.test-layout {
  flex: 1;
  overflow: auto;
  padding: 0.75rem;
  display: grid;
  grid-template-columns: 1fr;
  gap: 0.75rem;
  align-items: start;
}

@media (min-width: 768px) and (orientation: landscape) {
  main.test-layout {
    grid-template-columns: 320px 1fr;
    gap: 1rem;
    padding: 1rem;
    align-items: start;
  }
}

@media (min-width: 1024px) {
  main.test-layout {
    grid-template-columns: 340px 1fr;
  }
}

/* ===== Cards ===== */
.card {
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 0.9rem;
  padding: 0.85rem;
  display: flex;
  flex-direction: column;
  gap: 0.7rem;
}

.card-title {
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--muted);
  text-transform: uppercase;
  letter-spacing: 0.05em;
  border-bottom: 1px solid var(--border);
  padding-bottom: 0.4rem;
  margin-bottom: 0.2rem;
}

.joystick-card {
  background: var(--panel);
  border: 1px solid var(--border);
  border-radius: 0.9rem;
  padding: 0.85rem;
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 0.5rem;
}

.joystick-title {
  font-size: 0.8rem;
  font-weight: 700;
  color: var(--accent);
  text-transform: uppercase;
  letter-spacing: 0.05em;
}

/* ===== Joystick ===== */
.joystick-base {
  position: relative;
  width: 220px;
  height: 220px;
  background: radial-gradient(circle, #1e293b 0%, #0f172a 70%);
  border: 2px solid var(--border);
  border-radius: 50%;
  box-shadow: inset 0 0 24px rgba(0,0,0,0.7), 0 0 0 1px rgba(255,255,255,0.03);
  touch-action: none;
  user-select: none;
}

.joystick-ring {
  position: absolute;
  top: 50%; left: 50%;
  width: 100%; height: 100%;
  border: 1px dashed rgba(56, 189, 248, 0.15);
  border-radius: 50%;
  transform: translate(-50%, -50%);
  pointer-events: none;
}

.joystick-center-dot {
  position: absolute;
  top: 50%; left: 50%;
  width: 10px; height: 10px;
  background: #475569;
  border-radius: 50%;
  transform: translate(-50%, -50%);
  pointer-events: none;
}

.joystick-knob {
  position: absolute;
  top: 50%; left: 50%;
  width: 64px; height: 64px;
  background: var(--accent);
  border: 3px solid var(--accent-dim);
  border-radius: 50%;
  box-shadow: 0 0 24px rgba(56, 189, 248, 0.55);
  transform: translate(-50%, -50%);
  touch-action: none;
  user-select: none;
  cursor: grab;
  transition: transform 0.05s ease-out;
}

.joystick-knob:active { cursor: grabbing; }

.joystick-hint {
  font-size: 0.78rem;
  color: var(--muted);
  text-align: center;
  max-width: 200px;
}

.joy-readout {
  display: flex;
  gap: 1rem;
  font-size: 0.8rem;
  color: var(--muted);
  font-variant-numeric: tabular-nums;
}

/* ===== DPad ===== */
.dpad {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  grid-template-rows: 1fr 1fr;
  gap: 0.5rem;
  margin-top: 0.2rem;
}

.dpad .dpad-btn:nth-child(1) { grid-column: 2; grid-row: 1; }      /* Frente */
.dpad .dpad-btn:nth-child(4) { grid-column: 1; grid-row: 2; }      /* Esquerda */
.dpad .dpad-btn:nth-child(5) { grid-column: 2; grid-row: 2; }      /* Parar */
.dpad .dpad-btn:nth-child(6) { grid-column: 3; grid-row: 2; }      /* Direita */
.dpad .dpad-btn:nth-child(7) { grid-column: 2; grid-row: 3; }      /* Tras */

.dpad-btn {
  border: none;
  border-radius: 0.85rem;
  padding: 0.95rem 0.6rem;
  font-size: 0.9rem;
  font-weight: 800;
  cursor: pointer;
  text-transform: uppercase;
  letter-spacing: 0.03em;
  transition: all 0.1s;
  touch-action: manipulation;
  background: var(--panel-hover);
  color: var(--text);
  border: 1px solid var(--border);
}

.dpad-btn:active {
  transform: scale(0.94);
  filter: brightness(1.15);
}

.dpad-btn.btn-stop {
  background: var(--danger);
  color: #fff;
  border-color: #b91c1c;
}

.dpad-btn.btn-stop:active {
  background: var(--danger-hover);
}

/* ===== Sliders ===== */
.slider-row {
  display: flex;
  align-items: center;
  gap: 0.6rem;
  width: 100%;
}

.slider-label {
  font-size: 0.82rem;
  color: var(--muted);
  width: 62px;
  flex-shrink: 0;
}

.hslider {
  flex: 1;
  height: 6px;
  -webkit-appearance: none;
  appearance: none;
  background: #334155;
  border-radius: 3px;
  outline: none;
  accent-color: var(--accent);
  cursor: pointer;
  touch-action: none;
}

.hslider::-webkit-slider-thumb {
  -webkit-appearance: none;
  width: 18px; height: 18px;
  background: var(--accent);
  border: 2px solid #fff;
  border-radius: 50%;
  cursor: pointer;
  box-shadow: 0 0 10px rgba(56, 189, 248, 0.5);
}

.hslider::-moz-range-thumb {
  width: 18px; height: 18px;
  background: var(--accent);
  border: 2px solid #fff;
  border-radius: 50%;
  cursor: pointer;
}

.speed-val {
  font-size: 0.82rem;
  font-weight: 700;
  color: var(--accent);
  min-width: 34px;
  text-align: right;
  flex-shrink: 0;
}

/* Config inputs */
.config-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 0.6rem;
  width: 100%;
}

.config-row label {
  font-size: 0.82rem;
  color: var(--muted);
  width: 78px;
  flex-shrink: 0;
}

.cfg-input {
  flex: 1;
  background: #0f172a;
  border: 1px solid var(--border);
  color: var(--text);
  padding: 0.45rem 0.55rem;
  border-radius: 0.5rem;
  font-size: 0.85rem;
  max-width: 90px;
}

/* ===== Buttons gerais ===== */
.btn {
  border: none;
  border-radius: 0.75rem;
  padding: 0.8rem 1rem;
  font-size: 0.88rem;
  font-weight: 700;
  cursor: pointer;
  transition: all 0.15s;
  text-transform: uppercase;
  width: 100%;
  touch-action: manipulation;
  background: var(--panel-hover);
  color: var(--text);
  border: 1px solid var(--border);
}

.btn:active { transform: scale(0.97); }

.btn-outline {
  background: transparent;
  color: var(--accent);
  border-color: var(--accent);
}

.btn-outline:active {
  background: rgba(56, 189, 248, 0.12);
}

.btn-block { width: 100%; }

.btn-danger {
  background: var(--danger);
  color: #fff;
  border-color: #b91c1c;
}

.btn-danger:hover { background: var(--danger-hover); }

/* ===== Sistema info ===== */
.sys-info {
  margin-top: 0.5rem;
  padding: 0.6rem;
  background: #0f172a;
  border: 1px solid var(--border);
  border-radius: 0.6rem;
  font-family: monospace;
  font-size: 0.72rem;
  color: var(--muted);
  line-height: 1.5;
  white-space: pre-wrap;
}

/* ===== Telemetria ao vivo ===== */
.telemetry {
  position: fixed;
  bottom: 0.5rem;
  left: 0.5rem;
  right: 0.5rem;
  background: rgba(15, 23, 42, 0.92);
  border: 1px solid var(--border);
  border-radius: 0.7rem;
  padding: 0.65rem 0.9rem;
  display: flex;
  flex-wrap: wrap;
  gap: 0.9rem 1.6rem;
  font-family: monospace;
  font-size: 0.75rem;
  color: var(--muted);
  pointer-events: none;
  max-width: 900px;
  margin: 0 auto;
  z-index: 40;
}

.telemetry.hidden { display: none; }

.tel-item { display: flex; align-items: baseline; gap: 0.35rem; }
.tel-label { color: #64748b; font-size: 0.68rem; text-transform: uppercase; letter-spacing: 0.04em; }
.tel-value { color: var(--text); font-variant-numeric: tabular-nums; }

.tel-value.online { color: var(--success); }
.tel-value.offline { color: #ef4444; }

/* ===== Util ===== */
.hidden { display: none !important; }

)rawliteral";
static const char* app_js = R"rawliteral("
const API = {
  async _try(method, path, body) {
    const opts = { method };
    if (body) {
      opts.headers = { 'Content-Type': 'application/json' };
      opts.body = JSON.stringify(body);
    }
    const res = await fetch(path, opts);
    if (!res.ok) throw new Error('HTTP ' + res.status + ' ' + path);
    const ct = res.headers.get('content-type') || '';
    return ct.includes('application/json') ? await res.json() : await res.text();
  },

  async post(path, body) { return this._try('POST', path, body); },
  async get(path) { return this._try('GET', path); }
};

var els = {};
var dpadBtns = [];
var state = {
  powerOn: true,
  speedPct: 70,
  leftPct: 0,
  rightPct: 0,
  joyX: 0,
  joyY: 0,
  intensity: 0,
  active: false,
  lastSent: { l: 0, r: 0, dir: '', speed: 0 },
  online: false
};
var joy = { cx: 0, cy: 0, radius: 0, knobRadius: 32 };
var activeTouchId = null;

function getEls() {
  els = {
    badge: document.getElementById('status-badge'),
    powerToggle: document.getElementById('power-toggle'),
    joystickBase: document.getElementById('joystick-base'),
    joystickKnob: document.getElementById('joystick-knob'),
    joyDir: document.getElementById('joy-dir'),
    joyInt: document.getElementById('joy-int'),
    speedSlider: document.getElementById('speed-slider'),
    speedValue: document.getElementById('speed-value'),
    leftSlider: document.getElementById('left-slider'),
    leftValue: document.getElementById('left-value'),
    rightSlider: document.getElementById('right-slider'),
    rightValue: document.getElementById('right-value'),
    btnStop: document.getElementById('btn-stop'),
    btnApplyDiff: document.getElementById('btn-apply-diff'),
    btnSaveConfig: document.getElementById('btn-save-config'),
    btnSystemInfo: document.getElementById('btn-system-info'),
    cfgPwmMin: document.getElementById('cfg-pwm-min'),
    cfgPwmMax: document.getElementById('cfg-pwm-max'),
    cfgPwmDefault: document.getElementById('cfg-pwm-default'),
    cfgMotorsEnabled: document.getElementById('cfg-motors-enabled'),
    sysInfo: document.getElementById('system-info'),
    telStatus: document.getElementById('tel-status'),
    telLeft: document.getElementById('tel-left'),
    telRight: document.getElementById('tel-right'),
    telMoving: document.getElementById('tel-moving'),
    telConn: document.getElementById('tel-conn'),
    telApiError: document.getElementById('tel-api-error'),
    telemetry: document.getElementById('telemetry')
  };
  dpadBtns = Array.from(document.querySelectorAll('.dpad-btn[data-dir]'));
}

function pctToPwm(pct) { return Math.round((pct / 100) * 255); }

function initJoystickGeometry() {
  var rect = els.joystickBase.getBoundingClientRect();
  joy.cx = rect.left + rect.width / 2;
  joy.cy = rect.top + rect.height / 2;
  joy.radius = Math.max(10, (rect.width / 2) - joy.knobRadius - 6);
}

function getPointerPos(e) {
  if (e.touches && e.touches.length) {
    return { x: e.touches[0].clientX, y: e.touches[0].clientY };
  }
  return { x: e.clientX, y: e.clientY };
}

function setKnobPosition(x, y) {
  var dx = x - joy.cx;
  var dy = y - joy.cy;
  var dist = Math.hypot(dx, dy);

  var clampedX = x;
  var clampedY = y;
  if (dist > joy.radius && dist > 0) {
    var ratio = joy.radius / dist;
    clampedX = joy.cx + dx * ratio;
    clampedY = joy.cy + dy * ratio;
  }

  var offsetX = clampedX - joy.cx;
  var offsetY = clampedY - joy.cy;
  els.joystickKnob.style.transform = 'translate(calc(-50% + ' + offsetX + 'px), calc(-50% + ' + offsetY + 'px))';

  var finalDx = clampedX - joy.cx;
  var finalDy = clampedY - joy.cy;
  var clampedDist = Math.min(dist / joy.radius, 1);

  state.joyX = finalDx / joy.radius;
  state.joyY = -finalDy / joy.radius;
  state.intensity = clampedDist;

  if (clampedDist < 0.07) {
    state.joyX = 0;
    state.joyY = 0;
    state.intensity = 0;
  }

  updateJoyVisuals();
  updateMotorsFromJoy();
}

function resetKnob() {
  els.joystickKnob.style.transform = 'translate(-50%, -50%)';
  state.joyX = 0;
  state.joyY = 0;
  state.intensity = 0;
  updateJoyVisuals();
  updateMotorsFromJoy();
}

function updateJoyVisuals() {
  var i = state.intensity;
  var dir = getJoyDirLabel();
  els.joyDir.textContent = dir;
  els.joyInt.textContent = 'Int: ' + Math.round(i * 100) + '%';
}

function getJoyDirLabel() {
  var x = state.joyX;
  var y = state.joyY;
  var i = state.intensity;
  if (i < 0.07) return 'Parado';
  if (Math.abs(y) > Math.abs(x)) {
    return y > 0 ? 'Frente' : 'Tras';
  }
  return x > 0 ? 'Direita' : 'Esquerda';
}

function updateMotorsFromJoy() {
  if (!state.powerOn) {
    if (state.lastSent.l !== 0 || state.lastSent.r !== 0) {
      sendDifferential(0, 0);
    }
    return;
  }

  var speedScale = state.speedPct / 100;
  var intensity = state.intensity;
  var forward = state.joyY * intensity;
  var turn = state.joyX * intensity;

  var left = forward - turn;
  var right = forward + turn;
  left = Math.max(-1, Math.min(1, left));
  right = Math.max(-1, Math.min(1, right));

  var lSpeed = Math.round(left * 100 * speedScale);
  var rSpeed = Math.round(right * 100 * speedScale);

  sendDifferential(lSpeed, rSpeed);
}

async function sendDifferential(left, right) {
  if (left === state.lastSent.l && right === state.lastSent.r) return;
  try {
    await API.post('/api/motor/move', { leftSpeed: left, rightSpeed: right });
    state.online = true;
    setBadgeOnline();
    state.lastSent = { l: left, r: right, dir: '', speed: 0 };
  } catch (err) {
    console.error('Erro diferencial:', err);
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

async function sendDir(dir, speedPct) {
  var speed = pctToPwm(speedPct);
  if (dir === state.lastSent.dir && speed === state.lastSent.speed && !state.active) return;
  try {
    await API.post('/api/motor/move', { direction: dir, speed: speed });
    state.online = true;
    setBadgeOnline();
    state.lastSent = { l: 0, r: 0, dir: dir, speed: speed };
  } catch (err) {
    console.error('Erro direcao:', err);
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

async function stopAll() {
  try {
    await API.post('/api/motor/stop', {});
    state.online = true;
    setBadgeOnline();
  } catch (err) {
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
  state.lastSent = { l: 0, r: 0, dir: '', speed: 0 };
  resetKnob();
}

async function fetchStatus() {
  try {
    var st = await API.get('/api/motor/status');
    state.online = true;
    setBadgeOnline();
    updateTelemetry(st);
    showApiError('OK');
  } catch (err) {
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

function updateTelemetry(st) {
  if (!els.telStatus) return;
  var dirMap = { 0: 'Parado', 1: 'Frente', 2: 'Tras', 3: 'Esquerda', 4: 'Direita' };
  els.telStatus.textContent = dirMap[st.direction] || 'Parado';
  els.telLeft.textContent = st.leftSpeed;
  els.telRight.textContent = st.rightSpeed;
  els.telMoving.textContent = st.moving ? 'Sim' : 'Nao';
  els.telMoving.className = 'tel-value ' + (st.moving ? 'online' : 'offline');
  els.telConn.textContent = 'Online';
  els.telConn.className = 'tel-value online';
}

async function loadConfig() {
  try {
    var cfg = await API.get('/api/config');
    state.config = cfg;
    els.cfgPwmMin.value = cfg.pwmMin;
    els.cfgPwmMax.value = cfg.pwmMax;
    els.cfgPwmDefault.value = cfg.pwmDefault;
    els.cfgMotorsEnabled.checked = cfg.motorsEnabled === true || cfg.motorsEnabled === 1 || cfg.motorsEnabled === 'true';
    state.online = true;
    setBadgeOnline();
  } catch (err) {
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

async function saveConfig() {
  var payload = {
    pwmMin: parseInt(els.cfgPwmMin.value) || 0,
    pwmMax: parseInt(els.cfgPwmMax.value) || 255,
    pwmDefault: parseInt(els.cfgPwmDefault.value) || 150,
    motorsEnabled: els.cfgMotorsEnabled.checked
  };
  try {
    await API.post('/api/config', payload);
    state.online = true;
    setBadgeOnline();
  } catch (err) {
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

async function fetchSystemInfo() {
  try {
    var info = await API.get('/api/system/info');
    els.sysInfo.textContent = JSON.stringify(info, null, 2);
    state.online = true;
    setBadgeOnline();
  } catch (err) {
    els.sysInfo.textContent = 'Erro ao ler sistema';
    state.online = false;
    setBadgeOffline();
    showApiError(err.message || 'Erro desconhecido');
  }
}

function setBadgeOnline() {
  els.badge.textContent = 'Conectado';
  els.badge.className = 'badge online';
  els.telConn.textContent = 'Online';
  els.telConn.className = 'tel-value online';
}

function setBadgeOffline() {
  els.badge.textContent = 'Desconectado';
  els.badge.className = 'badge offline';
  els.telConn.textContent = 'Offline';
  els.telConn.className = 'tel-value offline';
}

function showApiError(msg) {
  if (!els.telApiError) return;
  els.telApiError.textContent = msg;
  els.telApiError.className = 'tel-value offline';
  console.error('API Error:', msg);
}

function onPointerStart(e) {
  e.stopPropagation();
  e.preventDefault();
  var pos = getPointerPos(e);
  activeTouchId = e.touches && e.touches.length ? e.touches[0].identifier : null;
  initJoystickGeometry();
  state.active = true;
  setKnobPosition(pos.x, pos.y);
  els.joystickKnob.style.cursor = 'grabbing';
}

function onPointerMove(e) {
  if (!state.active) return;
  var pos = getPointerPos(e);
  if (e.touches && e.touches.length) {
    var t = e.touches[0];
    if (activeTouchId !== null && t.identifier !== activeTouchId) return;
  }
  e.stopPropagation();
  e.preventDefault();
  setKnobPosition(pos.x, pos.y);
}

function onPointerEnd(e) {
  if (e.touches && e.touches.length) {
    var t = e.touches[0];
    if (activeTouchId !== null && t.identifier !== activeTouchId) return;
  }
  state.active = false;
  activeTouchId = null;
  resetKnob();
  els.joystickKnob.style.cursor = 'grab';
  sendDifferential(0, 0);
}

function onSpeedChange() {
  state.speedPct = parseInt(els.speedSlider.value) || 0;
  els.speedValue.textContent = state.speedPct + '%';
  updateMotorsFromJoy();
}

function onLeftChange() {
  state.leftPct = parseInt(els.leftSlider.value) || 0;
  els.leftValue.textContent = state.leftPct;
}

function onRightChange() {
  state.rightPct = parseInt(els.rightSlider.value) || 0;
  els.rightValue.textContent = state.rightPct;
}

function applyDifferential() {
  sendDifferential(state.leftPct, state.rightPct);
}

function onPowerChange() {
  state.powerOn = els.powerToggle.checked;
  if (!state.powerOn) {
    stopAll();
  } else {
    updateMotorsFromJoy();
  }
}

function onStopClick() {
  resetKnob();
  stopAll();
}

function onDpadClick(e) {
  var btn = e.currentTarget;
  var dir = btn.getAttribute('data-dir');
  resetKnob();
  sendDir(dir, state.speedPct);
}

function init() {
  getEls();

  if (els.joystickBase) {
    els.joystickBase.style.touchAction = 'none';
    els.joystickKnob.style.touchAction = 'none';
  }

  els.joystickBase.addEventListener('mousedown', onPointerStart);
  window.addEventListener('mousemove', onPointerMove);
  window.addEventListener('mouseup', onPointerEnd);

  els.joystickBase.addEventListener('touchstart', onPointerStart, { passive: false });
  els.joystickBase.addEventListener('touchmove', onPointerMove, { passive: false });
  els.joystickBase.addEventListener('touchend', onPointerEnd, { passive: false });
  els.joystickBase.addEventListener('touchcancel', onPointerEnd, { passive: false });
  window.addEventListener('touchmove', onPointerMove, { passive: false });
  window.addEventListener('touchend', onPointerEnd, { passive: false });
  window.addEventListener('touchcancel', onPointerEnd, { passive: false });

  els.speedSlider.addEventListener('input', onSpeedChange);
  els.leftSlider.addEventListener('input', onLeftChange);
  els.rightSlider.addEventListener('input', onRightChange);
  els.btnApplyDiff.addEventListener('click', applyDifferential);
  els.powerToggle.addEventListener('change', onPowerChange);
  els.btnStop.addEventListener('click', onStopClick);

  dpadBtns.forEach(function(btn) { btn.addEventListener('click', onDpadClick); });

  els.btnSaveConfig.addEventListener('click', saveConfig);
  els.btnSystemInfo.addEventListener('click', fetchSystemInfo);

  requestAnimationFrame(initJoystickGeometry);
  window.addEventListener('resize', initJoystickGeometry);

  loadConfig();
  fetchStatus();
  setInterval(fetchStatus, 1000);
}

document.addEventListener('DOMContentLoaded', init);

)rawliteral";

#endif // WEB_ASSETS_H