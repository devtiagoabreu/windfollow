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
    bgFrame: null,
    frameWidth: 0,
    frameHeight: 0,
    detectWidth: 160,   // resolução menor para detecção rápida
    detectHeight: 120,
    cameraActive: false,
    animFrame: null,
    lastMoveTime: 0,
    settleUntil: 0,
    smoothErrorX: 0,
    missCount: 0,
    inDeadZone: false,
    targetLocked: false,
    refCenterX: 0,
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

    // Configuracao de hardware (padrao T-SIMCAM V1.3, modem 4G desligado)
    config: {
        gpio_in1: 45,
        gpio_in2: 46,
        gpio_ena: 21,
        motor_voltage: 12,
        motor_enabled: true,
        dead_zone: 30,
        sensitivity: 30,
        min_area: 100,
        bg_alpha: 0.95,
        kp_x: 2.5,          // ms de movimento por pixel de erro (controle proporcional)
        move_min_ms: 40,    // duracao minima de um passo (só usado quando valido mover)
        move_max_ms: 400,   // duracao maxima de um passo
        settle_ms: 300,     // tempo de assento apos mover (suprime detecao por ego-movimento)
        ema_alpha_err: 0.7, // suavizacao temporal do erro (remove jitter)
        miss_frames: 8,     // quadros sem alvo antes de soltar o rastreamento
        pwm_kp: 1.2,        // PWM por pixel de erro (desacelera perto do centro)
        pwm_min: 90,        // PWM mínimo para vencer atrito/inercia do motor DC
        pwm_max: 200,       // PWM máximo
        dead_zone_hyst: 1.5,// histerese: só sai da zona morta se erro > dead_zone*hyst
        trackWindowW: 40,   // largura da janela de rastreamento (px, resolucao de detect)
        trackWindowH: 40,   // altura (reservado)
        refAlpha: 0.15,     // deslize suave do centro de referencia quando parado na zona
        step_duration_ms: 150,
        stabilization_ms: 300,
        auto_tracking: false
    },

    occupiedGpios: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 47, 48],

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

        const now = Date.now();
        // Durante o ajuste do motor (settle) suprime a detecçăo: o fundo inteiro se
        // desloca com o movimento da câmera (ego-movimento) e geraria falso positivo.
        if (now < this.settleUntil) {
            this.animFrame = requestAnimationFrame(() => this.loop());
            return;
        }

        const motion = this.detectMotion(currFrame);
        this.handleMotion(motion, now);

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

    detectMotion(curr) {
        const w = this.detectWidth;
        const h = this.detectHeight;
        const size = w * h;
        const sensitivity = this.config.sensitivity;
        const minArea = this.config.min_area;
        const alpha = this.config.bg_alpha;

        // Inicializa o modelo de fundo no primeiro frame
        if (!this.bgFrame) {
            this.bgFrame = new Uint8Array(curr);
            this.visited = new Uint8Array(size);
            return { valid: false, area: 0 };
        }

        const bg = this.bgFrame;
        const visited = this.visited;
        for (let i = 0; i < size; i++) visited[i] = 0;

        // Diferença contra o modelo de fundo (EMA), não contra o quadro anterior.
        // Isso ignora mudanças lentas de iluminação (nuvens, lâmpadas).
        const changed = new Uint8Array(size);
        let anyChanged = 0;
        for (let i = 0; i < size; i++) {
            const d = curr[i] - bg[i];
            const diff = d < 0 ? -d : d;
            if (diff > sensitivity) {
                changed[i] = 1;
                anyChanged = 1;
            }
        }

        if (!anyChanged) {
            for (let i = 0; i < size; i++) {
                bg[i] = (alpha * bg[i] + (1 - alpha) * curr[i]) | 0;
            }
            return { valid: false, area: 0 };
        }

        // Connected components (4-conectados): fica com o MAIOR blob, ignorando
        // ruído disperso e mudanças pontuais de fundo.
        let bestArea = 0;
        let bestMinX = w, bestMinY = h, bestMaxX = 0, bestMaxY = 0;
        const queue = [];

        for (let seed = 0; seed < size; seed++) {
            if (!changed[seed] || visited[seed]) continue;
            let area = 0, minX = w, minY = h, maxX = 0, maxY = 0;
            queue.length = 0;
            queue.push(seed);
            visited[seed] = 1;
            let qi = 0;
            while (qi < queue.length) {
                const ci = queue[qi++];
                const cx = ci % w;
                const cy = (ci / w) | 0;
                area++;
                if (cx < minX) minX = cx;
                if (cx > maxX) maxX = cx;
                if (cy < minY) minY = cy;
                if (cy > maxY) maxY = cy;
                if (cx > 0) {
                    const ni = ci - 1; if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue.push(ni); }
                }
                if (cx < w - 1) {
                    const ni = ci + 1; if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue.push(ni); }
                }
                if (cy > 0) {
                    const ni = ci - w; if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue.push(ni); }
                }
                if (cy < h - 1) {
                    const ni = ci + w; if (changed[ni] && !visited[ni]) { visited[ni] = 1; queue.push(ni); }
                }
            }
            if (area > bestArea) {
                bestArea = area;
                bestMinX = minX; bestMinY = minY; bestMaxX = maxX; bestMaxY = maxY;
            }
        }

        if (bestArea < minArea) {
            for (let i = 0; i < size; i++) {
                bg[i] = (alpha * bg[i] + (1 - alpha) * curr[i]) | 0;
            }
            return { valid: false, area: bestArea };
        }

        const boxW = bestMaxX - bestMinX + 1;
        const boxH = bestMaxY - bestMinY + 1;

        // Adapta o fundo à iluminação lenta (EMA)
        for (let i = 0; i < size; i++) {
            bg[i] = (alpha * bg[i] + (1 - alpha) * curr[i]) | 0;
        }

        return {
            valid: true,
            x: bestMinX,
            y: bestMinY,
            width: boxW,
            height: boxH,
            area: bestArea,
            centerX: bestMinX + boxW / 2,
            centerY: bestMinY + boxH / 2
        };
    },

    handleMotion(motion, now) {
        const box = document.getElementById('motion-box');
        const statusMotion = document.getElementById('status-motion');
        const statusCoords = document.getElementById('status-coords');

        // Elementos visuais de direção
        const arrow = document.getElementById('direction-arrow');
        const zoneBar = document.getElementById('zone-bar');
        const infoPosX = document.getElementById('info-pos-x');
        const infoError = document.getElementById('info-error');

        if (!motion.valid) {
            this.missCount++;
            // So solta o alvo apos miss_frames quadros sem detecçăo.
            // Um frame perdido (ruizo/iluminaçăo) nao reseta o centro de referencia.
            if (this.missCount >= this.config.miss_frames) {
                this.targetLocked = false;
                this.inDeadZone = false;
                this.stopMotor();
                this.smoothErrorX = 0;
            }
            box.style.display = 'none';
            statusMotion.textContent = 'Não';
            statusCoords.textContent = '—';
            this.resetDirectionVisual(arrow, zoneBar, infoPosX, infoError);
            return;
        }

        this.missCount = 0;
        // Se ainda nao travamos um alvo, travamos neste maior blob (unico alvo)
        // e fixa o centro de referencia na posicao onde ele esta.
        if (!this.targetLocked) {
            this.targetLocked = true;
            this.smoothErrorX = 0;
            this.refCenterX = realCenterX;
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

        // Filtro temporal do erro (EMA): remove jitter quadro-a-quadro
        const a = this.config.ema_alpha_err;
        this.smoothErrorX = a * this.smoothErrorX + (1 - a) * errorX;

        // Posiciona a seta visual (0% = esquerda, 50% = centro, 100% = direita)
        const arrowLeftPercent = 50 + ((this.smoothErrorX / centerImage) * 100) / 2;
        arrow.style.left = arrowLeftPercent + '%';

        // Posiciona a barra de zona morta
        const zoneLeftPercent = 50 + ((this.smoothErrorX / centerImage) * 100) / 2;
        zoneBar.style.left = zoneLeftPercent + '%';

        // Cores baseadas na direção (usa erro suavizado)
        if (Math.abs(this.smoothErrorX) <= this.config.dead_zone) {
            arrow.style.borderTopColor = varCss('--color-success');
            infoPosX.style.color = varCss('--color-success');
            infoError.style.color = varCss('--color-success');
        } else if (this.smoothErrorX < 0) {
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
        infoError.textContent = Math.round(this.smoothErrorX) + ' px';

        // Aciona motor apenas se rastreamento automático estiver ativo
        if (this.config.auto_tracking) {
            this.doTracking(realCenterX, now);
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

    doTracking(centerX, now) {
        now = now || Date.now();
        const halfW = this.config.trackWindowW / 2;
        const err = centerX - this.refCenterX; // erro em relacao ao centro de referencia movel
        const absErr = Math.abs(err);

        // Histerese na janela de rastreamento: se ja estamos dentro, so sai quando
        // o objeto deixa completamente a janela (halfW * dead_zone_hyst).
        if (this.inDeadZone) {
            if (absErr <= halfW * this.config.dead_zone_hyst) {
                // Desliza suavemente o centro de referencia para onde o objeto parou
                this.refCenterX = this.refCenterX + (centerX - this.refCenterX) * this.config.refAlpha;
                this.stopMotor();
                return;
            }
            this.inDeadZone = false;
        }

        if (absErr <= halfW) {
            this.inDeadZone = true;
            this.refCenterX = this.refCenterX + (centerX - this.refCenterX) * this.config.refAlpha;
            this.stopMotor();
            return;
        }

        if (!this.targetLocked) return;

        // Fora da janela: motor se move para trazer o objeto de volta ao centro da janela.
        // O erro efetivo e somente o que excede a metade da janela (dead zone),
        // para nao mover quando o objeto esta dentro da zona de tolerancia.
        const effectiveError = absErr - halfW;

        // Controle proporcional: erro maior = movimento mais longo e PWM mais alto.
        // Desacelera naturalmente ao se aproximar do centro da janela.
        let duration = this.config.kp_x * effectiveError;
        duration = Math.min(this.config.move_max_ms,
            Math.max(this.config.move_min_ms, duration));

        let pwm = this.config.pwm_kp * effectiveError;
        pwm = Math.min(this.config.pwm_max, Math.max(this.config.pwm_min, pwm));

        const dir = err < 0 ? 'esquerda' : 'direita';
        this.activateMotor(dir, duration, pwm);
        // Suprime detecçăo enquanto o motor se move e durante o ajuste:
        // o fundo inteiro se desloca com a câmera (ego-movimento).
        this.settleUntil = now + duration + this.config.settle_ms;
    },

    // ============ MOTOR (simulado) ============
    activateMotor(dir, durationMs, pwm = this.config.pwm_max) {
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
            gpio_in1: 45,
            gpio_in2: 46,
            gpio_ena: 21,
            motor_voltage: 12,
            motor_enabled: true,
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
