#include "utf8character.h"

const UTF8Character NULL_CHAR = {.length = -1};

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

