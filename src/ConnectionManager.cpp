#include "ConnectionManager.h"

#include <cstring>
#include <iostream>
#include <utility>
#include <unistd.h>

ConnectionManager::ConnectionManager(int _port, Engine& _engine) :
  port(_port), engine(_engine)
{
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0)
    {
        std::cout << "ConnectionManager constructor: Could not open socket\n";
        exit(1);
    }
    bzero((char*) &bindAddress, sizeof(bindAddress));
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_port = htons(port);
    bindAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd, (struct sockaddr*) &bindAddress, sizeof(bindAddress)) < 0)
    {
        std::cout << "ConnectionManager constructor: Could not bind socket to an address\n";
        exit(1);
    }
    listen(sockfd, BACKLOG);
    socketLen = sizeof(bindAddress);
}

ConnectionManager::~ConnectionManager()
{
    for (auto i : _userConns)
        _userConns->Remove();
    // By the time a ConnectionManager is destructed, all user connections have been cut
    close(sockfd);
}

void ConnectionManager::AddNewConnection(int sock)
{
    std::cout << "Accepted new connection on socket #" << sock << "\n";
    _userConns.push_back(std::make_unique<UserConnection>(sock));
    ++_numConns;
}

bool ConnectionManager::RemoveConnection(int _sock)
{
    // Used to cut a connection off from the server-side control
    auto iterator = std::find_if(_userConns.begin(),
                                 _userConns.end(),
                                 [_sock](std::unique_ptr<UserConnection> u) -> bool
                                 {
                                     return _sock == u->GetSocket();
                                 });
    if (iterator == _userConns.end())
    {
        std::cout << "ConnectionManager::RemoveConnection: Socket #" << _sock << " not found.\n";
        return false;
    }
    iterator->Remove();
    return true;
}

void ConnectionManager::Run()
{
    int newSockFD;
    while (!isShutdown)
    {
        newSockFD = accept(sockfd, (struct sockaddr*) &acceptAddress, &socketLen);
        // ^ blocking syscall
        if (newSockFD < 0) { std::cout << "WARNING: could not accept a user connection\n"; break; }
        AddNewConnection(newSockFD);
    }
}

int ConnectionManager::GetNumConns() { return _numConns; }
