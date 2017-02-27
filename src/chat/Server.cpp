#include "Server.h"

#include <cstring>
#include <iostream>
#include <unistd.h>

Server::Server(int _port) : port(_port) {
  sockFileDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockFileDesc < 0) {
    std::cout << "Could not open socket\n";
    exit(1);
  }
  bzero((char*) &bindAddress, sizeof(bindAddress));
  bindAddress.sin_family = AF_INET;
  bindAddress.sin_port = htons(port);
  bindAddress.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockFileDesc, (struct sockaddr*) &bindAddress, sizeof(bindAddress)) < 0) {
    std::cout << "Could not bind socket to an address\n";
    exit(1);
  }
  listen(sockFileDesc, BACKLOG);
  socketLen = sizeof(bindAddress);
}

Server::~Server() {
  close(sockFileDesc);
  int numThreads = clientConnections.size();
  for (int i = 0; i < numThreads; ++i) {
    clientSendThreads[i].join();
    clientReadThreads[i].join();
  }
}

void Server::addNewConnection(int _sockFileDesc, int _conn) {
  std::cout << "Accepted new connection #" << _conn << " on socket " << _sockFileDesc << "\n";
  clientConnections.push_back(UserConnection(_sockFileDesc, &chatroom));
  clientReadThreads.push_back(std::thread(&UserConnection::runListening, &clientConnections[_conn]));
  // clientReadThreads.push_back(std::thread(clientConnections[_conn].runListening));
  std::cout << "Spawned thread runListening on connection #" << _conn << "\n";
  clientReadThreads.push_back(std::thread(&UserConnection::runSending, &clientConnections[_conn]));
  // clientSendThreads.push_back(std::thread(clientConnections[_conn].runSending));
  std::cout << "Spawned thread runSending on connection #" << _conn << "\n";  
}

void Server::run() {
  int newSockFileDesc, i; // i is connection number
  for (i = 0; ; ++i) {
    newSockFileDesc = accept(sockFileDesc, (struct sockaddr*) &acceptAddress, &socketLen);
    if (newSockFileDesc < 0) {
      std::cout << "WARNING: could not accept a user connection\n";
    }
    addNewConnection(newSockFileDesc, i);
  }
}
