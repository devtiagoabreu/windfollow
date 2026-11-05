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
