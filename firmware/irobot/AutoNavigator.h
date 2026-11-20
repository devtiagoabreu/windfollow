#ifndef AUTO_NAVIGATOR_H
#define AUTO_NAVIGATOR_H

#include <Arduino.h>
#include "HardwareConfig.h"
#include "MotorEngine.h"
#include "SensorService.h"
#include "ConfigManager.h"

// Modo autonomo do Prototipo-67 (roda traseira fixa, sem encoder).
//
// Ciclo de desvio, inspirado nos FSMs de obstacle avoidance para ESP32
// (stop -> backup -> pivot turn -> scan -> forward):
//   FRENTE:    anda reto medindo a frente; para a 50 cm do obstaculo.
//   RE:        recua (500 ms; ate 800 ms a cada 3 obstaculos) para abrir espaco.
//   GIRANDO:   pivo no proprio eixo (uma roda em re, outra em frente) e fica
//              girando ate a frente ter AUTO_CLEAR_MM (1 m) por 2 leituras.
//              Se girar demais sem achar saida, recua um tico e continua; se
//              ainda assim nao achar, troca de lado.
//   MEDINDO:   assenta e confirma a clareza antes de retomar o reto.
//
// RECONHECIMENTO (novo): ao ligar o autonomo o robô faz uma varredura de 360°,
// parando e medindo a cada passo (4-6 passos). Leituras invalidas/sem alvo
// gravam a distancia maxima do sensor (SCAN_MAX_MM). Ao final ele gira para a
// direcao com maior distancia encontrada e segue em frente. Durante o desvio
// normal ele continua atualizando o heading estimado para saber pra onde aponta.
//
// Nunca para sozinho: encerra apenas quando o usuario desliga ou quando
// acumula AUTO_STUCK_ATTEMPTS obstaculos sem AUTO_FREE_FORWARD_MS de avanco.
class AutoNavigator {
public:
  enum class State : uint8_t {
    OFF,
    SCAN,            // reconhecimento de 360 graus em andamento
    SCAN_MEASURE,    // parado, medindo um ponto do escaneamento
    SCAN_TURN,       // girando para o proximo ponto de medida
    ALIGN_BEST,      // girando para a direcao com maior distancia livre
    FORWARD,
    BACKUP,
    TURN,
    SETTLE
  };

  struct ScanEntry {
    uint16_t angleDeg;    // 0, 60, 120 ... (estimado por tempo de giro)
    uint16_t distanceMm;  // distancia medida ou SCAN_MAX_MM se sem alvo
  };

  static const uint8_t  SCAN_STEPS        = 6;   // 6 x 60 = 360 graus
  static const uint16_t SCAN_STEP_DEG     = 60;  // graus por passo
  static const uint16_t SCAN_TURN_MS      = 420; // ms para girar um passo (pivo)
  static const uint16_t SCAN_MEASURE_MS   = 150; // ms parado para leitura estavel
  static const uint16_t SCAN_SETTLE_MS    = 200; // ms de assentamento apos giro
  static const uint16_t SCAN_MAX_MM       = 8000; // sem alvo = distancia maxima

  bool begin(MotorEngine* engine, SensorService* sensors, ConfigManager* configMgr);
  void loop();

  bool setEnabled(bool on);
  bool enabled() const { return _enabled; }
  bool stuck() const { return _stuck; }
  uint32_t stuckSeq() const { return _stuckSeq; }
  uint16_t avoidCount() const { return _avoidCount; }
  State state() const { return _state; }
  const char* stateName() const;

  // Consulta do ultimo reconhecimento (para dashboard/web)
  uint16_t headingDeg() const { return _headingDeg; }
  uint16_t bestAngleDeg() const { return _bestAngleDeg; }
  uint16_t bestDistanceMm() const { return _bestDistanceMm; }
  bool scanDone() const { return _scanDone; }
  const ScanEntry* scanTable() const { return _scan; }
  uint8_t scanStep() const { return _scanStep; }

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
  uint8_t _turnSegments = 0;   // giros consecutivos sem achar 1 m livre
  uint8_t _clearStreak = 0;

  // Reconhecimento / heading estimado (sem encoder)
  ScanEntry _scan[SCAN_STEPS];
  uint8_t _scanStep = 0;
  uint16_t _headingDeg = 0;    // direção estimada do robô (graus, 0 = inicial)
  uint16_t _bestAngleDeg = 0;
  uint16_t _bestDistanceMm = 0;
  bool _scanDone = false;
  bool _scanSettling = false;
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

  // Reconhecimento de 360 graus
  void startScan();
  void scanMeasure();
  void scanTurn();
  void alignBest();
  void recordScan(uint16_t angleDeg, uint16_t distanceMm);
  void updateHeading(int16_t deltaDeg);
  uint16_t scanBestAngle() const;
  bool tofReady() const;
  uint8_t autoMoveSpeed() const;
};

#endif // AUTO_NAVIGATOR_H
