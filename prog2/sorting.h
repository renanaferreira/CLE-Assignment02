/**
 *  \file sorting.h (definition file)
 *  \brief Header file containing the declarations for the functions used in the bitonic sort algorithm.
 */
#ifndef SORTING_H
#define SORTING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

/**
 * \brief Struct representing a list of integers.
 */
typedef struct
{
    unsigned int length; /** The length of the list. */
    int *sequence;       /** The sequence of integers in the list. */
} List;

extern List create_list(char *filepath);
extern int *create_list_sequence(char *filepath);
extern List generate_random_list(int size);
extern List copy_list(List);
extern List copy_list_sequence(int length, int *sequence);
extern List copy_sublist(List, int, int);
extern void update_list(List *, List , int start);


extern void bitonic_sort(List *list);
extern void sort(List *);
extern void bitonic_sort_sequence(unsigned int length, int *sequence);
extern void sort_sequence(unsigned int length, int *sequence);

extern int get_list_size(char *path);
extern bool validate_sequence(List list);

extern int get_number_bases(int length);
extern int get_num_seq_by_base(int length, int base);
extern int get_seq_len_by_base(int length, int base);
extern int get_initial_base(unsigned int length, unsigned int nWorkers);

extern void print_list(List list);

#endif /* SORTING_H */