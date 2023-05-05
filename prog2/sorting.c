/**
 *  \file sorting.c (definition file)
 *  \brief Bitonic Sort Algorithm Implementation.
 *
 * Author:  Renan Ferreira
 *          João Reis
 */
#include "sorting.h"


/**
 * \brief Creates a list of integers from a binary file.
 *
 * \param path The path to the binary file containing the list of integers.
 * 
 * \return The created list of integers.
 */
List create_list(char* path)
{
    List list;
    list.length = get_list_size(path);
    list.sequence = create_list_sequence(path);
    return list;
}

/**
 * \brief Creates a copy of a list of integers.
 *
 * \param list The list to copy.
 * 
 * \return The copied list.
 */
List copy_list(List list)
{
    printf("copy_list(): start\n");
    List new_list;
    new_list.length = list.length;
    new_list.sequence = (int*)malloc(new_list.length * sizeof(int));
    for (int i = 0; i < new_list.length; i++) {
        new_list.sequence[i] = list.sequence[i];
    }
    printf("copy_list(): end\n");
    return new_list;
}

/**
 * \brief Creates a copy of a sublist of a list of integers.
 *
 * \param list The list to copy the sublist from.
 * \param length The length of the sublist to copy.
 * \param start The starting index of the sublist to copy.
 * 
 * \return The copied sublist.
 */
List copy_sublist(List list, int length, int start)
{
    printf("copy_sublist(): start\n");
    if (start + length > list.length) {
        fprintf(stderr, "Surpass list memory limits: list.len=%d, start=%d, new_list.len=%d\n", list.length, start, length);
        exit(EXIT_FAILURE);
    }
    List new_list;
    new_list.length = length;
    new_list.sequence = (int*)malloc(length * sizeof(int));
    for (int i = 0; i < new_list.length; i++) {
        printf("copy_sublist(): count=%d\n", i);
        new_list.sequence[i] = list.sequence[start + i];
    }
    printf("copy_sublist(): end\n");
    return new_list;
}

/**
 * \brief Updates a sublist of a list of integers with the values of another list.
 *
 * \param list The list to update the sublist in.
 * \param sublist The list containing the values to update the sublist with.
 * \param start The starting index of the sublist to update.
 */
void update_list(List* list, List sublist, int start)
{
    if (start + sublist.length > list->length) {
        fprintf(stderr, "Surpass list memory limits: list.len=%d, start=%d, sublist.len=%d\n", list->length, start, sublist.length);
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < sublist.length; i++) {
        list->sequence[start + i] = sublist.sequence[i];
    }
}

void sort(List *list)
{
    sort_sequence(list->length, list->sequence);
}

/**
 * \brief Validates if a sequence of integers is sorted in ascending order.
 *
 * \param list The list of integers to validate.
 * 
 * \return Whether the sequence is sorted in ascending order or not.
 */
bool validate_sequence(List list)
{
    for (int i = 0; i < list.length - 1; i++)
    {
        int num01 = *(list.sequence + i);
        int num02 = *(list.sequence + i + 1);
        if (num01 > num02)
        {
            fprintf(stderr,
                    "Error in position %d between element %d and %d\n", i, num01, num02);
            return false;
        }
    }
    return true;
}

/**
 * \brief Prints a list of integers to the standard output.
 *
 * \param list The list of integers to print.
 */
void print_list(List list)
{
    int i;

    printf("[%d", list.sequence[0]);
    for (i = 1; i < list.length; i++)
    {
        printf(",%d", list.sequence[i]);
    }
    printf("]\n");
}

/**
 * \brief Prints a list of integers to the standard output.
 *
 * \param list Pointer to a List struct containing the integers to print.
 */
void bitonic_sort(List *list) {
    bitonic_sort_sequence(list->length, list->sequence);
}

/**
 * \brief Sorts a sequence of integers using the bitonic sort algorithm.
 *
 * \param length Length of the sequence to sort.
 * \param sequence Pointer to the start of the sequence.
 */
void bitonic_sort_sequence(unsigned int length, int *sequence) {
    for (int base = 2; base <= length; base *= 2)
    {
        for (int i = 0; i < length; i += base)
        {
            sort_sequence(base, (sequence + i));
        }
    }
}

/**
 * \brief Generates a list of random integers.
 *
 * \param size Size of the list to generate.
 * 
 * \return A List struct containing the generated sequence.
 */
List generate_random_list(int size) {
    List list;
    list.length = size;
    list.sequence = (int *) malloc(list.length * sizeof(int));
    srandom(getpid());
    for(int i = 0; i < list.length; i++)
    {
        list.sequence[i] = (int)(((double)rand() / RAND_MAX) * 1000);
    }
    return list;
}

