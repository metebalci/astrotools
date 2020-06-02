
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#include "libraw/libraw.h"

#include "common.h"

#define MAX_FILES_TO_STACK 1024

double avg(double *line, unsigned int len, ...) { 
  
  va_list ap;
  va_start(ap, len);
  double upper_limit = va_arg(ap, double);
  double lower_limit = va_arg(ap, double); 
  va_end(ap);
  
  double r = 0.0;

  unsigned int num_processed = 0;

  if (upper_limit > 0 && lower_limit > 0) {

    for (int i = 0; i < len; i++) {

      if (line[i] > lower_limit && line[i] < upper_limit) {

        r += line[i];
        num_processed++;

      }

    }

  } else {

    for (int i = 0; i < len; i++) {
      r += line[i];
      num_processed++;
    }

  }

  return r / num_processed;

}

double med(double *line, unsigned int len, ...) {

  qsort(line, len, sizeof(double), double_cmp);
  
  return line[len/2];

}

#define MODE_AVG 0
#define MODE_MEDIAN 1
#define MODE_AVG_3STD 2
#define MODE_MEDIAN_3STD 3

int stack(struct RawImage **ris, int num_files, char *mode_str, struct RawImage *output) {

  unsigned int mode = 0;

  if (strcmp(mode_str, "avg")) {

    mode = MODE_AVG;

  } else if (strcmp(mode_str, "med")) {

    mode = MODE_MEDIAN;

  } else if (strcmp(mode_str, "avg3")) {

    mode = MODE_AVG_3STD;

  } else if (strcmp(mode_str, "med3")) {

    mode = MODE_MEDIAN_3STD;

  } else {

    fprintf(stderr, "error: invalid mode: %s\n", mode_str);
    return -1;

  }

  if (num_files == 0) return 0;

  output->width = ris[0]->width;
  output->height = ris[0]->height;
  unsigned int npixels = output->width * output->height;
  output->data = calloc(npixels, sizeof(double));

#pragma omp parallel for
  for (unsigned int y = 0; y < output->height; y++) {

    double *line = calloc(num_files, sizeof(double));

    unsigned int offset = y * output->width;

    for (unsigned int x = 0; x < output->width; x++) {

      unsigned int line_elements = 0;

      for (unsigned int n = 0; n < num_files; n++) {

        double d = ris[n]->data[offset];
        double m = ris[n]->mean;
        double s = ris[n]->stddev;
        switch (mode) {
          case MODE_AVG_3STD: 
          case MODE_MEDIAN_3STD:
            if (d > (m - 3 * s) && d < (m + 3 * s)) {
              line[n] = d;
              line_elements++;
            }
            break;

          default: 
            line[n] = d;
            line_elements++;
                    
        }

      }

      // in a rare case line is empty (i.e. all values are out of 3*sigma
      // then take all values and use normal avg or med methods
      if (line_elements == 0) {

        for (unsigned int n = 0; n < num_files; n++) {

          double d = ris[n]->data[offset];
          line[n] = d;
          line_elements++;
                      
        }

      }

      double v;
      switch (mode) {
        case MODE_AVG:
        case MODE_AVG_3STD:
          v = avg(line, line_elements, 0, 0);
          break;
        case MODE_MEDIAN:
        case MODE_MEDIAN_3STD:
          v = med(line, line_elements);
          break;
        default:
          fprintf(stderr, "error: probable bug\n");
          exit(1);
      }
      output->data[offset] = v;
      offset++;

    }

    free(line);

  }

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc < 4) {
    fprintf(stderr, "usage: stack <fits_file_list> <mode> <output_file>\n");
    fprintf(stderr, "mode can be:\n");
    fprintf(stderr, "- avg (average)\n");
    fprintf(stderr, "- med (median)\n");
    fprintf(stderr, "- avg3 (avg but reject less and more than 3 sigma)\n");
    fprintf(stderr, "- med3 (med but reject less and more than 3 sigma)\n");
    return 1;
  }

  char path[PATH_MAX];
  char *file_list = argv[1];
  char *mode = argv[2];
  char *output_file = argv[3];

  FILE *fp = fopen(file_list, "r");

  if (fp == NULL) {
    fprintf(stderr, "error: cannot open file list %s", file_list);
    return 1;
  }

  struct RawImage *ris[MAX_FILES_TO_STACK];

  unsigned int num_files = 0;

  bool load_error = false;

  while (feof(fp) == 0) {

    if (num_files >= MAX_FILES_TO_STACK) {
      load_error = true;
      break;
    }

    fscanf(fp, "%s\n", path);

    struct RawImage *ri = calloc(1, sizeof(struct RawImage));

    if (load_fits(path, ri) == 0) {

      ris[num_files] = ri;
      num_files++;

      printf("%s %.1lf %.1lf\n", ri->name, ri->mean, ri->stddev);

    }

  }

  fclose(fp);

  if (load_error) {

    fprintf(stderr, "error: too many files to stack, max %d files supported\n", MAX_FILES_TO_STACK);

  } else {

    if (num_files > 0) {

      struct RawImage output = {0};

      if (stack(ris, num_files, mode, &output) == 0) {

        save_fits(output_file, &output);

        unsigned int npixels = output.width * output.height;
        double mean = calc_mean(output.data, npixels);
        double stddev = calc_stddev(output.data, npixels, mean);

        printf("%s %.1lf %.1lf\n", output_file, mean, stddev);

      } else {

        return 1;

      }

      for (int i = 0; i < num_files; i++) {
        free(ris[i]->data);
        free(ris[i]);
      }

      free(output.data);

    } else {

      fprintf(stderr, "no input files found\n");

    }

  }

}
