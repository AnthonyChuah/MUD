#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <vector>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Engine.h"
#include "UserConnection.h"

/*
CircularBuffer should be an efficient templated FIFO queue with contiguous memory
It can be fixed length, in fact its default constructor should be disallowed and
it should always be constructed with a fixed length. Any attempt to push beyond its
fixed length should crash the user connection.
#include "../utils/CircularBuffer.hpp"

The user should have its commands buffered into this FIFO queue.
If the user attempts to overflow the command buffer, send a signal to Engine to remove
the Player and disconnect the user.
*/

class ConnectionManager
{
public:
    explicit ConnectionManager(int _port, Engine& _engine);
    ~ConnectionManager();
    int GetNumConns();
    void Run();
    void RemoveConnection(int _sock);

private:
    using UserConn = std::unique_ptr<UserConnection>;

    bool AddNewConnection(int sock);

    static constexpr int BACKLOG = 16;
    std::vector<UserConn> _userConns;
    int _sockfd;
    int _port;
    socklen_t _socketLen;
    struct sockaddr_in bindAddress, acceptAddress;
    Engine& engine;
    bool _isShutdown = false;
    int _numConns = 0;
};

#endif
