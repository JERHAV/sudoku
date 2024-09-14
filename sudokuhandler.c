#include "headers.c"
#include "sudokuv2.c"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define UNABLE_TO_DECODE_SUDOKU_RESPONSE BAD_REQUEST_RESPONSE_HEADER "Unable to read sudoku request"
#define INCORRECTVALUE_TEMPLATE OK_RESPONSE_TEXT_HEADER "Incorrect value: %d for %d,%d"

sudoku_t *decodeSudokuRequest(char *requestBody, const int conn) {
  uint8_t state = 0;
  char incorrectValueError[strlen(INCORRECTVALUE_TEMPLATE) + 3];
  int i = 0;
  uint8_t x[2];
  uint8_t value;
  sudoku_t *s = NewDefaultSudoku();
  while (requestBody[i] != '\0') {
    switch (state) {
    case 0:
    case 1:
      if (requestBody[i] < '0' || requestBody[i] > '9') {
        goto errorResponse;
      }
      state++;
      x[state-1] = requestBody[i] - '0';
      i++;
      break;
    case 2:
      if (requestBody[i] != '=') {
        goto errorResponse;
      }
      i++;
      state++;
      break;
    case 3:
      if (requestBody[i] == '&') {
        state = 0;
        i++;
      } else if (requestBody[i] < '1' || requestBody[i] > '9') {
        goto errorResponse;
      } else {
        value = requestBody[i] - '0';
        if (!addValue(s, x[0], x[1], value)) {
          goto incorrectvalue;
        }
        i++;
        state++;
      }
      break;
    case 4:
      i++;
      state = 0;
      break;
    default:
      goto errorResponse;
    }
  }

  return s;
errorResponse:
  send(conn, UNABLE_TO_DECODE_SUDOKU_RESPONSE, strlen(UNABLE_TO_DECODE_SUDOKU_RESPONSE), 0);
  free(s);
  return NULL;
incorrectvalue:
  sprintf(incorrectValueError, INCORRECTVALUE_TEMPLATE, value, x[0], x[1]);
  send(conn, &incorrectValueError, strlen(incorrectValueError), 0);
  return s;
}

void handleSendSudoku(http_header_t *header, const int conn) {
  if (strcmp(header->request_type, "POST") != 0) {
    send(conn, METHOD_NOT_ALLOWED_RESPONSE, strlen(METHOD_NOT_ALLOWED_RESPONSE), 0);
    return;
  }
  if (strlen(header->body) == 0) {
    send(conn, UNABLE_TO_DECODE_SUDOKU_RESPONSE, strlen(UNABLE_TO_DECODE_SUDOKU_RESPONSE), 0);
    return;
  }

  sudoku_t *s = decodeSudokuRequest(header->body, conn);
  if (s == NULL) {
    return;
  }

  printSudoku(s);
}
