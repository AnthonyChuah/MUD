#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

ConnectionManager::ConnectionManager(int _port, Engine* _engine) :
  port(_port), engine(_engine), isShutdown(false), numConns(0) {
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
  close(sockfd);
  int numThreads = userConns.size();
  for (auto i = userSendThreads.begin(); i != userSendThreads.end(); ++i)
    i->join();
  for (auto i = userReadThreads.begin(); i != userReadThreads.end(); ++i)
    i->join();
}

bool ConnectionManager::addNewConnection(int _sock) {
  std::cout << "Accepted new connection on socket #" << _sock << "\n";
  userConns.emplace(std::make_pair<int, UserConnection>(_sock, UserConnection(_sock, &engine)));
  // I do not take the iterator returned by emplace to spawn threads, because it may trigger a
  // re-hash of the hashmap, which means that the iterator will no longer be valid.
  auto it = userConn.find(_sock); // it is an iterator
  if (it == userConn.end()) {
    std::cout << "STL hashmap did not contain the inserted key! Abort...\n"; return false;
  }
  std::cout << "Spawned a user-listening thread for socket #" << _sock << "\n";
  userReadThreads.push_back(std::thread(&UserConnection::runListening, it));
  std::cout << "Spawned a user-sending thread for socket #" << _sock << "\n";
  userSendThreads.push_back(std::thread(&UserConnection::runSending, it));
  ++numConns;
  return true;
}

bool ConnectionManager::removeConnection(int _sock) {
  std::cout << "Removing connection corresponding to socket #" << _sock << "\n";
  userConns.erase(_sock);
  --numConns;
}

void ConnectionManager::run() {
  int newSockFD;
  while (!isShutdown) {
    newSockFD = accept(sockfd, (struct sockaddr*) &acceptAddress, &socketLen);
    if (newSockFD < 0) std::cout << "WARNING: could not accept a user connection\n";
    addNewConnection(newSockFD) ? break : ;
  }
}