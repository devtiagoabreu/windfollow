// WindFollowBlind Simulador (sem camera)
// Simulacao visual de 2 motores DC via L298N + configuracao Wi-Fi simulada.

const sim = {
    // Estado da simulacao dos motores (posicao: -100..100, centro=0)
    motor: {
        pos1: 0,
        pos2: 0,
        moving1: false,
        moving2: false,
        direction1: 'parado',
        direction2: 'parado',
        step1: null,
        step2: null
    },

    // Configuracao
    config: {
        gpio_in1: 26, gpio_in2: 25, gpio_ena: -1,
        gpio_in3: 32, gpio_in4: 33, gpio_enb: -1,
        gpio_btn1: 27, gpio_btn2: 14,
        step_duration_ms: 150,
        motor1_enabled: true, motor2_enabled: true
    },

    // Wi-Fi simulado
    wifi: {
        mode: 'AP',
        ssid: 'WindFollowBlind-Setup',
        password: 'windfollow123',
        localIp: '192.168.4.1',
        configured: false,
        connectedNetwork: null
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
        this.updateMotorUI();
    },

    loadState() {
        const saved = localStorage.getItem('windflollowblind_sim_state');
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
        localStorage.setItem('windflollowblind_sim_state', JSON.stringify(state));
    },

    // ========================================================================
    // Tema / Tabs
    // ========================================================================
    toggleTheme() {
        const isLight = document.getElementById('theme-toggle').checked;
        if (isLight) {
            document.documentElement.setAttribute('data-theme', 'light');
            localStorage.setItem('windflollowblind_sim_theme', 'light');
        } else {
            document.documentElement.removeAttribute('data-theme');
            localStorage.setItem('windflollowblind_sim_theme', 'dark');
        }
    },

    setupTabs() {
        document.querySelectorAll('.tab').forEach(tab => {
            tab.addEventListener('click', (e) => {
                this.switchTab(e.currentTarget.dataset.tab, e.currentTarget);
            });
        });
        const themeToggle = document.getElementById('theme-toggle');
        const savedTheme = localStorage.getItem('windflollowblind_sim_theme');
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
    // Formulario
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
        this.config = { ...this.config, ...c };
        this.saveState();
        this.renderConfig();
        this.showMsg(document.getElementById('msg-result'), 'Configuracao salva (simulacao).', 'success');
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
            this.showMsg(msg, 'Pinos validos (simulacao).', 'success');
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
    // Wi-Fi (simulado)
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

    applyWifiSim() {
        const modeSel = document.getElementById('wifi-mode');
        const ssidIn = document.getElementById('wifi-ssid');
        const passIn = document.getElementById('wifi-password');
        if (modeSel) this.wifi.mode = modeSel.value;
        if (ssidIn) this.wifi.ssid = ssidIn.value;
        if (passIn) this.wifi.password = passIn.value;
        this.wifi.configured = true;
        this.wifi.localIp = '192.168.1.' + Math.floor(Math.random() * 200 + 20);
        this.saveState();
        this.updateWifiUI();
        this.showMsg(document.getElementById('wifi-msg'), 'Wi-Fi aplicado (simulacao).', 'success');
    },

    // ========================================================================
    // Motores (simulacao visual)
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

        const duration = this.config.step_duration_ms || 150;

        // Atualiza estado visual
        if (motor === '1') {
            this.motor.moving1 = true;
            this.motor.direction1 = direction;
            clearTimeout(this.motor.step1);
            if (direction === 'esquerda') {
                this.motor.pos1 = -80; // desloca para esquerda
            } else if (direction === 'direita') {
                this.motor.pos1 = 80;  // desloca para direita
            }
            this.motor.step1 = setTimeout(() => {
                this.motor.moving1 = false;
                this.motor.direction1 = 'parado';
                this.motor.pos1 = 0; // retorna ao centro
                this.updateMotorUI();
            }, duration);
        } else if (motor === '2') {
            this.motor.moving2 = true;
            this.motor.direction2 = direction;
            clearTimeout(this.motor.step2);
            if (direction === 'esquerda') {
                this.motor.pos2 = -80;
            } else if (direction === 'direita') {
                this.motor.pos2 = 80;
            }
            this.motor.step2 = setTimeout(() => {
                this.motor.moving2 = false;
                this.motor.direction2 = 'parado';
                this.motor.pos2 = 0;
                this.updateMotorUI();
            }, duration);
        } else {
            // motorStop todos
            this.motorStop();
            return;
        }

        this.updateMotorUI();
        this.showMsg(document.getElementById('msg-result'),
            'Motor ' + motor + ' -> ' + direction + ' (' + duration + ' ms)', 'info');
    },

    async motorStop(motor) {
        if (motor === '1') {
            clearTimeout(this.motor.step1);
            this.motor.moving1 = false;
            this.motor.direction1 = 'parado';
            this.motor.pos1 = 0;
        } else if (motor === '2') {
            clearTimeout(this.motor.step2);
            this.motor.moving2 = false;
            this.motor.direction2 = 'parado';
            this.motor.pos2 = 0;
        } else {
            clearTimeout(this.motor.step1);
            clearTimeout(this.motor.step2);
            this.motor.moving1 = false;
            this.motor.moving2 = false;
            this.motor.direction1 = 'parado';
            this.motor.direction2 = 'parado';
            this.motor.pos1 = 0;
            this.motor.pos2 = 0;
        }
        this.updateMotorUI();
    },

    async toggleMotor1(enabled) {
        this.config.motor1_enabled = enabled;
        this.saveState();
        if (!enabled) this.motorStop('1');
    },

    async toggleMotor2(enabled) {
        this.config.motor2_enabled = enabled;
        this.saveState();
        if (!enabled) this.motorStop('2');
    },

    updateMotorUI() {
        // Atualiza textos
        const m1 = document.getElementById('status-motor1');
        const m2 = document.getElementById('status-motor2');
        const d1 = document.getElementById('status-dir1');
        const d2 = document.getElementById('status-dir2');
        if (m1) m1.textContent = this.motor.moving1 ? 'Movendo' : 'Parado';
        if (m2) m2.textContent = this.motor.moving2 ? 'Movendo' : 'Parado';
        if (d1) d1.textContent = this.motor.direction1;
        if (d2) d2.textContent = this.motor.direction2;

        // Atualiza indicadores visuais (posicao -100..100 mapeada para 0..100% da faixa)
        const mapPos = (p) => 50 + (p / 2); // -100->0%, 0->50%, 100->100%
        const p1 = document.getElementById('motor1-pos');
        const p2 = document.getElementById('motor2-pos');
        const c1 = document.getElementById('motor1-center');
        const c2 = document.getElementById('motor2-center');
        if (p1) p1.style.left = mapPos(this.motor.pos1) + '%';
        if (p2) p2.style.left = mapPos(this.motor.pos2) + '%';
        if (c1) c1.style.left = '50%';
        if (c2) c2.style.left = '50%';

        // Cores dos indicadores
        const setColor = (el, moving, dir) => {
            if (!el) return;
            if (dir === 'esquerda') el.style.background = '#8be9fd'; // cyan
            else if (dir === 'direita') el.style.background = '#ff79c6'; // pink
            else el.style.background = moving ? '#50fa7b' : '#6272a4';
        };
        setColor(p1, this.motor.moving1, this.motor.direction1);
        setColor(p2, this.motor.moving2, this.motor.direction2);
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
