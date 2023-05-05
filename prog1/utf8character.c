#include "utf8character.h"

const UTF8Character NULL_CHAR = {.length = -1};

bool is_equal(UTF8Character char01, UTF8Character char02)
{
    if (char01.length == char02.length && char01.length == -1)
        return true;

    if (char01.length != char02.length)
        return false;
    for (int i = 0; i < char01.length; i++)
    {
        if (char01.character[i] != char02.character[i])
            return false;
    }

    return true;
}

bool is_word_char(UTF8Character character)
{
    if (is_whitespace(character))
        return false;
    if (is_separation(character))
        return false;
    if (is_punctuation(character))
        return false;

    if (is_merger(character))
        return true;
    if (is_underscore(character))
    {
        return true;
    }

    return true;
}

bool is_alphanumeric(UTF8Character character)
{
    if (is_consonant(character))
    {
        return true;
    }
    if (is_vowel(character))
    {
        return true;
    }
    if (is_number(character))
    {
        return true;
    }
    return false;
}

bool is_consonant(UTF8Character character)
{
    if (is_vowel(character))
    {
        return false;
    }

    if (character.length == 1)
    {
        // upper
        if (0x41 <= character.character[0] && character.character[0] <= 0x5A)
        {
            return true;
        }
        // lower
        if (0x61 <= character.character[0] && character.character[0] <= 0x7A)
        {
            return true;
        }
    }
    if (is_cedilla(character))
    {
        return true;
    }
    return false;
}

bool is_vowel(UTF8Character character)
{
    if (is_vowel_A(character))
        return true;
    if (is_vowel_E(character))
        return true;
    if (is_vowel_I(character))
        return true;
    if (is_vowel_O(character))
        return true;
    if (is_vowel_U(character))
        return true;
    if (is_vowel_Y(character))
        return true;

    return false;
}

bool is_number(UTF8Character character)
{
    if (character.length != 1)
        return false;

    if (0x30 <= character.character[0] && character.character[0] <= 0x39)
        return true;

    return false;
}

bool is_vowel_A(UTF8Character character)
{
    // Standard A
    if (character.length == 1)
    {
        // A upper
        if (character.character[0] == 0x41)
            return true;
        // A lower
        if (character.character[0] == 0x61)
            return true;
    }

    // A with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // A upper
        if (0x80 <= character.character[1] && character.character[1] <= 0x85)
            return true;
        // A lower
        if (0xA0 <= character.character[1] && character.character[1] <= 0xA5)
            return true;
    }

    return false;
}

bool is_vowel_E(UTF8Character character)
{
    // Standard E
    if (character.length == 1)
    {
        // E upper
        if (character.character[0] == 0x45)
            return true;
        // E lower
        if (character.character[0] == 0x65)
            return true;
    }

    // E with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // E upper
        if (0x88 <= character.character[1] && character.character[1] <= 0x8B)
            return true;
        // E lower
        if (0xA8 <= character.character[1] && character.character[1] <= 0xAB)
            return true;
    }

    return false;
}

bool is_vowel_I(UTF8Character character)
{
    // Standard I
    if (character.length == 1)
    {
        // I upper
        if (character.character[0] == 0x49)
            return true;
        // I lower
        if (character.character[0] == 0x69)
            return true;
    }

    // I with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // I upper
        if (0x8C <= character.character[1] && character.character[1] <= 0x8F)
            return true;
        // I lower
        if (0xAC <= character.character[1] && character.character[1] <= 0xAF)
            return true;
    }

    return false;
}

bool is_vowel_O(UTF8Character character)
{
    // Standard O
    if (character.length == 1)
    {
        // O upper
        if (character.character[0] == 0x4F)
            return true;
        // O lower
        if (character.character[0] == 0x6F)
            return true;
    }

    // O with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // O upper
        if (0x92 <= character.character[1] && character.character[1] <= 0x96)
            return true;
        // O lower
        if (0xB2 <= character.character[1] && character.character[1] <= 0xB6)
            return true;
    }

    return false;
}

bool is_vowel_U(UTF8Character character)
{
    // Standard U
    if (character.length == 1)
    {
        // U upper
        if (character.character[0] == 0x55)
            return true;
        // U lower
        if (character.character[0] == 0x75)
            return true;
    }

    // U with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // U upper
        if (0x99 <= character.character[1] && character.character[1] <= 0x9C)
            return true;
        // U lower
        if (0xB9 <= character.character[1] && character.character[1] <= 0xBC)
            return true;
    }

    return false;
}

