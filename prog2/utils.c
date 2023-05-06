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

/**
 * \brief Check if a given double is an integer.
 *
 * \param x A double value to be checked.
 * 
 * \return true if the double value is an integer, false otherwise.
 */
bool is_int(double x)
{
    return floor(x) == x;
}

/**
 * \brief Convert a boolean value to a corresponding string representation.
 *
 * \param value A boolean value to be converted to a string.
 * 
 * \return "true" if the boolean value is true, "false" otherwise.
 */
char *get_char_bool(bool value)
{
    if (value)
    {
        return "true";
    }
    else
    {
        return "false";
    }
}

void print_results(char *path, List list)
{
    printf("file: %s\n", path);
    if (validate_sequence(list))
    {
        printf("Everything is ok!\n");
    }
    else
    {
        printf("Fail to sort list\n");
    }
}

/**
 * \brief Print the validation results of a list sequence.
 *
 * \param path A null-terminated string representing the file path of the list.
 * \param list A List struct containing the list to be validated.
 * 
 * \return void
 */
void print_bool_list(bool *array, int n)
{
    printf("[%s", get_char_bool(array[0]));
    for (int i = 1; i < n; i++)
    {
        printf(", %s", get_char_bool(array[i]));
    }
    printf("]");
}

/**
 * \brief Get an array of workers' ranks, excluding the dispatcher's rank.
 *
 * \param size The total number of ranks in the MPI communicator.
 * \param rank_dispatcher The rank of the dispatcher process.
 * 
 * \return A dynamically allocated array of integers containing the ranks of all workers, except the dispatcher.
 */
int *get_workers(int size, int rank_dispatcher)
{
    int *workers = (int *)malloc((size-1) * sizeof(int));
    int pointer = 0;
    for(int i = 0; i < size; i++)
    {
        if(i != rank_dispatcher)
        {
            workers[pointer++] = i;
        }
    }
    return workers;
}