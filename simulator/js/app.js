// WindFollow Simulador — detecção de movimento REAL via webcam + configuracao Wi-Fi simulada
// Persiste em localStorage (simula Preferences/NVS do ESP32)

function varCss(name) {
    return getComputedStyle(document.documentElement).getPropertyValue(name).trim();
}

const sim = {
    // Estado da camera
    stream: null,
    video: null,
    canvas: null,
    ctx: null,
    prevFrame: null,
    frameWidth: 0,
    frameHeight: 0,
    detectWidth: 160,   // resolução menor para detecção rápida
    detectHeight: 120,
    cameraActive: false,
    animFrame: null,
    lastMoveTime: 0,
    frameCount: 0,
    targetX: 320,       // posição X do alvo no vídeo (inicia no centro)
    targetY: 240,       // posição Y do alvo no vídeo

    // Estado Wi-Fi simulado
    wifi: {
        configured: false,
        mode: 'AP', // 'AP' ou 'STA'
        ssid: 'WindFollow-Setup',
        password: '',
        localIp: '192.168.4.1',
        mdns: 'windfollow.local',
        connectedNetwork: null,
        signalStrength: 0
    },

    // Redes simuladas para scan (no ESP real isso viria do WiFi.scanNetworks())
    availableNetworks: [
        { ssid: 'Casa_WiFi', rssi: -45 },
        { ssid: 'Vizinho_2G', rssi: -68 },
        { ssid: 'Empresa_Guest', rssi: -72 },
        { ssid: 'Meu_Internet', rssi: -55 },
        { ssid: 'WiFi_Livre', rssi: -80 }
    ],

    // Configuracao de hardware
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

    occupiedGpios: [4, 5, 8, 9, 10, 17, 18, 25, 26, 36, 39, 41, 42, 43, 44, 45, 46, 48],

    init() {
        this.video = document.getElementById('cam-video');
        this.canvas = document.getElementById('cam-canvas');
        this.ctx = this.canvas.getContext('2d');

        this.loadState();
        this.renderConfig();
        this.updateWifiUI();
        this.setupTabs();
        this.setupForm();
        this.updateStatus();
    },

    // ============ TABS ============
    setupTabs() {
        document.querySelectorAll('.tab').forEach(tab => {
            tab.addEventListener('click', (e) => {
                const targetTab = e.currentTarget.dataset.tab;
                this.switchTab(targetTab, e.currentTarget);
            });
        });

        // Tema
        const themeToggle = document.getElementById('theme-toggle');
        const savedTheme = localStorage.getItem('windfollow_theme');
        if (savedTheme === 'light') {
            document.documentElement.setAttribute('data-theme', 'light');
            themeToggle.checked = true;
        } else {
            document.documentElement.removeAttribute('data-theme');
            themeToggle.checked = false;
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
        // Atualiza botoes de tab
        document.querySelectorAll('.tab').forEach(t => t.classList.remove('active'));
        if (btn) btn.classList.add('active');

        // Mostra/esconde painéis
        document.querySelectorAll('.tab-panel').forEach(p => p.classList.remove('active'));
        document.getElementById('tab-' + tabId).classList.add('active');
    },

    // ============ ESTADO / PERSISTENCIA ============
    loadState() {
        const saved = localStorage.getItem('windfollow_state');
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
        localStorage.setItem('windfollow_state', JSON.stringify(state));
    },

    // ============ WIFI / SETUP ============
    updateWifiUI() {
        document.getElementById('wifi-mode-badge').textContent =
            this.wifi.mode === 'AP' ? 'Modo: Access Point (AP)' : 'Modo: Estacao (STA)';
        document.getElementById('wifi-current-ssid').textContent = this.wifi.configured
            ? this.wifi.connectedNetwork
            : this.wifi.ssid;
        document.getElementById('wifi-local-ip').textContent = this.wifi.localIp;
        document.getElementById('wifi-mdns').textContent = 'http://' + this.wifi.mdns + '/setup';

        const badge = document.getElementById('wifi-mode-badge');
        badge.style.background = this.wifi.mode === 'AP' ? '#2563eb' : '#16a34a';

        this.updateStatus();
    },

    async scanWifi() {
        const select = document.getElementById('wifi-networks');
        const msg = document.getElementById('wifi-msg');

        select.innerHTML = '<option value="">Procurando redes...</option>';

        await new Promise(r => setTimeout(r, 1200));

        select.innerHTML = '<option value="">-- Selecione uma rede --</option>';
        this.availableNetworks.forEach(net => {
            const opt = document.createElement('option');
            opt.value = net.ssid;
            opt.textContent = `${net.ssid} (${net.rssi} dBm)`;
            select.appendChild(opt);
        });

        this.showMsg(msg, `${this.availableNetworks.length} redes encontradas`, 'info');
        setTimeout(() => msg.style.display = 'none', 2000);
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

        await new Promise(r => setTimeout(r, 1500));

        this.wifi.configured = true;
        this.wifi.mode = 'STA';
        this.wifi.connectedNetwork = ssid;
        this.wifi.localIp = '192.168.1.' + Math.floor(Math.random() * 200 + 20);
        this.wifi.signalStrength = -50;
        this.wifi.password = password;

        this.saveState();
        this.updateWifiUI();

        btn.disabled = false;
        btn.textContent = origText;

        this.showMsg(msg, `Conectado a "${ssid}" — IP: ${this.wifi.localIp}`, 'success');
        this.showMsg(document.getElementById('msg-result'),
            `Acesse o ESP32 em: http://windfollow.local/setup ou http://${this.wifi.localIp}`, 'info');

        setTimeout(() => {
            msg.style.display = 'none';
            document.getElementById('msg-result').style.display = 'none';
        }, 4000);
    },

    // ============ CAMERA ============
    async toggleCamera() {
        if (this.cameraActive) {
            this.stopCamera();
        } else {
            await this.startCamera();
        }
    },

    async startCamera() {
        try {
            const stream = await navigator.mediaDevices.getUserMedia({
                video: { width: 640, height: 480, facingMode: 'environment' },
                audio: false
            });
            this.stream = stream;
            this.video.srcObject = stream;
            await this.video.play();
            await new Promise((resolve) => {
                this.video.onloadedmetadata = resolve;
            });

            this.frameWidth = this.video.videoWidth;
            this.frameHeight = this.video.videoHeight;
            // Canvas de detecção em resolução menor para performance
            this.canvas.width = this.detectWidth;
            this.canvas.height = this.detectHeight;
            this.prevFrame = new Uint8Array(this.detectWidth * this.detectHeight);

            // Inicializa alvo no centro do vídeo
            this.targetX = Math.round(this.frameWidth / 2);
            this.targetY = Math.round(this.frameHeight / 2);
            this.updateTargetPosition();

            this.cameraActive = true;
            document.getElementById('btn-camera').textContent = 'Desativar camera';
            document.getElementById('camera-status').textContent = 'Camera ativa';
            document.getElementById('camera-status').className = 'camera-status camera-on';

            this.updateStatus();
            this.loop();
        } catch (err) {
            console.error('Erro ao acessar camera:', err);
            alert('Nao foi possível acessar a camera. Permita o acesso e tente novamente.');
        }
    },

    stopCamera() {
        if (this.animFrame) {
            cancelAnimationFrame(this.animFrame);
            this.animFrame = null;
        }
        if (this.stream) {
            this.stream.getTracks().forEach(track => track.stop());
            this.stream = null;
        }
        this.video.srcObject = null;
        this.cameraActive = false;
        this.prevFrame = null;
        document.getElementById('btn-camera').textContent = 'Ativar camera';
        document.getElementById('camera-status').textContent = 'Camera desativada';
        document.getElementById('camera-status').className = 'camera-status';
        document.getElementById('motion-box').style.display = 'none';
        document.getElementById('status-motion').textContent = 'Não';
        document.getElementById('status-coords').textContent = '—';
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        const infoPosX = document.getElementById('info-pos-x');
        const infoError = document.getElementById('info-error');
        this.resetDirectionVisual(arrow, zoneBar, infoPosX, infoError);
        this.updateStatus();
    },

    loop() {
        if (!this.cameraActive) return;

        // Desenha frame redimensionado para detecção (mais rápido)
        this.ctx.drawImage(
            this.video,
            0, 0, this.video.videoWidth, this.video.videoHeight,
            0, 0, this.detectWidth, this.detectHeight
        );

        const currFrame = this.toGrayscale(this.ctx.getImageData(0, 0, this.detectWidth, this.detectHeight));

        if (this.prevFrame) {
            const motion = this.detectMotion(currFrame, this.prevFrame);
            this.handleMotion(motion);
        }

        this.prevFrame = currFrame;
        this.frameCount++;

        this.animFrame = requestAnimationFrame(() => this.loop());
    },

    toGrayscale(imageData) {
        const data = imageData.data;
        const gray = new Uint8Array(data.length / 4);
        for (let i = 0, j = 0; i < data.length; i += 4, j++) {
            gray[j] = (data[i] * 0.299) + (data[i + 1] * 0.587) + (data[i + 2] * 0.114);
        }
        return gray;
    },

    detectMotion(curr, prev) {
        const w = this.frameWidth;
        const h = this.frameHeight;
        const size = w * h;
        const sensitivity = this.config.sensitivity;
        const minArea = this.config.min_area;

        let minX = w, minY = h, maxX = 0, maxY = 0;
        let changedCount = 0;

        for (let i = 0; i < size; i++) {
            const diff = Math.abs(curr[i] - prev[i]);
            if (diff > sensitivity) {
                changedCount++;
                const x = i % w;
                const y = (i / w) | 0;
                if (x < minX) minX = x;
                if (x > maxX) maxX = x;
                if (y < minY) minY = y;
                if (y > maxY) maxY = y;
            }
        }

        if (changedCount < minArea) {
            return { valid: false, area: changedCount };
        }

        const boxW = maxX - minX + 1;
        const boxH = maxY - minY + 1;

        return {
            valid: true,
            x: minX,
            y: minY,
            width: boxW,
            height: boxH,
            area: changedCount,
            centerX: minX + boxW / 2,
            centerY: minY + boxH / 2
        };
    },

    handleMotion(motion) {
        const box = document.getElementById('motion-box');
        const statusMotion = document.getElementById('status-motion');
        const statusCoords = document.getElementById('status-coords');

        // Elementos visuais de direção
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        const infoPosX = document.getElementById('info-pos-x');
        const infoError = document.getElementById('info-error');

        if (!motion.valid) {
            box.style.display = 'none';
            statusMotion.textContent = 'Não';
            statusCoords.textContent = '—';
            this.resetDirectionVisual(arrow, zoneBar, infoPosX, infoError);
            return;
        }

        // Mapeia coordenadas da resolução de detecção para o tamanho real do vídeo
        const scaleX = this.frameWidth / this.detectWidth;
        const scaleY = this.frameHeight / this.detectHeight;

        const realX = motion.x * scaleX;
        const realY = motion.y * scaleY;
        const realW = motion.width * scaleX;
        const realH = motion.height * scaleY;
        const realCenterX = motion.centerX * scaleX;
        const realCenterY = motion.centerY * scaleY;

        // Mostra caixa de movimento no vídeo real
        box.style.display = 'block';
        box.style.left = realX + 'px';
        box.style.top = realY + 'px';
        box.style.width = realW + 'px';
        box.style.height = realH + 'px';

        // Atualiza status (valores numéricos fixos - sem layout shift)
        statusMotion.textContent = 'Sim';
        statusMotion.className = 'value motion-detected';
        statusCoords.textContent = Math.round(realCenterX) + ', ' + Math.round(realCenterY);

        // Calcula direção em relação ao centro da imagem real
        const centerImage = this.frameWidth / 2;
        const errorX = realCenterX - centerImage;
        const percentX = (errorX / centerImage) * 100;

        // Posiciona a seta visual (0% = esquerda, 50% = centro, 100% = direita)
        const arrowLeftPercent = 50 + (percentX / 2);
        arrow.style.left = arrowLeftPercent + '%';

        // Posiciona a barra de zona morta
        const zoneLeftPercent = 50 + (percentX / 2);
        zoneBar.style.left = zoneLeftPercent + '%';

        // Cores baseadas na direção
        if (Math.abs(errorX) <= this.config.dead_zone) {
            arrow.style.borderTopColor = varCss('--color-success');
            infoPosX.style.color = varCss('--color-success');
            infoError.style.color = varCss('--color-success');
        } else if (errorX < 0) {
            arrow.style.borderTopColor = varCss('--color-info');
            infoPosX.style.color = varCss('--color-info');
            infoError.style.color = varCss('--color-info');
        } else {
            arrow.style.borderTopColor = varCss('--color-warning');
            infoPosX.style.color = varCss('--color-warning');
            infoError.style.color = varCss('--color-warning');
        }

        // Atualiza valores numéricos (monospace, largura fixa - sem layout shift)
        infoPosX.textContent = Math.round(realCenterX);
        infoError.textContent = Math.round(errorX) + ' px';

        // Aciona motor apenas se rastreamento automático estiver ativo
        if (this.config.auto_tracking) {
            this.doTracking(realCenterX);
        }
    },

    resetDirectionVisual(arrow, zoneBar, infoPosX, infoError) {
        arrow.style.left = '50%';
        arrow.style.borderTopColor = varCss('--color-text-muted');
        zoneBar.style.left = '50%';
        infoPosX.textContent = '—';
        infoError.textContent = '—';
        infoPosX.style.color = varCss('--color-text-muted');
        infoError.style.color = varCss('--color-text-muted');
    },

    doTracking(centerX) {
        const now = Date.now();
        const centerImage = this.frameWidth / 2;
        const errorX = centerX - centerImage;

        if (Math.abs(errorX) <= this.config.dead_zone) {
            this.stopMotor();
            return;
        }

        if (now - this.lastMoveTime < (this.config.stabilization_ms || 300)) {
            return;
        }

        const dir = errorX < 0 ? 'esquerda' : 'direita';
        const duration = this.config.step_duration_ms || 150;

        this.activateMotor(dir, duration);
        this.lastMoveTime = now;
    },

    // ============ MOTOR (simulado) ============
    activateMotor(dir, durationMs) {
        const status = document.getElementById('status-motor');
        const msg = document.getElementById('msg-result');

        if (this.config.gpio_in1 < 0 || this.config.gpio_in2 < 0 || this.config.gpio_ena < 0) {
            this.showMsg(msg, 'Configure os GPIOs primeiro na aba Configuracao de Dispositivos.', 'error');
            return;
        }

        status.textContent = dir === 'esquerda' ? 'Movendo esquerda' : 'Movendo direita';
        this.lastMoveTime = Date.now();

        setTimeout(() => {
            this.stopMotor();
        }, durationMs);
    },

    moveLeft() {
        const duration = this.config.step_duration_ms || 150;
        this.activateMotor('esquerda', duration);
        // Move o alvo visual para a esquerda
        this.targetX = Math.max(20, this.targetX - 120);
        this.updateTargetPosition();
    },

    moveRight() {
        const duration = this.config.step_duration_ms || 150;
        this.activateMotor('direita', duration);
        // Move o alvo visual para a direita
        this.targetX = Math.min(this.frameWidth > 0 ? this.frameWidth : 640 - 20, this.targetX + 120);
        this.updateTargetPosition();
    },

    centerTarget() {
        this.targetX = Math.round(this.frameWidth / 2);
        this.targetY = Math.round(this.frameHeight / 2);
        this.updateTargetPosition();
        document.getElementById('status-motor').textContent = 'Centralizado';
    },

    stopMotor() {
        document.getElementById('status-motor').textContent = 'Parado';
    },

    updateTargetPosition() {
        const dot = document.getElementById('target-dot');
        if (!dot) return;

        // Se frameWidth ainda não foi definido (câmera inativa), usa centro padrão
        const fw = this.frameWidth > 0 ? this.frameWidth : 640;
        const fh = this.frameHeight > 0 ? this.frameHeight : 480;

        // Limita alvo dentro dos bounds do vídeo
        this.targetX = Math.max(20, Math.min(fw - 20, this.targetX));
        this.targetY = Math.max(20, Math.min(fh - 20, this.targetY));

        dot.style.left = this.targetX + 'px';
        dot.style.top = this.targetY + 'px';
        dot.style.display = 'block';

        // Atualiza coordenadas exibidas
        const coords = document.getElementById('status-coords');
        if (coords) coords.textContent = this.targetX + ', ' + this.targetY;

        // Atualiza info visual de direção
        const infoPosX = document.getElementById('info-pos-x');
        const infoError = document.getElementById('info-error');
        if (infoPosX) infoPosX.textContent = this.targetX;
        if (infoError) {
            const centerImage = fw / 2;
            const errorX = this.targetX - centerImage;
            infoError.textContent = errorX + ' px';
        }

        // Atualiza seta visual
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        if (arrow && zoneBar) {
            const centerImage = fw / 2;
            const percentX = ((this.targetX - centerImage) / centerImage) * 100;
            const arrowLeft = 50 + percentX / 2;
            arrow.style.left = arrowLeft + '%';
            zoneBar.style.left = arrowLeft + '%';
        }
    },

    // ============ CONFIGURACAO HARDWARE ============
    renderConfig() {
        const setVal = (id, val) => {
            const el = document.getElementById(id);
            if (el) el.value = val >= 0 ? val : '';
        };
        setVal('gpio_in1', this.config.gpio_in1);
        setVal('gpio_in2', this.config.gpio_in2);
        setVal('gpio_ena', this.config.gpio_ena);
        document.getElementById('motor_voltage').value = this.config.motor_voltage;
        document.getElementById('dead_zone').value = this.config.dead_zone;
        document.getElementById('sensitivity').value = this.config.sensitivity;
        document.getElementById('min_area').value = this.config.min_area;
        document.getElementById('step_duration_ms').value = this.config.step_duration_ms;
        document.getElementById('stabilization_ms').value = this.config.stabilization_ms;
        
        // Sincroniza estado visual do toggle com config salvo
        const toggle = document.getElementById('toggle-tracking');
        if (toggle) toggle.checked = this.config.auto_tracking;
        
        this.updateTrackingStatusUI();
    },

    setupForm() {
        const form = document.getElementById('form-settings');
        const inputs = form.querySelectorAll('input');
        inputs.forEach(input => {
            input.addEventListener('change', (e) => {
                const key = e.target.id;
                let val = e.target.value;
                if (key === 'motor_voltage') val = parseFloat(val);
                else val = parseInt(val);
                if (key === 'gpio_in1' || key === 'gpio_in2' || key === 'gpio_ena') {
                    if (val === '' || isNaN(val)) val = -1;
                }
                this.config[key] = val;
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

        await new Promise(r => setTimeout(r, 800));

        this.saveConfig();
        this.saveState();
        this.updateStatus();

        btn.textContent = 'Salvo com sucesso!';
        btn.classList.remove('btn-primary');
        btn.classList.add('btn-outline');
        this.showMsg(msg, 'Configuracao salva e persistida. No ESP real isso reiniciaria o sistema.', 'success');

        setTimeout(() => {
            btn.disabled = false;
            btn.textContent = 'Salvar configuracao';
            btn.classList.remove('btn-outline');
            btn.classList.add('btn-primary');
            msg.style.display = 'none';
        }, 2000);
    },

    saveConfig() {
        localStorage.setItem('windfollow_config', JSON.stringify(this.config));
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
        if (cfg.motor_voltage <= 0) return 'Tensao do motor deve ser maior que 0';
        return null;
    },

    async validate() {
        const msg = document.getElementById('msg-result');
        const cfg = { ...this.config };
        const error = this.validateConfig(cfg);
        if (error) {
            this.showMsg(msg, error, 'error');
        } else {
            this.showMsg(msg, 'Pinos validos e disponiveis.', 'success');
        }
    },

    reset() {
        this.config = {
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
        };
        this.saveConfig();
        this.renderConfig();
        const msg = document.getElementById('msg-result');
        this.showMsg(msg, 'Configuracao resetada para defaults.', 'info');
        this.updateStatus();
    },

    toggleTracking(enabled) {
        this.config.auto_tracking = enabled;
        this.saveConfig();
        this.saveState();
        this.updateTrackingStatusUI();
        const msg = document.getElementById('msg-result');
        this.showMsg(msg, 'Rastreamento ' + (enabled ? 'ativado' : 'desativado'), 'info');
        setTimeout(() => msg.style.display = 'none', 1500);
    },

    showMsg(el, text, type) {
        el.textContent = text;
        el.className = 'msg-result ' + type;
        el.style.display = 'block';
    },

    updateStatus() {
        const wifiStatus = document.getElementById('status-wifi');
        if (this.wifi.configured) {
            wifiStatus.textContent = 'Conectado (' + this.wifi.localIp + ')';
        } else {
            wifiStatus.textContent = 'Modo AP (sem Wi-Fi)';
        }
        document.getElementById('status-mode').textContent = this.config.auto_tracking ? 'Rastreamento' : 'Idle';
        document.getElementById('status-motor').textContent = this.config.motor_enabled ? 'Configurado' : 'Nao configurado';
        document.getElementById('status-tracking').textContent = this.config.auto_tracking ? 'Ativo' : 'Desligado';

        const camStatus = document.getElementById('camera-status');
        if (this.cameraActive) {
            camStatus.textContent = 'Camera ativa (' + this.frameCount + ' frames)';
        }

        this.updateTrackingStatusUI();
    },

    updateTrackingStatusUI() {
        const statusEl = document.getElementById('tracking-status');
        if (statusEl) {
            statusEl.textContent = this.config.auto_tracking ? 'Ligado' : 'Desligado';
            statusEl.className = 'control-status ' + (this.config.auto_tracking ? 'status-on' : 'status-off');
        }
    }
};

document.addEventListener('DOMContentLoaded', () => sim.init());

window.addEventListener('beforeunload', () => {
    if (sim.stream) {
        sim.stream.getTracks().forEach(track => track.stop());
    }
});
