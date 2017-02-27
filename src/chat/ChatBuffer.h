#ifndef CHATBUFFER_H
#define CHATBUFFER_H

/* Header file for chat buffer
This should contain:
(1) String msg contains message
(2) readMessage() function for user reading threads to block on
*/

#include <string>
#include <condition_variable>
#include <mutex>

class ChatBuffer {
  std::string msg;
  std::mutex mutexPublish;
  std::condition_variable condVarRead;
 public:
  ChatBuffer();
  void publish(std::string _msg); // Publish this message to all waiting threads
  std::string readMessage(); // Block waiting until threads notified
};

#endif
