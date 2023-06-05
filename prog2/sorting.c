
/**
 *  \file sorting.c (definition file)
 *  \brief Bitonic Sort Algorithm Implementation.
 *
 * Author:  Renan Ferreira
 *          João Reis
 */

#include "sorting.h"

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
 * \brief Sorts a sequence of integers using bitonic sort.
 *
 * \param length Length of the sequence.
 * \param sequence Pointer to the sequence of integers.
 */
void sort_sequence(unsigned int length, int *sequence) {
    printf("sort_sequence(length=%d)\n", length);
    for(unsigned int m = 0; m < length/2; m++)
    {
        for (unsigned int n = 0; (m + n) < length/2; n++)
        {
            printf("iteration(m=%d,n=%d)\n", m, n);
            if (sequence[m+n] > sequence[length/2 + n])
            {
                int temp = sequence[m+n];
                sequence[m+n] = sequence[length/2 + n];
                sequence[length/2 + n] = temp;
            }
        }
    }
    printf("\n\n");
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
