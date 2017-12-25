#ifndef ENGINE_H
#define ENGINE_H

#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

class ConnectionManager;
class UserConnection;

class Engine
{
public:
    Engine(); // Since initialized first, attach pointers post-construction
    void AttachConnectionManager(ConnectionManager* _cm);
    void AddNewConnection(UserConnection* _conn);
    void ShutdownUserConn(UserConnection* _conn);
    void GetUserNextCmd(UserConnection* _conn, const int& _index);
    void Run();

private:
    static constexpr uint16_t TIMESTEP_CYCLE_MS = 100;
    static constexpr uint8_t CYCLES_IN_ROUND = 10;
    static constexpr uint8_t ROUNDS_IN_TICK = 20;
    uint64_t timeSinceStart = 0;
    uint8_t cyclesToNextRound = CYCLES_IN_ROUND;
    uint8_t roundsToNextTick = ROUNDS_IN_TICK;

    // Make a player class + use std::unordered_map
    std::vector<UserConnection*> userConns; // Vector holding pointers to UCs on the heap
    std::vector<uint16_t> userDelayToNextCommand; // To prevent spam, each command gives at least 1
    // cycle of "delay".
    ConnectionManager* connManager;

    std::mutex _mutexWakeCycle;
    std::condition_variable _condvarWakeCycle;
    std::thread _gameThread;
    std::thread _clockThread;
    bool _isRunning = true;

    void ExecuteCycle(); // Executes every 100 ms
    void ExecuteRound(); // Executes every 10 cycles
    void ExecuteTick(); // Executes every 10 rounds
    uint16_t ParseUserInput(std::string _cmd, UserConnection* _conn);
    // The two functions below work concurrently together to execute cycle every 100 ms real-time
    void BlockingCycleExecutor();
    void UnblockingClock();
};

#endif
