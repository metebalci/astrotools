
#ifndef __COMMON_H__

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdint.h>

#define PATH_SEPARATOR "/"
#define MAX_NUM_FILES 4096

struct RawImage {
  char name[PATH_MAX];
  uint16_t width;
  uint16_t height;
  double *data;
  double mean;
  double stddev;
};

int double_cmp(const void *p1, const void *p2);

double calc_mean(double *b, unsigned int len);
double calc_stddev(double *b, unsigned int len, double mean);

int is_dir_or_file(char *path);

int get_file_paths(char *dir_name, char file_paths[MAX_NUM_FILES][PATH_MAX]);

int load_raw(const char *file_path, struct RawImage *ri);

int load_fits(const char *file_path, struct RawImage *ri);
int save_fits(const char *file_path, struct RawImage *ri);

#endif
