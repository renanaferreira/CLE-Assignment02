#include "textProcessing.h"

typedef struct UTF8Character {
    unsigned char character[MAX_UTF8_CHAR_SIZE];
    short length;
} UTF8Character;

typedef struct TextPartialResult {
    TextResult results;
    bool inWord;
    bool vowelPresence[TOTAL_VOWELS];
} TextPartialResult;

static bool isEqual(UTF8Character char01, UTF8Character char02);
static bool isWordCharacter(UTF8Character character);
static bool isAlphanumeric(UTF8Character character);
static bool isConsonant(UTF8Character character);
static bool isVowel(UTF8Character character);
static bool isNumber(UTF8Character character);
static bool isVowelA(UTF8Character character);
static bool isVowelE(UTF8Character character);
static bool isVowelI(UTF8Character character);
static bool isVowelO(UTF8Character character);
static bool isVowelU(UTF8Character character);
static bool isVowelY(UTF8Character character);
static bool isMerger(UTF8Character character);
static bool isPunctuation(UTF8Character character);
static bool isSeparation(UTF8Character character);
static bool isWhitespace(UTF8Character character);
static bool isUnderscore(UTF8Character character);
static bool isCedilla(UTF8Character character);
static void printChar(UTF8Character character);
static int getCharSize(unsigned char first_byte);

static TextPartialResult get_initial_partial_result();
static int get_vowel_idx(UTF8Character character);
static UTF8Character get_char(Chunk *chunk);
static void process_char(UTF8Character character, TextPartialResult *results);

static const UTF8Character NULL_CHAR = {.length = -1};

bool isEqual(UTF8Character char01, UTF8Character char02) {
    if (char01.length == char02.length && char01.length == -1) {
        return true;
    }
    if (char01.length != char02.length) {
        return false;
    }
    for (int i = 0; i < char01.length; i++) {
        if (char01.character[i] != char02.character[i]) {
            return false;
        }
    }
    return true;
}

bool isWordCharacter(UTF8Character character) {
    if (isWhitespace(character))
        return false;
    if (isSeparation(character))
        return false;
    if (isPunctuation(character))
        return false;

    if (isMerger(character))
        return true;
    if (isUnderscore(character)) {
        return true;
    }

    return true;
}

bool isAlphanumeric(UTF8Character character) {
    if (isConsonant(character)) {
        return true;
    }
    if (isVowel(character)) {
        return true;
    }
    if (isNumber(character)) {
        return true;
    }
    return false;
}

bool isConsonant(UTF8Character character) {
    if (isVowel(character)) {
        return false;
    }
    if (isCedilla(character)) {
        return true;
    }

    if (character.length == 1) {
        if (0x41 <= character.character[0] && character.character[0] <= 0x5A) { // upper
            return true;
        }
        if (0x61 <= character.character[0] && character.character[0] <= 0x7A) { // lower
            return true;
        }
    }
    return false;
}

bool isVowel(UTF8Character character) {
    if (isVowelA(character))
        return true;
    if (isVowelE(character))
        return true;
    if (isVowelI(character))
        return true;
    if (isVowelO(character))
        return true;
    if (isVowelU(character))
        return true;
    if (isVowelY(character))
        return true;

    return false;
}

bool isNumber(UTF8Character character) {
    if (character.length != 1)
        return false;

    if (0x30 <= character.character[0] && character.character[0] <= 0x39)
        return true;

    return false;
}

bool isVowelA(UTF8Character character) {
    // Standard A
    if (character.length == 1) {
        // A upper
        if (character.character[0] == 0x41)
            return true;
        // A lower
        if (character.character[0] == 0x61)
            return true;
    }

    // A with accents
    if (character.length == 2 && character.character[0] == 0xC3) {
        // A upper
        if (0x80 <= character.character[1] && character.character[1] <= 0x85)
            return true;
        // A lower
        if (0xA0 <= character.character[1] && character.character[1] <= 0xA5)
            return true;
    }
    return false;
}

