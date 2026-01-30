#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

bool parse_offset(const char* s, size_t* out);   // "123" oder "0x7B"
bool parse_byte_value(const char* s, uint8_t* out); // "FF" / "0xFF" / "10101010" / "'A'"

#endif

