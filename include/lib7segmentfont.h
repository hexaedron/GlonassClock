#pragma once

// Please note that this font is for common anode displays.
// To use it with common cathode, you should invert chars
// like this: ~numberSegments[3]   for example

// We use these to handle the final array lengths correctly
// in both cases:
// 1. We define ENABLE_UNPRINTABLE_CHARS
// 2. We don't define ENABLE_UNPRINTABLE_CHARS and thus save 7 or 21 bytes
//    depending on what font variant (default or ALPHANUMERIC_ONLY) we use.
#if !defined(ENABLE_UNPRINTABLE_CHARS)
    #define UNPRINTABLE_CHARS_FULL 21
    #define UNPRINTABLE_CHARS_ALPHANUMERIC 7
#else
    #define UNPRINTABLE_CHARS_FULL 0
    #define UNPRINTABLE_CHARS_ALPHANUMERIC 0
#endif

// Please note that we put the numberSegments array to progmem to save RAM

#if !(defined(NUMBERS_ONLY) || defined(ALPHANUMERIC_ONLY)) // Digits 0-9 + A-Z + Cyrillic А-Я
    #define  FONT_ARRAY_LENGTH_RAW 70
    #define  FONT_ARRAY_LENGTH (FONT_ARRAY_LENGTH_RAW - UNPRINTABLE_CHARS_FULL)
    const PROGMEM byte numberSegmentsFont[FONT_ARRAY_LENGTH] = 
    {
        0b11111111, // SPACE
        0b11111101, // - (minus sign)
        0b11111110, // . Dot (DP)
        0b00000011, // 0
        0b10011111, // 1
        0b00100101, // 2
        0b00001101, // 3
        0b10011001, // 4
        0b01001001, // 5
        0b01000001, // 6
        0b00011111, // 7
        0b00000001, // 8
        0b00001001, // 9
        0b00010001, // A
        0b11000001, // B
        0b01100011, // С
        0b10000101, // D
        0b01100001, // E
        0b01110001, // F
        0b01000001, // G
        0b10010001, // H
        0b11110011, // I
        0b10001111, // J
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // K (blank, can't display)
        #endif
        0b11100011, // L
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // M (blank, can't display)
        #endif
        0b11010101, // N 
        0b11000101, // O
        0b00110001, // P
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Q (blank, can't display)
        #endif
        0b11110101, // R
        0b01001001, // S
        0b11100001, // T
        0b10000011, // U
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // V (blank, can't display)
            0b11111111, // W (blank, can't display)
            0b11111111, // X (blank, can't display)
        #endif
        0b10001001, // Y
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Z (blank, can't display)
        #endif
        0b00010001, // А (From now on, Cyrillic letters start)
        0b01000001, // Б
        0b00000001, // В
        0b01110011, // Г
        0b10000101, // Д
        0b01100001, // E
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Ж (blank, can't display)
        #endif
        0b00001101, // З
        0b10000011, // И
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // К (blank, can't display)
        #endif
        0b00010011, // Л
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // М (blank, can't display)
        #endif
        0b10010001, // Н
        0b11000101, // О
        0b11010101, // П
        0b00110001, // Р
        0b01100011, // С
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Т (blank, can't display)
        #endif
        0b10001001, // У
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Ф (blank, can't display)
            0b11111111, // Х (blank, can't display)
            0b11111111, // Ц (blank, can't display)
        #endif
        0b10011001, // Ч
        #if (defined ENABLE_UNPRINTABLE_CHARS) 
            0b11111111, // Ш (blank, can't display)
            0b11111111, // Щ (blank, can't display)
            0b11111111, // Ъ (blank, can't display)
            0b11111111, // Ы (blank, can't display)
            0b11111111, // Ь (blank, can't display)
        #endif
        0b00001101 // Э
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            , // Comma is not a typo here
            0b11111111, // Ю (blank, can't display)
            0b11111111 // Я (blank, can't display)
        #endif
    };
#endif

#if (defined NUMBERS_ONLY) && !(defined ALPHANUMERIC_ONLY) // Digits 0-9 + A-F only
    #define  FONT_ARRAY_LENGTH 19
    const PROGMEM byte numberSegmentsFont[FONT_ARRAY_LENGTH] = 
    {
        0b11111111, // SPACE
        0b11111101, // - (minus sign)
        0b11111110, // . Dot (DP)
        0b00000011, // 0
        0b10011111, // 1
        0b00100101, // 2
        0b00001101, // 3
        0b10011001, // 4
        0b01001001, // 5
        0b01000001, // 6
        0b00011111, // 7
        0b00000001, // 8
        0b00001001, // 9
        0b00010001, // A
        0b11000001, // B
        0b01101011, // С
        0b10001011, // D
        0b01100001, // E
        0b01110001  // F
    };
#endif

#if (!defined NUMBERS_ONLY) && (defined ALPHANUMERIC_ONLY) // Digits 0-9 + A-Z
    #define  FONT_ARRAY_LENGTH_RAW 39
    #define  FONT_ARRAY_LENGTH (FONT_ARRAY_LENGTH_RAW - UNPRINTABLE_CHARS_ALPHANUMERIC)
    const PROGMEM byte numberSegmentsFont[FONT_ARRAY_LENGTH] = 
    {
        0b11111111, // SPACE
        0b11111101, // - (minus sign)
        0b11111110, // . Dot (DP)
        0b00000011, // 0
        0b10011111, // 1
        0b00100101, // 2
        0b00001101, // 3
        0b10011001, // 4
        0b01001001, // 5
        0b01000001, // 6
        0b00011111, // 7
        0b00000001, // 8
        0b00001001, // 9
        0b00010001, // A
        0b11000001, // B
        0b01101011, // С
        0b10001011, // D
        0b01100001, // E
        0b01110001, // F
        0b01000001, // G
        0b10010001, // H
        0b11110011, // I
        0b10001111, // J
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // K (blank, can't display)
        #endif
        0b11100011, // L
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // M (blank, can't display)
        #endif
        0b11011101, // N 
        0b11000101, // O
        0b00110001, // P
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // Q (blank, can't display)
        #endif
        0b11110101, // R
        0b01001001, // S
        0b11100001, // T
        0b10000011, // U
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            0b11111111, // V (blank, can't display)
            0b11111111, // W (blank, can't display)
            0b11111111, // X (blank, can't display)
        #endif
        0b10001001 // Y
        #if (defined ENABLE_UNPRINTABLE_CHARS)
            , // Comma is not a typo here
            0b11111111 // Z (blank, can't display)
        #endif
    };
#endif

