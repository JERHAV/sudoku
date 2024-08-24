#include "setupsocket.c"
#include "sudoku.c"
#include "requesthandling.c"
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
  struct sockaddr_in6 address = {
      .sin6_addr = in6addr_any,
      .sin6_family = AF_INET6,
      .sin6_port = htons(8080),
  };
  socklen_t address_len = sizeof(address);
  int sock = setupIP6Socket((struct sockaddr *)&address, address_len);

  struct sudokuToSolve *sudoku = newDefaultSudoku();
  setLocation(sudoku, 1, (coord){1, 1});

  printf(calculateResult(sudoku) ? "succes" : "failure");
  
  handleIncommingRequests(sock, (struct sockaddr *)&address, &address_len);
  close(sock);
  return 0;
}

// TODO: Add enpoints for adding numbers and returning ok if succesfull
// TODO: Add endpoint to request calculation and return endresult (and if succesfull)
// TODO: Add endpoint to add collection
// TODO: Create webpage that interacts with endpoint (maybe with HTMX)
