#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <utility>
#include <unistd.h>

ConnectionManager::ConnectionManager(int _port, Engine& _engine) :
  port(_port), engine(_engine) {
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd < 0) {
    std::cout << "ConnectionManager constructor: Could not open socket\n";
    exit(1);
  }
  bzero((char*) &bindAddress, sizeof(bindAddress));
  bindAddress.sin_family = AF_INET;
  bindAddress.sin_port = htons(port);
  bindAddress.sin_addr.s_addr = INADDR_ANY;
  if (bind(sockfd, (struct sockaddr*) &bindAddress, sizeof(bindAddress)) < 0) {
    std::cout << "ConnectionManager constructor: Could not bind socket to an address\n";
    exit(1);
  }
  listen(sockfd, BACKLOG);
  socketLen = sizeof(bindAddress);
  userConns.reserve(MAXUSERS);
}

ConnectionManager::~ConnectionManager() {
  for (auto i = userConns.begin(); i != userConns.end(); ++i) {
    delete i->second;
    userConns.erase(i);
  }
  closeSocket();
  for (auto i = userSendThreads.begin(); i != userSendThreads.end(); ++i)
    i->join(); // Once UCs out of scope (delete), their threads should end and join this
  for (auto i = userReadThreads.begin(); i != userReadThreads.end(); ++i)
    i->join();
}

bool ConnectionManager::addNewConnection(int _sock) {
  std::cout << "Accepted new connection on socket #" << _sock << "\n";
  UserConnection* newConn;
  userConns.emplace(std::pair<int, UserConnection*>(_sock, newConn));
  userConns[_sock] = new UserConnection(_sock);
  // I do not take the iterator returned by emplace to spawn threads, because it may trigger a
  // re-hash of the hashmap, which means that the iterator will no longer be valid.
  auto it = userConns.find(_sock); // it is an iterator
  if (it == userConns.end()) {
    std::cout << "STL hashmap did not contain the inserted key! Abort...\n"; return false;
  }
  engine.addNewConnection(it->second);
  std::cout << "Spawned a user-listening thread for socket #" << _sock << "\n";
  userReadThreads.push_back(std::thread(&UserConnection::runListening, it->second));
  std::cout << "Spawned a user-sending thread for socket #" << _sock << "\n";
  userSendThreads.push_back(std::thread(&UserConnection::runSending, it->second));
  ++numConns;
  return true;
}

void ConnectionManager::removeConnection(int _sock) {
  std::cout << "Removing connection corresponding to socket #" << _sock << "\n";
  userConns[_sock]->notifySendThread(); // This should make client disconnect
  delete userConns[_sock];
  userConns.erase(_sock);
  --numConns;
  // When a connection is removed, the thread dies
}

void ConnectionManager::run() {
  int newSockFD;
  while (!isShutdown) {
    newSockFD = accept(sockfd, (struct sockaddr*) &acceptAddress, &socketLen);
    if (newSockFD < 0) { std::cout << "WARNING: could not accept a user connection\n"; break; }
    if (!addNewConnection(newSockFD)) break;
  }
}

void ConnectionManager::closeSocket() { close(sockfd); }
int ConnectionManager::getNumConns() { return numConns; }
