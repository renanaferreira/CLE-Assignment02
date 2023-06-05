#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <mpi.h>
#include <math.h>
#include <string.h>
#include <libgen.h>

#include "sorting.h"

#define MAX_NUMBER_PROCESSES 8 /* maximum number of processes */

/** \brief print proper use of the program parameters and arguments */
static void printUsage(char *cmdName);

/**
 *  \brief Process execution.
 *
 *  Design and flow of the dispatcher:
 *
 *  1 - Process the arguments from the command line.
 *
 *  2 - Initialize the shared region with the necessary structures.
 *
 *  3 - Create tasks.
 *
 *  4 - Send tasks to worker process and wait to receive results.
 *
 *  5 - Sign workers the execution finished.
 *
 *  6 - Print final results.
 *
 *  Design and flow of the worker:
 *
 *  1 - Wait for work status from dispatcher.
 *
 *  2 - If there is work to do, wait to receive task.
 *
 *  3 - Process task.
 *
 *  4 - Send results.
 *
 *  5 - Do 2-4, until there is no more work to do.
 *
 *  \param argc number of arguments in the command line
 *  \param argv list of arguments in the command line
 *
 *  \return status of operation
 */
int main(int argc, char *argv[]) {
    int rank, nProcesses, nProcessesNow;

    MPI_Comm presentComm, nextComm;
    MPI_Group presentGroup, nextGroup;
    int gMembersId[MAX_NUMBER_PROCESSES];

    int seq_length;
    int nIter;

    int *recListSeq;

    /* dispatcher variables */
    int listLength;
    int *sendListSeq = NULL;
    char *filepath = NULL;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    for (int j = 0; j < nProcesses; j++) {
        gMembersId[j] = j;
    }

    /* process command line options */
    int opt; /* selected option */
    if (nProcesses > MAX_NUMBER_PROCESSES) { /* maximum number of workers surpass */
        if (rank == 0) {
            printf("Too many processes! It should be a power of 2, less or equal to 8.\n");
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    do {
        switch ((opt = getopt(argc, argv, "f:h"))) {
            case 'f':                 /* file */
                if (optarg[0] == '-') /* filename is missing */
                {
                    if (rank == 0) {
                        fprintf(stderr, "%s: file name is missing\n", basename(argv[0]));
                        printUsage(basename(argv[0]));
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                if (!is_file_open(optarg)) {
                    if (rank == 0) {
                        fprintf(stderr, "%s: file %s cannot be open.\n", basename(argv[0]), optarg);
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                filepath = optarg;
                break;
            case 'h': /* help mode */
                if (rank == 0) {
                    printUsage(basename(argv[0]));
                }
                MPI_Finalize();
                exit(EXIT_SUCCESS);
            case '?': /* invalid option */
                if (rank == 0) {
                    fprintf(stderr, "%s: invalid option\n", basename(argv[0]));
                    printUsage(basename(argv[0]));
                }
                MPI_Finalize();
                exit(EXIT_FAILURE);
            case -1:
                break;
        }
    } while (opt != -1);
    if (optind < argc) {
        if (rank == 0) {
            fprintf(stderr, "%s: invalid format\n", basename(argv[0]));
            printUsage(basename(argv[0]));
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    FILE *fp;
    /* initialise data */
    if (rank == 0) {  /* initialise list */
        fp = fopen(filepath, "rb");
        if (fread(&listLength, sizeof(int), 1, fp) != 1) {
            if (feof(fp)) {
                fprintf(stderr, "Unexpected end of file while reading file\n");
                exit(EXIT_FAILURE);
            } else if (ferror(fp)) {
                fprintf(stderr, "Error while reading file\n");
                exit(EXIT_FAILURE);
            }
        }
        // Allocate memory for the sequence of integers in the list
        sendListSeq = (int*)malloc(listLength * sizeof(int));
        // Read the sequence of integers from the binary file
        for (int i = 0; i < listLength; i++) {
            int* pos = (sendListSeq + i);
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
    } else {
        fp = fopen(filepath, "rb");
        if (fread(&listLength, sizeof(int), 1, fp) != 1) {
            if (feof(fp)) {
                fprintf(stderr, "Unexpected end of file while reading file\n");
                exit(EXIT_FAILURE);
            } else if (ferror(fp)) {
                fprintf(stderr, "Error while reading file\n");
                exit(EXIT_FAILURE);
            }
        }
        fclose(fp);
    }

    recListSeq = (int *) malloc(listLength * sizeof(int));

    /* start sorting process */
    if(rank == 0) {
        (void) get_delta_time();
    }

    nProcessesNow = nProcesses;
    presentComm = MPI_COMM_WORLD;
    MPI_Comm_group(presentComm, &presentGroup);

    nIter = (int) (log2(nProcessesNow) + 1.1);
    for (int iter = 0; iter < nIter; iter++) {
        seq_length = listLength / nProcessesNow;
        if (iter > 0) {
            MPI_Group_incl(presentGroup, nProcessesNow, gMembersId, &nextGroup);
            MPI_Comm_create(presentComm, nextGroup, &nextComm);
            presentGroup = nextGroup;
            presentComm = nextComm;
            if (rank >= nProcessesNow) {
                free(recListSeq);
                MPI_Finalize();
                exit(EXIT_SUCCESS);
            }
        }
        MPI_Comm_size(presentComm, &nProcesses);
        /* send: Scatter */
        MPI_Scatter(sendListSeq, seq_length, MPI_INT,
                    recListSeq, seq_length, MPI_INT, 0, presentComm);
        /* sorting process */
        if (iter > 0) {
            bool dir = (rank % 2 == 0);
            bitonic_merge(recListSeq, listLength, dir);
            //bitonic_sort(recListSeq, seq_length);
        } else {
            bitonic_sort(recListSeq, seq_length);
        }
        /*receive: Gather */
        MPI_Gather(recListSeq, seq_length, MPI_INT,
                   sendListSeq, seq_length, MPI_INT, 0, presentComm);
        nProcessesNow = nProcessesNow >> 1;
    }

    free(recListSeq);

    if (rank == 0) {
        printf("\nElapsed time multi process = %.6f s\n", get_delta_time());
        printf("file: %s\n", filepath);
        bool sorted = true;
        for (int i = 0; i < listLength - 1; i++) {
            int num01 = sendListSeq[i];
            int num02 = sendListSeq[i + 1];
            if (num01 > num02) {
                fprintf(stderr, "Error in position %d between element %d and %d\n", i, num01, num02);
                sorted = false;
                break;
            }
        }
        if (sorted) {
            printf("Everything is ok!\n");
        } else {
            printf("Fail to sort list\n");
        }
        printf("\n");
        free(sendListSeq);
    }

    MPI_Finalize();
    exit(EXIT_SUCCESS);
}

/**
 *  \brief Print command usage.
 *
 *  A message specifying how the program should be called is printed.
 *
 *  \param cmdName string with the name of the command
 */
static void printUsage(char *cmdName) {
    fprintf(stderr, "\nSynopsis: %s OPTIONS [-f filename / -w number of workers / -h help]\n"
                    "  OPTIONS:\n"
                    "  -f      --- filename to process\n"
                    "  -w      --- number of workers\n"
                    "  -h      --- print this help\n",
            cmdName);
}
