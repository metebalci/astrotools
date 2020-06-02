
#ifndef __COMMON_H__

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <math.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libraw/libraw.h"
#include "fitsio.h"

#include "common.h"

int double_cmp(const void *p1, const void *p2) {

  double *a1 = (double *) p1;
  double *a2 = (double *) p2;

  if (*a1 > *a2) {
    return 1;
  } else if (*a1 < *a2) {
    return -1;
  } else {
    return 0;
  }

}

double calc_mean(double *b, unsigned int len) {
  double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
  for (unsigned int i = 0; i < len; i++) {
    sum += b[i];
  }
  return sum / len;
}

double calc_stddev(double *b, unsigned int len, double mean) {
  double sum = 0.0;
#pragma omp parallel for reduction(+:sum)
  for (unsigned int i = 0; i < len; i++) {
    sum += ((b[i] - mean) * (b[i] - mean));
  }
  return sqrt(sum / len);
}

int is_dir_or_file(char *path) {

  struct stat path_stat;
  memset(&path_stat, 0, sizeof(path_stat));
  lstat(path, &path_stat);

  if (S_ISDIR(path_stat.st_mode)) {
    return 1;
  } else if (S_ISREG(path_stat.st_mode)) {
    return 2;
  } else {
    return -1;
  }

}

int get_file_paths(char *dir_name, char file_paths[MAX_NUM_FILES][PATH_MAX]) {

  DIR *dr = opendir(dir_name);

  if (dr == NULL) {
    printf("cannot open directory: %s\n", dir_name);
    return -1 ;
  }

  struct dirent *de;

  unsigned int num_files = 0;

  while ((de = readdir(dr)) != NULL) {

    if ((de->d_type == DT_REG) || (de->d_type == DT_UNKNOWN)) {

      strcpy(file_paths[num_files], dir_name);
      strcat(file_paths[num_files], PATH_SEPARATOR);
      strcat(file_paths[num_files], de->d_name);

      char *pos = strrchr(file_paths[num_files], '.');

      if (pos == NULL) {
        break;

      }

      if (strcasecmp(pos+1, "RAF") != 0) {
        break;
      }

      num_files++;

      if (num_files == MAX_NUM_FILES) {
        fprintf(stderr, "num_files > %u\n", MAX_NUM_FILES);
        closedir(dr);
        return -1;
      }

    }

  }

  closedir(dr);

  return num_files;

}

int load_raw(const char *file_path, struct RawImage *ri) {

  int ret;

  libraw_data_t *lr = libraw_init(0);

  if ((ret = libraw_open_file(lr, file_path)) != LIBRAW_SUCCESS) {
    fprintf(stderr, "error: cannot open_file: \"%s\" reason: %s\n", file_path, libraw_strerror(ret));
    return -1;
  }

  uint16_t raw_width = lr->sizes.raw_width;
  //not using raw_height
  //uint16_t raw_height = lr->sizes.raw_height;

  uint16_t width = lr->sizes.width;
  uint16_t height = lr->sizes.height;

  uint16_t top = lr->sizes.top_margin;
  uint16_t left = lr->sizes.left_margin;

  if ((ret = libraw_unpack(lr)) != LIBRAW_SUCCESS) {
    fprintf(stderr, "error in unpack: %s\n", libraw_strerror(ret));
    libraw_close(lr);
    return -1;
  }

  ri->data = calloc(sizeof(double), width * height);

  unsigned short *raw_image = lr->rawdata.raw_image;

#pragma omp parallel for
  for (unsigned int y = top; y < (top+height); y++) {
    for (unsigned int x = left; x < (left+width); x++) {

      unsigned short value = raw_image[y * raw_width + x];
      ri->data[(y-top)*width + (x-left)] = (double) value;

    }
  }

  ri->width = width;
  ri->height = height;

  libraw_close(lr);

  return 0;

}

int load_fits(const char *file_path, struct RawImage *ri) {

  fitsfile *ff;

  int status = 0;
  fits_open_image(&ff, file_path, READONLY, &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  int bitpix;
  int naxis;
  long naxes[2];
  int maxdim = 65536;

  fits_get_img_param(ff, maxdim, &bitpix, &naxis, naxes, &status);

  strcpy(ri->name, file_path);
  ri->width = naxes[0];
  ri->height = naxes[1];

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  if (bitpix != DOUBLE_IMG) {
    fprintf(stderr, "error: file is not DOUBLE\n");
    fits_close_file(ff, &status);
    return -1;
  }

  long fpixel[2] = {1, 1};
  long nelements = ri->width * ri->height;

  ri->data = calloc(sizeof(double), nelements);

  fits_read_pix(ff,
      TDOUBLE,
      fpixel,
      nelements,
      NULL,
      ri->data,
      NULL,
      &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  fits_close_file(ff, &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  ri->mean = calc_mean(ri->data, nelements);
  ri->stddev = calc_stddev(ri->data, nelements, ri->mean);

  return 0;
}

int save_fits(const char *file_path, struct RawImage *ri) {

  const char *no_compression = getenv("AT_NO_FITS_COMPRESSION");

  fitsfile *ff;

  int status = 0;
  fits_create_file(&ff, file_path, &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  if ((no_compression == NULL) || (strcmp(no_compression, "0") == 0)) {

    fits_set_compression_type(ff, GZIP_2, &status);

    if (status) {
      fits_report_error(stderr, status);
      return -1;
    }

  }

  long naxis = 2;
  long naxes[2] = {ri->width, ri->height};

  fits_create_img(ff, DOUBLE_IMG, naxis, naxes, &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  long fpixel[2] = {1, 1};
  long nelements = ri->width * ri->height;

  fits_write_pix(ff, 
      TDOUBLE, 
      fpixel, 
      nelements, 
      ri->data, 
      &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  fits_close_file(ff, &status);

  if (status) {
    fits_report_error(stderr, status);
    return -1;
  }

  return 0;
}

# endif
