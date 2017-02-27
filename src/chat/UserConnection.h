#ifndef USERCONNECTION_H
#define USERCONNECTION_H

/* Header file for user connection
This should contain: 
(2) the receive data buffer that is read from the user's socket
(2) the size of receive buffer
(3) the socket file descriptor for this particular user's connection
*/

#include "ChatBuffer.h"

class UserConnection {
  static constexpr int BUFFERSIZE = 1024;
  char readBuffer[BUFFERSIZE];
  char sendBuffer[BUFFERSIZE];
  int sockFileDesc;
  ChatBuffer* chatPtr;
 public:
  UserConnection(int _socket, ChatBuffer* _chatPtr);
  int getSocket(); // Gets UserConnection.sockFileDesc, not sure it is needed
  void publish(); // Publishes your sendBuffer data to the chat room
  void runListening(); // To be passed into thread: listens to user's socket
  void runSending(); // To be passed into thread: writes to user's socket
};

#endif
