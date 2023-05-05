#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "consts.h"
#include "utils.h"

#define MAX_UTF8_CHAR_SIZE 4

/** \brief define the total number of vowels */
#define TOTAL_VOWELS 6

/** \brief A vowel index */
#define A_IDX 0
/** \brief E vowel index */
#define E_IDX 1
/** \brief I vowel index */
#define I_IDX 2
/** \brief O vowel index */
#define O_IDX 3
/** \brief U vowel index */
#define U_IDX 4
/** \brief Y vowel index */
#define Y_IDX 5

typedef struct TextResult
{
    int nWords;
    int nWordsVowel[TOTAL_VOWELS];

} TextResult;

typedef struct Chunk
{
    int length;
    int position;
    unsigned char bytes[MAX_CHUNK_BYTES];
} Chunk;

extern TextResult get_initial_result();
extern Chunk get_chunk(FILE *fp, int maxChunkBytes);
extern TextResult process_chunk(Chunk chunk);
extern TextResult reduce(TextResult result01, TextResult result02);
extern void print_results(char *path, TextResult results);

#endif /* TEXT_PROCESSING_H */
