#include "main.h"


int main(int argc, char *argv[]) {
    int rank, nProcesses, globalNProcesses, nProcessesNow;

    MPI_Comm presentComm, nextComm;
    MPI_Group presentGroup, nextGroup;
    int gMembersId[MAX_NUMBER_WORKERS];

    unsigned int nFiles = 0; /* number of files */
    char *pathSpace[MAX_FILES];  /* file path array */
    List *listSpace;  /* list structure array */
    int list_length, seq_length;
    int nIter;
    int *sendListSeq = NULL;
    int *recvListSeq;

    /* reference handlers */
    char *path;
    List *list;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
    globalNProcesses = nProcesses;
    for (int j = 0; j < MAX_NUMBER_WORKERS; j++)
    {
        gMembersId[j] = j;
    }

    /* process command line options */
    int opt; /* selected option */
    if (nProcesses < 2) { /* This program requires at least 2 processes */
        if(rank == 0) {
            fprintf(stderr, "To run this program is necessary at least 2 processes.\n");
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    if (nProcesses > MAX_NUMBER_WORKERS) { /* maximum number of workers surpass */
        if(rank == 0) {
            printf("Too many processes! It should be a power of 2, less or equal to 8.\n");
        }
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }
    do {
        switch ((opt = getopt(argc, argv, "f:h"))) {
            case 'f':                 /* File */
                if (optarg[0] == '-') /* file path is missing */
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
                pathSpace[nFiles++] = optarg;
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
            MPI_Abort(MPI_COMM_WORLD, 1);
            exit(EXIT_FAILURE);
        }
        (void) get_delta_time();
    }



    /* start sorting process */
    for(int i = 0; i < nFiles; i++) {
        path = pathSpace[i];
        list_length = get_list_size(path);

        if(rank == 0) {
            sendListSeq = create_list_sequence(path);
        }
        recvListSeq = malloc(list_length * sizeof(int));

        nProcessesNow = globalNProcesses;
        presentComm = MPI_COMM_WORLD;
        MPI_Comm_group(presentComm, &presentGroup);

        nIter = (int)(log2(nProcessesNow) + 1.1);
        for(int j = 0; j < nIter; j++) {
            seq_length = list_length / nProcessesNow;
            if(j > 0) {
                MPI_Group_incl(presentGroup, nProcessesNow, gMembersId, &nextGroup);
                MPI_Comm_create(presentComm, nextGroup, &nextComm);
                int result;
                MPI_Comm_compare(presentComm, MPI_COMM_WORLD, &result);
                if(result != MPI_IDENT) /* if presentComm is not MPI_COMM_WORLD */
                {
                    MPI_Group_free(&presentGroup);
                    MPI_Comm_free(&presentComm);
                }
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
            list = (listSpace + i);
            *(list) = copy_list_sequence(list_length, recvListSeq);
            free(sendListSeq);
        }

        free(recvListSeq);
        MPI_Barrier(MPI_COMM_WORLD);
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
