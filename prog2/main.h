#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <libgen.h>

#include "consts.h"
#include "sorting.h"
#include "utils.h"

/** \brief Data structure representing a integer list to be sorted. */
typedef struct ListStruct
{
    char *path; /** The path of the list to be sorted. */
    int id;     /** The ID of the list. */
    List list;  /** The structure that represents the list. */

} ListStruct;

typedef struct Task
{
    int id;
    bool started;
    List list;    /**< The linked list that contains the sub-sequence to be sorted. */
} Task;

/** \brief print proper use of the program parameters and arguments */
static void printUsage(char *cmdName);

#endif /* MAIN_H */