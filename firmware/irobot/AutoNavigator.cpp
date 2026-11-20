#include "AutoNavigator.h"

bool AutoNavigator::begin(MotorEngine* engine, SensorService* sensors, ConfigManager* configMgr) {
  _engine = engine;
  _sensors = sensors;
  _configMgr = configMgr;
  _autoSpeedCache = AUTO_FORWARD_SPEED;
  _autoSpeedCacheMs = 0;
  return _engine != nullptr && _sensors != nullptr;
}

void AutoNavigator::enter(State s) {
  _state = s;
  _stateSince = millis();
}

const char* AutoNavigator::stateName() const {
  if (_stuck) return "enroscado";
  switch (_state) {
    case State::OFF:        return "off";
    case State::SCAN:       return "reconhecendo";
    case State::SCAN_MEASURE: return "medindo";
    case State::SCAN_TURN:  return "girando-scan";
    case State::ALIGN_BEST: return "alinhando";
    case State::FORWARD:    return "frente";
    case State::BACKUP:     return "re";
    case State::TURN:       return "girando";
    case State::SETTLE:     return "medindo";
    default:                return "off";
  }
}

uint8_t AutoNavigator::autoMoveSpeed() const {
  uint32_t now = millis();
  if (_configMgr && (now - _autoSpeedCacheMs >= 500 || _autoSpeedCacheMs == 0)) {
    _autoSpeedCache = _configMgr->load().autoSpeed;
    _autoSpeedCacheMs = now;
  }
  uint8_t speed = _autoSpeedCache;
  if (speed < PWM_MIN) speed = PWM_MIN;
  if (speed > PWM_MAX) speed = PWM_MAX;
  return speed;
}

bool AutoNavigator::blockedAhead() const {
  if (!_sensors || !_sensors->tofPresent()) return false;
  uint16_t mm = _sensors->distanceMm();
  return mm > 0 && mm < AUTO_STOP_MM;
}

bool AutoNavigator::clearAhead() const {
  if (!_sensors || !_sensors->tofPresent()) return true;
  if (_sensors->distanceAgeMs() > AUTO_CLEAR_MAX_AGE_MS) return false;
  uint16_t mm = _sensors->distanceMm();
  if (mm == 0) return true;
  return mm >= AUTO_CLEAR_MM;
}

void AutoNavigator::startForward() {
  _forwardSince = millis();
  _engine->move(RobotDirection::FRENTE, autoMoveSpeed());
  enter(State::FORWARD);
}

void AutoNavigator::startBackup(uint16_t ms) {
  _backupMs = ms;
  uint8_t s = autoMoveSpeed();
  if (s < PWM_MIN) s = PWM_MIN;
  _engine->move(RobotDirection::TRAS, s);
  enter(State::BACKUP);
}

void AutoNavigator::startTurn() {
  _lastTurnLogMs = 0;
  uint8_t s = autoMoveSpeed();
  if (s < PWM_MIN) s = PWM_MIN;
  if (_side == 0) {
    _engine->moveDifferential(-static_cast<int8_t>(s), static_cast<int8_t>(s));
  } else {
    _engine->moveDifferential(static_cast<int8_t>(s), -static_cast<int8_t>(s));
  }
  _clearStreak = 0;
  enter(State::TURN);
}

void AutoNavigator::goStuck() {
  _enabled = false;
  _stuck = true;
  _stuckSeq++;
  if (_engine) _engine->stop();
  if (_sensors) _sensors->setFastDistance(false);
  Serial.print("ENROSCADO: ");
  Serial.print(_avoidCount);
  Serial.println(" tentativas seguidas sem avanco livre. Autonomo desligado.");
  enter(State::OFF);
}

bool AutoNavigator::registerFailure() {
  _avoidCount++;
  if (_avoidCount >= AUTO_STUCK_ATTEMPTS) {
    goStuck();
    return true;
  }
  return false;
}

void AutoNavigator::handleObstacle() {
  _engine->stop();

  if (registerFailure()) return;

  _side = (_avoidCount % 2 == 0) ? 1 : 0;
  _turnSegments = 0;

  uint16_t back = (_avoidCount % 3 == 0) ? AUTO_BACKUP_LONG_MS : AUTO_BACKUP_MS;
  Serial.print("Obstaculo ");
  Serial.print(_avoidCount);
  Serial.print("/");
  Serial.print(AUTO_STUCK_ATTEMPTS);
  Serial.print(" - re ");
  Serial.print(back);
  Serial.print(" ms, girando pela ");
  Serial.println(_side == 0 ? "esquerda" : "direita");

  startBackup(back);
}

