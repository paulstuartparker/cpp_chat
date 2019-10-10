#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char** argv) {
  // Setup Port, Socket, Address.
  const char* sPort;
  struct sockaddr_in server_address;
  int port;
  char buffer[1024] = {0};
  const char* hostname = "localhost";
  if (argc < 2) {
    sPort = "5050";
  } else {
    sPort = *(argv + 1);
  }

  port = std::stoi(sPort);
  std::cout << port << std::endl;

  int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor == 0) {
    perror("Computer Glitch, please retry.");
    exit(EXIT_FAILURE);
  }

  struct hostent *server;
  server = gethostbyname(hostname);

  if (server == NULL) {
    perror("Computer Glitch, server not found, please retry.");
    exit(EXIT_FAILURE);
  }
  bzero((char *) &server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr,
       (char *)&server_address.sin_addr.s_addr,
       server->h_length);
  server_address.sin_port = htons(port);
  if (connect(socket_descriptor, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
    perror("Computer Glitch, please retry.");
    exit(EXIT_FAILURE);
  }

  while (true) {
    printf("Enter Message: ");
    bzero(buffer, 1024);
    fgets(buffer, 1023, stdin);
    int client_msg = write(socket_descriptor, buffer, strlen(buffer));
    if (client_msg < 0) {
      perror("Computer Glitch, please retry.");
      exit(EXIT_FAILURE);
    }
    bzero(buffer, 1024);
    int server_msg = read(socket_descriptor, buffer, 1023);
    if (server_msg < 0) {
      perror("Computer Glitch, please retry.");
      exit(EXIT_FAILURE);
    }
    printf("%s",buffer);
    return 0;
  }
}
