#ifndef FILEIO_H
#define FILEIO_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

bool file_get_size(FILE* f, size_t* out_size);
bool file_read_at(FILE* f, size_t offset, uint8_t* buf, size_t len, size_t* out_read);
bool file_write_byte(FILE* f, size_t offset, uint8_t value);

#endif

