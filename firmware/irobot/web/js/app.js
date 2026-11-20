'use strict';

// -------------------- API --------------------
const API = {
  async _try(method, path, body) {
    const opts = { method };
    if (body !== undefined) {
      opts.headers = { 'Content-Type': 'application/json' };
      opts.body = JSON.stringify(body);
    }
    const res = await fetch(path, opts);
    const ct = res.headers.get('content-type') || '';
    const data = ct.includes('application/json') ? await res.json() : await res.text();
    if (!res.ok) throw new Error('HTTP ' + res.status + ' ' + path);
    return data;
  },
  get(path) { return this._try('GET', path); },
  post(path, body) { return this._try('POST', path, body); }
};

// -------------------- estado --------------------
const state = {
  powerOn: true,
  speedPct: 70,
  autoSpeed: 179,
  leftPct: 0,
  rightPct: 0,
  autoOn: false,
  lastStuckSeq: 0,
  lastFire: false,
  online: false,
  activeTab: 'home',
  joystick: { base: null, knob: null, cx: 0, cy: 0, radius: 0, knobRadius: 22, active: false, touchId: null, x: 0, y: 0, intensity: 0 },
  lastSent: { left: 0, right: 0, dir: '', speed: 0 }
};

const joyState = {
  home: { base: null, knob: null, cx: 0, cy: 0, radius: 0, dirEl: null, intEl: null, active: false, touchId: null, x: 0, y: 0, intensity: 0 },
  dl:   { base: null, knob: null, cx: 0, cy: 0, radius: 0, dirEl: null, intEl: null, active: false, touchId: null, x: 0, y: 0, intensity: 0 }
};

