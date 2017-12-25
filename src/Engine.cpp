#include "ConnectionManager.h"
#include "Engine.h"

#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>

Engine::Engine() {}

void Engine::AttachConnectionManager(ConnectionManager* _cm)
{
    connManager = _cm;
}

void Engine::AddNewConnection(UserConnection* _conn)
{
    std::cout << "Engine added new connection!\n";
    // Synchronize this: or make sure no race conditions inside this
    userConns.push_back(_conn);
    userDelayToNextCommand.push_back(0);
}

void Engine::ExecuteCycle()
{
    int nConns = connManager->getNumConns();
    for (int i = 0; i < nConns; ++i)
    {
        if (userDelayToNextCommand[i] == 0)
        {
            getUserNextCmd(userConns[i], i);
        }
        if (userDelayToNextCommand[i] > 0)
        {
            --userDelayToNextCommand[i];
        }
    }
    ++timeSinceStart;

    if (--cyclesToNextRound == 0)
    {
        ExecuteRound();
        cyclesToNextRound = CYCLES_IN_ROUND;
        if (--roundsToNextTick == 0)
        {
            ExecuteTick();
            roundsToNextTick = ROUNDS_IN_TICK;
        }
    }
}

void Engine::ExecuteRound()
{
    // Do nothing for now: to be added in the future
    // std::cout << "Round happened. timeSinceStart: " << timeSinceStart << "\n";
}

void Engine::ExecuteTick()
{
    // Do nothing for now: to be added in the future
    // std::cout << "Tick happened! timeSinceStart: " << timeSinceStart << "\n";
}

void Engine::GetUserNextCmd(UserConnection* _conn, const int& _index)
{
    if (_conn->cmdBuffer.getNumel() > 0)
    {
        std::cout << "Engine::getUserNextCmd grabbed a command\n";
        userDelayToNextCommand[_index] = parseUserInput(_conn->deQCommand(), _conn);
    }
}

unsigned Engine::ParseUserInput(std::string _cmd, UserConnection* _conn)
{
    // Treat this like a stub process for now, just publish what the user wrote to all
    // It should return the "delay" of a given command, for now all commands are 1 cycle
    std::stringstream ss;
    ss << "User " << _conn->getSocket() << ": " << _cmd;
    std::string toWrite = ss.str();
    std::cout << "User string toWrite is: " << toWrite;
    for (auto i = userConns.begin(); i != userConns.end(); ++i)
    {
        std::cout << "Writing to a given user: " << (*i)->getSocket() << "\n";
        (*i)->pushSendQ(toWrite);
        (*i)->notifySendThread();
    }
    if (_cmd.length() >= 4 && _cmd.substr(0, 4) == "quit") shutdownUserConn(_conn);
    return 1; // Minimum delay of a command
}

void Engine::ShutdownUserConn(UserConnection* _conn)
{
    std::cout << "Shutting down user connection: " << _conn->getSocket() << "\n";
    // First, remove _conn from vector<UC*> userConns and vector<int> userDelay...
    std::vector<UserConnection*>::iterator it = std::find(userConns.begin(), userConns.end(), _conn);
    size_t index = std::distance(userConns.begin(), it);
    // O(1) way of popping from vector: just std::swap with back() and then pop_back.
    // But this leads to unpredictable order of parsing commands, so let's keep the inefficient way.
    userDelayToNextCommand.erase(userDelayToNextCommand.begin() + index);
    userConns.erase(userConns.begin() + index);
    _conn->isQuit = true;
    int socket = _conn->getSocket();
    connManager->removeConnection(socket);
}

void Engine::Run()
{
    _gameThread = std::thread{&Engine::BlockingCycleExecutor, this};
    _clockThread = std::thread{&Engine::UnblockingClock, this};
}

void Engine::BlockingCycleExecutor()
{
    std::cout << "Engine blocking thread launched.\n";
    while (_isRunning)
    {
        std::unique_lock<std::mutex> locker(_mutexWakeCycle);
        _condvarWakeCycle.wait(locker);
        ExecuteCycle();
    }
}

void Engine::UnblockingClock()
{
    std::cout << "Clock cycle thread launched in parallel with engine blocking thread.\n";
    while (_isRunning)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(TIMESTEP_CYCLE_MS));
        _condvarWakeCycle.notify_all();
    }
}
