#include "parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static bool is_all_bin(const char* s) {
    if (!s) return false;
    size_t n = strlen(s);
    if (n != 8) return false;
    for (size_t i = 0; i < n; i++) {
        if (s[i] != '0' && s[i] != '1') return false;
    }
    return true;
}

bool parse_offset(const char* s, size_t* out) {
    if (!s || !out) return false;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return false;

    char* end = NULL;
    unsigned long v = 0;

    if (strlen(s) > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        v = strtoul(s + 2, &end, 16);
    } else {
        v = strtoul(s, &end, 10);
    }

    if (!end || *end != '\0') return false;
    *out = (size_t)v;
    return true;
}

bool parse_byte_value(const char* s, uint8_t* out) {
    if (!s || !out) return false;

    // Char-Form: 'A'
    size_t n = strlen(s);
    if (n == 3 && s[0] == '\'' && s[2] == '\'') {
        *out = (uint8_t)s[1];
        return true;
    }

    // Binär-Form: 8 Bits
    if (is_all_bin(s)) {
        uint8_t v = 0;
        for (size_t i = 0; i < 8; i++) {
            v = (uint8_t)(v << 1);
            if (s[i] == '1') v |= 1;
        }
        *out = v;
        return true;
    }

    // Hex: "FF" oder "0xFF"
    const char* p = s;
    if (n > 2 && p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) p += 2;

    if (*p == '\0') return false;
    for (const char* q = p; *q; q++) {
        if (!isxdigit((unsigned char)*q)) return false;
    }

    char* end = NULL;
    unsigned long v = strtoul(p, &end, 16);
    if (!end || *end != '\0' || v > 255UL) return false;
    *out = (uint8_t)v;
    return true;
}

