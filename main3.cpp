#include "src/Server.h"

#include <iostream>

int main(int argc, char* argv[]) {
  if (argc < 2) return 1;
  int portNum = atoi(argv[1]);
  {
    Server chatServer(portNum);
    chatServer.run();
  }
  std::cout << "Server main parent thread terminated by admin.\n";
  return 0;
}