bool isVowelE(UTF8Character character) {
    if (character.length == 1) { // Standard E
        if (character.character[0] == 0x45) { // E upper
            return true;
        }
        if (character.character[0] == 0x65) { // E lower
            return true;
        }
    }
    if (character.length == 2 && character.character[0] == 0xC3) { // E with accents
        if (0x88 <= character.character[1] && character.character[1] <= 0x8B) { // E upper
            return true;
        }
        if (0xA8 <= character.character[1] && character.character[1] <= 0xAB) { // E lower
            return true;
        }
    }
    return false;
}

bool isVowelI(UTF8Character character) {
    // Standard I
    if (character.length == 1) {
        // I upper
        if (character.character[0] == 0x49)
            return true;
        // I lower
        if (character.character[0] == 0x69)
            return true;
    }

    // I with accents
    if (character.length == 2 && character.character[0] == 0xC3) {
        // I upper
        if (0x8C <= character.character[1] && character.character[1] <= 0x8F)
            return true;
        // I lower
        if (0xAC <= character.character[1] && character.character[1] <= 0xAF)
            return true;
    }

    return false;
}

bool isVowelO(UTF8Character character) {
    // Standard O
    if (character.length == 1) {
        // O upper
        if (character.character[0] == 0x4F)
            return true;
        // O lower
        if (character.character[0] == 0x6F)
            return true;
    }

    // O with accents
    if (character.length == 2 && character.character[0] == 0xC3) {
        // O upper
        if (0x92 <= character.character[1] && character.character[1] <= 0x96)
            return true;
        // O lower
        if (0xB2 <= character.character[1] && character.character[1] <= 0xB6)
            return true;
    }

    return false;
}

bool isVowelU(UTF8Character character) {
    // Standard U
    if (character.length == 1) {
        // U upper
        if (character.character[0] == 0x55)
            return true;
        // U lower
        if (character.character[0] == 0x75)
            return true;
    }

    // U with accents
    if (character.length == 2 && character.character[0] == 0xC3) {
        // U upper
        if (0x99 <= character.character[1] && character.character[1] <= 0x9C)
            return true;
        // U lower
        if (0xB9 <= character.character[1] && character.character[1] <= 0xBC)
            return true;
    }

    return false;
}

bool isVowelY(UTF8Character character) {
    // Standard Y
    if (character.length == 1) {
        // Y upper
        if (character.character[0] == 0x59)
            return true;
        // Y lower
        if (character.character[0] == 0x79)
            return true;
    }


    if (character.length == 2 && character.character[0] == 0xC3) { // Y with accents

        if (character.character[1] == 0x9D) { // Y upper acute
            return true;
        }
        if (character.character[1] == 0xBD) { // Y lower acute
            return true;
        }
        if (character.character[1] == 0xBF) { // Y lower diaeresis
            return true;
        }
    }

    return false;
}

bool isMerger(UTF8Character character) {
    if (character.length == 1) {
        if (character.character[0] == 0x27) { // apostrophe
            return true;
        }
        if (character.character[0] == 0x60) { // grave accent
            return true;
        }
    }
    if (character.length == 3) {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80)) {
            if (character.character[2] == 0x98) { // left single quotation mark
                return true;
            }
            if (character.character[2] == 0x99) { // right single quotation mark
                return true;
            }
        }
    }
    return false;
}

bool isPunctuation(UTF8Character character) {
    if (character.length == 1) {
        if (character.character[0] == 0x21) { // exclamation mark
            return true;
        }
        if (character.character[0] == 0x2C) { // comma
            return true;
        }
        if (character.character[0] == 0x2E) { // full point
            return true;
        }
        if (character.character[0] == 0x3A) { // colon
            return true;
        }
        if (character.character[0] == 0x3B) { // semicolon
            return true;
        }
        if (character.character[0] == 0x3F) { // question mark
            return true;
        }
    }
    if (character.length == 3) {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80)) {
            if (character.character[2] == 0x93) { // en dash
                return true;
            }
            if (character.character[2] == 0x94) { // em dash
                return true;
            }
            if (character.character[2] == 0xA6) { // horizontal ellipsis
                return true;
            }
        }
    }
    return false;
}

