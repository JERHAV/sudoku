#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCOLLECTIONSAMOUNT 36
#define DEFAULTCOLLECTIONAMOUNT 27

struct location {
  int value;
  uint16_t possibleValues;
};

typedef struct coordStruct {
  int i, j;
} coord;

struct location *NewLocation() {
  struct location *l = (struct location *)malloc(sizeof(struct location));
  l->value = 0;
  l->possibleValues = 0;
  for (int i = 0; i < 9; i++) {
    l->possibleValues |= (1 << i);
  }
  return l;
}

 int applyPossibleValues(struct location *l) {
  if (l->value != 0)
    return 0;

  for (int i = 0; i < 9; i++) {
    l->value += (l->possibleValues == (1 << i)) * (i + 1);
  }

  if (l->value != 0)
    return 1;
  return 0;
}

struct collection {
  struct location *locations[9];
};

 void calculateChanges(struct collection c) {
  for (int i = 0; i < 9; i++) {
    if (c.locations[i]->value != 0) {
      for (int j = 0; j < 9; j++) {
        c.locations[j]->possibleValues &= ~(1 << (c.locations[i]->value - 1));
      }
    }
  }
}

 int calculateOnlyPlaceInCollection(struct collection c, int value) {
  struct location *tbd = NULL;
  for (int i = 0; i < 9; i++) {
    if (c.locations[i]->value == value)
      return 0;

    if (c.locations[i]->value == 0 &&
        (c.locations[i]->possibleValues & (1 << (value - 1)))) {
      if (tbd != NULL)
        return 0;
      tbd = c.locations[i];
    }
  }

  if (tbd == NULL)
    return 0;
  tbd->value = value;
  return 1;
}

 int calculateOnlyPlacesInCollection(struct collection c) {
  int changed = 0;
  for (int i = 0; i < 9; i++) {
    changed += calculateOnlyPlaceInCollection(c, i + 1);
  }

  return changed;
}

struct sudokuToSolve {
  struct location *locations[9][9];
  struct collection *collections;
  int collectionSize;
  int allocatedSize;
};

 void printSudoku(struct location *locations[9][9]) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      printf("%d ", locations[i][j]->value);
    }
    printf("\n");
  }
  printf("------------------\n");
}

 void createNewLocations(struct sudokuToSolve *sudoku) {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      sudoku->locations[i][j] = NewLocation();
    }
  }
}

 void addDefaultCollections(struct sudokuToSolve *sudoku) { 
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      sudoku->collections[i].locations[j] = sudoku->locations[i][j];
      sudoku->collections[i + 9].locations[j] = sudoku->locations[j][i];
    }
  }
  printf("succesfully created line collections\n");
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      for (int k = 0; k < 3; k++) {
        for (int l = 0; l < 3; l++) {
          sudoku->collections[(i * 3) + j + 18].locations[k * 3 + l] =
              sudoku->locations[i * 3 + k][j * 3 + l];
        }
      }
    }
  }
}

struct sudokuToSolve *newDefaultSudoku() {
  struct sudokuToSolve *sudoku = malloc(sizeof(struct sudokuToSolve));
  createNewLocations(sudoku);
  sudoku->allocatedSize = MAXCOLLECTIONSAMOUNT * sizeof(struct collection);
  sudoku->collections = malloc(sudoku->allocatedSize);
  sudoku->collectionSize = DEFAULTCOLLECTIONAMOUNT;
  addDefaultCollections(sudoku);

  return sudoku;
}

 int setLocation(struct sudokuToSolve *sudoku, int value, coord c) {
  if (sudoku->locations[c.i][c.j]->possibleValues && ~(1 >> (value - 1))) {
    sudoku->locations[c.i][c.j]->value = value;
    return 1;
  }

  return 0;
}

 int calculateResult(struct sudokuToSolve *sudoku) {
  int changed;
  do {
    changed = 0;
    printSudoku(sudoku->locations);

    for (int i = 0; i < sudoku->collectionSize; i++) {
      calculateChanges(sudoku->collections[i]);
    }

    printf("Calculated changes\n");

    for (int i = 0; i < sudoku->collectionSize; i++) {
      changed += calculateOnlyPlacesInCollection(sudoku->collections[i]);
    }

    printf("Calculated only places\n");

    for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {
        changed += applyPossibleValues(sudoku->locations[i][j]);
      }
    }

    printf("Calculated possible values\n");

  } while (changed);
  int sum = 0;
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 9; j++) {
      sum += sudoku->locations[i][j]->value;
    }
  }

  return sum == 405;
}

 int addCollection(struct sudokuToSolve *sudoku, coord c[9]) {
  if (sudoku->collectionSize >= MAXCOLLECTIONSAMOUNT) return 0;

  for (int i; i < 9; i++) {
   sudoku->collections[sudoku->collectionSize].locations[i] = sudoku->locations[c[i].i][c[i].j];
  }

  sudoku->collectionSize++;

  return 1;
}
