#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

/* Header file for Connection Manager
This is like a ThreadPool for managing the UserConnections and the runSending and
runListening threads.
*/

#include <vector>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "UserConnection.h"
#include "Engine.h"

/*
CircularBuffer should be an efficient templated FIFO queue with contiguous memory
It can be fixed length, in fact its default constructor should be disallowed and
it should always be constructed with a fixed length. Any attempt to push beyond its
fixed length should crash the user connection.
#include "../utils/CircularBuffer.hpp"

The user should have its commands buffered into this FIFO queue.
If the user attempts to overflow the command buffer, send a signal to Engine to remove
the Player and disconnect the user.
*/

class ConnectionManager {
  static constexpr int BACKLOG = 16;
  std::vector<UserConnection> userConns;
  std::vector<std::thread> userSendThreads;
  std::vector<std::thread> userReadThreads;
  int sockfd;
  int port;
  socklen_t sockLen;
  struct sockaddr_in bindAddress, acceptAddress;
  Engine* engine;
  void addNewConnection(int _sockFileDesc, int _conn);
  ConnectionManager(); // Default constructor should never be called
 public:
  ConnectionManager(int _port, Engine* _engine);
  ~ConnectionManager();
  void run();
};

#endif
