
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

int do_minus(struct RawImage *op1, struct RawImage *op2, struct RawImage *r) {

  r->width = op1->width;
  r->height = op1->height;
  r->data = calloc(r->width * r->height, sizeof(double));

#pragma omp parallel for
  for (unsigned int i = 0; i < op1->width * op1->height; i++) {
    r->data[i] = fmax(0, op1->data[i] - op2->data[i]);
  }
  
  return 0;

}

int do_division(struct RawImage *op1, struct RawImage *op2, struct RawImage *r) {

  r->width = op1->width;
  r->height = op1->height;
  r->data = calloc(r->width * r->height, sizeof(double));

#pragma omp parallel for
  for (unsigned int i = 0; i < op1->width * op1->height; i++) {
    r->data[i] = op1->data[i] / op2->data[i];
  }
  
  return 0;

}

int main(int argc, char *argv[]) {

  char *operator;
  char *op1_file;
  char *op2_file;
  char *r_file;

  if (argc < 5) {

    fprintf(stderr, "usage: ./at_calc <op> <op1_file> <op2_file> <r_file>\n");
    fprintf(stderr, "op can be: -, /\n");
    return 1;

  } else {

    operator = argv[1];
    op1_file = argv[2];
    op2_file = argv[3];
    r_file = argv[4];

  }

  struct RawImage op1_ri = {0};
  struct RawImage op2_ri = {0};
  struct RawImage r_ri = {0};

  if (load_fits(op1_file, &op1_ri) == 0) {

    if (load_fits(op2_file, &op2_ri) == 0) {

      switch (operator[0]) {

        case '-':
          do_minus(&op1_ri, &op2_ri, &r_ri);
          break;

        case '/':
          do_division(&op1_ri, &op2_ri, &r_ri);
          break;

      }

      int ret = save_fits(r_file, &r_ri);

      free(op1_ri.data);
      free(op2_ri.data);

      if (ret ==  0) {

        return 0;

      } else {

        fprintf(stderr, "error: cannot save output to %s\n", r_file);
        return 1;

      }

    } else {

      fprintf(stderr, "error: cannot load op2 file %s\n", op2_file);

    }

  } else {

    fprintf(stderr, "error: cannot load op1 file %s\n", op1_file);

  }

  return 1;
    
}