// -------------------- util --------------------
function $(id) { return document.getElementById(id); }
function pctToPwm(pct) { return Math.round((pct / 100) * 255); }
function safe(v, d) { return (v === undefined || v === null || Number.isNaN(v)) ? d : v; }
function fmtTemp(v) { return (v === undefined || v === null || Number.isNaN(v)) ? '--' : v.toFixed(1) + ' Â°C'; }
function fmtHum(v) { return (v === undefined || v === null || Number.isNaN(v)) ? '--' : v.toFixed(0) + ' %'; }
function fmtPres(v) { return (v === undefined || v === null || Number.isNaN(v)) ? '--' : v.toFixed(0) + ' hPa'; }
function fmtDist(mm) { return (mm === undefined || mm === null || !mm) ? '--' : (mm / 10).toFixed(1) + ' cm'; }
function escapeHtml(str) {
  return String(str).replace(/[&<>"']/g, c => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' }[c]));
}

// -------------------- tabs --------------------
function initTabs() {
  const tabs = Array.from(document.querySelectorAll('.tab'));
  tabs.forEach(tab => {
    tab.addEventListener('click', () => selectTab(tab.getAttribute('data-tab')));
  });
  selectTab('home');
}

function selectTab(name) {
  state.activeTab = name;
  document.querySelectorAll('.tab').forEach(t => {
    const active = t.getAttribute('data-tab') === name;
    t.classList.toggle('active', active);
    t.setAttribute('aria-selected', active ? 'true' : 'false');
  });
  ['home', 'dl', 'config'].forEach(n => {
    const p = $('panel-' + n);
    if (p) {
      p.classList.toggle('active', n === name);
      p.classList.toggle('hidden', n !== name);
    }
  });
  if (name === 'home') requestAnimationFrame(() => { measureJoystick('home'); });
  if (name === 'dl') requestAnimationFrame(() => { measureJoystick('dl'); });
  try { history.replaceState(null, '', '#' + name); } catch (e) {}
  try { localStorage.setItem('irobot-tab', name); } catch (e) {}
}

// -------------------- conectado --------------------
function setOnline(ok) {
  state.online = !!ok;
  const badge = $('badge-conn');
  if (badge) {
    badge.textContent = ok ? 'Conectado' : 'Desconectado';
    badge.className = 'badge ' + (ok ? 'online' : 'offline');
  }
  const homeConn = $('home-conn');
  if (homeConn) {
    homeConn.textContent = ok ? 'Conectado' : 'Desconectado';
    homeConn.className = 'status-v ' + (ok ? 'online' : 'offline');
  }
  const dlConn = $('dl-tel-conn');
  if (dlConn) {
    dlConn.textContent = ok ? 'Online' : 'Offline';
    dlConn.className = 'tel-value ' + (ok ? 'online' : 'offline');
  }
}

// -------------------- joystick --------------------
function measureJoystick(key) {
  const j = joyState[key];
  if (!j.base) return;
  const r = j.base.getBoundingClientRect();
  const k = j.knob.getBoundingClientRect();
  j.cx = r.left + r.width / 2;
  j.cy = r.top + r.height / 2;
  j.knobRadius = k.width / 2;
  j.radius = Math.max(10, r.width / 2 - j.knobRadius - 4);
}

function pointerPos(e) {
  if (e.touches && e.touches.length) return { x: e.touches[0].clientX, y: e.touches[0].clientY, id: e.touches[0].identifier };
  return { x: e.clientX, y: e.clientY, id: null };
}

function setKnob(key, x, y) {
  const j = joyState[key];
  if (!j.base || !j.knob) return;
  let dx = x - j.cx, dy = y - j.cy;
  let dist = Math.hypot(dx, dy);
  let cx = x, cy = y;
  if (dist > j.radius && dist > 0) {
    const ratio = j.radius / dist;
    cx = j.cx + dx * ratio;
    cy = j.cy + dy * ratio;
  }
  const ox = cx - j.cx, oy = cy - j.cy;
  j.knob.style.transform = 'translate(calc(-50% + ' + ox + 'px), calc(-50% + ' + oy + 'px))';
  const norm = Math.min(dist / j.radius, 1);
  j.x = ox / j.radius;
  j.y = -oy / j.radius;
  j.intensity = norm < 0.07 ? 0 : norm;
  if (j.intensity === 0) { j.x = 0; j.y = 0; }
  updateJoyReadout(key);
  sendJoystick(key);
}

function joyLabel(x, y, i) {
  if (i < 0.07) return 'Parado';
  if (Math.abs(y) > Math.abs(x)) return y > 0 ? 'Frente' : 'TrÃ¡s';
  return x > 0 ? 'Direita' : 'Esquerda';
}

function updateJoyReadout(key) {
  const j = joyState[key];
  if (j.dirEl) j.dirEl.textContent = joyLabel(j.x, j.y, j.intensity);
  if (j.intEl) j.intEl.textContent = 'Int: ' + Math.round(j.intensity * 100) + '%';
}

function resetJoystick(key) {
  const j = joyState[key];
  if (j.knob) j.knob.style.transform = 'translate(-50%, -50%)';
  j.x = 0; j.y = 0; j.intensity = 0; j.active = false; j.touchId = null;
  updateJoyReadout(key);
  sendDifferential(0, 0);
}

function sendJoystick(key) {
  if (!state.powerOn) return;
  const j = joyState[key];
  const scale = state.speedPct / 100;
  let left = j.y * j.intensity - j.x * j.intensity;
  let right = j.y * j.intensity + j.x * j.intensity;
  left = Math.max(-1, Math.min(1, left));
  right = Math.max(-1, Math.min(1, right));
  sendDifferential(Math.round(left * 100 * scale), Math.round(right * 100 * scale));
}

function bindJoystick(key, baseId, knobId, dirId, intId) {
  const j = joyState[key];
  j.base = $(baseId);
  j.knob = $(knobId);
  j.dirEl = $(dirId);
  j.intEl = $(intId);
  if (!j.base || !j.knob) return;
  j.base.style.touchAction = 'none';
  j.knob.style.touchAction = 'none';

  const start = e => {
    e.preventDefault(); e.stopPropagation();
    const p = pointerPos(e);
    measureJoystick(key);
    j.active = true; j.touchId = p.id;
    setKnob(key, p.x, p.y);
  };
  const move = e => {
    if (!j.active) return;
    const p = pointerPos(e);
    if (j.touchId !== null && p.id !== j.touchId) return;
    e.preventDefault(); e.stopPropagation();
    setKnob(key, p.x, p.y);
  };
  const end = e => {
    if (!j.active) return;
    if (e.touches && e.touches.length) {
      const t = e.touches[0];
      if (j.touchId !== null && t.identifier !== j.touchId) return;
    }
    e.preventDefault(); e.stopPropagation();
    resetJoystick(key);
  };

  j.base.addEventListener('mousedown', start);
  j.base.addEventListener('touchstart', start, { passive: false });
  window.addEventListener('mousemove', move);
  window.addEventListener('touchmove', move, { passive: false });
  window.addEventListener('mouseup', end);
  window.addEventListener('touchend', end, { passive: false });
  window.addEventListener('touchcancel', end, { passive: false });
  window.addEventListener('resize', () => measureJoystick(key));
}

// -------------------- comandos --------------------
async function sendDifferential(left, right) {
  if (left === state.lastSent.left && right === state.lastSent.right) return;
  try {
    await API.post('/api/motor/move', { leftSpeed: left, rightSpeed: right });
    state.lastSent = { left, right, dir: '', speed: 0 };
    setOnline(true);
  } catch (err) {
    setOnline(false);
  }
}

async function sendDir(dir) {
  const speed = pctToPwm(state.speedPct);
  try {
    await API.post('/api/motor/move', { direction: dir, speed });
    state.lastSent = { left: 0, right: 0, dir, speed };
    setOnline(true);
    updateStatusLocal(dir);
  } catch (err) {
    setOnline(false);
  }
}

async function stopAll() {
  try {
    await API.post('/api/motor/stop', {});
    setOnline(true);
  } catch (err) {
    setOnline(false);
  }
  state.lastSent = { left: 0, right: 0, dir: '', speed: 0 };
  updateStatusLocal('stop');
}

function updateStatusLocal(dir) {
  const label = { frente: 'Andando', tras: 'Andando', esquerda: 'Andando', direita: 'Andando', stop: 'Parado' }[dir] || 'Parado';
  const el = $('home-state');
  if (el) el.textContent = label;
  const mov = $('home-moving');
  if (mov) mov.textContent = label;
  const dlSt = $('dl-tel-status');
  if (dlSt) dlSt.textContent = ({ frente: 'Frente', tras: 'TrÃ¡s', esquerda: 'Esquerda', direita: 'Direita', stop: 'Parado' }[dir] || 'Parado');
}

async function toggleAuto() {
  const turnOn = !state.autoOn;
  try {
    const res = await API.post('/api/auto', { enabled: turnOn });
    state.autoOn = res.enabled === true;
    setOnline(true);
    syncAutoButtons();
    if (!state.autoOn) { resetJoystick('home'); resetJoystick('dl'); }
    fetchSensors();
  } catch (err) {
    setOnline(false);
  }
}

function syncAutoButtons() {
  const txt = 'Modo Autonomo: ' + (state.autoOn ? 'ON' : 'OFF');
  ['btn-home-auto', 'dl-auto', 'btn-auto'].forEach(id => {
    const el = $(id);
    if (el) {
      el.textContent = txt;
      el.classList.toggle('on', state.autoOn);
    }
  });
}

// -------------------- sensores --------------------
function buildAdviceMessages(s) {
  const a = s.alerts || {};
  const msgs = [];
  if (a.fire) msgs.push('INCENDIO: temperatura subiu rapido â€” verifique o ambiente!');
  if (a.rainSensor) msgs.push('Chuva agora (sensor molhado)');
  else if (a.rain) msgs.push('Pode chover: pressao caiu ' + Math.abs(a.trend3h || 0).toFixed(1) + ' hPa/3h');
  if (a.hot) msgs.push('Calor: ligue o ar-condicionado');
  if (a.cold) msgs.push('Frio: desligue o ar-condicionado');
  if (a.dry) msgs.push('Ar seco: beba agua');
  return msgs;
}

function renderTicker(s) {
  const el = $('ticker-text');
  if (!el) return;
  el.textContent = 'Temp ' + fmtTemp(s.temperature) + '  â€¢  Umid ' + fmtHum(s.humidity) +
    '  â€¢  Press ' + fmtPres(s.pressure) + '  â€¢  Dist ' + fmtDist(s.distanceMm) +
    '  â€¢  Autonomo ' + (s.auto ? (s.autoState || 'ON') : 'OFF');
}

function alertSeverity(text) {
  if (/INCENDIO/i.test(text)) return 'crit';
  if (/Chuva agora/i.test(text)) return 'crit';
  if (/Pode chover|Calor|seco/i.test(text)) return 'warn';
  return 'info';
}

function renderHeaderAlerts(msgs) {
  const el = $('header-alerts');
  if (!el) return;
  if (!msgs.length) {
    el.innerHTML = '<span class="alert-chip ok">Sem avisos</span>';
    return;
  }
  el.innerHTML = msgs.map(m => '<span class="alert-chip ' + alertSeverity(m) + '">' + escapeHtml(m) + '</span>').join('');
}

function renderHomeSensors(s, msgs) {
  $('home-temp').textContent = fmtTemp(s.temperature);
  $('home-hum').textContent = fmtHum(s.humidity);
  $('home-pres').textContent = fmtPres(s.pressure);
  $('home-dist').textContent = fmtDist(s.distanceMm);
  if (s.alerts && s.alerts.fire && !state.lastFire) notifyFire();
  state.lastFire = !!(s.alerts && s.alerts.fire);
}

function renderDlSensors(s, msgs) {
  $('dl-temp').textContent = fmtTemp(s.temperature);
  $('dl-hum').textContent = fmtHum(s.humidity);
  $('dl-pres').textContent = fmtPres(s.pressure);
  $('dl-dist').textContent = fmtDist(s.distanceMm);
  const present = [];
  if (s.aht) present.push('AHT20');
  if (s.bmp) present.push('BMP280');
  if (s.tof) present.push('VL53L0X');
  const st = $('dl-env-status');
  if (st) st.textContent = (present.length ? 'sensores: ' + present.join(', ') : 'sensores: nenhum') +
    (s.auto ? ' | autonomo: ' + s.autoState + ' (' + safe(s.avoidCount, 0) + '/100)' : '');
  const list = $('dl-advice-list');
  if (list) {
    if (!msgs.length) list.innerHTML = '<li class="advice-none">Nenhum aviso no momento</li>';
    else list.innerHTML = msgs.map(m => '<li>' + escapeHtml(m) + '</li>').join('');
  }
  const tr = $('dl-trend-status');
  if (tr) {
    const a = s.alerts || {};
    if (a.trendReady) tr.textContent = 'tendencia: ' + (a.trend30m || 0).toFixed(1) + ' hPa/30min | ' + (a.trend3h || 0).toFixed(1) + ' hPa/3h';
    else tr.textContent = 'tendencia: coletando dados...';
  }
}

async function fetchSensors() {
  try {
    const s = await API.get('/api/sensors');
    setOnline(true);
    state.autoOn = s.auto === true;
    syncAutoButtons();
    if (s.autoSpeed !== undefined) setAutoSpeedUI(s.autoSpeed);
    const msgs = buildAdviceMessages(s);
    renderTicker(s);
    renderHeaderAlerts(msgs);
    renderHomeSensors(s, msgs);
    renderDlSensors(s, msgs);
    if (s.stuck && s.stuckSeq !== state.lastStuckSeq) {
      state.lastStuckSeq = s.stuckSeq;
      notifyStuck();
    }
  } catch (err) {
    setOnline(false);
    const el = $('ticker-text');
    if (el) el.textContent = 'Falha ao ler sensores: ' + (err.message || 'erro');
  }
}

async function fetchStatus() {
  try {
    const st = await API.get('/api/motor/status');
    setOnline(true);
    const dirMap = { 0: 'Parado', 1: 'Frente', 2: 'TrÃ¡s', 3: 'Esquerda', 4: 'Direita' };
    const dir = dirMap[st.direction] || 'Parado';
    const homeState = $('home-state');
    if (homeState) homeState.textContent = st.moving ? 'Andando' : 'Parado';
    const homeMoving = $('home-moving');
    if (homeMoving) homeMoving.textContent = st.moving ? 'Andando (' + dir + ')' : 'Parado';
    const dlSt = $('dl-tel-status'); if (dlSt) dlSt.textContent = dir;
    const dlL = $('dl-tel-left'); if (dlL) dlL.textContent = st.leftSpeed;
    const dlR = $('dl-tel-right'); if (dlR) dlR.textContent = st.rightSpeed;
    const dlM = $('dl-tel-moving'); if (dlM) { dlM.textContent = st.moving ? 'Sim' : 'Nao'; dlM.className = 'tel-value ' + (st.moving ? 'online' : 'offline'); }
    const dlE = $('dl-tel-api-error'); if (dlE) { dlE.textContent = 'OK'; dlE.className = 'tel-value online'; }
  } catch (err) {
    setOnline(false);
    const dlE = $('dl-tel-api-error'); if (dlE) { dlE.textContent = err.message || 'erro'; dlE.className = 'tel-value offline'; }
  }
}

// -------------------- alertas --------------------
function notifyFire() {
  const el = $('alert-fire'); if (el) el.hidden = false;
  try { if (navigator.vibrate) navigator.vibrate([900, 200, 900, 200, 1500]); } catch (e) {}
  try {
    const Ctx = window.AudioContext || window.webkitAudioContext;
    if (Ctx) {
      const ctx = new Ctx();
      [0, 0.4, 0.8, 1.2].forEach(t => {
        const o = ctx.createOscillator(), g = ctx.createGain();
        o.type = 'sawtooth'; o.frequency.value = 1200; g.gain.value = 0.09;
        o.connect(g); g.connect(ctx.destination);
        o.start(ctx.currentTime + t); o.stop(ctx.currentTime + t + 0.25);
      });
    }
  } catch (e) {}
}

function notifyStuck() {
  const el = $('alert-stuck'); if (el) el.hidden = false;
  try { if (navigator.vibrate) navigator.vibrate([500, 200, 500, 200, 800]); } catch (e) {}
}

// -------------------- config --------------------
function setAutoSpeedUI(v) {
  const val = Math.max(60, Math.min(255, parseInt(v) || 179));
  state.autoSpeed = val;
  ['home-auto-speed', 'dl-auto-speed', 'cfg-auto-speed', 'dl-auto-speed-cfg'].forEach(id => {
    const el = $(id); if (el) el.value = val;
  });
  ['home-auto-speed-val', 'dl-auto-speed-val'].forEach(id => {
    const el = $(id); if (el) el.textContent = val;
  });
}

async function loadConfig() {
  try {
    const cfg = await API.get('/api/config');
    state.config = cfg;
    const set = (id, v) => { const el = $(id); if (el && v !== undefined && v !== null) el.value = v; };
    set('cfg-pwm-min', cfg.pwmMin); set('cfg-pwm-max', cfg.pwmMax); set('cfg-pwm-default', cfg.pwmDefault);
    set('dl-pwm-min', cfg.pwmMin); set('dl-pwm-max', cfg.pwmMax); set('dl-pwm-default', cfg.pwmDefault);
    setAutoSpeedUI(cfg.autoSpeed !== undefined ? cfg.autoSpeed : 179);
    const mot = cfg.motorsEnabled === true || cfg.motorsEnabled === 1 || cfg.motorsEnabled === 'true';
    if ($('cfg-motors-enabled')) $('cfg-motors-enabled').checked = mot;
    if ($('dl-motors-enabled')) $('dl-motors-enabled').checked = mot;
    setOnline(true);
  } catch (err) { setOnline(false); }
}

async function saveConfig(prefix) {
  const get = id => { const el = $(id); return el ? el.value : undefined; };
  const autoSpeedId = prefix === 'dl' ? 'dl-auto-speed-cfg' : 'cfg-auto-speed';
  const payload = {
    pwmMin: parseInt(get(prefix + '-pwm-min')) || 0,
    pwmMax: parseInt(get(prefix + '-pwm-max')) || 255,
    pwmDefault: parseInt(get(prefix + '-pwm-default')) || 150,
    autoSpeed: parseInt(get(autoSpeedId)) || state.autoSpeed || 179,
    motorsEnabled: $(prefix + '-motors-enabled') ? $(prefix + '-motors-enabled').checked : true
  };
  try { await API.post('/api/config', payload); setOnline(true); await loadConfig(); }
  catch (err) { setOnline(false); }
}

async function fetchSystemInfo(target) {
  try {
    const info = await API.get('/api/system/info');
    const el = $(target);
    if (el) el.textContent = JSON.stringify(info, null, 2);
    setOnline(true);
  } catch (err) { setOnline(false); }
}

function initWifiMode() {
  const sel = $('cfg-wifi-mode');
  if (!sel) return;
  const apply = () => {
    const isSta = sel.value === 'sta';
    ['cfg-wifi-sta-row', 'cfg-wifi-pass-row'].forEach(id => { const el = $(id); if (el) el.style.display = isSta ? 'flex' : 'none'; });
    ['cfg-wifi-ap-row', 'cfg-wifi-ap-pass-row'].forEach(id => { const el = $(id); if (el) el.style.display = isSta ? 'none' : 'flex'; });
  };
  sel.addEventListener('change', apply);
  apply();
}

// -------------------- tela cheia / PWA --------------------
function initFullscreen() {
  const cb = $('fs-toggle');
  if (!cb) return;
  cb.addEventListener('change', async () => {
    try {
      if (cb.checked) {
        const el = document.documentElement;
        if (el.requestFullscreen) await el.requestFullscreen();
        else if (el.webkitRequestFullscreen) await el.webkitRequestFullscreen();
        try { await screen.orientation.lock('landscape'); } catch (e) {}
      } else {
        if (document.exitFullscreen) await document.exitFullscreen();
        else if (document.webkitExitFullscreen) await document.webkitExitFullscreen();
      }
    } catch (e) { cb.checked = false; }
  });
  document.addEventListener('fullscreenchange', () => {
    cb.checked = !!(document.fullscreenElement || document.webkitFullscreenElement);
  });
}

function initPwa() {
  if ('serviceWorker' in navigator) {
    navigator.serviceWorker.register('/sw.js').catch(() => {});
  }
  window.addEventListener('beforeinstallprompt', e => {
    e.preventDefault();
    window.__deferredPrompt = e;
  });
}

// -------------------- init --------------------
function init() {
  initTabs();
  initWifiMode();
  initFullscreen();
  initPwa();

  bindJoystick('home', 'joy-base', 'joy-knob', 'joy-dir', 'joy-int');
  bindJoystick('dl', 'dl-joy-base', 'dl-joy-knob', 'dl-joy-dir', 'dl-joy-int');
  requestAnimationFrame(() => { measureJoystick('home'); measureJoystick('dl'); });

  document.querySelectorAll('.dpad-btn[data-dir]').forEach(btn => {
    btn.addEventListener('click', e => {
      e.preventDefault();
      const dir = btn.getAttribute('data-dir');
      const key = btn.id.startsWith('dl-') ? 'dl' : 'home';
      resetJoystick(key);
      sendDir(dir);
      document.querySelectorAll('.dpad-btn').forEach(b => b.classList.toggle('active', b === btn));
      setTimeout(() => btn.classList.remove('active'), 200);
    });
  });

  const stopBtns = [$('btn-stop'), $('dl-stop')];
  stopBtns.forEach(b => { if (b) b.addEventListener('click', () => {
    resetJoystick('home'); resetJoystick('dl'); stopAll();
  }); });

  ['btn-home-auto', 'dl-auto', 'btn-auto'].forEach(id => {
    const el = $(id); if (el) el.addEventListener('click', toggleAuto);
  });

  const bindSpeed = (sliderId, valId) => {
    const sl = $(sliderId), vv = $(valId);
    if (!sl) return;
    sl.addEventListener('input', () => {
      state.speedPct = parseInt(sl.value) || 0;
      document.querySelectorAll('#home-speed, #dl-speed').forEach(s => { if (s) s.value = state.speedPct; });
      document.querySelectorAll('#home-speed-val, #dl-speed-val').forEach(s => { if (s) s.textContent = state.speedPct + '%'; });
    });
  };
  bindSpeed('home-speed', 'home-speed-val');
  bindSpeed('dl-speed', 'dl-speed-val');

  const bindAutoSpeed = (sliderId, valId) => {
    const sl = $(sliderId), vv = $(valId);
    if (!sl) return;
    sl.addEventListener('input', () => {
      const v = parseInt(sl.value) || 179;
      if (vv) vv.textContent = v;
    });
    sl.addEventListener('change', async () => {
      const v = parseInt(sl.value) || 179;
      setAutoSpeedUI(v);
      try { await API.post('/api/auto', { autoSpeed: v }); setOnline(true); }
      catch (err) { setOnline(false); }
    });
  };
  bindAutoSpeed('home-auto-speed', 'home-auto-speed-val');
  bindAutoSpeed('dl-auto-speed', 'dl-auto-speed-val');

  const bindDiffSlider = (id, valId, which) => {
    const sl = $(id), vv = $(valId);
    if (!sl) return;
    sl.addEventListener('input', () => {
      const v = parseInt(sl.value) || 0;
      if (vv) vv.textContent = v;
      state[which] = v;
    });
  };
  bindDiffSlider('dl-left', 'dl-left-val', 'leftPct');
  bindDiffSlider('dl-right', 'dl-right-val', 'rightPct');
  const apply = $('dl-apply-diff');
  if (apply) apply.addEventListener('click', () => sendDifferential(state.leftPct, state.rightPct));

  const savePwm = $('cfg-save-pwm'); if (savePwm) savePwm.addEventListener('click', () => saveConfig('cfg'));
  const saveDl = $('dl-save-config'); if (saveDl) saveDl.addEventListener('click', () => saveConfig('dl'));
  const sysA = $('cfg-sys-info'); if (sysA) sysA.addEventListener('click', () => fetchSystemInfo('cfg-system-info'));
  const sysB = $('dl-sys-info'); if (sysB) sysB.addEventListener('click', () => fetchSystemInfo('dl-system-info'));
  const restart = $('cfg-restart'); if (restart) restart.addEventListener('click', async () => {
    try { await fetch('/api/system/restart', { method: 'POST' }); } catch (e) {}
  });
  const saveWifi = $('cfg-save-wifi'); if (saveWifi) saveWifi.addEventListener('click', async () => {
    try {
      const mode = $('cfg-wifi-mode').value;
      const payload = mode === 'sta'
        ? { mode, ssid: $('cfg-wifi-ssid').value, pass: $('cfg-wifi-pass').value }
        : { mode, ssid: $('cfg-ap-ssid').value, pass: $('cfg-ap-pass').value };
      await API.post('/api/wifi', payload);
      setOnline(true);
    } catch (err) { setOnline(false); }
  });

  const fa = $('btn-fire-ok'); if (fa) fa.addEventListener('click', () => { const e = $('alert-fire'); if (e) e.hidden = true; });
  const sa = $('btn-alert-ok'); if (sa) sa.addEventListener('click', () => { const e = $('alert-stuck'); if (e) e.hidden = true; });
  const fb = $('dl-btn-fire-ok'); if (fb) fb.addEventListener('click', () => { const e = $('dl-alert-fire'); if (e) e.hidden = true; });
  const sb = $('dl-btn-alert-ok'); if (sb) sb.addEventListener('click', () => { const e = $('dl-alert-stuck'); if (e) e.hidden = true; });

  loadConfig();
  fetchStatus();
  fetchSensors();
  setInterval(fetchStatus, 1000);
  setInterval(fetchSensors, 1500);

  try {
    const saved = localStorage.getItem('irobot-tab');
    if (saved) selectTab(saved);
  } catch (e) {}
  if (location.hash) {
    const h = location.hash.replace('#', '');
    if (['home', 'dl', 'config'].includes(h)) selectTab(h);
  }
}

document.addEventListener('DOMContentLoaded', init);

