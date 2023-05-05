#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <libgen.h>

#include "consts.h"
#include "textProcessing.h"
#include "utf8character.h"

typedef struct
{
    char *path;
    int id;
    TextResult results;

} TextStruct;

typedef struct
{
    int id;
    Chunk chunk;
} WorkerTask;

typedef struct 
{
    int id;
    TextResult results;
} WorkerResult;

/** \brief print proper use of the program parameters and arguments */
static void printUsage(char *cmdName);

#endif /* MAIN_H */