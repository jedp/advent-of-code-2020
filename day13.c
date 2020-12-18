#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

typedef struct bus {
  int id;
  long best_time;
} bus_t;

int count_buses(const char* bus_input) {
  int i = 0;
  int n_buses = 0;
  while(bus_input[i] != 0) {
    if (bus_input[i++] == ',') {
      n_buses += 1;
    }
  }
  // There's one more bus after the final comma.
  return n_buses + 1;
}

void parse_bus_list(const char* bus_input, bus_t *buses) {
  char c;
  int acc = 0;
  int n = 0;
  int i = 0;
  while ((c = bus_input[i++]) != 0) {
    switch (c) {
      case 'x':
        acc = -1;
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
        acc = acc * 10 + (c - '0');
        break;
      case ',':
        buses[n++].id = acc;
        acc = 0;
        break;
      default:
        printf("woe!\n");
        exit(-1);
    }
  }
  // Last one after the final comma
  buses[n].id = acc;
}

void find_bus(long earliest_departure_time,
         bus_t buses[],
         int n_buses,
         bus_t *best_bus) {

  best_bus->id = -1;
  best_bus->best_time = LONG_MAX;

  for (int i = 0; i < n_buses; ++i) {

    if (buses[i].id != -1) {
      /*
       * The time this loop takes a particular bus is also its ID number: the
       * bus with ID 5 departs from the sea port at timestamps 0, 5, 10, 15, and
       * so on. The bus with ID 11 departs at 0, 11, 22, 33, and so on. If you
       * are there when the bus departs, you can ride that bus to the airport!
       */
      long time = (earliest_departure_time / buses[i].id) * buses[i].id;
      if (time < earliest_departure_time) {
        time += buses[i].id;
      }

      if (time < best_bus->best_time) {
        best_bus->id = buses[i].id;
        best_bus->best_time = time;
      }
    }
  }
}

void test() {
  long earliest_departure_time = 939;
  char *bus_input = "7,13,x,x,59,x,31,19";
  int n_buses = count_buses(bus_input);
  bus_t *buses = malloc(sizeof(bus_t) * n_buses);
  assert(n_buses == 8);

  parse_bus_list(bus_input, buses);
  for (int i = 0; i < n_buses; ++i) {
    printf("buses[%d] = %d\n", i, buses[i].id);
  }
  bus_t *best_bus = malloc(sizeof(bus_t));
  find_bus(earliest_departure_time, buses, n_buses, best_bus);

  printf("Best time: bus %d, time=%ld\n", best_bus->id, best_bus->best_time);

  assert(best_bus->id == 59);
  assert(best_bus->best_time == 944);

  printf("wait x bus id = %ld\n", best_bus->id * (best_bus->best_time - earliest_departure_time));
}

void part1() {
  long earliest_departure_time = 1009310L;
  char* bus_input = "19,x,x,x,x,x,x,x,x,x,x,x,x,37,x,x,x,x,x,599,x,29,x,x,x,x,x,x,x,x,x,x,x,x,x,x,17,x,x,x,x,x,23,x,x,x,x,x,x,x,761,x,x,x,x,x,x,x,x,x,41,x,x,13";

  int n_buses = count_buses(bus_input);
  bus_t *buses = malloc(sizeof(bus_t) * n_buses);

  parse_bus_list(bus_input, buses);
  for (int i = 0; i < n_buses; ++i) {
    printf("buses[%d] = %d\n", i, buses[i].id);
  }
  bus_t *best_bus = malloc(sizeof(bus_t));
  find_bus(earliest_departure_time, buses, n_buses, best_bus);

  printf("Best time: bus %d, time=%ld\n", best_bus->id, best_bus->best_time);
  printf("wait x bus id = %ld\n", best_bus->id * (best_bus->best_time - earliest_departure_time));
}

int main(int argc, char** argv) {
  test();

  part1();

  return 0;
}