bool AutoNavigator::setEnabled(bool on) {
  if (!on) {
    if (_engine && _enabled) _engine->stop();
    _enabled = false;
    _stuck = false;
    _avoidCount = 0;
    _side = 0;
    _turnSegments = 0;
    _clearStreak = 0;
    _headingDeg = 0;
    _scanDone = false;
    _scanSettling = false;
    if (_sensors) _sensors->setFastDistance(false);
    enter(State::OFF);
    Serial.println("Autonomo OFF");
    return true;
  }

  if (_enabled) return true;
  if (!_sensors || !_sensors->tofPresent()) {
    Serial.println("Autonomo recusado: VL53L0X ausente");
    return false;
  }

  _autoSpeedCacheMs = 0; // recarrega velocidade configurada ao ligar
  _enabled = true;
  _stuck = false;
  _avoidCount = 0;
  _side = 0;
  _turnSegments = 0;
  _clearStreak = 0;
  _headingDeg = 0;
  _scanDone = false;
  _scanSettling = false;
  for (uint8_t i = 0; i < SCAN_STEPS; i++) {
    _scan[i].angleDeg = i * SCAN_STEP_DEG;
    _scan[i].distanceMm = 0;
  }
  _sensors->setFastDistance(true);
  Serial.println("Autonomo ON - iniciando reconhecimento de 360 graus");
  startScan();
  return true;
}

bool AutoNavigator::tofReady() const {
  if (!_sensors || !_sensors->tofPresent()) return false;
  return _sensors->distanceAgeMs() <= SCAN_MEASURE_MS;
}

void AutoNavigator::recordScan(uint16_t angleDeg, uint16_t distanceMm) {
  for (uint8_t i = 0; i < SCAN_STEPS; i++) {
    if (_scan[i].angleDeg == angleDeg) {
      _scan[i].distanceMm = distanceMm;
      break;
    }
  }
}

uint16_t AutoNavigator::scanBestAngle() const {
  uint16_t best = 0;
  uint16_t bestDist = 0;
  for (uint8_t i = 0; i < SCAN_STEPS; i++) {
    if (_scan[i].distanceMm > bestDist) {
      bestDist = _scan[i].distanceMm;
      best = _scan[i].angleDeg;
    }
  }
  return best;
}

void AutoNavigator::updateHeading(int16_t deltaDeg) {
  int32_t h = (int32_t)_headingDeg + deltaDeg;
  while (h < 0) h += 360;
  while (h >= 360) h -= 360;
  _headingDeg = (uint16_t)h;
}

void AutoNavigator::startScan() {
  _scanStep = 0;
  _headingDeg = 0;
  _bestAngleDeg = 0;
  _bestDistanceMm = 0;
  _scanSettling = false;
  for (uint8_t i = 0; i < SCAN_STEPS; i++) {
    _scan[i].distanceMm = 0;
  }
  _engine->stop();
  enter(State::SCAN_MEASURE);
  Serial.print("[scan] iniciando reconhecimento ");
  Serial.print(SCAN_STEPS);
  Serial.println(" passos");
}

void AutoNavigator::scanMeasure() {
  uint32_t elapsed = millis() - _stateSince;

  // Assentamento minimo apos parar/girar antes de confiar na leitura
  if (elapsed < 120) return;

  bool fresh = _sensors && _sensors->tofPresent() && _sensors->distanceAgeMs() <= 120;
  if (!fresh && elapsed < 800) return; // espera leitura nova do VL53L0X (modo rapido ~80 ms)

  uint16_t mm = SCAN_MAX_MM;
  if (fresh) {
    mm = _sensors->distanceMm();
    uint16_t raw = _sensors->lastRawMm();
    if (raw == 9999 || mm == 0 || mm >= 8000) mm = SCAN_MAX_MM;
  }
  recordScan(_headingDeg, mm);

  Serial.print("[scan] ");
  Serial.print(_scanStep + 1);
  Serial.print("/");
  Serial.print(SCAN_STEPS);
  Serial.print(" ang=");
  Serial.print(_headingDeg);
  Serial.print(" dist=");
  Serial.print(mm);
  Serial.println(mm == SCAN_MAX_MM ? " mm (sem alvo)" : " mm");

  _scanStep++;
  if (_scanStep >= SCAN_STEPS) {
    _bestAngleDeg = scanBestAngle();
    _bestDistanceMm = 0;
    for (uint8_t i = 0; i < SCAN_STEPS; i++) {
      if (_scan[i].angleDeg == _bestAngleDeg) {
        _bestDistanceMm = _scan[i].distanceMm;
        break;
      }
    }
    _scanDone = true;
    Serial.print("[scan] concluido - melhor direcao: ");
    Serial.print(_bestAngleDeg);
    Serial.print(" graus (");
    Serial.print(_bestDistanceMm);
    Serial.println(" mm)");
    enter(State::ALIGN_BEST);
    return;
  }

  _scanSettling = false;
  enter(State::SCAN_TURN);
}

