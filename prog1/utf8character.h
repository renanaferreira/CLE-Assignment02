#ifndef UTF8_CHAR_H
#define UTF8_CHAR_H

#include <stdio.h>
#include <stdbool.h>

#include "consts.h"
#include "utils.h"

#define MAX_UTF8_CHAR_SIZE 4

typedef struct UTF8Character
{
    unsigned char character[MAX_UTF8_CHAR_SIZE];
    short length;
} UTF8Character;

extern const UTF8Character NULL_CHAR;

bool isEqual(UTF8Character char01, UTF8Character char02);
bool isWordCharacter(UTF8Character character);
bool isAlphanumeric(UTF8Character character);
bool isConsonant(UTF8Character character);
bool isVowel(UTF8Character character);
bool isNumber(UTF8Character character);
bool isVowelA(UTF8Character character);
bool isVowelE(UTF8Character character);
bool isVowelI(UTF8Character character);
bool isVowelO(UTF8Character character);
bool isVowelU(UTF8Character character);
bool isVowelY(UTF8Character character);
bool isMerger(UTF8Character character);
bool isPunctuation(UTF8Character character);
bool isSeparation(UTF8Character character);
bool isWhitespace(UTF8Character character);
bool isUnderscore(UTF8Character character);
bool isCedilla(UTF8Character character);
void printChar(UTF8Character character);
int getCharSize(unsigned char first_byte);

#endif /* UTF8_CHAR_H */