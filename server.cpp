#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>

class ChatRoom {
  public:
    std::vector<int> sockets;

    void broadcast_message(char buffer[1024], int sender) {
      for (int i = 0; i < sockets.size(); ++i) {
        int client = sockets[i];
        if (client != sender) {
          send(client, buffer, strlen(buffer), 0);
        }
      }
    }
};

ChatRoom chat = ChatRoom();

void stream_messages(int socketdesc) {
  while (true) {
    char buffer[1024] = {0};
    int valread = read(socketdesc, buffer, 1024);
    printf("%s\n", buffer);
    chat.broadcast_message(buffer, socketdesc);
  }
}

int main(int argc, char** argv) {

  // Setup Port, Socket, Address.
  const char* sPort;
  struct sockaddr_in address;
  int port;
  if (argc < 2) {
    sPort = "5050";
  } else {
    sPort = *(argv + 1);
  }
  port = std::stoi(sPort);
  std::cout << port << std::endl;

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);
  int addrlen = sizeof(address);

  int socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_descriptor == 0) {
    perror("Computer Glitch, please retry.");
    exit(EXIT_FAILURE);
  }

  if (bind(socket_descriptor, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  if (listen(socket_descriptor, 3) < 0) {
    perror("Computer Glitch, please retry. -- listen");
    exit(EXIT_FAILURE);
  }

  std::vector<std::thread> connections;

  while (true) {
    int new_socket = accept(socket_descriptor, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    if (new_socket < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    chat.sockets.push_back(new_socket);

    std::thread stream(stream_messages, new_socket);
    connections.push_back(std::move(stream));
  }

  for (int i = 0; i < connections.size(); ++i) {
    connections[i].join();
  }
  std::cout << "goodbye" << std::endl;
  return 0;
}
