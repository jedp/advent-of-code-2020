#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <limits.h>

int ARR_SIZE = 2000;

int cmp(const void *e1, const void *e2) {
  int a = *((int*) e1);
  int b = *((int*) e2);

  if (a > b) return 1;
  if (b > a) return -1;
  return 0;
}

/**
 * Read the joltages file into a sorted array of joltages.
 *
 * Return number of elements read.
 */
int readJoltages(const char* filename,
                  int *joltages,
                  size_t joltages_size) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("fopen");
    return 0;
  }

  memset(joltages, -1, sizeof(int) * joltages_size);

  int acc = 0;
  int i = 0;
  char c;

  // joltage of the device we're connecting to.
  joltages[i++] = 0;

  while ((c = getc(fp)) != EOF) {
    if (c >= '0' && c <= '9') {
      acc *= 10;
      acc += c - '0';
    } else {
      joltages[i++] = acc;
      acc = 0;
    }
  }

  // Sort in place
  qsort(joltages, i, sizeof(*joltages), cmp);

  fclose(fp);
  return i;
}

void findDifferences(const int* joltages, int joltages_size,
                     int* results, int results_size) {
  int prev = 0;

  memset(results, 0, sizeof(int) * results_size);

  for (int i = 0; i < joltages_size; ++i) {
    results[joltages[i] - prev] += 1;
    prev = joltages[i];
  }

  // Our device is 3 jolts above the highest adapter.
  results[3] += 1;
}

/**
 * Return the number of valid charger chains.
 *
 * Assume joltages is a sorted list. If we are the last joltage in the list,
 * by definition we can connect to the device and there are no other
 * permutations, so in that case, we return 1.
 *
 * The first joltage should be 0, representing the outlet we're connecting to.
 *
 * The second-to-last joltage in the list can't connect to the device, which
 * is by definition three-jolts higher than the highest-joltage adapter.
 */
long validPermutations(const int* joltages,
                      int joltages_size,
                      int current_index) {

  if (current_index >= joltages_size - 1) {
    return 1;
  }

  long sum = 0L;

  for (int i = current_index + 1;
      joltages[i] - joltages[current_index] <= 3
      && i < joltages_size;
      ++i) {

    sum += validPermutations(joltages, joltages_size, i);
  }

  return sum;
}

long validPermutationsMemoized(const int* joltages,
                      long* memo,
                      int joltages_size,
                      int current_index) {

  if (current_index >= joltages_size - 1) {
    return 1;
  }

  long sum = 0L;

  for (int i = current_index + 1;
      joltages[i] - joltages[current_index] <= 3
      && i < joltages_size;
      ++i) {

    if (memo[i] != -1) {
      sum += memo[i];
    } else {
      sum += validPermutationsMemoized(joltages, memo, joltages_size, i);
    }
  }

  memo[current_index] = sum;
  return sum;
}

void test() {
  int test_arr_size = 50;
  int num_joltages;
  int *joltages = malloc(sizeof(int) * test_arr_size);

  num_joltages = readJoltages("day10_test_data.txt", joltages, test_arr_size);

  for (int i = 0; i < num_joltages; ++i) {
    printf("%d ", joltages[i]);
  }
  printf("\n");

  // Find the differences
  int differences_size = 4;
  int *differences = malloc(sizeof(int) * differences_size);
  findDifferences(joltages, num_joltages, differences, differences_size);

  for (int i = 1; i < differences_size; ++i) {
    printf("%d-jolt diffs = %d\n", i, differences[i]);
  }

  assert(differences[1] == 22);
  assert(differences[3] == 10);

  free(joltages);
  free(differences);
}

/**
 * Find a chain that uses all of your adapters to connect the charging outlet
 * to your device's built-in adapter and count the joltage differences between
 * the charging outlet, the adapters, and your device. What is the number of
 * 1-jolt differences multiplied by the number of 3-jolt differences?
 */
void run() {
  int test_arr_size = 400;
  int num_joltages;
  int *joltages = malloc(sizeof(int) * test_arr_size);

  num_joltages = readJoltages("day10_data.txt", joltages, test_arr_size);

  for (int i = 0; i < num_joltages; ++i) {
    printf("%d ", joltages[i]);
  }
  printf("\n");

  // Find the differences
  int differences_size = 4;
  int *differences = malloc(sizeof(int) * differences_size);
  findDifferences(joltages, num_joltages, differences, differences_size);

  for (int i = 1; i < differences_size; ++i) {
    printf("%d-jolt diffs = %d\n", i, differences[i]);
  }

  printf("1-jolt diffs x 3-jolt diffs = %d\n", differences[1] * differences[3]);
}

/**
 * There should be 19208 distinct arrangements of plugs in test_data.
 */
void test2_dumb_recursion() {
  int test_arr_size = 50;
  int num_joltages;
  int *joltages = malloc(sizeof(int) * test_arr_size);

  num_joltages = readJoltages("day10_test_data.txt", joltages, test_arr_size);

  // Include our device's joltage in the chain
  joltages[num_joltages] = joltages[num_joltages - 1] + 3;
  num_joltages++;

  int p = validPermutations(joltages, num_joltages, 0);
  printf("Test data: found %d valid chains\n", p);

  assert(p == 19208);
}

void test2_memoized() {
  int test_arr_size = 50;
  int num_joltages;
  int *joltages = malloc(sizeof(int) * test_arr_size);

  num_joltages = readJoltages("day10_test_data.txt", joltages, test_arr_size);

  // Include our device's joltage in the chain
  joltages[num_joltages] = joltages[num_joltages - 1] + 3;
  num_joltages++;

  long *memo = malloc(sizeof(long) * test_arr_size);
  memset(memo, -1, sizeof(long) * num_joltages);

  long p = validPermutationsMemoized(joltages, memo, num_joltages, 0);
  printf("Test data (now, with memoization): found %ld valid chains\n", p);

  assert(p == 19208L);
}

void run2() {
  int test_arr_size = 400;
  int num_joltages;
  int *joltages = malloc(sizeof(int) * test_arr_size);

  num_joltages = readJoltages("day10_data.txt", joltages, test_arr_size);

  // Include our device's joltage in the chain
  joltages[num_joltages] = joltages[num_joltages - 1] + 3;
  num_joltages++;

  long *memo = malloc(sizeof(long) * test_arr_size);
  memset(memo, -1, sizeof(long) * num_joltages);

  long p = validPermutationsMemoized(joltages, memo, num_joltages, 0);
  printf("Real data (now, with memoization): found %ld valid chains\n", p);
  printf("                  For sanity, LONG_MAX = %ld\n", LONG_MAX);
}

int main(int argc, char** argv) {
  test();

  run();

  test2_dumb_recursion();
  test2_memoized();

  run2();
}
