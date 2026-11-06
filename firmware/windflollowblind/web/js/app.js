// WindFollowBlind Simulador / Interface Web (sem camera)
// Controle de 2 motores DC via L298N + configuracao Wi-Fi igual ao WindFollow.

const sim = {
    // Estado da interface
    stream: null,
    cameraActive: false,

    // Estado Wi-Fi simulado/persistido
    wifi: {
        configured: false,
        mode: 'AP', // 'AP' ou 'STA'
        ssid: 'WindFollowBlind-Setup',
        password: '',
        localIp: '192.168.4.1',
        mdns: 'windflollowblind.local',
        connectedNetwork: null,
        signalStrength: 0
    },

    // Redes simuladas para scan (no ESP real isso vem do WiFi.scanNetworks())
    availableNetworks: [
        { ssid: 'Casa_WiFi', rssi: -45 },
        { ssid: 'Vizinho_2G', rssi: -68 },
        { ssid: 'Empresa_Guest', rssi: -72 },
        { ssid: 'Meu_Internet', rssi: -55 },
        { ssid: 'WiFi_Livre', rssi: -80 }
    ],

    // Configuracao de hardware (padrao ESP32 + L298N + 2 motores)
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

        // Atualizacao periodica do status (motor + wifi)
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
        document.querySelectorAll('.tab').forEach(tab => {
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
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
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
        const configured = this.wifi.configured;
        if (badge) {
            badge.textContent = this.wifi.mode === 'AP' ? 'Modo: Access Point (AP)' : 'Modo: Estacao (STA)';
            badge.style.background = this.wifi.mode === 'AP' ? '#2563eb' : '#16a34a';
        }
        document.getElementById('wifi-current-ssid').textContent = configured
            ? (this.wifi.connectedNetwork || this.wifi.ssid)
            : this.wifi.ssid;
        document.getElementById('wifi-local-ip').textContent = this.wifi.localIp;
        document.getElementById('wifi-mdns').textContent = 'http://' + this.wifi.mdns + '/';
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
                'Acesse o ESP32 em: http://windflollowblind.local/ ou http://' + this.wifi.localIp, 'info');
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
        try {
            await this.api('POST', '/api/motor/move', { motor, direction });
            this.showMsg(document.getElementById('msg-result'),
                'Motor ' + motor + ' -> ' + direction, 'info');
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
    },

    async toggleMotor2(enabled) {
        this.config.motor2_enabled = enabled;
        await this.api('POST', '/api/config', this.config);
        this.saveState();
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
        const i1 = document.getElementById('info-dir1');
        const i2 = document.getElementById('info-dir2');
        if (m1) m1.textContent = this.motor.moving1 ? 'Movendo' : 'Parado';
        if (m2) m2.textContent = this.motor.moving2 ? 'Movendo' : 'Parado';
        if (d1) d1.textContent = this.motor.direction1;
        if (d2) d2.textContent = this.motor.direction2;
        if (i1) i1.textContent = this.motor.direction1;
        if (i2) i2.textContent = this.motor.direction2;
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        if (arrow && zoneBar) {
            // M1 direcao influencia a seta visual (esquerda=-50%, direita=+50%)
            let pct = 50;
            if (this.motor.direction1 === 'esquerda') pct = 0;
            else if (this.motor.direction1 === 'direita') pct = 100;
            arrow.style.left = pct + '%';
            zoneBar.style.left = pct + '%';
        }
    },

    // ========================================================================
    // Camera (desabilitada — sem camera neste projeto)
    // ========================================================================
    async toggleCamera() {
        this.showMsg(document.getElementById('msg-result'),
            'Esta versao nao possui camera.', 'info');
    },

    // ========================================================================
    // Mensagens / utilitarios
    // ========================================================================
    showMsg(el, text, type) {
        if (!el) return;
        el.textContent = text;
        el.className = 'msg-result ' + type;
        el.style.display = 'block';
        setTimeout(() => { el.style.display = 'none'; }, 3000);
    },

    updateStatusUI() {
        const statusEl = document.getElementById('connection-status');
        if (statusEl) {
            const connected = this.wifi.configured && this.wifi.mode === 'STA';
            statusEl.textContent = connected ? 'Conectado (STA)' : 'Modo AP';
            statusEl.className = 'status-indicator ' + (connected ? 'status-on' : 'status-off');
        }
    }
};

// Inicializa quando o DOM estiver pronto
document.addEventListener('DOMContentLoaded', () => {
    sim.init();
});
