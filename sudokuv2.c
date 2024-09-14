#ifndef SUDOKU
#define SUDOKU

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCOLLECTIONSAMOUNT 36

typedef struct location {
  uint8_t value;
  uint16_t impossibleValues;
} location_t;

typedef struct collection {
  location_t *locations[9];
  uint16_t filledValues;
} collection_t;

typedef struct sudoku {
  location_t locations[9][9];
  collection_t *collections;
  size_t num_collections;
} sudoku_t;

int containsLocation(collection_t c, location_t *l) {
  for (int i = 0; i < 9; i++) {
    if (c.locations[i] == l) {
      return 1;
    }
  }
  return 0;
}

void addDefaultCollections(sudoku_t *s) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      s->collections[i].locations[j] = &s->locations[i][j];
      s->collections[i + 9].locations[j] = &s->locations[j][i];
    }
  }

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
          s->collections[(i * 3) + j + 18].locations[k * 3 + l] = &s->locations[i * 3 + k][j * 3 + l];
        }
      }
    }
  }
  s->num_collections += 36;
}

sudoku_t *NewDefaultSudoku() {
  sudoku_t *s = malloc(sizeof(sudoku_t));
  s->collections = malloc(sizeof(collection_t) * MAXCOLLECTIONSAMOUNT);
  addDefaultCollections(s);

  return s;
}

void applyValuesToLocations(collection_t c) {
  for (int i = 0; i < 9; i++) {
    c.locations[i]->impossibleValues |= c.filledValues;
  }
}

void recalculateCollections(sudoku_t *s, location_t *l) {
  if (!l->value) {
    return;
  }
  for (int i = 0; i < s->num_collections; i++) {
    if (containsLocation(s->collections[i], l)) {
      s->collections[i].filledValues &= 1 << (l->value - 1);
      applyValuesToLocations(s->collections[i]);
    }
  }
}

int addValue(sudoku_t *s, uint8_t x, uint8_t y, uint8_t value) {
  if (s->locations[x][y].value != 0 || (s->locations[x][y].impossibleValues & (1 << (value - 1)))) {
    return 0;
  }

  s->locations[x][y].value = value;

  return 1;
}

void printSudoku(sudoku_t *s) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      printf("%d ", s->locations[i][j].value);
    }
    printf("\n");
  }
  printf("------------------\n");
}
#endif
