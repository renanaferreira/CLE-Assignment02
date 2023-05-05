#ifndef CONSTS_H
#define CONSTS_H

#define MAX_NUMBER_WORKERS 8 /* maximum number of workers */
#define MAX_FILES 6          /* maximum number of files */

typedef struct FifoParameters
{
    int storage_size;
    unsigned int head;
    unsigned int tail;
    unsigned int length;
    
} FifoParameters;

#endif /* CONSTS_H */