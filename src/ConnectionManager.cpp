#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

ConnectionManager::ConnectionManager(int _port, Engine* _engine) :
  port(_port), engine(_engine) {
}

ConnectionManager::~ConnectionManager() {
}

void ConnectionManager::addNewConnection(int _sockFileDesc, int _conn) {
}

void ConnectionManager::run() {
}
