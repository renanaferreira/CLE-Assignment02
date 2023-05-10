#include "prog2.h"

#define MAX_NUMBER_PROCESSES 8 /* maximum number of processes */
#define MAX_FILES 6            /* maximum number of files */

/** \brief print proper use of the program parameters and arguments */
static void printUsage(char *cmdName);

static const unsigned int WORK_TO_DO = 0;
static const unsigned int EXECUTE_ERROR = 1;

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
    unsigned int *workStatus = malloc(sizeof(unsigned int));
    int *listLength = malloc(sizeof(int));

    int rank, nProcesses, globalNProcesses, nProcessesNow;

    MPI_Comm presentComm, nextComm;
    MPI_Group presentGroup, nextGroup;
    int gMembersId[MAX_NUMBER_PROCESSES];

    unsigned int nFiles = 0; /* number of files */

    int seq_length;
    int nIter;

    int *recvListSeq;

    /* dispatcher variables */
    List *listSpace = NULL;  /* list structure array */
    int *sendListSeq = NULL;
    char **pathSpace = NULL;  /* file path array */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    globalNProcesses = nProcesses;
    *workStatus = WORK_TO_DO;
    for (int j = 0; j < MAX_NUMBER_PROCESSES; j++) {
        gMembersId[j] = j;
    }

    if(rank == 0) {
        pathSpace = (char **) malloc(MAX_FILES * sizeof(char *));
    }

    /* process command line options */
    int opt; /* selected option */
    if (nProcesses > MAX_NUMBER_PROCESSES) { /* maximum number of workers surpass */
        if(rank == 0) {
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
                    if(rank == 0) {
                        fprintf(stderr, "%s: file name is missing\n", basename(argv[0]));
                        printUsage(basename(argv[0]));
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                if (nFiles > MAX_FILES) /* Surpassing number of files */
                {
                    if(rank == 0) {
                        fprintf(stderr, "%s: maximum number of files is %d\n", basename(argv[0]), MAX_FILES);
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                if (!is_file_open(optarg)) {
                    if(rank == 0) {
                        fprintf(stderr, "%s: file %s cannot be open.\n", basename(argv[0]), optarg);
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                }
                if(rank == 0) {
                    pathSpace[nFiles] = optarg;
                }
                nFiles++;
                break;
            case 'h': /* help mode */
                if(rank == 0) {
                    printUsage(basename(argv[0]));
                }
                MPI_Finalize();
                exit(EXIT_SUCCESS);
            case '?': /* invalid option */
                if(rank == 0) {
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
        if(rank == 0) {
            fprintf(stderr, "%s: invalid format\n", basename(argv[0]));
            printUsage(basename(argv[0]));
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    if (rank == 0) { /* dispatcher */
        /* initialise fileSpace region */
        if ((listSpace = malloc(nFiles * sizeof(List))) == NULL) {
            fprintf(stderr, "error on allocating space to the file space memory region\n");
            *workStatus = EXECUTE_ERROR;
        }
    }
    MPI_Bcast(workStatus, sizeof(unsigned int), MPI_UNSIGNED, 0, MPI_COMM_WORLD);
    if(*(workStatus) == EXECUTE_ERROR) {
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    if(rank == 0) {
        (void) get_delta_time();
    }

    /* start sorting process */
    for(int i = 0; i < nFiles; i++) {

        if(rank == 0) {
            char *path = pathSpace[i];
            *listLength = get_list_size(path);
            sendListSeq = create_list_sequence(path);
        }
        MPI_Bcast(listLength, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);

        recvListSeq = malloc(*(listLength) * sizeof(int));

        nProcessesNow = globalNProcesses;
        presentComm = MPI_COMM_WORLD;
        MPI_Comm_group(presentComm, &presentGroup);

        nIter = (int)(log2(nProcessesNow) + 1.1);
        for(int j = 0; j < nIter; j++) {
            seq_length = *(listLength) / nProcessesNow;
            if(j > 0) {
                MPI_Group_incl(presentGroup, nProcessesNow, gMembersId, &nextGroup);
                MPI_Comm_create(presentComm, nextGroup, &nextComm);
                presentGroup = nextGroup;
                presentComm = nextComm;
                if (rank >= nProcessesNow)
                {
                    break;
                }
            }
            MPI_Comm_size(presentComm, &nProcesses);
            /* send: Scatter */
            MPI_Scatter(sendListSeq,seq_length,MPI_INT,
                        recvListSeq,seq_length,MPI_INT,0,presentComm);
            /* sorting process */
            if(j > 0){
                sort_sequence(seq_length, recvListSeq);
            } else {
                bitonic_sort_sequence(seq_length, recvListSeq);
            }
            /*receive: Gather */
            MPI_Gather(recvListSeq, seq_length, MPI_INT,
                       sendListSeq, seq_length, MPI_INT, 0, presentComm);
            nProcessesNow = nProcessesNow >> 1;
        }

        if(rank == 0)
        {
            List *list = (listSpace + i);
            *(list) = copy_list_sequence(*listLength, recvListSeq);
            free(sendListSeq);
        }

        free(recvListSeq);
    }

    if (rank == 0)
    {
        printf("\nElapsed time multi process = %.6f s\n\n", get_delta_time());
        for (int i = 0; i < nFiles; i++) { /* print results */
            print_results(pathSpace[i], listSpace[i]);
            printf("\n");
            free((listSpace + i)->sequence);
        }
        free(listSpace);
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
