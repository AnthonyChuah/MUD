#ifndef USERCONNECTION_H
#define USERCONNECTION_H

/* Header file for user connection
This should contain: 
(2) the receive data buffer that is read from the user's socket
(2) the size of receive buffer
(3) the socket file descriptor for this particular user's connection
*/

#include "Engine.h"
#include "../utils/CircularBuffer.hpp"

#include <mutex>
#include <condition_variable>

class UserConnection {
  static constexpr int BUFFERSIZE = 2048;
  static constexpr int CMDBUFFERSIZE = 32;
  static constexpr int OUTPUTBUFFERSIZE = 256;
  CircularBuffer<std::string, CMDBUFFERSIZE> cmdBuffer;
  CircularBuffer<std::string, OUTPUTBUFFERSIZE> outBuffer;
  // output should work like this: the Engine has a collection of Players,
  // each with their own buffered output-to-user strings, called "echoes".
  // the Sending Thread should block waiting until notified to clear that buffer.
  // To clear the buffer, stream each string into a stringstream, delimited by newlines.
  // Ensure that you NEVER exceed 2048 characters, or you WILL OVERFLOW the sendBuffer.
  // Concurrency issue can appear here: maybe change the CircularBuffer to be concurrent?
  char readBuffer[BUFFERSIZE];
  char sendBuffer[BUFFERSIZE];
  std::mutex mutexSend;
  std::condition_variable condvarSend;
  int sockfd;
  bool isQuit;
  Engine& engine;
  bool enQCommand(); // Add new command to the CircularBuffer queue
  // Add Engine's shutdown signal as a friend function here
  void loadSendBuffer(); // Loads the sendBuffer from the outBuffer queue of strings
 public:
  UserConnection(int _socket, Engine& _engine);
  ~UserConnection();
  int getSocket(); // Gets UserConnection.sockFileDesc, not sure it is needed
  // void publish(); // Publishes your sendBuffer data to the chat room
  void runListening(); // To be passed into thread: listens to user's socket
  void runSending(); // To be passed into thread: writes to user's socket
  void notifySendThread(); // Triggers the sending thread to send buffered output to user
  std::string deQCommand(); // Extract a command from the CircularBuffer queue
};

#endif
