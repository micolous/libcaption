#include "cea708_charmap.h"

// prototype for re2c generated function
uint16_t _cea708_from_utf8(const utf8_char_t* s);
uint16_t cea708_from_utf8_1(const utf8_char_t* c) {
    return _cea708_from_utf8(c);
}
