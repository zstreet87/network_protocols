#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rdma_client.h"
#include "rdma_server.h"
#include "tcp_client.h"
#include "tcp_server.h"

void print_usage() {
  printf("Usage:\n");
  printf(" main tcp_server <port>\n");
  printf(" main tcp_client <server_ip> <port>\n");
  printf(" main rdma_server\n");
  printf(" main rdma_client\n");
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    print_usage();
    return EXIT_FAILURE;
  }

  if (strcmp(argv[1], "tcp_server") == 0) {
    if (argc != 3) {
      printf("Error: Missing port for TCP server.\n");
      print_usage();
      return EXIT_FAILURE;
    }
    int port = atoi(argv[2]);
    printf("Starting TCP server on port %d...\n", port);
    return run_tcp_server(port);
  }

  if (strcmp(argv[1], "tcp_client") == 0) {
    if (argc != 4) {
      printf("Error: Missing IP and port for TCP client.\n");
      print_usage();
      return EXIT_FAILURE;
    }
    const char* server_ip = argv[2];
    int port = atoi(argv[3]);
    printf("Starting TCP client to connect to %s:%d...\n", server_ip, port);
    return run_tcp_client(server_ip, port);
  }

  if (strcmp(argv[1], "rdma_server") == 0) {
    printf("Starting RDMA server...\n");
    return run_rdma_server();
  }

  if (strcmp(argv[1], "rdma_client") == 0) {
    printf("Starting RDMA client...\n");
    return run_rdma_client();
  }

  printf("Error: Unkown protocol '%s'\n", argv[1]);
  print_usage();
  return EXIT_FAILURE;
}