bool is_vowel_Y(UTF8Character character)
{
    // Standard Y
    if (character.length == 1)
    {
        // Y upper
        if (character.character[0] == 0x59)
            return true;
        // Y lower
        if (character.character[0] == 0x79)
            return true;
    }

    // Y with accents
    if (character.length == 2 && character.character[0] == 0xC3)
    {
        // Y upper acute
        if (character.character[1] == 0x9D)
            return true;
        // Y lower acute
        if (character.character[1] == 0xBD)
            return true;
        // Y lower diaeresis
        if (character.character[1] == 0xBF)
            return true;
    }

    return false;
}

bool is_merger(UTF8Character character)
{
    if (character.length == 1)
    {
        // apostrophe
        if (character.character[0] == 0x27)
        {
            return true;
        }
        // grave accent
        if (character.character[0] == 0x60)
        {
            return true;
        }
    }
    if (character.length == 3)
    {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80))
        {
            // left single quotation mark
            if (character.character[2] == 0x98)
            {
                return true;
            }
            // right single quotation mark
            if (character.character[2] == 0x99)
            {
                return true;
            }
        }
    }

    return false;
}

bool is_punctuation(UTF8Character character)
{
    if (character.length == 1)
    {
        // exclamation mark
        if (character.character[0] == 0x21)
        {
            return true;
        }
        // comma
        if (character.character[0] == 0x2C)
        {
            return true;
        }
        // full point
        if (character.character[0] == 0x2E)
        {
            return true;
        }
        // colon
        if (character.character[0] == 0x3A)
        {
            return true;
        }
        // semicolon
        if (character.character[0] == 0x3B)
        {
            return true;
        }
        // question mark
        if (character.character[0] == 0x3F)
        {
            return true;
        }
    }
    if (character.length == 3)
    {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80))
        {
            // en dash
            if (character.character[2] == 0x93)
            {
                return true;
            }
            // em dash
            if (character.character[2] == 0x94)
            {
                return true;
            }
            // horizontal ellipsis
            if (character.character[2] == 0xA6)
            {
                return true;
            }
        }
    }

    return false;
}

bool is_separation(UTF8Character character)
{
    if (character.length == 1)
    {
        // quotation mark
        if (character.character[0] == 0x22)
        {
            return true;
        }
        // left parenthesis
        if (character.character[0] == 0x28)
        {
            return true;
        }
        // right parenthesis
        if (character.character[0] == 0x29)
        {
            return true;
        }
        // hyphen minus
        if (character.character[0] == 0x2D)
        {
            return true;
        }
        // left square bracket
        if (character.character[0] == 0x5B)
        {
            return true;
        }
        // right square bracket
        if (character.character[0] == 0x5D)
        {
            return true;
        }
    }
    if (character.length == 2)
    {
        if (character.character[0] == 0xC2)
        {
            // left angle quotation
            if (character.character[1] == 0xAB)
            {
                return true;
            }
            // right angle quotation
            if (character.character[1] == 0xBB)
            {
                return true;
            }
        }
    }
    if (character.length == 3)
    {
        if ((character.character[0] = 0xE2) && (character.character[1] = 0x80))
        {
            // left double quotation mark
            if (character.character[2] == 0x9C)
            {
                return true;
            }
            // right double quotation mark
            if (character.character[2] == 0x9D)
            {
                return true;
            }
        }
    }

    return false;
}

bool is_whitespace(UTF8Character character)
{
    if (character.length == 1)
    {
        // tab
        if (character.character[0] == 0x9)
        {
            return true;
        }
        // newline
        if (character.character[0] == 0xA)
        {
            return true;
        }
        // carriage return
        if (character.character[0] == 0xD)
        {
            return true;
        }
        // space
        if (character.character[0] == 0x20)
        {
            return true;
        }
    }

    return false;
}

bool is_underscore(UTF8Character character)
{
    // Underscore
    if (character.length == 1)
    {
        if (character.character[0] == 0x5F)
        {
            return true;
        }
    }

    return false;
}

bool is_cedilla(UTF8Character character)
{
    if (character.length == 2)
    {
        if (character.character[0] == 0xC3)
        {
            // C cedilla Upper
            if (character.character[1] == 0x87)
            {
                return true;
            }
            // C cedilla Lower
            if (character.character[1] == 0xA7)
            {
                return true;
            }
        }
    }

    return false;
}

int get_char_size(unsigned char first_byte)
{
    if ((1 << 7) & first_byte)
    {
        if ((1 << 6) & first_byte)
        {
            if ((1 << 5) & first_byte)
            {
                if ((1 << 4) & first_byte)
                {
                    if ((1 << 3) & first_byte)
                    {
                        // 11110xxx
                        return 4;
                    }
                }
                else
                {
                    // 1110xxxx
                    return 3;
                }
            }
            else
            {
                // 110xxxxx
                return 2;
            }
        }
    }
    else
    {
        // 0xxxxxxx
        return 1;
    }

    return -1;
}

