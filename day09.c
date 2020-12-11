#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>

// there are 1001 elements in the longest input
const int ARR_SIZE = 2000;

/**
 * Read sample data files.
 * Convert them to an array of long values.
 */
int fileToNumbers(const char *filename, long numbers[]) {

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    perror("fopen");
    return -1;
  }

  // Clear and init to sentinel value.
  for (int i = 0; i < ARR_SIZE; ++i) {
    numbers[i] = -1;
  }

  long acc = 0L;
  int digit = 0;

  for (int i = 0; ; ++i) {
    char curr = getc(fp);

    if (curr >= '0' && curr <= '9') {
      // Reading a digit.
      acc *= 10;
      acc += curr - '0';
    } else {
      // Return length of array when we are done reading.
      if (curr == EOF) {
        fclose(fp);
        return digit;
      }

      // Done reading a number.
      numbers[digit++] = acc;
      acc = 0L;
    }

  }
}

/**
 * Return index of first bad number; -1 if all are ok.
 */
int firstBadNumber(long numbers[], int num_numbers, int preamble_length) {
  // Bounds checking?

  int index = -1;
  int found = -1;
  // Iterate through each digit after the preamble.
  for (int i = preamble_length; i < num_numbers; ++i) {

    // Iterate through all combinations of numbers in the previous window.
    for (int j = i - preamble_length; j < i - 1; ++j) {
      for (int k = j + 1; k < i; ++k) {
        if (numbers[i] == numbers[j] + numbers[k]) {
          printf("ok: %ld = %ld + %ld\n", numbers[i], numbers[j], numbers[k]);
          found = i;
          goto found;
        }
      }
    }

found:
    if (found == -1) {
      return i;
    }

    found = -1;
  }

  return index;
}

/**
 * Find a contiguous set of at least two numbers that sum to the target
 * number in the array of numbers.
 *
 * If such a set is found, return the lower and upper indices. If nothing
 * found, return -1, -1.
 */
void seriesSummingToTarget(const long numbers[],
                           const int num_numbers,
                           const long target,
                           int endpoints[]) {
  int start = 0;
  long max_so_far;

  // Iterate through all the numbers until we find the sequence.
  for (int i = 0; i < num_numbers - 1; ++i) {
    max_so_far = numbers[i];
    for (int end = i + 1; i < num_numbers && max_so_far <= target; ++end) {
      max_so_far += numbers[end];
      if (max_so_far == target) {
        endpoints[0] = i;
        endpoints[1] = end;
        break;
      }
    }
  }
}

long smallestInRange(const long numbers[],
                     const int start,
                     const int end) {
  long least_so_far = LONG_MAX;

  for (int i = start; i <= end; ++i) {
    if (numbers[i] < least_so_far) {
      least_so_far = numbers[i];
    }
  }

  return least_so_far;
}

long largestInRange(const long numbers[],
                    const int start,
                    const int end) {
  long max_so_far = 0L;

  for (int i = start; i <= end; ++i) {
    if (numbers[i] > max_so_far) {
      max_so_far = numbers[i];
    }
  }

  return max_so_far;
}

long revealFirstBadNumber(const char* filename, int preamble_length) {
  long *numbers;
  numbers = (long*) malloc(sizeof(long) * ARR_SIZE);

  int num_numbers = fileToNumbers(filename, numbers);
  printf("value: %d numbers; first is %ld, last is %ld\n", num_numbers, numbers[0], numbers[num_numbers-1]);

  int index = firstBadNumber(numbers, num_numbers, preamble_length);
  long bad = numbers[index];
  printf("bad number: %ld\n", bad);

  int endpoints[2] = { -1, -1 };
  seriesSummingToTarget(numbers, num_numbers, bad, endpoints);
  printf("Special series bounds: %d, %d\n", endpoints[0], endpoints[1]);
  for (int i = endpoints[0]; i <= endpoints[1]; ++i) {
    printf("    %ld\n", numbers[i]);
  }

  long smallest = smallestInRange(numbers, endpoints[0], endpoints[1]);
  long largest = largestInRange(numbers, endpoints[0], endpoints[1]);
  printf("XXX Encryption Weakness XXX  = %ld + %ld = %ld\n", smallest, largest, smallest + largest);

  free(numbers);

  return bad;
}

void test() {
  long bad = revealFirstBadNumber("day09_test_data.txt", 5);
  assert(bad == 127L);
}

void run() {
  revealFirstBadNumber("day09_data.txt", 25);
}

int main(int argc, char** argv) {
  test();

  run();

  return 0;
}
