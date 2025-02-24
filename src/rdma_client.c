#include "rdma_client.h"
#include <infiniband/verbs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int run_rdma_client() {
  struct ibv_device** dev_list = ibv_get_device_list(NULL);
  if (!dev_list) {
    perror("ibv_get_device_list");
    return -1;
  }

  struct ibv_context* context = ibv_open_device(dev_list[0]);
  if (!context) {
    fprintf(stderr, "Could not open RDMA device.\n");
    ibv_free_device_list(dev_list);
    return -1;
  }

  struct ibv_pd* pd = ibv_alloc_pd(context);
  if (!pd) {
    fprintf(stderr, "Could not allocate PD.\n");
    goto clean_context;
  }

  struct ibv_cq* cq = ibv_create_cq(context, 16, NULL, NULL, 0);
  if (!cq) {
    fprintf(stderr, "Could not create CQ.\n");
    goto clean_pd;
  }

  struct ibv_qp_init_attr qp_init_attr = {
      .send_cq = cq,
      .recv_cq = cq,
      .cap =
          {
              .max_send_wr = 16,
              .max_recv_wr = 16,
              .max_send_sge = 1,
              .max_recv_sge = 1,
          },
      .qp_type = IBV_QPT_RC,
  };
  struct ibv_qp* qp = ibv_create_qp(pd, &qp_init_attr);
  if (!qp) {
    fprintf(stderr, "Could not create QP.\n");
    goto clean_cq;
  }

  // Exchane QP info with server (outside scope).
  // Transition QP to INIT -> RTR -> RTS (requires server's QP info).

  //
  char* buf = calloc(1, BUFFER_SIZE);
  snprintf(buf, BUFFER_SIZE, "Hello from RDMA Client!");
  struct ibv_mr* mr =
      ibv_reg_mr(pd, buf, BUFFER_SIZE,
                 IBV_ACCESS_LOCAL_WRITE | IBV_ACCESS_REMOTE_WRITE |
                     IBV_ACCESS_REMOTE_READ);
  if (!mr) {
    fprintf(stderr, "Could not register MR.\n");
    goto clean_qp;
  }

  printf("RDMA client set up. Local buffer: %s\n", buf);

  // In a real example, we'd post an RDMA Write/Read or Send/Recv to the server's buffer,
  // then pool for completions.

  // cleanup
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
