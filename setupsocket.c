#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int setupIP6Socket(struct sockaddr *address, socklen_t address_length) {
  int sock = socket(address->sa_family, SOCK_STREAM, 0);
  int opt = 1;

  if (sock < 0) {
    printf("Creating ipv6 socket, errno: %s\n", strerror(errno));
    exit(errno);
  }

  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

  while (bind(sock, address, address_length) < 0) {
    printf("Binding to socket: %s\n", strerror(errno));
    sleep(5);
  }

  printf("bound\n");
  if (listen(sock, 5) < 0) {
    perror("unable to listen");
    exit(errno);
  }

  return sock;
}
