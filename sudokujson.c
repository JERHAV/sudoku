#include "sudoku.c"
#include <stdio.h>

char *encodeSudokuToJson(struct sudokuToSolve *sudoku) {
  char *writeBuffer = malloc(1024);
  char *returnPtr  = writeBuffer - 1;
  writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, "[");
  for (int i = 0; i < 9; i++) {
    if (i == 0) {
      writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, "[");
    } else {
      writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, ",[");
    }
    for (int j = 0; j < 9; j++) {
      if (j != 0) {
        writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, ",");
      }

      writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, "%d", sudoku->locations[i][j]->value);
    }

    writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, "]");
  }
  writeBuffer  = writeBuffer - 1 + sprintf(writeBuffer, "]");

  return returnPtr;
}
