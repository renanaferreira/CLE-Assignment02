#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "consts.h"
#include "utils.h"
#include "utf8character.h"

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
extern TextResult textProcess(char *filepath, int maxChunkBytes);
extern Chunk get_chunk(FILE *fp, int maxChunkBytes);
extern TextResult process_chunk(Chunk chunk);
extern TextResult reduce(TextResult result01, TextResult result02);
extern void print_results(char *path, TextResult results);
extern void print_char(UTF8Character character);
#endif /* TEXT_PROCESSING_H */
