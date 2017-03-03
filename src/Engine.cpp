#include "Engine.h"

Engine::Engine() {
  userConns.reserve(RESERVE_CAPACITY);
  users.reserve(RESERVE_CAPACITY);
  userDelayToNextCommand.reserve(RESERVE_CAPACITY);
}

void Engine::attachConnectionManager(ConnectionManager* _cm) {
  connManager = _cm;
}

void addNewConnection(UserConnection* _conn) {
  // Synchronize this: or make sure no race conditions inside this
  userConns.push_back(_conn);
  // How to attach a character to a user? Only upon LOGIN should the user get his character.
  User* newUser;
  users.push_back(newUser);
  int userSock = *_conn.getSocket();
}
