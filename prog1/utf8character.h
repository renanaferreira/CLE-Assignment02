#ifndef UTF8_CHAR_H
#define UTF8_CHAR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

#include "consts.h"
#include "utils.h"

#define MAX_UTF8_CHAR_SIZE 4

typedef struct UTF8Character
{
    unsigned char character[MAX_UTF8_CHAR_SIZE];
    short length;
} UTF8Character;

extern const UTF8Character NULL_CHAR;

extern bool is_equal(UTF8Character char01, UTF8Character char02);

extern bool is_word_char(UTF8Character character);

extern bool is_alphanumeric(UTF8Character character);
extern bool is_consonant(UTF8Character character);
extern bool is_vowel(UTF8Character character);
extern bool is_number(UTF8Character character);

extern bool is_vowel_A(UTF8Character character);
extern bool is_vowel_E(UTF8Character character);
extern bool is_vowel_I(UTF8Character character);
extern bool is_vowel_O(UTF8Character character);
extern bool is_vowel_U(UTF8Character character);
extern bool is_vowel_Y(UTF8Character character);

extern bool is_merger(UTF8Character character);
extern bool is_punctuation(UTF8Character character);
extern bool is_separation(UTF8Character character);
extern bool is_whitespace(UTF8Character character);

extern bool is_underscore(UTF8Character character);
extern bool is_cedilla(UTF8Character character);

extern int get_char_size(unsigned char first_byte);
extern void print_char(UTF8Character character);

#endif /* UTF8_CHAR_H */