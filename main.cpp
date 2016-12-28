#include "tmud.h"
#include "logger.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
The steps involved in establishing a socket on the server side are as follows:

Create a socket with the socket() system call
Bind the socket to an address using the bind() system call. For a server socket on the Internet, an address consists of a port number on the host machine.
Listen for connections with the listen() system call
Accept a connection with the accept() system call. This call typically blocks until a client connects with the server.
Send and receive data
*/

void error(const char msg[]) {
  perror(msg);
  exit(1);
}

int main(int argc, char* argv[]) {
  int sockfd, newsockfd, portno; // sockfd are file descriptors (array subscripts into the file descriptor table
  // for the current proc. sockfd and newsockfd store values returned by socket() and accept() syscalls.
  socklen_t clilen; // Stores size of address of client, needed for accept() syscall.
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;
  int n;
  if (argc < 2) {
    std::cout << "ERROR: no port number provided.\n";
    exit(1);
  }
  sockfd = socket(AF_INET, SOCK_STREAM, 0); // sockfd is some sort of ID returned by socket() function.
  if (sockfd < 0)
    error("ERROR: could not open socket.\n");
  bzero((char*) &serv_addr, sizeof(serv_addr));
  portno = atoi(argv[1]); // Get port number.
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno); // Assign port number to the serv_addr struct.
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR: could not bind the socket to an address.\n");
  listen(sockfd, 5); // Listen for new connections.
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Accept user connection.
  if (newsockfd < 0)
    error("ERROR: could not accept user connection.\n");
  bzero(buffer, 25);
  n = read(newsockfd, buffer, 255); // This should read the stream of socket chars into the buffer.
  if (n < 0)
    error("ERROR: could not read from socket.\n");
  std::cout << "Here is the message from the user: " << buffer << ".\n";
  n = write(newsockfd, "I got your message.\n", 20);
  if (n < 0)
    error("ERROR: could not write to socket.\n");
  close(newsockfd);
  close(sockfd);
  return 0;
}