void AutoNavigator::scanTurn() {
  uint32_t elapsed = millis() - _stateSince;
  uint8_t s = autoMoveSpeed();
  if (s < PWM_MIN) s = PWM_MIN;
  uint16_t turnMs = (uint16_t)((uint32_t)SCAN_TURN_MS * AUTO_TURN_SPEED / s);
  if (turnMs < 150) turnMs = 150;

  if (!_scanSettling) {
    // Pivo para a esquerda para varrer 360 graus (heading cresce)
    _engine->moveDifferential(-static_cast<int8_t>(s), static_cast<int8_t>(s));
    if (elapsed < turnMs) return;

    _engine->stop();
    _scanSettling = true;
    _stateSince = millis();
    elapsed = 0;
  }

  if (elapsed < SCAN_SETTLE_MS) return;
  updateHeading(SCAN_STEP_DEG);
  _scanSettling = false;
  enter(State::SCAN_MEASURE);
}

void AutoNavigator::alignBest() {
  // Heading atual apos o scan e o ultimo ponto medido; calcula menor giro
  int16_t delta = (int16_t)_bestAngleDeg - (int16_t)_headingDeg;
  while (delta > 180) delta -= 360;
  while (delta < -180) delta += 360;

  if (abs(delta) <= 8) {
    _engine->stop();
    _headingDeg = _bestAngleDeg;
    Serial.print("[alinhado] seguindo para ");
    Serial.print(_bestAngleDeg);
    Serial.println(" graus (mais livre)");
    startForward();
    return;
  }

  uint8_t s = autoMoveSpeed();
  if (s < PWM_MIN) s = PWM_MIN;
  uint16_t turnMs = (uint16_t)((uint32_t)SCAN_TURN_MS * AUTO_TURN_SPEED / s);
  if (turnMs < 150) turnMs = 150;
  uint32_t needMs = (uint32_t)turnMs * (uint32_t)abs(delta) / SCAN_STEP_DEG;
  uint32_t elapsed = millis() - _stateSince;

  if (elapsed >= needMs) {
    _engine->stop();
    _headingDeg = _bestAngleDeg;
    Serial.print("[alinhado] seguindo para ");
    Serial.print(_bestAngleDeg);
    Serial.println(" graus (mais livre)");
    startForward();
    return;
  }

  if (delta > 0) {
    _engine->moveDifferential(-static_cast<int8_t>(s), static_cast<int8_t>(s));
  } else {
    _engine->moveDifferential(static_cast<int8_t>(s), -static_cast<int8_t>(s));
  }
}

void AutoNavigator::loop() {
  if (!_enabled || !_engine) return;

  uint32_t elapsed = millis() - _stateSince;

  switch (_state) {
    case State::SCAN_MEASURE:
      scanMeasure();
      break;

    case State::SCAN_TURN:
      scanTurn();
      break;

    case State::ALIGN_BEST:
      alignBest();
      break;

    case State::FORWARD:
      if (blockedAhead()) {
        handleObstacle();
      } else if (millis() - _forwardSince >= AUTO_FREE_FORWARD_MS) {
        if (_avoidCount > 0) {
          _avoidCount = 0;
          Serial.println("Avanco livre: contador de obstaculos zerado");
        }
      }
      break;

    case State::BACKUP:
      if (elapsed >= _backupMs) {
        _engine->stop();
        startTurn();
      }
      break;

    case State::TURN: {
      bool clear = clearAhead();

      if (millis() - _lastTurnLogMs >= 500) {
        _lastTurnLogMs = millis();
        Serial.print("[giro] dist=");
        Serial.print(_sensors ? _sensors->distanceMm() : 0);
        Serial.print(" mm (bruto ");
        Serial.print(_sensors ? _sensors->lastRawMm() : 0);
        Serial.print(") livre=");
        Serial.println(clear ? "sim" : "nao");
      }

      if (clear && elapsed >= AUTO_TURN_MIN_MS) {
        if (++_clearStreak >= AUTO_CLEAR_SAMPLES) {
          _engine->stop();
          enter(State::SETTLE);
          break;
        }
      } else if (!clear) {
        _clearStreak = 0;
      }

      if (elapsed >= AUTO_TURN_MAX_MS) {
        _engine->stop();
        if (!registerFailure()) {
          _turnSegments++;
          if (_turnSegments >= 2) {
            _side ^= 1;
            _turnSegments = 0;
            Serial.println("Giro sem saida: trocando de lado");
          } else {
            Serial.println("Giro sem saida: recuando e continuando");
          }
          startBackup(AUTO_BACKUP_NUDGE_MS);
        }
      }
      break;
    }

    case State::SETTLE:
      if (elapsed >= AUTO_SETTLE_MS) {
        if (clearAhead()) {
          startForward();
        } else {
          startTurn();
        }
      }
      break;

    case State::OFF:
    default:
      break;
  }
}
