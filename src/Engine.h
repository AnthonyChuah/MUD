#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <string>
#include <mutex>
#include <condition_variable>

#define TIMESTEP_CYCLE_MS 100

class ConnectionManager;
class UserConnection;

// Linker error: undefined reference to Engine::TIMESTEP_CYCLE_MS
// Solving it: Stackoverflow 9074073
// Basically I must have the definition in at least ONE cpp file I am linking
// But it does not work. I think I have to use #define...

class Engine {
  // static constexpr int TIMESTEP_CYCLE_MS = 100;
  static constexpr int CYCLES_IN_ROUND = 10;
  static constexpr int ROUNDS_IN_TICK = 20;
  static constexpr int RESERVE_CAPACITY = 256;
  unsigned long long timeSinceStart = 0;
  unsigned int cyclesToNextRound = CYCLES_IN_ROUND;
  unsigned int roundsToNextTick = ROUNDS_IN_TICK;
  std::vector<UserConnection*> userConns; // Vector holding pointers to UCs on the heap
  std::vector<int> userDelayToNextCommand; // To prevent spam, each command gives at least 1
  // cycle of "delay".
  ConnectionManager* connManager;
  std::mutex mutexWakeCycle;
  std::condition_variable condvarWakeCycle;
  bool isRunning = true;
  // All other objects necessary, e.g. locations and people and such
  void executeCycle(); // Executes every 100 ms
  void executeRound(); // Executes every 10 cycles
  void executeTick(); // Executes every 10 rounds
  int parseUserInput(std::string&& _cmd, UserConnection* _conn);
  // The two functions below work concurrently together to execute cycle every 100 ms real-time
  void blockingCycleExecutor();
  void unblockingClock();
 public:
  Engine(); // Since initialized first, attach pointers post-construction
  void attachConnectionManager(ConnectionManager* _cm);
  void addNewConnection(UserConnection* _conn);
  void shutdownUserConn(UserConnection* _conn);
  void getUserNextCmd(UserConnection* _conn, const int& _index);
  void start();
};

#endif
