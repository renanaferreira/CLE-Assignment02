/**
 *  \file sorting.h (definition file)
 *  \brief Header file containing the declarations for the functions used in the bitonic sort algorithm.
 */
#ifndef SORTING_H
#define SORTING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * \brief Struct representing a list of integers.
 */
typedef struct
{
    unsigned int length; /** The length of the list. */
    int *sequence;       /** The sequence of integers in the list. */
} List;

extern int *create_list_sequence(char *filepath);
extern List copy_list_sequence(int length, int *sequence);


extern void bitonic_sort_sequence(unsigned int length, int *sequence);
extern void sort_sequence(unsigned int length, int *sequence);

extern int get_list_size(char *path);
extern bool validate_sequence(List list);

#endif /* SORTING_H */