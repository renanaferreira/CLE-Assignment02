/**
 *  \file utils.c (source file)
 *  \brief source file containing declaration utility functions for our problem.
 *
 *  This file contains declaractions for several utility functions that may be useful
 *  in a computer intensive problems. These functions include timing functions, file I/O functions,
 *  and functions for working with process ranks.
 */
#include "utils.h"

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
 *  \brief Get the size of a file in bytes.
 *
 *  \param path The path to the file to be checked.
 *  \return The size of the file in bytes as a long integer.
 */
long get_file_size(char *path)
{
    FILE *fp;
    long size;
    fp = fopen(path, "rb");
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    fclose(fp);
    return size;
}

/**
 *  \brief Check if a file can be opened for reading.
 *
 *  \param path The name of the file to be checked.
 *  \return true if the file can be opened for reading, false otherwise.
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

/**
 *  \brief Get an array of worker ranks for a given number of processes and a rank of a dispatcher process.
 *
 *  \param size The total number of processes in the job.
 *  \param rank_dispatcher The rank of the dispatcher process.
 *  \return A dynamically allocated integer array containing the ranks of all worker processes.
 *          This array must be freed by the caller after use.
 */
int *get_workers(int size, int rank_dispatcher)
{
    int *workers;
    if((workers = (int *)malloc((size-1) * sizeof(int))) == NULL)
    {
        fprintf(stderr, "get_workers(): error while allocating memory in array structure");
        exit(EXIT_FAILURE);
    }
    int head = 0;
    for(int i = 0; i < size; i++)
    {
        if(i != rank_dispatcher)
        {
            workers[head++] = i;
        }
    }
    return workers;
}