bool isSeparation(UTF8Character character) {
    if (character.length == 1) {
        // quotation mark
        if (character.character[0] == 0x22) {
            return true;
        }
        // left parenthesis
        if (character.character[0] == 0x28) {
            return true;
        }
        // right parenthesis
        if (character.character[0] == 0x29) {
            return true;
        }
        // hyphen minus
        if (character.character[0] == 0x2D) {
            return true;
        }
        // left square bracket
        if (character.character[0] == 0x5B) {
            return true;
        }
        // right square bracket
        if (character.character[0] == 0x5D) {
            return true;
        }
    }
    if (character.length == 2) {
        if (character.character[0] == 0xC2) {
            // left angle quotation
            if (character.character[1] == 0xAB) {
                return true;
            }
            // right angle quotation
            if (character.character[1] == 0xBB) {
                return true;
            }
        }
    }
    if (character.length == 3) {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80)) {
            // left double quotation mark
            if (character.character[2] == 0x9C) {
                return true;
            }
            // right double quotation mark
            if (character.character[2] == 0x9D) {
                return true;
            }
        }
    }

    return false;
}

bool isWhitespace(UTF8Character character) {
    if (character.length == 1) {
        // tab
        if (character.character[0] == 0x9) {
            return true;
        }
        // newline
        if (character.character[0] == 0xA) {
            return true;
        }
        // carriage return
        if (character.character[0] == 0xD) {
            return true;
        }
        // space
        if (character.character[0] == 0x20) {
            return true;
        }
    }

    return false;
}

bool isUnderscore(UTF8Character character) {
    // Underscore
    if (character.length == 1) {
        if (character.character[0] == 0x5F) {
            return true;
        }
    }

    return false;
}

bool isCedilla(UTF8Character character) {
    if (character.length == 2) {
        if (character.character[0] == 0xC3) {
            // C cedilla Upper
            if (character.character[1] == 0x87) {
                return true;
            }
            // C cedilla Lower
            if (character.character[1] == 0xA7) {
                return true;
            }
        }
    }

    return false;
}

int getCharSize(unsigned char first_byte) {
    if ((1 << 7) & first_byte) {
        if ((1 << 6) & first_byte) {
            if ((1 << 5) & first_byte) {
                if ((1 << 4) & first_byte) {
                    if ((1 << 3) & first_byte) { // 11110xxx
                        return 4;
                    }
                } else { // 1110xxxx
                    return 3;
                }
            } else { // 110xxxxx
                return 2;
            }
        }
    } else { // 0xxxxxxx
        return 1;
    }
    return -1;
}

void printChar(UTF8Character character) {
    printf("0x");
    for (int i = 0; i < character.length; i++) {
        printf("%x", character.character[i]);
    }
}

TextResult get_initial_result() {
    TextResult result;
    result.nWords = 0;
    for (int i = 0; i < TOTAL_VOWELS; i++) {
        result.nWordsVowel[i] = 0;
    }
    return result;
}

TextPartialResult get_initial_partial_result() {
    TextPartialResult partialResult;
    partialResult.inWord = false;
    for (int i = 0; i < TOTAL_VOWELS; i++) {
        partialResult.vowelPresence[i] = false;
    }
    partialResult.results = get_initial_result();
    return partialResult;
}

int get_vowel_idx(UTF8Character character) {
    if (isVowelA(character))
        return A_IDX;
    if (isVowelE(character))
        return E_IDX;
    if (isVowelI(character))
        return I_IDX;
    if (isVowelO(character))
        return O_IDX;
    if (isVowelU(character))
        return U_IDX;
    if (isVowelY(character))
        return Y_IDX;

    return -1;
}

void print_results(char *filename, TextResult results) {
    printf("File name: %s\n", filename);
    printf("Total number of words = %d\n", results.nWords);
    printf("N. of words with an:\n");
    printf("A\tE\tI\tO\tU\tY\n");

    printf("%d", results.nWordsVowel[0]);
    for (int i = 1; i < TOTAL_VOWELS; i++) {
        printf("\t%d", results.nWordsVowel[i]);
    }
    printf("\n");
}

TextResult reduce(TextResult first, TextResult second) {
    TextResult result;
    result.nWords = first.nWords + second.nWords;
    for (int i = 0; i < TOTAL_VOWELS; i++) {
        result.nWordsVowel[i] = first.nWordsVowel[i] + second.nWordsVowel[i];
    }
    return result;
}

