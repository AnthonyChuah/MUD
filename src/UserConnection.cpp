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
#include <sstream>
#include <unistd.h>

#include "UserConnection.h"

UserConnection::UserConnection(int _socket) :
  sockfd(_socket) {
  bzero(readBuffer, BUFFERSIZE);
  bzero(sendBuffer, BUFFERSIZE);
}

UserConnection::~UserConnection() {
  close(sockfd); // Erase from hashmap of UCs means that socket will close in destructor
}

int UserConnection::getSocket() { return sockfd; }

bool UserConnection::enQCommand() {
  std::string cmd(readBuffer);
  std::cout << "Enqueueing cmd into cmdBuffer: " << cmd;
  // To parse multiple line input by users, you should split the string by newline
  // Then insert each line separately into the cmdBuffer
  return cmdBuffer.push(cmd);
}
std::string UserConnection::deQCommand() { return cmdBuffer.pop(); }

void UserConnection::runListening() {
  std::cout << "runListening thread started for socket: " << sockfd << "\n";
  int retFromReadSocket;
  while (!isQuit) {
    retFromReadSocket = read(sockfd, readBuffer, BUFFERSIZE - 1);
    if (retFromReadSocket < 0) {
      std::cout << "runListening thread got a bad return value from read()\n"; break;
    }
    if (!enQCommand()) {
      std::cout << "enQCommand() returned false, buffer full. Kick user out.\n";
      isQuit = true;
    }
  }
  std::cout << "Thread UC::runListening() has ended.\n";
}

void UserConnection::runSending() {
  std::cout << "runSending thread started for socket: " << getSocket() << "\n";
  int retFromWriteSocket;
  while (!isQuit) {
    {
      std::unique_lock<std::mutex> lock1(mutexSend);
      std::cout << "Locked waiting on the send mutex\n";
      condvarSend.wait(lock1);
      std::cout << "Woke and unlocked waiting on the send mutex\n";
      loadSendBuffer();
    }
    retFromWriteSocket = write(sockfd, sendBuffer, BUFFERSIZE);
    if (retFromWriteSocket < 0) {
      std::cout << "runSending thread got a bad return value from write()\n"; break;
    }
  }
  std::cout << "Thread UC::runSending() has ended.\n";
}

bool UserConnection::pushSendQ(std::string _toPush) {
  std::cout << "UC::pushSendQ with string: " << _toPush;
  return outBuffer.push(_toPush);
}

void UserConnection::notifySendThread() {
  condvarSend.notify_all();
}

void UserConnection::loadSendBuffer() {
  bzero(sendBuffer, BUFFERSIZE);
  std::stringstream ss;
  for (int i = outBuffer.getNumel(); i > 0; --i) {
    ss << outBuffer.pop();
  }
  // Now the Engine should ensure that you never overflow the output buffer.
  // So I do not explicitly check it here! Programmer beware.
  strcpy(sendBuffer, ss.str().c_str());
}
