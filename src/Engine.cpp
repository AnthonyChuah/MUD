#include "ConnectionManager.h"
#include "Engine.h"

#include <iostream>
#include <chrono>
#include <sstream>

Engine::Engine() {
  userConns.reserve(RESERVE_CAPACITY);
  userDelayToNextCommand.reserve(RESERVE_CAPACITY);
}

void Engine::attachConnectionManager(ConnectionManager* _cm) {
  connManager = _cm;
}

void Engine::addNewConnection(UserConnection* _conn) {
  std::cout << "Engine added new connection!\n";
  // Synchronize this: or make sure no race conditions inside this
  userConns.push_back(_conn);
  userDelayToNextCommand.push_back(0);
}

void Engine::executeCycle() {
  int nConns = connManager->getNumConns();
  for (int i = 0; i < nConns; ++i) {
    if (userDelayToNextCommand[i] == 0) {
      getUserNextCmd(userConns[i], i);
    }
  }
  ++timeSinceStart;
  if (--cyclesToNextRound == 0) {
    executeRound();
    cyclesToNextRound = CYCLES_IN_ROUND;
    if (--roundsToNextTick == 0) {
      executeTick();
      roundsToNextTick = ROUNDS_IN_TICK;
    }
  }
}

void Engine::executeRound() {
  // Do nothing for now: to be added in the future
  std::cout << "Round happened. timeSinceStart: " << timeSinceStart << "\n";
}

void Engine::executeTick() {
  // Do nothing for now: to be added in the future
  std::cout << "Tick happened! timeSinceStart: " << timeSinceStart << "\n";
}

void Engine::getUserNextCmd(UserConnection* _conn, const int& _index) {
  if (_conn->cmdBuffer.getNumel() > 0) {
    std::cout << "Engine::getUserNextCmd grabbed a command\n";
    userDelayToNextCommand[_index] = parseUserInput(_conn->deQCommand(), _conn);
  }
}

int Engine::parseUserInput(std::string _cmd, UserConnection* _conn) {
  // Treat this like a stub process for now, just publish what the user wrote to all
  // It should return the "delay" of a given command, for now all commands are 1 cycle
  std::stringstream ss;
  ss << "User " << _conn->getSocket() << ": " << _cmd;
  std::string toWrite = ss.str();
  std::cout << "User string toWrite is: " << toWrite;
  for (auto i = userConns.begin(); i != userConns.end(); ++i) {
    (*i)->pushSendQ(toWrite);
    (*i)->notifySendThread();
  }
  if (_cmd == "quit") shutdownUserConn(_conn);
  return 1; // Minimum delay of a command
}

void Engine::shutdownUserConn(UserConnection* _conn) {
  _conn->isQuit = true;
}

void Engine::start() {
  std::thread blockingThread(&Engine::blockingCycleExecutor, this);
  unblockingClock();
}

void Engine::blockingCycleExecutor() {
  std::cout << "Engine blocking thread launched.\n";
  while (isRunning) {
    std::unique_lock<std::mutex> locker(mutexWakeCycle);
    condvarWakeCycle.wait(locker);
    executeCycle();
  }
}

void Engine::unblockingClock() {
  std::cout << "Clock cycle thread launched in parallel with engine blocking thread.\n";
  while (isRunning) {
    std::this_thread::sleep_for(std::chrono::milliseconds(TIMESTEP_CYCLE_MS));
    condvarWakeCycle.notify_all();
  }
}