TextResult process_chunk(Chunk chunk) {
    UTF8Character character;
    TextPartialResult result = get_initial_partial_result();

    chunk.position = 0;
    while (true) {
        character = get_char(&chunk);
        if (isEqual(character, (UTF8Character) NULL_CHAR)) {
            break;
        }
        process_char(character, &result);
    }
    return result.results;
}

UTF8Character get_char(Chunk *chunk) {
    if (chunk->position == chunk->length) {
        return NULL_CHAR;
    }

    if (chunk->position > chunk->length) {
        fprintf(stderr, "Cannot get character by position %d, above chunk length%d\n", chunk->position, chunk->length);
        exit(EXIT_FAILURE);
    }

    int length = getCharSize(chunk->bytes[chunk->position]);
    if (length == -1) {
        fprintf(stderr, "get_char(): Character is defected. byte 0x%x. chunk length %d. chunk position %d\n",
                chunk->bytes[chunk->position], chunk->length, chunk->position);
        exit(EXIT_FAILURE);
    }
    if (chunk->position + length > chunk->length) {
        fprintf(stderr, "Chunk is defected. character to read has size above chunk length.\n");
        exit(EXIT_FAILURE);
    }

    UTF8Character character;
    character.length = length;
    for (int i = 0; i < character.length; i++) {
        character.character[i] = chunk->bytes[chunk->position++];
    }
    return character;
}

void process_char(UTF8Character character, TextPartialResult *results) {
    int vowelIdx;

    if (results->inWord) {
        if (isWordCharacter(character)) {
            if (isVowel(character)) {
                vowelIdx = get_vowel_idx(character);
                if (!results->vowelPresence[vowelIdx]) {
                    results->results.nWordsVowel[vowelIdx]++;
                    results->vowelPresence[vowelIdx] = true;
                }
            }
        } else {
            results->inWord = false;
        }
    } else {
        if (isWordCharacter(character)) {
            results->inWord = true;
            results->results.nWords++;
            set_to_false(TOTAL_VOWELS, results->vowelPresence);
            if (isVowel(character)) {
                vowelIdx = get_vowel_idx(character);
                results->results.nWordsVowel[vowelIdx]++;
                results->vowelPresence[vowelIdx] = true;
            }
        }
    }
}

Chunk get_chunk(FILE *fp, int maxChunkBytes) {
    UTF8Character character;
    unsigned char byte;

    Chunk chunk;
    chunk.length = 0;

    bool inWord = false;
    int word_size = 0;

    int count = 0;

    while (true) {
        if (feof(fp) || chunk.length == maxChunkBytes) {
            break;
        }

        if (fread(&byte, sizeof(unsigned char), 1, fp) != 1) {
            if (feof(fp)) {
                break;
            }

            if (ferror(fp)) {
                fprintf(stderr, "Unexpected end of file. There is something wrong\n");
                exit(EXIT_FAILURE);
            }
        }
        fseek(fp, -1L, SEEK_CUR);
        character.length = getCharSize(byte);
        if (character.length == -1) {
            fprintf(stderr, "unexpected utf8 byte %x. File is compromised and not correct written in utf8\n", byte);
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < character.length; i++) {
            if (fread(&character.character[i], sizeof(unsigned char), 1, fp) != 1) {
                if (feof(fp)) {
                    fprintf(stderr, "Unexpected end of file. File is compromised and not correct written in utf8\n");
                    exit(EXIT_FAILURE);
                } else if (ferror(fp)) {
                    fprintf(stderr, "Unexpected end of file. There is something wrong\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
        fseek(fp, (long) -character.length, SEEK_CUR);

        if ((chunk.length + character.length >= maxChunkBytes)) {
            break;
        }

        fseek(fp, (long) character.length, SEEK_CUR);
        for (int i = 0; i < character.length; i++) {
            chunk.bytes[chunk.length++] = character.character[i];
        }

        if (isWordCharacter(character)) {
            if (!inWord) {
                inWord = true;
            }
            word_size += character.length;
        } else {
            if (inWord) {
                inWord = false;
                word_size = 0;
            }
        }
        count++;
    }
    if (inWord) {
        chunk.length = chunk.length - word_size;
        fseek(fp, (long) -word_size, SEEK_CUR);
    }
    return chunk;
}