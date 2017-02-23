#include "ChatBuffer.h"

ChatBuffer::ChatBuffer() : msg("") {}

void ChatBuffer::publish(std::string _msg) {
  std::lock_guard<std::mutex>(mutexPublish, std::adopt_lock);
  msg = _msg;
  condVarRead.notify_all();
}

std::string ChatBuffer::readMessage() {
  std::unique_lock<std::mutex> lock1(mutexPublish);
  condVarRead.wait(lock1);
  lock1.unlock();
  return msg;
}
