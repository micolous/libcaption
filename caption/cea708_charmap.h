#ifndef LIBCAPTION_CEA708_CHARMAP
#define LIBCAPTION_CEA708_CHARMAP
#ifdef __cplusplus
extern "C" {
#endif

#include "utf8.h"
#include <stdint.h>

// #define EIA608_CHAR_COUNT 176
// extern const char* eia608_char_map[EIA608_CHAR_COUNT];

uint16_t cea708_from_utf8_1(const utf8_char_t* c);

#define CEA708_CHAR_SPACE "\x20"
#define CEA708_CHAR_EXCLAMATION_MARK "\x21"
#define CEA708_CHAR_QUOTATION_MARK "\x22"
#define CEA708_CHAR_NUMBER_SIGN "\x23"
#define CEA708_CHAR_DOLLAR_SIGN "\x24"
#define CEA708_CHAR_PERCENT_SIGN "\x25"
#define CEA708_CHAR_AMPERSAND "\x26"
#define CEA708_CHAR_APOSTROPHE "\x27"
#define CEA708_CHAR_LEFT_PARENTHESIS "\x28"
#define CEA708_CHAR_RIGHT_PARENTHESIS "\x29"
#define CEA708_CHAR_ASTERISK "\x2a"
#define CEA708_CHAR_PLUS_SIGN "\x2b"
#define CEA708_CHAR_COMMA "\x2c"
#define CEA708_CHAR_HYPHEN_MINUS "\x2d"
#define CEA708_CHAR_FULL_STOP "\x2e"
#define CEA708_CHAR_SOLIDUS "\x2f"
#define CEA708_CHAR_DIGIT_ZERO "\x30"
#define CEA708_CHAR_DIGIT_ONE "\x31"
#define CEA708_CHAR_DIGIT_TWO "\x32"
#define CEA708_CHAR_DIGIT_THREE "\x33"
#define CEA708_CHAR_DIGIT_FOUR "\x34"
#define CEA708_CHAR_DIGIT_FIVE "\x35"
#define CEA708_CHAR_DIGIT_SIX "\x36"
#define CEA708_CHAR_DIGIT_SEVEN "\x37"
#define CEA708_CHAR_DIGIT_EIGHT "\x38"
#define CEA708_CHAR_DIGIT_NINE "\x39"
#define CEA708_CHAR_COLON "\x3a"
#define CEA708_CHAR_SEMICOLON "\x3b"
#define CEA708_CHAR_LESS_THAN_SIGN "\x3c"
#define CEA708_CHAR_EQUALS_SIGN "\x3d"
#define CEA708_CHAR_GREATER_THAN_SIGN "\x3e"
#define CEA708_CHAR_QUESTION_MARK "\x3f"
#define CEA708_CHAR_COMMERCIAL_AT "\x40"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A "\x41"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_B "\x42"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_C "\x43"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_D "\x44"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_E "\x45"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_F "\x46"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_G "\x47"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_H "\x48"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_I "\x49"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_J "\x4a"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_K "\x4b"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_L "\x4c"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_M "\x4d"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_N "\x4e"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O "\x4f"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_P "\x50"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_Q "\x51"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_R "\x52"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_S "\x53"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_T "\x54"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_U "\x55"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_V "\x56"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_W "\x57"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_X "\x58"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_Y "\x59"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_Z "\x5a"
#define CEA708_CHAR_LEFT_SQUARE_BRACKET "\x5b"
#define CEA708_CHAR_REVERSE_SOLIDUS "\x5c"
#define CEA708_CHAR_RIGHT_SQUARE_BRACKET "\x5d"
#define CEA708_CHAR_CIRCUMFLEX_ACCENT "\x5e"
#define CEA708_CHAR_LOW_LINE "\x5f"
#define CEA708_CHAR_GRAVE_ACCENT "\x60"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A "\x61"
#define CEA708_CHAR_LATIN_SMALL_LETTER_B "\x62"
#define CEA708_CHAR_LATIN_SMALL_LETTER_C "\x63"
#define CEA708_CHAR_LATIN_SMALL_LETTER_D "\x64"
#define CEA708_CHAR_LATIN_SMALL_LETTER_E "\x65"
#define CEA708_CHAR_LATIN_SMALL_LETTER_F "\x66"
#define CEA708_CHAR_LATIN_SMALL_LETTER_G "\x67"
#define CEA708_CHAR_LATIN_SMALL_LETTER_H "\x68"
#define CEA708_CHAR_LATIN_SMALL_LETTER_I "\x69"
#define CEA708_CHAR_LATIN_SMALL_LETTER_J "\x6a"
#define CEA708_CHAR_LATIN_SMALL_LETTER_K "\x6b"
#define CEA708_CHAR_LATIN_SMALL_LETTER_L "\x6c"
#define CEA708_CHAR_LATIN_SMALL_LETTER_M "\x6d"
#define CEA708_CHAR_LATIN_SMALL_LETTER_N "\x6e"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O "\x6f"
#define CEA708_CHAR_LATIN_SMALL_LETTER_P "\x70"
#define CEA708_CHAR_LATIN_SMALL_LETTER_Q "\x71"
#define CEA708_CHAR_LATIN_SMALL_LETTER_R "\x72"
#define CEA708_CHAR_LATIN_SMALL_LETTER_S "\x73"
#define CEA708_CHAR_LATIN_SMALL_LETTER_T "\x74"
#define CEA708_CHAR_LATIN_SMALL_LETTER_U "\x75"
#define CEA708_CHAR_LATIN_SMALL_LETTER_V "\x76"
#define CEA708_CHAR_LATIN_SMALL_LETTER_W "\x77"
#define CEA708_CHAR_LATIN_SMALL_LETTER_X "\x78"
#define CEA708_CHAR_LATIN_SMALL_LETTER_Y "\x79"
#define CEA708_CHAR_LATIN_SMALL_LETTER_Z "\x7a"
#define CEA708_CHAR_LEFT_CURLY_BRACKET "\x7b"
#define CEA708_CHAR_VERTICAL_LINE "\x7c"
#define CEA708_CHAR_RIGHT_CURLY_BRACKET "\x7d"
#define CEA708_CHAR_TILDE "\x7e"
#define CEA708_CHAR_EIGHTH_NOTE "\xe2\x99\xaa"
#define CEA708_CHAR_NO_BREAK_SPACE "\xc2\xa0"
#define CEA708_CHAR_INVERTED_EXCLAMATION_MARK "\xc2\xa1"
#define CEA708_CHAR_CENT_SIGN "\xc2\xa2"
#define CEA708_CHAR_POUND_SIGN "\xc2\xa3"
#define CEA708_CHAR_CURRENCY_SIGN "\xc2\xa4"
#define CEA708_CHAR_YEN_SIGN "\xc2\xa5"
#define CEA708_CHAR_BROKEN_BAR "\xc2\xa6"
#define CEA708_CHAR_SECTION_SIGN "\xc2\xa7"
#define CEA708_CHAR_DIAERESIS "\xc2\xa8"
#define CEA708_CHAR_COPYRIGHT_SIGN "\xc2\xa9"
#define CEA708_CHAR_FEMININE_ORDINAL_INDICATOR "\xc2\xaa"
#define CEA708_CHAR_LEFT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK "\xc2\xab"
#define CEA708_CHAR_NOT_SIGN "\xc2\xac"
#define CEA708_CHAR_SOFT_HYPHEN "\xc2\xad"
#define CEA708_CHAR_REGISTERED_SIGN "\xc2\xae"
#define CEA708_CHAR_MACRON "\xc2\xaf"
#define CEA708_CHAR_DEGREE_SIGN "\xc2\xb0"
#define CEA708_CHAR_PLUS_MINUS_SIGN "\xc2\xb1"
#define CEA708_CHAR_SUPERSCRIPT_TWO "\xc2\xb2"
#define CEA708_CHAR_SUPERSCRIPT_THREE "\xc2\xb3"
#define CEA708_CHAR_ACUTE_ACCENT "\xc2\xb4"
#define CEA708_CHAR_MICRO_SIGN "\xc2\xb5"
#define CEA708_CHAR_PILCROW_SIGN "\xc2\xb6"
#define CEA708_CHAR_MIDDLE_DOT "\xc2\xb7"
#define CEA708_CHAR_CEDILLA "\xc2\xb8"
#define CEA708_CHAR_SUPERSCRIPT_ONE "\xc2\xb9"
#define CEA708_CHAR_MASCULINE_ORDINAL_INDICATOR "\xc2\xba"
#define CEA708_CHAR_RIGHT_POINTING_DOUBLE_ANGLE_QUOTATION_MARK "\xc2\xbb"
#define CEA708_CHAR_VULGAR_FRACTION_ONE_QUARTER "\xc2\xbc"
#define CEA708_CHAR_VULGAR_FRACTION_ONE_HALF "\xc2\xbd"
#define CEA708_CHAR_VULGAR_FRACTION_THREE_QUARTERS "\xc2\xbe"
#define CEA708_CHAR_INVERTED_QUESTION_MARK "\xc2\xbf"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_GRAVE "\xc3\x80"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_ACUTE "\xc3\x81"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_CIRCUMFLEX "\xc3\x82"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_TILDE "\xc3\x83"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_DIAERESIS "\xc3\x84"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_A_WITH_RING_ABOVE "\xc3\x85"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_AE "\xc3\x86"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_C_WITH_CEDILLA "\xc3\x87"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_E_WITH_GRAVE "\xc3\x88"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_E_WITH_ACUTE "\xc3\x89"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_E_WITH_CIRCUMFLEX "\xc3\x8a"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_E_WITH_DIAERESIS "\xc3\x8b"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_I_WITH_GRAVE "\xc3\x8c"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_I_WITH_ACUTE "\xc3\x8d"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_I_WITH_CIRCUMFLEX "\xc3\x8e"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_I_WITH_DIAERESIS "\xc3\x8f"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_ETH "\xc3\x90"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_N_WITH_TILDE "\xc3\x91"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_GRAVE "\xc3\x92"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_ACUTE "\xc3\x93"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_CIRCUMFLEX "\xc3\x94"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_TILDE "\xc3\x95"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_DIAERESIS "\xc3\x96"
#define CEA708_CHAR_MULTIPLICATION_SIGN "\xc3\x97"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_O_WITH_STROKE "\xc3\x98"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_U_WITH_GRAVE "\xc3\x99"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_U_WITH_ACUTE "\xc3\x9a"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_U_WITH_CIRCUMFLEX "\xc3\x9b"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_U_WITH_DIAERESIS "\xc3\x9c"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_Y_WITH_ACUTE "\xc3\x9d"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_THORN "\xc3\x9e"
#define CEA708_CHAR_LATIN_SMALL_LETTER_SHARP_S "\xc3\x9f"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_GRAVE "\xc3\xa0"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_ACUTE "\xc3\xa1"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_CIRCUMFLEX "\xc3\xa2"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_TILDE "\xc3\xa3"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_DIAERESIS "\xc3\xa4"
#define CEA708_CHAR_LATIN_SMALL_LETTER_A_WITH_RING_ABOVE "\xc3\xa5"
#define CEA708_CHAR_LATIN_SMALL_LETTER_AE "\xc3\xa6"
#define CEA708_CHAR_LATIN_SMALL_LETTER_C_WITH_CEDILLA "\xc3\xa7"
#define CEA708_CHAR_LATIN_SMALL_LETTER_E_WITH_GRAVE "\xc3\xa8"
#define CEA708_CHAR_LATIN_SMALL_LETTER_E_WITH_ACUTE "\xc3\xa9"
#define CEA708_CHAR_LATIN_SMALL_LETTER_E_WITH_CIRCUMFLEX "\xc3\xaa"
#define CEA708_CHAR_LATIN_SMALL_LETTER_E_WITH_DIAERESIS "\xc3\xab"
#define CEA708_CHAR_LATIN_SMALL_LETTER_I_WITH_GRAVE "\xc3\xac"
#define CEA708_CHAR_LATIN_SMALL_LETTER_I_WITH_ACUTE "\xc3\xad"
#define CEA708_CHAR_LATIN_SMALL_LETTER_I_WITH_CIRCUMFLEX "\xc3\xae"
#define CEA708_CHAR_LATIN_SMALL_LETTER_I_WITH_DIAERESIS "\xc3\xaf"
#define CEA708_CHAR_LATIN_SMALL_LETTER_ETH "\xc3\xb0"
#define CEA708_CHAR_LATIN_SMALL_LETTER_N_WITH_TILDE "\xc3\xb1"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_GRAVE "\xc3\xb2"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_ACUTE "\xc3\xb3"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_CIRCUMFLEX "\xc3\xb4"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_TILDE "\xc3\xb5"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_DIAERESIS "\xc3\xb6"
#define CEA708_CHAR_DIVISION_SIGN "\xc3\xb7"
#define CEA708_CHAR_LATIN_SMALL_LETTER_O_WITH_STROKE "\xc3\xb8"
#define CEA708_CHAR_LATIN_SMALL_LETTER_U_WITH_GRAVE "\xc3\xb9"
#define CEA708_CHAR_LATIN_SMALL_LETTER_U_WITH_ACUTE "\xc3\xba"
#define CEA708_CHAR_LATIN_SMALL_LETTER_U_WITH_CIRCUMFLEX "\xc3\xbb"
#define CEA708_CHAR_LATIN_SMALL_LETTER_U_WITH_DIAERESIS "\xc3\xbc"
#define CEA708_CHAR_LATIN_SMALL_LETTER_Y_WITH_ACUTE "\xc3\xbd"
#define CEA708_CHAR_LATIN_SMALL_LETTER_THORN "\xc3\xbe"
#define CEA708_CHAR_LATIN_SMALL_LETTER_Y_WITH_DIAERESIS "\xc3\xbf"
#define CEA708_CHAR_HORIZONTAL_ELLIPSIS "\xe2\x80\xa6"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_S_WITH_CARON "\xc5\xa0"
#define CEA708_CHAR_LATIN_CAPITAL_LIGATURE_OE "\xc5\x92"
#define CEA708_CHAR_FULL_BLOCK "\xe2\x96\x88"
#define CEA708_CHAR_LEFT_SINGLE_QUOTATION_MARK "\xe2\x80\x98"
#define CEA708_CHAR_RIGHT_SINGLE_QUOTATION_MARK "\xe2\x80\x99"
#define CEA708_CHAR_LEFT_DOUBLE_QUOTATION_MARK "\xe2\x80\x9c"
#define CEA708_CHAR_RIGHT_DOUBLE_QUOTATION_MARK "\xe2\x80\x9d"
#define CEA708_CHAR_BULLET "\xe2\x80\xa2"
#define CEA708_CHAR_TRADE_MARK_SIGN "\xe2\x84\xa2"
#define CEA708_CHAR_LATIN_SMALL_LETTER_S_WITH_CARON "\xc5\xa1"
#define CEA708_CHAR_LATIN_SMALL_LIGATURE_OE "\xc5\x93"
#define CEA708_CHAR_SERVICE_MARK "\xe2\x84\xa0"
#define CEA708_CHAR_LATIN_CAPITAL_LETTER_Y_WITH_DIAERESIS "\xc5\xb8"
#define CEA708_CHAR_VULGAR_FRACTION_ONE_EIGHTH "\xe2\x85\x9b"
#define CEA708_CHAR_VULGAR_FRACTION_THREE_EIGHTHS "\xe2\x85\x9c"
#define CEA708_CHAR_VULGAR_FRACTION_FIVE_EIGHTHS "\xe2\x85\x9d"
#define CEA708_CHAR_VULGAR_FRACTION_SEVEN_EIGHTHS "\xe2\x85\x9e"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_VERTICAL "\xe2\x94\x82"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_DOWN_AND_LEFT "\xe2\x94\x90"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_UP_AND_RIGHT "\xe2\x94\x94"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_HORIZONTAL "\xe2\x94\x80"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_UP_AND_LEFT "\xe2\x94\x98"
#define CEA708_CHAR_BOX_DRAWINGS_LIGHT_DOWN_AND_RIGHT "\xe2\x94\x8c"

#ifdef __cplusplus
}
#endif
#endif // LIBCAPTION_CEA708_CHARMAP
