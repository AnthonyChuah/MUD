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
}

ConnectionManager::~ConnectionManager() {
  close(sockfd);
  int numThreads = userConns.size();
  for (auto i = userSendThreads.begin(); i != userSendThreads.end(); ++i)
    i->join();
  for (auto i = userReadThreads.begin(); i != userReadThreads.end(); ++i)
    i->join();
}

void ConnectionManager::addNewConnection(int _sock) {
  std::cout << "Accepted new connection #" << _conn << " on socket " << _sock << "\n";
  userConns.add(_sock, UserConnection(_sock, &engine));
  std::cout << "Spawned a user-listening thread for socket #" << _sock << "\n";
  userReadThreads.push_back(std::thread(&UserConnection::runListening, &userConns.get(_sock)));
  std::cout << "Spawned a user-sending thread for socket #" << _sock << "\n";
  userSendThreads.push_back(std::thread(&UserConnection::runSending, &userConns.get(_sock)));
  ++numConns;
}

bool ConnectionManager::removeConnection(int _sock) {
  std::cout << "Removing connection corresponding to socket #" << _sock << "\n";
  userConns.remove(_sock);
  --numConns;
}

void ConnectionManager::run() {
  int newSockFD;
  while (!isShutdown) {
    newSockFD = accept(sockfd, (struct sockaddr*) &acceptAddress, &socketLen);
    if (newSockFD < 0) std::cout << "WARNING: could not accept a user connection\n";
    addNewConnection(newSockFD);
  }
}
