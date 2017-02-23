#ifndef SERVER_H
#define SERVER_H

/* Header file for server
Server manages a collection of UserConnections
Each UserConnection has a pair of threads: Send and Read threads
*/

#include <vector>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "ChatBuffer.h"
#include "UserConnection.h"

class Server {
  static constexpr int BACKLOG = 16;
  std::vector<UserConnection> clientConnections;
  std::vector<std::thread> clientSendThreads;
  std::vector<std::thread> clientReadThreads;
  int sockFileDesc; // Main server's socket file descriptor
  int port; // Server's port number
  socklen_t socketLen; // Server's socket length
  struct sockaddr_in bindAddress, acceptAddress;
  ChatBuffer chatroom;
  void addNewConnection(int _sockFileDesc, int _conn);
 public:
  Server(int _port);
  ~Server();
  void run();
};

#endif
