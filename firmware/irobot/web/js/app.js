const API = {
  base: '',
  endpoints: ['http://192.168.4.1', 'http://irobot.local', ''],
  current: 0,

  async _try(method, path, body) {
    let lastErr = null;
    for (let i = 0; i < this.endpoints.length; i++) {
      var base = this.endpoints[(this.current + i) % this.endpoints.length];
      var url = base + path;
      try {
        var res = await fetch(url, {
          method: method,
          headers: { 'Content-Type': 'application/json' },
          body: body ? JSON.stringify(body) : undefined,
          signal: AbortSignal.timeout(2500)
        });
        if (res.ok) {
          this.current = (this.current + i) % this.endpoints.length;
          return res.json();
        }
        lastErr = new Error('HTTP ' + res.status + ' ' + url);
      } catch (err) {
        lastErr = err;
      }
    }
    throw lastErr || new Error('falha na API');
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
