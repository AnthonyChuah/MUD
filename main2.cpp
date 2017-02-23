#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <thread>
#include <mutex>

void error(const char msg[]) {
  std::cerr << msg;
  exit(1);
}

std::mutex bufferMutex;
constexpr int bufferSize = 256;

void launchClientThread(int sockFileDesc) {
  char buffer[bufferSize];
  int retFromReadSocket;
  while (true) {
    // Wipe clean the buffer and then read socket data into buffer
    bzero(buffer, bufferSize);
    retFromReadSocket = read(sockFileDesc, buffer, bufferSize);
    // read(int fileDescriptor, char* byteBuffer, int bufferSize)
    if (retFromReadSocket < 0) error("Could not read from socket\n");
    std::cout << "User on socket " << sockFileDesc << ": " << buffer << "\n";
    // retFromReadSocket = write(newSockFileDesc, "Your message was:\n", bufferSize);
    retFromReadSocket = write(sockFileDesc, buffer, bufferSize);
    if (strncmp(buffer, "quit", 4) == 0) {
      // retFromReadSocket = write(newSockFileDesc, "Sad to see you go! Closing session...\n", bufferSize);
      break;
    }
    if (retFromReadSocket < 0) error("Could not write to socket\n");
  }
  close(sockFileDesc);
}

int main(int argc, char* argv[]) {
  int sockFileDesc, newSockFileDesc, portNum; portNum = atoi(argv[1]);
  socklen_t clientLen;
  struct sockaddr_in serverAddr, clientAddr;
  if (argc < 2) { error("No port number provided.\n"); }

  sockFileDesc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockFileDesc < 0) { error("Could not open socket.\n"); }
  bzero((char*) &serverAddr, sizeof(serverAddr)); // Writes zeros into byte buffer of size
  
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(portNum); // htons is Host to Network Short
  serverAddr.sin_addr.s_addr = INADDR_ANY;

  // Bind the socket with the sockaddr struct
  if (bind(sockFileDesc, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0)
    error("Could not bind socket to an address\n");

  // Start listening for new connections
  int backlog = 16;
  listen(sockFileDesc, backlog); // Listen to this socket with a backlog of 16
  // The backlog allows connections to pile up before we accept them
  // If 17 connections come in without us accepting any, the 17th will not connect

  // Accept the connections
  // Now I want to be able to accept multiple user connections!
  clientLen = sizeof(clientAddr);
  std::vector<int> clientsSockets;
  std::vector<std::thread> clientThreads;
  clientsSockets.reserve(256);
  while (true) {
    newSockFileDesc = accept(sockFileDesc, (struct sockaddr*) &clientAddr, &clientLen);
    if (newSockFileDesc < 0) error("Could not accept user connection\n");
    std::cout << "User has made new connection, user socket: " << newSockFileDesc << "\n";
    clientsSockets.push_back(newSockFileDesc);
    clientThreads.push_back(std::thread(launchClientThread, newSockFileDesc));
  }
  
  // while (true) {
  //   // Wipe clean the buffer and then read socket data into buffer
  //   bzero(buffer, bufferSize);
  //   retFromReadSocket = read(newSockFileDesc, buffer, bufferSize);
  //   // read(int fileDescriptor, char* byteBuffer, int bufferSize)
  //   if (retFromReadSocket < 0) error("Could not read from socket\n");
  //   std::cout << "User: " << buffer << "\n";
  //   // retFromReadSocket = write(newSockFileDesc, "Your message was:\n", bufferSize);
  //   retFromReadSocket = write(newSockFileDesc, buffer, bufferSize);
  //   if (strncmp(buffer, "quit", 4) == 0) {
  //     // retFromReadSocket = write(newSockFileDesc, "Sad to see you go! Closing session...\n", bufferSize);
  //     break;
  //   }
  //   if (retFromReadSocket < 0) error("Could not write to socket\n");
  // }

  int numThreads = clientThreads.size();
  for (int i = 0; i < numThreads; ++i) {
    clientThreads[i].join();
  }
  close(sockFileDesc);
  return 0;
}
