#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "UserConnection.h"
#include "ConnectionManager.h"

class Engine {
  static constexpr int TIMESTEP_CYCLE_MS = 100;
  static constexpr int CYCLES_IN_ROUND = 10;
  static constexpr int ROUNDS_IN_TICK = 20;
  unsigned long long timeSinceStart = 0;
  unsigned int cyclesToNextRound = CYCLES_IN_ROUND - 1;
  unsigned int roundsToNextTick = ROUNDS_IN_TICK - 1;
  std::vector<UserConnection*> userConns;
  ConnectionManager* connManager;
  // All other objects necessary, e.g. locations and people and such
  void executeCycle(); // Executes every 100 ms
  void executeRound(); // Executes every 10 cycles
  void executeTick(); // Executes every 10 rounds
  Engine(); // Default constructor not to be used
 public:
  Engine();
  void start();
};

#endif
