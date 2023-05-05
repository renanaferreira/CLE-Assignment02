#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "sorting.h"


extern bool is_file_open(char *path);

extern double get_delta_time(void);
extern bool is_int(double x);
extern char *get_char_bool(bool value);
extern void print_results(char *path, List list);
extern void print_bool_list(bool *array, int n);
extern int *get_workers(int size, int rank_dispatcher);

#endif /* UTILS_H */