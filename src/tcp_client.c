#include "tcp_client.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int run_tcp_client(const char* server_ip, int port) {
  int sock;
  struct sockaddr_in server_addr;
  char buffer[1024];

  // create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    return -1;
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
    perror("inet_pton");
    close(sock);
    return -1;
  }

  //connect
  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("connect");
    close(sock);
    return -1;
  }
  printf("Connected to server %s:%d\n", server_ip, port);

  //send message
  char* message = "Hello from TCP client!";
  write(sock, message, strlen(message));

  //receive response
  ssize_t received = read(sock, buffer, sizeof(buffer) - 1);
  if (received > 0) {
    buffer[received] = '\0';
    printf("Received from server: %s\n", buffer);
  }

  close(sock);
  return 0;
}
