
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
