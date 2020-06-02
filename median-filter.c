
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

int filter(struct RawImage *input, struct RawImage *output, unsigned int filter_size) {

  output->width = input->width;
  output->height = input->height;
  output->data = calloc(output->width * output->height, sizeof(double));

#pragma omp parallel for
  for (unsigned int y = 0; y < input->height; y++) {
    
    double *line = calloc((2 * filter_size + 1) * (2 * filter_size + 1), sizeof(double));

    for (unsigned int x = 0; x < input->width; x++) {

      unsigned int line_elements = 0;

      for (unsigned int ky = fmax(0, y-filter_size); ky < fmin(input->height, y+filter_size); ky++) {
        for (unsigned int kx = fmax(0, x-filter_size); kx < fmin(input->width, x+filter_size); kx++) {
          line[line_elements++] = input->data[ky * input->width + kx];
        }
      }

      qsort(line, line_elements, sizeof(double), double_cmp);

      output->data[y * output->width + x] = line[line_elements / 2];

    }

    free(line);

  }

  return 0;

}

int main(int argc, char *argv[]) {

  char *input_file;
  char *output_file;
  unsigned int filter_size;

  if (argc < 4) {

    fprintf(stderr, "usage: ./at_median_filter <input_file> <output_file> <size>\n");
    return 1;

  } else {

    input_file = argv[1];
    output_file = argv[2];
    filter_size = atoi(argv[3]);

  }

  struct RawImage input = {0};
  struct RawImage output = {0};

  if (load_fits(input_file, &input) == 0) {

    filter(&input, &output, filter_size);

    int ret = save_fits(output_file, &output);

    free(input.data);
    free(output.data);

    if (ret == 0) {
      return 0;
    }

    return 1;

  } else {

    fprintf(stderr, "error: cannot load input file %s\n", input_file);

  }

  return 1;
    
}
