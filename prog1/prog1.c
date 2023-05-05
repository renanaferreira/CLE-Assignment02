#include "prog1.h"

/** \brief maximum number of worker processes */
#define MAX_WORKERS    8

/** \brief maximum number of files */
#define MAX_FILES  6

/** \brief maximum file size */
#define MAX_FILE_SIZE  100000

typedef struct TextStruct
{
    char *path;
    int id;
    TextResult results;

} TextStruct;

typedef struct WorkerTask
{
    int id;
    Chunk chunk;
} WorkerTask;

typedef struct WorkerResult
{
    int id;
    TextResult results;
} WorkerResult;

static const unsigned int WORK_TO_DO = 0;
static const unsigned int NO_MORE_WORK = 1;
static const unsigned int EXECUTE_ERROR = 2;

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
 *  3 - process task.
 *
 *  4 - Send results.
 *
 *  5 - Do 2-4, until there is no more work to do.
 *
 *  \param argc number of words of the command line
 *  \param argv list of words of the command line
 *
 *  \return status of operation
 */
int main(int argc, char *argv[]) {
    int rank, nProcesses;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);

    if (nProcesses < 2) /* This program requires at least 2 processes */
    {
        fprintf(stderr, "To run this program is necessary at least 2 processes.\n");
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    if (nProcesses - 1 > MAX_WORKERS) /* This program cannot have more than <MAX_WORKERS> workers processes */
    {
        fprintf(stderr, "The program cannot have more than %d workers \n", MAX_WORKERS);
        MPI_Finalize();
        exit(EXIT_FAILURE);
    }

    if (rank == 0) /* Dispatcher */
    {
        /* Set program variables */
        int nFiles = 0;                      /* number of text files (maximum=MAX_FILES) */
        char *files[MAX_FILES];              /* text file array (maximum=MAX_FILES)*/
        int maxChunkBytes = MAX_CHUNK_BYTES; /* maximum number of bytes per chunk */
        int maxNumTasks = 0; /* maximum number of tasks */
        int nTasks;                          /* number of tasks */
        unsigned int workStatus;             /* work status flag variable */
        int filePointer = 0; /* file pointer to the fileSpace data structure */
        int tasksPointer; /* pointer variable to the next worker to send a task */

        WorkerTask *taskSpace;                        /* array of tasks */
        TextStruct *fileSpace;                        /* file data array structure */
        WorkerTask *tmpTaskSpace; /* temporary tasks data structure */
        int *workers = get_workers(nProcesses, rank); /* workers rank array structure */
        int nWorkers = nProcesses - 1;             /* number of workers(number of processes - 1) */
        int nWorkersNow; /* number of workers necessary to a current iteration */

        /* MPI variables */
        MPI_Request reqSend[nWorkers], reqRec[nWorkers];
        bool allMsgRec, recVal;
        bool msgRec[nWorkers];
        WorkerTask *sendStruct;
        WorkerResult *recStruct;

        /* Reference handlers */
        TextStruct *file;
        WorkerTask task;
        WorkerResult result;

        /* process command line options */
        int opt; /* selected option */
        do {
            switch ((opt = getopt(argc, argv, "f:w:b:h"))) {
                case 'f':                 /* file */
                    if (optarg[0] == '-') /* filename is missing */
                    {
                        fprintf(stderr, "%s: filename is missing\n", basename(argv[0]));
                        printUsage(basename(argv[0]));
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    if (nFiles + 1 > MAX_FILES) /* Exceeding number of files allowed */
                    {
                        fprintf(stderr, "%s: maximum number of files is %d\n", basename(argv[0]), MAX_FILES);
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    if (!is_file_open(optarg)) { /* Error while opening file */
                        fprintf(stderr, "%s: file %s cannot be open.\n", basename(argv[0]), optarg);
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    if (get_file_size(optarg) > MAX_FILE_SIZE) { /* Exceeding maximum file size */
                        fprintf(stderr, "%s: file %s is too big maximum file size is %d.\n",
                                basename(argv[0]), optarg, MAX_FILE_SIZE);
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    files[nFiles++] = optarg;
                    break;
                case 'b': /* maximum number of bytes per chunk */
                    if (atoi(optarg) <= 0) { /* non-positive number */
                        fprintf(stderr, "%s: non positive number\n", basename(argv[0]));
                        printUsage(basename(argv[0]));
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    if (atoi(optarg) > MAX_CHUNK_BYTES) { /* Exceeding maximum number of bytes per chunk */
                        fprintf(stderr, "%s: chunk size cannot be greater than %d\n", basename(argv[0]),
                                MAX_CHUNK_BYTES);
                        printUsage(basename(argv[0]));
                        workStatus = EXECUTE_ERROR;
                        for (int i = 0; i < nWorkers; i++) {
                            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                     workers[i], 0, MPI_COMM_WORLD);
                        }
                        MPI_Finalize();
                        exit(EXIT_FAILURE);
                    }
                    maxChunkBytes = (int) atoi(optarg);
                    break;
                case 'h': /* help mode */
                    printUsage(basename(argv[0]));
                    workStatus = NO_MORE_WORK;
                    for (int i = 0; i < nWorkers; i++) {
                        MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                 workers[i], 0, MPI_COMM_WORLD);
                    }
                    MPI_Finalize();
                    exit(EXIT_SUCCESS);
                case '?': /* invalid option */
                    fprintf(stderr, "%s: invalid option\n", basename(argv[0]));
                    printUsage(basename(argv[0]));
                    workStatus = EXECUTE_ERROR;
                    for (int i = 0; i < nWorkers; i++) {
                        MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                                 workers[i], 0, MPI_COMM_WORLD);
                    }
                    MPI_Finalize();
                    exit(EXIT_FAILURE);
                case -1:
                    break;
            }
        } while (opt != -1);
        if (optind < argc) {
            fprintf(stderr, "%s: invalid format\n", basename(argv[0]));
            printUsage(basename(argv[0]));
            workStatus = EXECUTE_ERROR;
            for (int i = 0; i < nWorkers; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        /* initialise fileSpace */
        if ((fileSpace = (TextStruct *) malloc(nFiles * sizeof(TextStruct))) == NULL) {
            fprintf(stderr, "error on allocating space to the file data array\n");
            workStatus = EXECUTE_ERROR;
            for (int i = 0; i < nWorkers; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nFiles; i++) {
            file = (fileSpace + i);
            file->path = files[i];
            file->id = i;
            file->results = get_initial_result();

            maxNumTasks += ((int) (get_file_size(file->path)) / maxChunkBytes) + 1;
        }

        /* Generate tasks */
        if ((tmpTaskSpace = (WorkerTask *) malloc(maxNumTasks * sizeof(WorkerTask))) == NULL) {
            fprintf(stderr, "error on allocating space to the tasks data array\n");
            workStatus = EXECUTE_ERROR;
            for (int i = 0; i < nWorkers; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
        nTasks = 0;
        while (filePointer < nFiles) {
            file = (fileSpace + filePointer);
            FILE *fp = fopen(file->path, "rb");
            while (!feof(fp)) {
                task.id = file->id;
                task.chunk = get_chunk(fp, maxChunkBytes);
                tmpTaskSpace[nTasks++] = task;
            }
            filePointer++;
        }
        if ((taskSpace = (WorkerTask *) malloc(nTasks * sizeof(WorkerTask))) == NULL) {
            fprintf(stderr, "error on allocating space to the tasks data array\n");
            workStatus = EXECUTE_ERROR;
            for (int i = 0; i < nWorkers; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < nTasks; i++) {
            taskSpace[i] = tmpTaskSpace[i];
        }
        free(tmpTaskSpace);

        /* distribute tasks */
        tasksPointer = 0;
        (void) get_delta_time();

        if (((sendStruct = malloc(nWorkers * sizeof(WorkerTask))) == NULL) ||
            ((recStruct = malloc(nWorkers * sizeof(WorkerResult))) == NULL)) {
            fprintf(stderr, "error on message box memory allocation \n");
            workStatus = EXECUTE_ERROR;
            for (int i = 0; i < nWorkers; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
            }
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        while (tasksPointer < nTasks) /* process tasks cycle. */
        {
            if (tasksPointer + nWorkers > nTasks) {
                nWorkersNow = nTasks - tasksPointer;
            } else {
                nWorkersNow = nWorkers;
            }
            workStatus = WORK_TO_DO;
            for (int i = 0; i < nWorkersNow; i++) {
                MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                         workers[i], 0, MPI_COMM_WORLD);
                sendStruct[i] = taskSpace[tasksPointer++];
                MPI_Isend(&sendStruct[i], sizeof(WorkerTask), MPI_BYTE,
                          workers[i], 0, MPI_COMM_WORLD, &reqSend[i]);
            }
            for (int i = 0; i < nWorkersNow; i++) {
                MPI_Irecv(&recStruct[i], sizeof(WorkerResult), MPI_BYTE,
                          workers[i], 0, MPI_COMM_WORLD, &reqRec[i]);
                msgRec[i] = false;
            }
            do {
                allMsgRec = true;
                for (int i = 0; i < nWorkersNow; i++) {
                    if (!msgRec[i]) {
                        recVal = false;
                        MPI_Test(&reqRec[i], (int *) &recVal, MPI_STATUS_IGNORE);
                        if (recVal) {
                            msgRec[i] = true;
                            result = recStruct[i];
                            file = (fileSpace + result.id);
                            file->results = reduce(file->results, result.results);
                        } else
                            allMsgRec = false;
                    }
                }

            } while (!allMsgRec);
        }

        workStatus = NO_MORE_WORK; /* Sign workers execution finished */
        for (int i = 0; i < nWorkers; i++) {
            MPI_Send(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                     workers[i], 0, MPI_COMM_WORLD);
        }

        printf("\nElapsed time multi thread = %.6fs\n\n", get_delta_time());
        for (int i = 0; i < nFiles; i++) { /* print results */
            printf("id: %d\n", fileSpace[i].id);
            print_results(files[i], fileSpace[i].results);
            printf("\n");
        }
    } else { /* Worker */
        unsigned int workStatus; /* work status flag variable */
        /* reference handlers */
        WorkerTask task;
        WorkerResult result;
        while (true) { /* work cycle */
            MPI_Recv(&workStatus, sizeof(unsigned int), MPI_UNSIGNED,
                     0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (workStatus != WORK_TO_DO) {
                break;
            }
            MPI_Recv((char *) &task, sizeof(WorkerTask), MPI_BYTE, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
            result.id = task.id;
            result.results = process_chunk(task.chunk);
            MPI_Send((char *) &result, sizeof(WorkerResult), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
        }
        if (workStatus == NO_MORE_WORK) {
            MPI_Finalize();
            exit(EXIT_SUCCESS);
        } else if (workStatus == EXECUTE_ERROR) {
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }
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
    fprintf(stderr,
            "\nSynopsis: %s OPTIONS [-f filename / -w number of workers / -b maximum number of bytes per chunk / -h help]\n"
            "  OPTIONS:\n"
            "  -f      --- filename to process\n"
            "  -w      --- number of workers\n"
            "  -b      --- maximum number of bytes per chunk\n"
            "  -h      --- print this help\n",
            cmdName);
}