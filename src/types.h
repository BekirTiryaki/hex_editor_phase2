#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    VIEW_ALL = 0,   // HEX + BIN + CHAR (8 bytes/row)
    VIEW_HEX = 1,   // HEX + CHAR (16 bytes/row)
    VIEW_BIN = 2    // BIN + CHAR (8 bytes/row)
} ViewMode;

typedef struct {
    size_t offset;
    uint8_t value;
} Change;

typedef struct {
    Change* items;
    size_t count;
    size_t cap;
} ChangeBuffer;

#endif

