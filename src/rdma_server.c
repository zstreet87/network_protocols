#include "rdma_server.h"
#include <infiniband/verbs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int run_rdma_server() {
  // 1. get list of rdma devices
  struct ibv_device** dev_list = ibv_get_device_list(NULL);
  if (!dev_list) {
    perror("ibv_get_device_list");
    return -1;
  }

  // 2. open first device
  struct ibv_context* context = ibv_open_device(dev_list[0]);
  if (!context) {
    fprintf(stderr, "Could not open RDMA device.\n");
    ibv_free_device_list(dev_list);
    return -1;
  }

  // 3. allocate protection domain
  struct ibv_pd* pd = ibv_alloc_pd(context);
  if (!pd) {
    fprintf(stderr, "Could not allocate Protection Domain.\n");
    goto clean_context;
  }

  // 4. create completion queue
  struct ibv_cq* cq = ibv_create_cq(context, 16, NULL, NULL, 0);
  if (!cq) {
    fprintf(stderr, "Could not create CQ.\n");
    goto clean_pd;
  }

  // 5. create queue pair
  struct ibv_qp_init_attr qp_init_attr = {
      .send_cq = cq,
      .recv_cq = cq,
      .cap = {.max_send_wr = 16,
              .max_recv_wr = 16,
              .max_send_sge = 1,
              .max_recv_sge = 1},
      .qp_type = IBV_QPT_RC,
  };

  struct ibv_qp* qp = ibv_create_qp(pd, &qp_init_attr);
  if (!qp) {
    fprintf(stderr, "Could not create QP.\n");
    goto clean_cq;
  }

  // Normally we'd exchange QP info (QP number, LID, etc.) with the client here.
  // For simplicity, let's assume we have it or we do a local loopback.

  // 6. Transition QP to INIT -> RTR -> RTS
  // This requires real LID?GID data. We'll omit for brevity.

  // 7. Register Memory
  char* buf = calloc(1, BUFFER_SIZE);
  snprintf(buf, BUFFER_SIZE, "Hello from RDMA Server!");
  struct ibv_mr* mr =
      ibv_reg_mr(pd, buf, BUFFER_SIZE,
                 IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE |
                     IBV_ACCESS_REMOTE_READ);
  if (!mr) {
    fprintf(stderr, "Could not reqister MR.\n");
    goto clean_qp;
  }

  printf("RDMA server set up. Memory registered. QP created.\n");
  printf("Message in server buffer: %s\n", buf);

  // In a real-world scenario, you'd post recieve bufferes, wait for client to write or read,
  // then pool completion queue, etc.

  // 8. cleanup
  ibv_dereg_mr(mr);
  free(buf);

clean_qp:
  ibv_destroy_qp(qp);
clean_cq:
  ibv_destroy_cq(cq);
clean_pd:
  ibv_dealloc_pd(pd);
clean_context:
  ibv_close_device(context);
  ibv_free_device_list(dev_list);

  return 0;
}
