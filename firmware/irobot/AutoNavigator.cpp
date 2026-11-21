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
    case State::OFF:     return "off";
    case State::FORWARD: return "frente";
    case State::BACKUP:  return "re";
    case State::TURN:    return "girando";
    case State::SETTLE:  return "medindo";
    default:             return "off";
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
  _engine->move(RobotDirection::TRAS, autoMoveSpeed());
  enter(State::FORWARD);
}

void AutoNavigator::startBackup(uint16_t ms) {
  _backupMs = ms;
  _engine->move(RobotDirection::FRENTE, autoMoveSpeed());
  enter(State::BACKUP);
}

void AutoNavigator::startTurn() {
  _lastTurnLogMs = 0;
  uint8_t s = autoMoveSpeed();
  if (_side == 0) {
    _engine->moveDifferential((int16_t)s, -(int16_t)s);
  } else {
    _engine->moveDifferential(-(int16_t)s, (int16_t)s);
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
  Serial.println(_side == 0 ? "direita" : "esquerda");

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

  _autoSpeedCacheMs = 0;
  _enabled = true;
  _stuck = false;
  _avoidCount = 0;
  _side = 0;
  _turnSegments = 0;
  _clearStreak = 0;
  _sensors->setFastDistance(true);
  Serial.println("Autonomo ON - desvio de obstaculos (sensor fixo)");
  startForward();
  return true;
}

void AutoNavigator::loop() {
  if (!_enabled || !_engine) return;

  uint32_t elapsed = millis() - _stateSince;

  switch (_state) {
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
