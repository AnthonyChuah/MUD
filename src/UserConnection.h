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

class UserConnection
{
public:
    explicit UserConnection(int socket);
    ~UserConnection();

    int GetSocket() { return _sockfd; }
    const std::string PopCommand(); // Extract a command from the CircularBuffer queue
    void AddToOutputQueue(const std::string& toPush);
private:
    void Run();

    static constexpr int BUFFERSIZE = 2048;
    static constexpr int CMDQUEUESIZE = 32;
    CircularBuffer<std::string, CMDQUEUESIZE> _cmdQueue;
    std::stringstream _outStream;
    char _readBuffer[BUFFERSIZE];
    char _sendBuffer[BUFFERSIZE];
    int _sockfd;
    bool _isQuit = false;
    std::thread _listeningThread;

    friend void Engine::shutdownUserConn(UserConnection* _conn);
    friend void Engine::getUserNextCmd(UserConnection* _conn, const int& _index);
};

#endif
