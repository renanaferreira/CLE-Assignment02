/**
 *  \file utils.h (header file)
 *  \brief header file containing declaration of utility functions for our problem job.
 *
 *  This file contains definitions for several utility functions that may be useful
 *  in a computer intensive problems. These functions include timing functions, file I/O functions,
 *  and functions for working with process ranks.
 */
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

long get_file_size(char *path);
bool is_file_open(char *path);
double get_delta_time(void);
int *get_workers(int size, int rank_dispatcher);

#endif /* UTILS_H */