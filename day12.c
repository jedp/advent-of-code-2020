#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

typedef enum orientation {
  N,
  E,
  S,
  W
} orientation_t;

typedef struct opcode {
  char op;
  int value;
} opcode_t;

typedef struct coord {
  int ns;
  int ew;
} coord_t;

void vector_for(orientation_t orientation, coord_t *vector) {
  switch(orientation) {
    case N:
      vector->ns = 1;
      vector->ew = 0;
      break;
    case S:
      vector->ns = -1;
      vector->ew = 0;
      break;
    case E:
      vector->ns = 0;
      vector->ew = 1;
      break;
    case W:
      vector->ns = 0;
      vector->ew = -1;
      break;
    default:
      exit(-1);
  }
}

orientation_t rotate_by(orientation_t orientation, int degrees) {
  int i;
  switch(orientation) {
    case N:
      i = 0;
      break;
    case E:
      i = 1;
      break;
    case S:
      i = 2;
      break;
    case W:
      i = 3;
      break;
  }

  if (degrees % 90 != 0) {
    printf("Uh oh.\n");
    exit(-1);
  }

  return (orientation_t) ((orientation + (degrees / 90)) % 4);
}

void readInstructions(const char* filename,
                      opcode_t *opcodes,
                      int n_opcodes) {
  int i = 0;
  int value = 0;

  FILE *fp = fopen(filename, "r");
  assert(fp);

  char c;
  while((c = getc(fp)) != EOF) {

    switch(c) {
      case '\n':
        // End of line. Save value and prepare for next line.
        opcodes[i].value = value;
        value = 0;
        if (++i > n_opcodes) {
          printf("oh no! too many instructions to read. At %d of %d.\n", i, n_opcodes);
          exit(-1);
        }
        break;

      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        value = value * 10 + (c - '0');
        break;

      case 'N':
      case 'S':
      case 'E':
      case 'W':
      case 'L':
      case 'R':
      case 'F':
        opcodes[i].op = c;
        break;

      default:
        printf("wat: '%c'\n", c);
        exit(-1);
    }
  }
}

void execute_wrongly(const opcode_t *opcodes,
             int n_opcodes,
             coord_t *coords) {

  orientation_t dir = E;
  coord_t *vec = malloc(sizeof(coord_t));

  for (int i = 0; i < n_opcodes; ++i) {
    opcode_t opcode = opcodes[i];

    switch(opcode.op) {
      case 'N':
        coords->ns += opcode.value;
        break;
      case 'S':
        coords->ns -= opcode.value;
        break;
      case 'W':
        coords->ew -= opcode.value;
        break;
      case 'E':
        coords->ew += opcode.value;
        break;
      case 'L':
        dir = rotate_by(dir, -opcode.value);
        break;
      case 'R':
        dir = rotate_by(dir, opcode.value);
        break;
      case 'F':
        vector_for(dir, vec);
        coords->ns += vec->ns * opcode.value;
        coords->ew += vec->ew * opcode.value;
        break;
      default:
        printf("wat??\n");
        exit(-1);
    }

    printf("%c %d -> (%d, %d)\n", opcode.op, opcode.value, coords->ns, coords->ew);
  }
}

void rotate_about_left(coord_t *waypoint, int degrees) {
  int temp;
  for (int i = 0; i < (degrees / 90) % 4; ++i) {
    temp = waypoint->ns;
    waypoint->ns = waypoint->ew;
    waypoint->ew = -temp;
  }
}

void rotate_about_right(coord_t *waypoint, int degrees) {
  int temp;
  for (int i = 0; i < (degrees / 90) % 4; ++i) {
    temp = waypoint->ns;
    waypoint->ns = -waypoint->ew;
    waypoint->ew = temp;
  }
}

void execute_correctly(const opcode_t *opcodes,
                       int n_opcodes,
                       coord_t *waypoint,
                       coord_t *location) {

  for (int i = 0; i < n_opcodes; ++i) {
    opcode_t opcode = opcodes[i];

    switch(opcode.op) {
      // Action N means to move the waypoint north by the given value.
      case 'N':
        waypoint->ns += opcode.value;
        break;
      // Action S means to move the waypoint south by the given value.
      case 'S':
        waypoint->ns -= opcode.value;
        break;
      // Action E means to move the waypoint east by the given value.
      case 'E':
        waypoint->ew += opcode.value;
        break;
      // Action W means to move the waypoint west by the given value.
      case 'W':
        waypoint->ew -= opcode.value;
        break;
      // Action L means to rotate the waypoint around the ship left (counter-clockwise) the given number of degrees.
      case 'L':
        rotate_about_left(waypoint, opcode.value);
        break;
      // Action R means to rotate the waypoint around the ship right (clockwise) the given number of degrees.
      case 'R':
        rotate_about_right(waypoint, opcode.value);
        break;
      // Action F means to move forward to the waypoint a number of times equal to the given value.
      case 'F':
        location->ns += waypoint->ns * opcode.value;
        location->ew += waypoint->ew * opcode.value;
        break;
      default:
        printf("ugh.\n");
        exit(-1);
    }
  }
}

