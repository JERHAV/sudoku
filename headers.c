#ifndef HTML_HEADERS_DEFINED
#define HTML_HEADERS_DEFINED
#include <bits/types/struct_iovec.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define NOTFOUNDRESPONSE "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\n404 Not Found"
#define METHOD_NOT_ALLOWED_RESPONSE "HTTP/1.1 405 Method Not Allowed\r\nContent-Type: textplain\r\n\r\n405 Method Not Allowed"
#define URI_TOO_LONG_RESPONSE "HTTP/1.1 414 URI Too Long\r\nContent-Type: textplain\r\n\r\n414 URI Too Long"

#define BAD_REQUEST_RESPONSE_HEADER "HTTP/1.1 400 BAD RESPONSE\r\\nCOntent-Type: text/plain\r\n\r\n"

#define OK_RESPONSE "HTTP/1.1 200 OK\r\n"
#define OK_RESPONSE_LENGTH 17
#define OK_RESPONSE_TEXT_HEADER OK_RESPONSE"Content-Type: textplain\r\n\r\n"

typedef struct kvll {
  char *key;
  char *value;
  struct kvll *next;
} kvll_t;

typedef struct http_header {
  char *request_type;
  char *relative_url;
  char *body;
  kvll_t *headers;
} http_header_t;

kvll_t *addHeader(kvll_t *ll, char *key, char *value) {
  if (ll == NULL) {
    ll = (kvll_t *)malloc(sizeof(kvll_t));
    ll->key = (char *)memmove(malloc(strlen(key) + 1), key, strlen(key));
    ll->value = (char *)memmove(malloc(strlen(value) + 1), value, strlen(value));
    return ll;
  } else {
    ll->next = addHeader(ll->next, key, value);
    ll->next = NULL;
    return ll;
  }
}

char *retrieveValue(char *key, kvll_t *kvll) {
  if (kvll == NULL) {
    return NULL;
  }
  if (strcmp(key, kvll->key)) {
    return kvll->value;
  }

  return retrieveValue(key, kvll->next);
}

char *_headerText(kvll_t *ll, char *buffer, size_t allocatedLength, size_t *length) {
  if (ll == NULL) {
    return buffer;
  }

  size_t newLength = *length + strlen(ll->value) + strlen(ll->key) + 4;
  if (newLength > allocatedLength) {
    allocatedLength = allocatedLength * 2;
    buffer = (char *)realloc(buffer, allocatedLength);
  }

  sprintf(&buffer[*length], "%s: %s\r\n", ll->key, ll->value);
  *length = newLength;
  return _headerText(ll->next, buffer, allocatedLength, length);
}

char *headerText(kvll_t *ll, size_t *length) {
  char *buffer = (char *)malloc(4096);
  return _headerText(ll, buffer, 4096, length);
}

void printAll(kvll_t *ll) {
  size_t length = 0;
  printf("%s", headerText(ll, &length));
}

void destruct(kvll_t *ll) {
  if (ll->next != NULL) {
    destruct(ll->next);
  }
  ll->key = NULL;
  free(ll->key);
  ll->value = NULL;
  free(ll->value);
  ll->next = NULL;
  free(ll);
}

void freeheader(http_header_t *header) {
  destruct(header->headers);
  free(header->request_type);
  free(header->relative_url);
  free(header);
}

kvll_t *GetHeaders(char *request, int length) {
  kvll_t *firstHeader = (kvll_t *)malloc(sizeof(&firstHeader));
  kvll_t *editingHeader = firstHeader;
  int i = 0;

  // Start at second line;
  while (request[i] != '\n' && request[i] != '\0' && i < length) {
    i++;
  }
  i++;

  while (request[i] != '\0' && i < length) {
    int j;
    for (j = 0; request[i] != '\0' && request[i] != ':'; j++) {
      i++;
    }
    editingHeader->key = (char *)memmove(malloc(j + 1), &request[i - j], j);
    editingHeader->key[j] = '\0';

    i += 2;
    for (j = 0; request[i] != '\0' && request[i] != '\r'; j++) {
      i++;
    }
    editingHeader->value = (char *)memmove(malloc(j + 1), &request[i - j], j);
    editingHeader->value[j] = '\0';

    if (request[i + 2] == '\r' || request[i + 1] == '\0') {
      editingHeader->next = NULL;
      return firstHeader;
    }

    editingHeader->next = (kvll_t *)malloc(sizeof(&editingHeader));
    editingHeader = editingHeader->next;
    i += 2;
  }
  return firstHeader;
}

char *getRelativeUrl(char *request, int length) {
  int i = 0, j = 0;
  while (request[i] != ' ' && i < length) {
    i++;
  }
  i++;
  while (request[i] != ' ' && i < length) {
    i++;
    j++;
  }
  char *url = (char *)memmove(malloc(j + 1), &request[i - j], j);
  url[j] = '\0';

  return url;
}

char *getProtocol(char *request, int length) {
  char *protocol;
  for (int i = 0; i < length && i < 7; i++) {
    if (request[i] == ' ') {
      protocol = (char *)memmove(malloc(i + 1), request, i);
      protocol[i] = '\0';
      return protocol;
    }
  }

  return NULL;
}
char *getBody(char *data, int length) {
  int i = 0;
  while (i < length - 3 && data[3] != '\0') {
    if (data[i] == '\n' && data[i + 2] == '\n') {
      return &data[i + 3];
    }

    i++;
  }
    return NULL;
}

http_header_t *decodeHTTPHeader(char *data, int length) {
  http_header_t *header = (http_header_t *)malloc(sizeof(http_header_t));
  header->headers = GetHeaders(data, length);
  printf("Got Headers\n");

  header->relative_url = getRelativeUrl(data, length);
  printf("Got URL \n");
  header->request_type = getProtocol(data, length);
  printf("Got Protocol\n");

  if (strcmp(header->request_type, "GET")) {
    header->body = getBody(data, length);
    printf("\n%s\n\n", header->body);
  }

  return header;
}

#endif