/**
 * \brief Gets the number of orders in a bitonic sequence of a given length.
 *
 * \param length The length of the bitonic sequence.
 * 
 * \return The number of orders in the bitonic sequence.
 */
int get_number_bases(int length)
{
    if(length < 2)
    {
        return 0;
    }
    return (int) ceil(log2(length));
}

/**
 * \brief Gets the number of bitonic sequences of a given length in a bitonic sequence of a given base.
 *
 * \param length The length of the bitonic sequence.
 * \param base The base to obtain the number of sequences.
 * 
 * \return The number of bitonic sequences of the given length in the given base in the bitonic sequence.
 */
int get_num_seq_by_base(int length, int base)
{
    return length >> base;
}

/**
 * \brief Gets the length of a bitonic sequence of a given order.
 *
 * \param length The length of the bitonic sequence.
 * \param order The given order of the bitonic sequece.
 * 
 * \return The length of the subsequences of the given in the bitonic sequence.
 */
int get_seq_len_by_base(int length, int order)
{
    return length / (get_num_seq_by_base(length, order));
}

/**
 * \brief Gets the size of a binary file containing a list of integers.
 *
 * \param path Path to the binary file to read.
 * 
 * \return The size of the list of integers stored in the file.
 */
int get_list_size(char *path)
{
    int size;
    FILE *fp = fopen(path, "rb");
    if (fread(&size, sizeof(int), 1, fp) != 1) {
        if (feof(fp)) {
            fprintf(stderr, "Unexpected end of file while reading file\n");
            exit(EXIT_FAILURE);
        } else if (ferror(fp)) {
            fprintf(stderr, "Error while reading file\n");
            exit(EXIT_FAILURE);
        }
    }
    fclose(fp);
    return size;
}

/**
 * \brief Calculates the initial base value for bitonic sort.
 *
 * \param length Length of the sequence to be sorted.
 * \param nWorkers Number of workers to use.
 *
 * \return The initial base value for bitonic sort.
 */
int get_initial_base(unsigned int length, unsigned int nWorkers)
{
    int new_length = (int)(length) / (int)(nWorkers);
    return get_number_bases(new_length);
}

/**
 * \brief Sorts a sequence of integers using bitonic sort.
 *
 * \param length Length of the sequence.
 * \param sequence Pointer to the sequence of integers.
 */
void sort_sequence(unsigned int length, int *sequence) {
    for(unsigned int m = 0; m < length/2; m++)
    {
        for (unsigned int n = 0; (m + n) < length/2; n++)
        {
            if (sequence[m+n] > sequence[length/2 + n])
            {
                int temp = sequence[m+n];
                sequence[m+n] = sequence[length/2 + n];
                sequence[length/2 + n] = temp;
            }
        }
    }
}

/**
 * \brief Creates a copy of a list of integers.
 *
 * \param length Length of the input list.
 * \param sequence Pointer to the input list of integers.
 * 
 * \return A new list that contains a copy of the input list.
 */
List copy_list_sequence(int length, int *sequence) {
    List new_list;
    new_list.length = length;
    new_list.sequence = (int*)malloc(new_list.length * sizeof(int));
    for (int i = 0; i < new_list.length; i++) {
        new_list.sequence[i] = sequence[i];
    }
    return new_list;
}


/**
* \brief Reads a list of integers from a binary file and returns it as an integer array.
*
* \param filepath The path of the binary file to read.
* \return An integer array containing the sequence of integers in the binary file.
*/
int *create_list_sequence(char *filepath) {
    int length;
    int *sequence;
    FILE* fp = fopen(filepath, "rb");
    if (fread(&length, sizeof(int), 1, fp) != 1) {
        if (feof(fp)) {
            fprintf(stderr, "Unexpected end of file while reading file\n");
            exit(EXIT_FAILURE);
        } else if (ferror(fp)) {
            fprintf(stderr, "Error while reading file\n");
            exit(EXIT_FAILURE);
        }
    }
    // Allocate memory for the sequence of integers in the list
    sequence = (int*)malloc(length * sizeof(int));
    // Read the sequence of integers from the binary file
    for (int i = 0; i < length; i++) {
        int* pos = (sequence + i);
        if (fread(pos, sizeof(int), 1, fp) != 1) {
            if (feof(fp)) {
                fprintf(stderr, "Unexpected end of file while reading file\n");
                exit(EXIT_FAILURE);
            } else if (ferror(fp)) {
                fprintf(stderr, "Error while reading file\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(fp);
    return sequence;
}
