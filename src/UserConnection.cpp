/* Implementation of UserConnection class

Generalized version:
There are two threads: runListening and runSending
runListening will read the socket in a loop, and send the input to the Engine
by calling Engine->parseInput
runSending will block waiting on reading its own sendBuffer: the Engine is
responsible for waking the thread by writing into the sendBuffer and waking
the runSending thread
UserConnection needs a pointer to Engine

Upon user exit, the runListening thread should send the "quit" command to the
Engine, and at the same time break its loop to finish its thread
Upon user exit, the Engine which receives the "quit" command will send a
signal to the runSending thread, which will break its loop and finish its
thread upon receiving "quit" from the Engine.

There should be a ConnectionManager class which will manage these
UserConnections and their threads. Upon user exit, the Manager should clean
up the UserConnection by destroying it.

Therefore all the Engine needs to do is parseInput and then use its internal
logic to figure out who should receive which outputs.

The Engine should run on an internal clock cycle, queueing the user commands
within a given cycle if necessary. This FIFO queue should buffer only up to
X commands, any more than that and the user will be disconnected.

The Engine should be aware of a series of Player objects. Player objects will
be aware of a UserConnection. Engine should have a function bindPlayerToUser
which allows messages that a Player would see to be sent to the User.

*/

#include <string>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <sstream>

#include "UserConnection.h"

UserConnection::UserConnection(int _socket, Engine& _engine) :
  sockfd(_socket), quitSignal(false), engine(_engine) {
  bzero(readBuffer, BUFFERSIZE);
  bzero(sendBuffer, BUFFERSIZE);
}

int UserConnection::getSocket() { return sockfd; }

// void UserConnection::publish() {
//   std::string published(readBuffer);
//   std::stringstream toWrite;
//   toWrite << "User " << getSocket() << ": " << published << "\n";
//   std::cout << "Calling UserConnection::publish() for socket " << getSocket()
// 	    << " with string: " << toWrite.str();
//   chatPtr->publish(toWrite.str());
// }

bool UserConnection::enQCommand() { return cmdBuffer.push(std::string(readBuffer)); }
std::string UserConnection::deQCommand() { return cmdBuffer.pop(); }

void UserConnection::runListening() {
  std::cout << "Thread runListening started for socket: " << getSocket() << "\n";
  int retFromReadSocket;
  while (!quitSignal) {
    retFromReadSocket = read(sockfd, readBuffer, BUFFERSIZE - 1);
    if (retFromReadSocket < 0) {
      std::cout << "runListening thread got a bad return value from read()\n"; break;
    }
    enQCommand();
  }
  close(sockfd);
}

// void UserConnection::runListening() {
//   std::cout << "UserConnection thread runListening started for socket: " << getSocket() << "\n";
//   int retFromReadSocket;
//   while (true) {
//     bzero(readBuffer, BUFFERSIZE);
//     retFromReadSocket = read(sockFileDesc, readBuffer, BUFFERSIZE - 1);
//     if (retFromReadSocket < 0) {
//       std::cout << "UserConnection::runListening got a bad return value from read()\n";
//       break;
//     }
//     publish();
//     if (strncmp(readBuffer, "quit", 4) == 0) break;
//   }
//   close(sockFileDesc);
// }

void UserConnection::runSending() {
  std::cout << "runSending thread started for socket: " << getSocket() << "\n";
  int retFromWriteSocket;
  // Function incomplete: to be continued by programmer-slave.
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
