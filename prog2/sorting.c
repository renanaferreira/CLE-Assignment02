
/**
 *  \file sorting.c (definition file)
 *  \brief Bitonic Sort Algorithm Implementation.
 *
 * Author:  Renan Ferreira
 *          João Reis
 */

#include "sorting.h"

/**
 * \brief Prints a list of integers to the standard output.
 *
 * \param list The list of integers to print.
 */
void print_list(int *list, unsigned int length) {
    int i;

    printf("[%d", list[0]);
    for (i = 1; i < length; i++)
    {
        printf(",%d", list[i]);
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
* \brief Reads a list of integers from a binary file and returns it as an integer array.
*
* \param filepath The path of the binary file to read.
* \return An integer array containing the sequence of integers in the binary file.
*/
int *create_list(char *filepath) {
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

/**
 *  \brief Get the process time that has elapsed since last call of this time.
 *
 *  \return process elapsed time
 */
double get_delta_time(void)
{
    static struct timespec t0, t1;

    t0 = t1;
    if (clock_gettime(CLOCK_MONOTONIC, &t1) != 0)
    {
        perror("clock_gettime");
        exit(1);
    }
    return (double)(t1.tv_sec - t0.tv_sec) + 1.0e-9 * (double)(t1.tv_nsec - t0.tv_nsec);
}

/**
 * \brief Check if a file can be opened for reading in binary mode.
 *
 * \param path A null-terminated string representing the file path to be checked.
 *
 * \return true if the file can be opened for reading, false otherwise.
 */
bool is_file_open(char *path)
{
    FILE *fp = fopen(path, "rb");
    if (NULL == fp)
    {
        return false;
    }
    fclose(fp);
    return true;
}

void bitonic_merge(int *list, unsigned int length, bool asc) {
    unsigned int half_len, nL, m, n, offset, t;
    unsigned int idx01, idx02;

    half_len = length >> 1; // length / 2
    nL = 1;
    for (m = 0; m < (int)log2f((float)length); m++) { // for each subsequence size, such as sub_size=2^m
        n = 0;
        offset = 0;
        while (n < nL) {
            for (t = 0; t < half_len; t++) {

                idx01 = t + offset;
                idx02 = t + offset + half_len;

                // CAPS
                if ((asc && list[idx01] > list[idx02]) || (!(asc) && list[idx01] < list[idx02])) {
                    int tmp = list[idx01];
                    list[idx01] = list[idx02];
                    list[idx02] = tmp;
                }
            }
            offset += (half_len << 1); // offset = offset + (half_len * 2)
            n += 1;
        }
        half_len >>= 1; // half_len = half_len / 2
        nL <<= 1; // nL = nL * 2
    }
}

void bitonic_sort(int *list, unsigned int length) {
    int sub_size, sub_offset;
    bool asc;
    for (sub_size = 2; sub_size <= length; sub_size <<= 1) { // for each subsequence size in the list
        for (sub_offset = 0; sub_offset < length; sub_offset += sub_size) { // for each subsequence offset
            asc = (sub_offset / sub_size) % 2 == 0; // divide j by list length to discover its direction
            bitonic_merge(list + sub_offset, sub_size, asc);
        }
    }
}
