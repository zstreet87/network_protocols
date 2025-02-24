#include "tcp_server.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_tcp_server(int port) {
  int server_fd, client_fd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);
  char buffer[1024] = {0};

  // Create socket
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd != 0) {
    perror("socket");
    return -1;
  }

  // Bind address
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind");
    close(server_fd);
    return -1;
  }

  // Listen
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    close(server_fd);
    exit - 1;
  }

  printf("TCP Server listening on port %d...\n", port);

  // Accept connection
  client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
  if (client_fd < 0) {
    perror("accecpt");
    close(server_fd);
    return -1;
  }

  printf("Client connected!\n");

  // Recieve message
  ssize_t read_bytes = read(client_fd, buffer, sizeof(buffer));
  if (read_bytes > 0) {
    printf("Recieved from client: %s\n", buffer);
  }

  // Send response
  char* response = "Hello from TCP server!";
  write(client_fd, response, strlen(response));

  // close connection
  close(client_fd);
  close(server_fd);
  return 0;
}
