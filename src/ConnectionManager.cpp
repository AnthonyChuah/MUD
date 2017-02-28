#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

ConnectionManager::ConnectionManager(int _port, Engine* _engine) :
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
}

ConnectionManager::~ConnectionManager() {
  close(sockfd);
  int numThreads = userConns.size();
  for (auto i = userSendThreads.begin(); i != userSendThreads.end(); ++i)
    i->join();
  for (auto i = userReadThreads.begin(); i != userReadThreads.end(); ++i)
    i->join();
}

void ConnectionManager::addNewConnection(int _sockFileDesc, int _conn) {
  std::cout << "Accepted new connection #" << _conn << " on socket " << _sockFileDesc << "\n";
  userConns.push_back(UserConnection(_sockFileDesc, &engine));
  std::cout << "Spawned a user-listening thread on connection #" << _conn << "\n";
  userReadThreads.push_back(std::thread(&UserConnection::runListening, &userConns[_conn]));
  std::cout << "Spawned a user-sending thread on connection #" << _conn << "\n";
  userSendThreads.push_back(std::thread(&UserConnection::runSending, &userConns[_conn]));
}

void ConnectionManager::run() {
  int newSockFD, i;
  for (i = 0; ; ++i) {
    newSockFD = accept(sockfd, (struct sockaddr*) &acceptAddress, &socketLen);
    if (newSockFD < 0) std::cout << "WARNING: could not accept a user connection\n";
    addNewConnection(newSockFD, i);
  }
}
