#include "filehandler.c"
#include "headers.c"
#include "sudokuhandler.c"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void handleIncomingRequests(int sock, struct sockaddr *address, socklen_t *address_len) {
  int conn;
  int readSize;
  char *charBuffer = malloc(8141);
  while ((conn = accept(sock, address, address_len)) > -1) {
    printf("Got connection\n");
    readSize = read(conn, charBuffer, 8141);
    if (readSize < 0) {
      exit(errno);
    }
    if (readSize == 8141) {
      charBuffer[8141] = '\0';
    }

    printf("\n\n%s\n\n", charBuffer);
    http_header_t *header = decodeHTTPHeader(charBuffer, readSize);
    printf("Url: %s\nType: %s\n", header->relative_url, header->request_type);
    printAll(header->headers);
    if (strcmp(header->relative_url, "/") == 0) {
      handleFileRequest(header, conn);
    } else if (strcmp(header->relative_url, "/sendsudoku") == 0) {
      handleSendSudoku(header, conn);
    } else if (strstr(header->relative_url, "/test.html")) {
      handleFileRequest(header, conn);
    } else {
      send(conn, NOTFOUNDRESPONSE, strlen(NOTFOUNDRESPONSE), 0);
    }
    close(conn);
    freeheader(header);
  }
  perror("Unable to accept connection\n");
}
