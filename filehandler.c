#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "headers.c"

#define DEFAULT_FILE "/index.html"

void handleFileRequest(http_header_t *header, const int conn) {
    if (strcmp(header->request_type, "GET")){
        printf("Request type: %s", header->request_type);
        send(conn, METHOD_NOT_ALLOWED_RESPONSE, strlen(METHOD_NOT_ALLOWED_RESPONSE), 0);
        return;
    }
    if (strlen(header->relative_url) > 1000){
        send (conn, URI_TOO_LONG_RESPONSE, strlen(URI_TOO_LONG_RESPONSE), 0);
        return;
    }
        
    char filepath[1024] = "pages";
    size_t length = 0;
    printf("%s\n", header->relative_url);

    FILE *file = fopen(strcat(filepath, header->relative_url), "r");
    if (file == NULL) {
        printf("file not found\n");
        length = strlen(NOTFOUNDRESPONSE);
        send(conn, NOTFOUNDRESPONSE, length, 0);
        return;
    } else {
        printf("opened file\n");
        kvll_t *headers = addHeader(NULL, "Content-Type", "text/html");
        printf("Added Headers\n");
        char *header_text = headerText(headers, &length);
        printf("%s\nLength: %zu\n", header_text, length);

        send(conn, OK_RESPONSE, OK_RESPONSE_LENGTH, 0);
        send(conn, header_text, length, 0);
        send(conn, "\r\n", 2, 0);

        char read;
        while ((read = fgetc(file)) != EOF) {
            send(conn, &read, 1, 0);
        }

    }
    fclose(file);
}
