#ifndef AUTO_NAVIGATOR_H
#define AUTO_NAVIGATOR_H

#include <Arduino.h>
#include "HardwareConfig.h"
#include "MotorEngine.h"
#include "SensorService.h"
#include "ConfigManager.h"

// Modo autonomo com sensor frontal fixo (VL53L0X), sem servo/scan.
// Chassi diferencial (2 rodas motrizes + caster): desvio simples por
// stop -> re -> giro no proprio eixo -> segue em frente quando livre.
class AutoNavigator {
public:
  enum class State : uint8_t {
    OFF,
    FORWARD,
    BACKUP,
    TURN,
    SETTLE
  };

  bool begin(MotorEngine* engine, SensorService* sensors, ConfigManager* configMgr);
  void loop();

  bool setEnabled(bool on);
  bool enabled() const { return _enabled; }
  bool stuck() const { return _stuck; }
  uint32_t stuckSeq() const { return _stuckSeq; }
  uint16_t avoidCount() const { return _avoidCount; }
  State state() const { return _state; }
  const char* stateName() const;
  void invalidateSpeedCache() { _autoSpeedCacheMs = 0; }

private:
  MotorEngine* _engine = nullptr;
  SensorService* _sensors = nullptr;
  ConfigManager* _configMgr = nullptr;

  bool _enabled = false;
  bool _stuck = false;
  uint32_t _stuckSeq = 0;
  uint16_t _avoidCount = 0;

  State _state = State::OFF;
  uint32_t _stateSince = 0;
  uint32_t _lastTurnLogMs = 0;
  uint32_t _forwardSince = 0;
  uint16_t _backupMs = AUTO_BACKUP_MS;
  uint8_t _side = 0;           // 0 = gira pela esquerda, 1 = pela direita
  uint8_t _turnSegments = 0;   // giros consecutivos sem achar saida
  uint8_t _clearStreak = 0;
  mutable uint8_t _autoSpeedCache = AUTO_FORWARD_SPEED;
  mutable uint32_t _autoSpeedCacheMs = 0;

  void enter(State s);
  void startForward();
  void startTurn();
  void startBackup(uint16_t ms);
  void handleObstacle();
  bool registerFailure();
  void goStuck();
  bool blockedAhead() const;
  bool clearAhead() const;
  uint8_t autoMoveSpeed() const;
};

#endif // AUTO_NAVIGATOR_H
