#include "sudoku.c"
#include <sys/socket.h>

void listenForSudoku(int sock, struct sockaddr *address, socklen_t address_len) {
    int connection;
    while ((connection = accept(sock, address, &address_len){
        readall()
    }
}
