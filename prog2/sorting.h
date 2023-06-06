/**
 *  \file sorting.h (definition file)
 *  \brief Header file containing the declarations for the functions used in the bitonic sort algorithm.
 */
#ifndef SORTING_H
#define SORTING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

extern void bitonic_merge(int *list, unsigned int length, bool asc);
extern void bitonic_sort(int *list, unsigned int length, bool asc);

extern void print_list(int *list, unsigned int length);

extern bool is_file_open(char *path);
extern double get_delta_time(void);

#endif /* SORTING_H */