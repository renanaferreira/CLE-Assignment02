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

bool is_int(double x)
{
    return floor(x) == x;
}

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

void print_bool_list(bool *array, int n)
{
    printf("[%s", get_char_bool(array[0]));
    for (int i = 1; i < n; i++)
    {
        printf(", %s", get_char_bool(array[i]));
    }
    printf("]");
}

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