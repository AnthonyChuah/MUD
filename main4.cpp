#include <iostream>
#include <thread>

#include "src/UserConnection.h"
#include "src/ConnectionManager.h"
#include "src/Engine.h"

int main(int argc, char* argv[])
{
    if (argc < 2) return 1;
    int portNum = atoi(argv[1]);
    Engine engine;
    ConnectionManager manager(portNum, engine);
    engine.AttachConnectionManager(&manager);
    engine.Run();
    manager.Run();
    std::cout << "Server main parent thread terminated by admin.\n";
    return 0;
}
