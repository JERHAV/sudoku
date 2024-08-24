#include <errno.h>
#include <string.h>
#include "headers.c"
#include "filehandler.c"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>


void handleIncommingRequests(int sock, struct sockaddr *address, socklen_t * address_len) {
    int conn;
    int readSize;
    char *charBuffer = malloc(8141);
    while ((conn = accept(sock, address, address_len)) > -1) {
        printf("Got connection\n");
        readSize = read(conn, charBuffer, 8141);
        if (readSize < 0) {
            printf(strerror(errno));
            exit(errno);
        }
        if (readSize == 8141){
            charBuffer[8141] = '\0';
        }
        printf("Read\n");
        
        http_header_t *header = decodeHTTPHeader(charBuffer, readSize);
        printf("Url: %s\nType: %s\n", header->relative_url, header->request_type);
        printAll(header->headers);
        if (strcmp(header->relative_url, "/")) {
            handleFileRequest(header, conn);
        } else {
            send(conn, NOTFOUNDRESPONSE, strlen(NOTFOUNDRESPONSE), 0);\
        }
        close(conn);
        freeheader(header);
    }
    perror("Unable to accept connection\n");
}
