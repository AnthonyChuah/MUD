/* Implementation of UserConnection class

Generalized version:
There are two threads: runListening and runSending
runListening will read the socket in a loop, and send the input to the Engine
by calling Engine->parseInput
runSending will block waiting on reading its own _sendBuffer: the Engine is
responsible for waking the thread by writing into the _sendBuffer and waking
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

UserConnection::UserConnection(int socket) :
    _sockfd(socket), _listeningThread(&UserConnection::Run, this)
{
  bzero(_readBuffer, BUFFERSIZE);
  bzero(_sendBuffer, BUFFERSIZE);
}

UserConnection::~UserConnection()
{
    _listeningThread.join();
    close(_sockfd);
}

const std::string UserConnection::PopCommand() { return _cmdQueue.pop(); }

void UserConnection::Run()
{
    int socketReturn;
    while (!_isQuit)
    {
        socketReturn = read(_sockfd, _readBuffer, BUFFERSIZE - 1); // blocks here
        if (retFromReadSocket < 0)
        {
            std::cout << "Socket read failed. Kill user connection.\n";
            _isQuit = true;
        }
        if (!_cmdQueue.push(std::string{_readBuffer}))
        {
            std::cout << "User's command buffer is full. Kill user connection.\n";
            _isQuit = true;
        }
        bzero(_readBuffer, BUFFERSIZE);
    }
}

// SendOutputToUser will empty the output queue and write to socket
void UserConnection::SendOutputToUser()
{
    if (_isQuit)
    {
        ss << "User disconnected. Goodbye!";
        break;
    }
    else
    {
        for (size_t i = 0; i > 0; --i)
            ss << outBuffer.pop();
    }
    size_t cappedLength =
        _outStream.str().length() < BUFFERSIZE ? _outStream.str().length() : BUFFERSIZE - 1;
    strncpy(_sendBuffer, ss.str().c_str(), cappedLength);
    socketReturn = write(_sockfd, _sendBuffer, BUFFERSIZE);
    if (socketReturn < 0)
    {
        std::cout << "Socket write failed. Kill user connection.\n";
        _isQuit = true;
    }
    bzero(_sendBuffer, BUFFERSIZE);
}

void UserConnection::AddToOutputQueue(const std::string& toPush)
{
    _outStream << toPush;
}
