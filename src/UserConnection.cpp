/* Implementation of UserConnection class
*/

#include <string>
#include <cstring>
#include <iostream>

#include <unistd.h>
// #include <sys/socket.h>

#include "UserConnection.h"
#include <sstream>

UserConnection::UserConnection(int _socket, ChatBuffer* _chatPtr) :
  sockFileDesc(_socket), chatPtr(_chatPtr) {
  bzero(readBuffer, BUFFERSIZE);
  bzero(sendBuffer, BUFFERSIZE);
}

int UserConnection::getSocket() { return sockFileDesc; }
void UserConnection::publish() {
  std::string published(readBuffer);
  std::stringstream toWrite;
  toWrite << "User " << getSocket() << ": " << published << "\n";
  std::cout << "Calling UserConnection::publish() for socket " << getSocket()
	    << " with string: " << toWrite.str();
  chatPtr->publish(toWrite.str());
}

void UserConnection::runListening() {
  std::cout << "UserConnection thread runListening started for socket: " << getSocket() << "\n";
  int retFromReadSocket;
  while (true) {
    bzero(readBuffer, BUFFERSIZE);
    retFromReadSocket = read(sockFileDesc, readBuffer, BUFFERSIZE);
    if (retFromReadSocket < 0) {
      std::cout << "UserConnection::runListening got a bad return value from read()\n";
      break;
    }
    publish();
    if (strncmp(readBuffer, "quit", 4) == 0) break;
  }
  close(sockFileDesc);
}

void UserConnection::runSending() {
  std::cout << "UserConnection thread runSending started for socket: " << getSocket() << "\n";
  int retFromWriteSocket;
  std::string toSend;
  while (true) {
    bzero(sendBuffer, BUFFERSIZE);
    std::cout << "UserConnection SendThread socket " << getSocket() << " blocks waiting to read.\n";
    toSend = chatPtr->readMessage();
    strcpy(sendBuffer, toSend.c_str());
    retFromWriteSocket = write(sockFileDesc, sendBuffer, BUFFERSIZE);
    if (retFromWriteSocket < 0) {
      std::cout << "UserConnection:runSending got a bad return value from send()\n"
		<< "If this happened right after user has quit, it is normal.\n";
      break;
    }
  }
}
