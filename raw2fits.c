#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "common.h"

int main(int argc, char *argv[]) {

  if (argc < 3) {
    fprintf(stderr, "usage: ./raw2fits <raw_file> <fits_file>\n");
    return 1;
  }

  const char *raw_file = argv[1];
  const char *fits_file = argv[2];

  struct RawImage ri;

  if (load_raw(raw_file, &ri) == 0) {

    if (save_fits(fits_file, &ri) == 0) {

      free(ri.data);

      return 0;

    } else {

      fprintf(stderr, "error: cannot write fits_file: %s\n", fits_file);
      return 1;

    }

  } else {

    fprintf(stderr, "error: cannot load raw_file: %s\n", raw_file);
    return 1;

  }

}