int manhattan_distance(coord_t* coord) {
  return abs(coord->ns) + abs(coord->ew);;;;
}

void test_read() {
  int test_len = 5;
  opcode_t *opcodes = malloc(sizeof(opcode_t) * test_len);
  readInstructions("day12_test_data.txt", opcodes, test_len);

  assert(opcodes[0].op == 'F');
  assert(opcodes[0].value == 10);

  assert(opcodes[1].op == 'N');
  assert(opcodes[1].value == 3);;

  assert(opcodes[4].op == 'F');
  assert(opcodes[4].value == 11);
}

void test_path() {
  int test_len = 5;
  opcode_t *opcodes = malloc(sizeof(opcode_t) * test_len);
  readInstructions("day12_test_data.txt", opcodes, test_len);

  coord_t *coords = malloc(sizeof(coord_t));
  coords->ns = 0;
  coords->ew = 0;

  execute_wrongly(opcodes, test_len, coords);

  assert(abs(coords->ns) == 8);
  assert(abs(coords->ew) == 17);
  assert(manhattan_distance(coords) == 25);
  printf("yay\n");
}

void test_rotate() {
  assert(rotate_by(N, 90) == E);
  assert(rotate_by(E, 90) == S);
  assert(rotate_by(S, 90) == W);
  assert(rotate_by(W, 90) == N);

  assert(rotate_by(E, -90) == N);
  assert(rotate_by(N, -90) == W);
}

void part1() {
  int n_opcodes = 769;
  opcode_t *opcodes = malloc(sizeof(opcode_t) * n_opcodes);
  readInstructions("day12_data.txt", opcodes, n_opcodes);

  coord_t *coords = malloc(sizeof(coord_t));
  coords->ns = 0;
  coords->ew = 0;

  execute_wrongly(opcodes, n_opcodes, coords);

  printf("Distance = %d\n", manhattan_distance(coords));

  free(opcodes);
}

void test_rotate_about_left() {
  coord_t *waypoint = malloc(sizeof(coord_t));
  waypoint->ew = 3;
  waypoint->ns = 2;

  rotate_about_left(waypoint, 90);
  assert(waypoint->ew == -2);
  assert(waypoint->ns == 3);

  rotate_about_left(waypoint, 90);
  assert(waypoint->ew == -3);
  assert(waypoint->ns == -2);

  rotate_about_left(waypoint, 90);
  assert(waypoint->ew == 2);
  assert(waypoint->ns == -3);

  rotate_about_left(waypoint, 90);
  assert(waypoint->ew == 3);
  assert(waypoint->ns == 2);
}

void test_rotate_about_right() {
  coord_t *waypoint = malloc(sizeof(coord_t));
  waypoint->ew = 3;
  waypoint->ns = 2;

  rotate_about_right(waypoint, 90);
  assert(waypoint->ew == 2);
  assert(waypoint->ns == -3);

  rotate_about_right(waypoint, 90);
  assert(waypoint->ew == -3);
  assert(waypoint->ns == -2);

  rotate_about_right(waypoint, 90);
  assert(waypoint->ew == -2);
  assert(waypoint->ns == 3);

  rotate_about_right(waypoint, 90);
  assert(waypoint->ew == 3);
  assert(waypoint->ns == 2);
}

void test_execute_correctly() {
  int n_opcodes = 5;
  opcode_t *opcodes = malloc(sizeof(opcode_t) * n_opcodes);
  readInstructions("day12_test_data.txt", opcodes, n_opcodes);

  coord_t *waypoint = malloc(sizeof(coord_t));
  waypoint->ew = 10;
  waypoint->ns = 1;

  coord_t *location = malloc(sizeof(coord_t));
  location->ew = 0;
  location->ns = 0;

  execute_correctly(opcodes, n_opcodes, waypoint, location);

  assert(manhattan_distance(location) == 286);
  printf("yay! executes correctly.\n");

  free(opcodes);
}

void part2() {
  int n_opcodes = 769;
  opcode_t *opcodes = malloc(sizeof(opcode_t) * n_opcodes);
  readInstructions("day12_data.txt", opcodes, n_opcodes);

  coord_t *waypoint = malloc(sizeof(coord_t));
  waypoint->ew = 10;
  waypoint->ns = 1;

  coord_t *coords = malloc(sizeof(coord_t));
  coords->ns = 0;
  coords->ew = 0;

  execute_correctly(opcodes, n_opcodes, waypoint, coords);

  printf("Distance = %d\n", manhattan_distance(coords));

  free(opcodes);
}

int main(int argc, char** argv) {
  test_read();
  test_rotate();
  test_path();

  part1();

  test_rotate_about_left();
  test_rotate_about_right();
  test_execute_correctly();

  part2();
}
