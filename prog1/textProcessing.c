#include "textProcessing.h"

typedef struct TextPartialResult
{
    TextResult results;

    bool inWord;
    bool vowelPresence[TOTAL_VOWELS];

} TextPartialResult;

static TextPartialResult get_initial_partial_result();
static int get_vowel_idx(UTF8Character character);
static UTF8Character get_char(Chunk *chunk);
static void process_char(UTF8Character character, TextPartialResult *results);

TextResult get_initial_result()
{
    TextResult result;
    result.nWords = 0;
    for (int i = 0; i < TOTAL_VOWELS; i++)
    {
        result.nWordsVowel[i] = 0;
    }
    return result;
}

TextPartialResult get_initial_partial_result()
{
    TextPartialResult partialResult;
    partialResult.inWord = false;
    for (int i = 0; i < TOTAL_VOWELS; i++)
    {
        partialResult.vowelPresence[i] = false;
    }
    partialResult.results = get_initial_result();
    return partialResult;
}

int get_vowel_idx(UTF8Character character)
{
    if (is_vowel_A(character))
        return A_IDX;
    if (is_vowel_E(character))
        return E_IDX;
    if (is_vowel_I(character))
        return I_IDX;
    if (is_vowel_O(character))
        return O_IDX;
    if (is_vowel_U(character))
        return U_IDX;
    if (is_vowel_Y(character))
        return Y_IDX;

    return -1;
}

void print_results(char *filename, TextResult results)
{
    printf("File name: %s\n", filename);
    printf("Total number of words = %d\n", results.nWords);
    printf("N. of words with an:\n");
    printf("A\tE\tI\tO\tU\tY\n");

    printf("%d", results.nWordsVowel[0]);
    for (int i = 1; i < TOTAL_VOWELS; i++)
    {
        printf("\t%d", results.nWordsVowel[i]);
    }
    printf("\n");
}

void print_char(UTF8Character character)
{
    printf("0x");
    for (int i = 0; i < character.length; i++)
    {
        printf("%x", character.character[i]);
    }
}

TextResult reduce(TextResult first, TextResult second)
{
    TextResult result;
    result.nWords = first.nWords + second.nWords;
    for (int i = 0; i < TOTAL_VOWELS; i++)
    {
        result.nWordsVowel[i] = first.nWordsVowel[i] + second.nWordsVowel[i];
    }
    return result;
}

TextResult process_chunk(Chunk chunk)
{
    UTF8Character character;
    TextPartialResult result = get_initial_partial_result();

    chunk.position = 0;
    while (true)
    {
        character = get_char(&chunk);
        if (is_equal(character, (UTF8Character)NULL_CHAR))
        {
            break;
        }
        process_char(character, &result);
    }
    return result.results;
}

UTF8Character get_char(Chunk *chunk)
{
    if (chunk->position == chunk->length)
    {
        return NULL_CHAR;
    }

    if (chunk->position > chunk->length)
    {
        fprintf(stderr, "Cannot get character by position %d, above chunk length%d\n", chunk->position, chunk->length);
        exit(EXIT_FAILURE);
    }

    int length = get_char_size(chunk->bytes[chunk->position]);
    if (length == -1)
    {
        fprintf(stderr, "get_char(): Character is defected. byte 0x%x. chunk length %d. chunk position %d\n", chunk->bytes[chunk->position], chunk->length, chunk->position);
        exit(EXIT_FAILURE);
    }
    if (chunk->position + length > chunk->length)
    {
        fprintf(stderr, "Chunk is defected. character to read has size above chunk length.\n");
        exit(EXIT_FAILURE);
    }

    UTF8Character character;
    character.length = length;
    for (int i = 0; i < character.length; i++)
    {
        character.character[i] = chunk->bytes[chunk->position++];
    }
    return character;
}

void process_char(UTF8Character character, TextPartialResult *results)
{
    int vowelIdx;

    if (results->inWord)
    {
        if (is_word_char(character))
        {
            if (is_vowel(character))
            {
                vowelIdx = get_vowel_idx(character);
                if (!results->vowelPresence[vowelIdx])
                {
                    results->results.nWordsVowel[vowelIdx]++;
                    results->vowelPresence[vowelIdx] = true;
                }
            }
        }
        else
        {
            results->inWord = false;
        }
    }
    else
    {
        if (is_word_char(character))
        {
            results->inWord = true;
            results->results.nWords++;
            set_to_false(TOTAL_VOWELS, results->vowelPresence);

            if (is_vowel(character))
            {
                vowelIdx = get_vowel_idx(character);
                results->results.nWordsVowel[vowelIdx]++;
                results->vowelPresence[vowelIdx] = true;
            }
        }
    }
}

Chunk get_chunk(FILE *fp, int maxChunkBytes)
{
    UTF8Character character;
    unsigned char byte;

    Chunk chunk;
    chunk.length = 0;

    bool inWord = false;
    int word_size = 0;

    int count = 0;

    while (true)
    {
        if (feof(fp) || chunk.length == maxChunkBytes)
        {
            break;
        }

        if (fread(&byte, sizeof(unsigned char), 1, fp) != 1)
        {
            if (feof(fp))
            {
                break;
            }

            if (ferror(fp))
            {
                fprintf(stderr, "Unexpected end of file. There is something wrong\n");
                exit(EXIT_FAILURE);
            }
        }
        fseek(fp, -1L, SEEK_CUR);
        character.length = get_char_size(byte);
        if (character.length == -1)
        {
            fprintf(stderr, "unexpected utf8 byte %x. File is compromised and not correct written in utf8\n", byte);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < character.length; i++)
        {
            if (fread(&character.character[i], sizeof(unsigned char), 1, fp) != 1)
            {
                if (feof(fp))
                {
                    fprintf(stderr, "Unexpected end of file. File is compromised and not correct written in utf8\n");
                    exit(EXIT_FAILURE);
                }
                else if (ferror(fp))
                {
                    fprintf(stderr, "Unexpected end of file. There is something wrong\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        fseek(fp, (long)-character.length, SEEK_CUR);

        if ((chunk.length + character.length >= maxChunkBytes))
        {
            break;
        }

        fseek(fp, (long)character.length, SEEK_CUR);
        for (int i = 0; i < character.length; i++)
        {
            chunk.bytes[chunk.length++] = character.character[i];
        }

        if (is_word_char(character))
        {
            if (!inWord)
            {
                inWord = true;
            }
            word_size += character.length;
        }
        else
        {
            if (inWord)
            {
                inWord = false;
                word_size = 0;
            }
        }
        count++;
    }
    if (inWord)
    {
        chunk.length = chunk.length - word_size;
        fseek(fp, (long)-word_size, SEEK_CUR);
    }
    return chunk;
}

TextResult textProcess(char *filepath, int maxChunkBytes)
{
    Chunk chunk;
    TextResult final = get_initial_result();

    FILE *fp = fopen(filepath, "rb");
    while (!feof(fp))
    {
        chunk = get_chunk(fp, maxChunkBytes);
        final = reduce(final, process_chunk(chunk));
    }
    fclose(fp);

    return final;
}