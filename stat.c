
#include <stdio.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>
#include <omp.h>
#include <libgen.h>

#include "libraw/libraw.h"

#include "common.h"

int main(int argc, char *argv[]) {

  char *operator;
  char *input_file;

  if (argc < 3) {

    fprintf(stderr, "usage: ./at_stat <op> <input_file>\n");
    fprintf(stderr, "op can be: m (mean), s (stddev/sigma)\n");
    return 1;

  } else {

    operator = argv[1];
    input_file = argv[2];

  }

  struct RawImage input_ri = {0};

  if (load_fits(input_file, &input_ri) == 0) {

    switch (operator[0]) {

      case 'm':
        printf("%lf\n", input_ri.mean);
        break;

      case 's':
        printf("%lf\n", input_ri.stddev);
        break;

    }

    free(input_ri.data);

    return 0;

  } else {

    fprintf(stderr, "error: cannot load input file %s\n", input_file);

  }

  return 1;
    
}
