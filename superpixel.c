#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

double ggr_ggb_brg(double *src, unsigned int src_width) {

    double r=0.0, g=0.0, b=0.0;

    g += src[0] + src[1];
    r += src[2];
    src += src_width;
    g += src[0] + src[1];
    b += src[2];
    src += src_width;
    b += src[0];
    r += src[1];
    g += src[2];

    r /= 2;
    g /= 5;
    b /= 2;

    return r + g + b;

}

double ggb_ggr_rbg(double *src, unsigned int src_width) {

    double r=0.0, g=0.0, b=0.0;

    g += src[0] + src[1];
    b += src[2];
    src += src_width;
    g += src[0] + src[1];
    r += src[2];
    src += src_width;
    r += src[0];
    b += src[1];
    g += src[2];

    r /= 2;
    g /= 5;
    b /= 2;

    return r + g + b;

}


int make_super_pixel(struct RawImage *from, struct RawImage *to) {

  to->height = from->height / 3;
  to->width = from->width / 3;

  to->data = calloc(sizeof(double), to->width * to->height);

  unsigned int src_width = from->width;

#pragma omp parallel for
  for (unsigned int y = 0; y < to->height; y++) {
    for (unsigned int x = 0; x < to->width; x++) {

      double *src = from->data + (y * 3) * from->width + (x * 3);
      double *dst = to->data + y * to->width + x;

      switch (x % 2) {

        case 0: {
                  switch (y % 2) {
                    case 0: // GGR GGB BRG
                      dst[0] = ggr_ggb_brg(src, src_width);
                      break;
                    case 1: // GGB GGR RBG
                      dst[0] = ggb_ggr_rbg(src, src_width);
                      break;
                  }
                  break;
                }

        case 1: {
                  switch (y % 2) {
                    case 0: // GGB GGR RBG
                      dst[0] = ggb_ggr_rbg(src, src_width);
                      break;
                    case 1: // GGR GGB BRG
                      dst[0] = ggr_ggb_brg(src, src_width);
                      break;
                  }
                  break;
                }

      }

    }
  }

  return 0;

}


int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "usage: ./at_superpixel <input-file> <output_file>\n");
    return 1;
  }

  const char *input_file = argv[1];
  const char *output_file = argv[2];

  struct RawImage input;
  struct RawImage output;

  if (load_fits(input_file, &input) == 0) {

    make_super_pixel(&input, &output); 

    if (save_fits(output_file, &output) == 0) {

      free(input.data);
      free(output.data);

    } else {

      fprintf(stderr, "error: cannot write output file %s\n", output_file);

    }

  } else {

    fprintf(stderr, "error: cannot load input file %s\n", input_file);

  }

}
