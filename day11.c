#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

typedef struct seating {
  int rows;
  int cols;
  char* last_state;
  char* state;
  char* next_state;
} seating_t;

typedef enum {
  NONE,
  FLOOR,
  AVAILABLE,
  OCCUPIED
} seat_t;

/**
 * Let's make our lives a little easier and know rows and cols ahead of time.
 */
void readSeatingChart(const char* filename,
                      seating_t* seating,
                      int rows,
                      int cols) {

  seating->rows = rows;
  seating->cols = cols;
  seating->last_state = malloc(sizeof(char) * rows * cols);
  seating->state = malloc(sizeof(char) * rows * cols);
  seating->next_state = malloc(sizeof(char) * rows * cols);

  int row = 0;
  int col = 0;
  char c;

  FILE *fp = fopen(filename, "r");
  assert(fp);

  while((c = getc(fp)) != EOF) {
    switch (c) {
      // Valid entries
      case 'L':  // Open seat
      case '.':  // Floor
      case '#':  // Occupied seat
        seating->state[row * cols + col] = c;
        col++;
        break;
      case '\n':
        row++;
        col = 0;
        break;
      default:
        printf("omg wat: %d, %d, %c\n", row, col, c);
        exit(-1);
    }
  }
}

void printSeatingChart(seating_t* seating) {
  for (int i = 0; i < seating->rows; ++i) {
    for (int j = 0; j < seating->cols; ++j) {
      printf("%c", seating->state[i * seating->cols + j]);
    }

    printf("\n");
  }
}

char seat_at(seating_t* seating, int row, int col) {
  return seating->state[row * seating->cols + col];
}

int neighbors(seating_t* seating, int row, int col) {
  int k = 0;
  int others[8][2] = {
    { row - 1, col - 1 },
    { row - 1, col     },
    { row - 1, col + 1 },
    { row    , col - 1 },
    { row    , col + 1 },
    { row + 1, col - 1 },
    { row + 1, col     },
    { row + 1, col + 1 }
  };

  int* other;
  for (int i = 0; i < 8; ++i) {
    other = others[i];
    if (other[0] >= 0 && other[0] < seating->rows &&
        other[1] >= 0 && other[1] < seating->cols) {
      if (seating->state[other[0] * seating->cols + other[1]] == '#') {
        k++;
      }
    }

  }

  return k;
}


/**
 * - If a seat is empty (L) and there are no occupied seats adjacent to it, the
 *   seat becomes occupied.
 *
 * - If a seat is occupied (#) and four or more seats adjacent to it are also
 *   occupied, the seat becomes empty.
 *
 * - Otherwise, the seat's state does not change.
 *
 * Return true if position is same as last position.
 */
bool tick(seating_t *seating) {
  for (int i = 0; i < seating->rows; ++i) {
    for (int j = 0; j < seating->cols; ++j) {
      int pos = i * seating->cols + j;
      switch(seating->state[pos]) {
        case 'L':
          seating->next_state[pos] = (neighbors(seating, i, j) == 0) ? '#' : 'L';
          break;
        case '#':
          if (neighbors(seating, i, j) >= 4) {
            // Becomes vacated.
            seating->next_state[pos] = 'L';
          } else {
            // Remains occupied.
            seating->next_state[pos] = '#';
          }
          break;
        default:
          seating->next_state[pos] = seating->state[pos];
          break;
      }
    }
  }

  // Swaperoo.
  char *temp = seating->last_state;
  seating->last_state = seating->state;
  seating->state = seating->next_state;
  seating->next_state = temp;

  printSeatingChart(seating);
  printf("\n");

  for (int i = 0; i < seating->rows * seating->cols; ++i) {
    if (seating->last_state[i] != seating->state[i]) {
      return false;
    }
  }
  printf("converged!\n");
  return true;
}

int occupied_seats(seating_t* seating) {
  int i = 0;
  for (int k = 0; k < seating->rows * seating->cols; ++k) {
    if (seating->state[k] == '#') {
      ++i;
    }
  }
  return i;
}

void test_read() {
  seating_t* seating = malloc(sizeof(seating_t));
  readSeatingChart("day11_test_data.txt", seating, 10, 10);
  printSeatingChart(seating);
  // Free? :)
}

void test_tick() {
  seating_t* seating = malloc(sizeof(seating_t));
  readSeatingChart("day11_test_data.txt", seating, 10, 10);

  int i = 0;
  while(!tick(seating)) ++i;

  printf("%d iterations\n", i);

  assert(occupied_seats(seating) == 37);
}


void part1() {
  seating_t* seating = malloc(sizeof(seating_t));
  readSeatingChart("day11_data.txt", seating, 97, 92);

  int i = 0;
  while(!tick(seating)) ++i ;

  printf("%d iterations\n", i);
  printf("%d occupied seats\n", occupied_seats(seating));
}

int main (int argc, char** argv) {
  test_read();
  test_tick();

  part1();
}


