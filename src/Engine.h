#ifndef ENGINE_H
#define ENGINE_H

#include <vector>

#include "UserConnection.h"
#include "ConnectionManager.h"

class Engine {
  static constexpr int TIMESTEP_CYCLE_MS = 100;
  static constexpr int CYCLES_IN_ROUND = 10;
  static constexpr int ROUNDS_IN_TICK = 20;
  static constexpr int RESERVE_CAPACITY = 256;
  unsigned long long timeSinceStart = 0;
  unsigned int cyclesToNextRound = CYCLES_IN_ROUND - 1;
  unsigned int roundsToNextTick = ROUNDS_IN_TICK - 1;
  std::vector<UserConnection*> userConns; // Vector holding pointers to UCs on the heap
  std::vector<User*> users; // Vector holding the logged-in entites of the user
  std::vector<int> userDelayToNextCommand; // To prevent spam, each command gives at least 1
  // cycle of "delay".
  ConnectionManager* connManager;
  // All other objects necessary, e.g. locations and people and such
  void executeCycle(); // Executes every 100 ms
  void executeRound(); // Executes every 10 cycles
  void executeTick(); // Executes every 10 rounds
 public:
  Engine(); // Since initialized first, attach pointers post-construction
  void attachConnectionManager(ConnectionManager* _cm);
  void addNewConnection(UserConnection* _conn);
  void shutdownUserConn(int _socket);
  void start();
};

#endif
