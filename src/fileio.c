#include "fileio.h"

bool file_get_size(FILE* f, size_t* out_size) {
    if (!f || !out_size) return false;
    long cur = ftell(f);
    if (cur < 0) return false;
    if (fseek(f, 0, SEEK_END) != 0) return false;
    long end = ftell(f);
    if (end < 0) return false;
    if (fseek(f, cur, SEEK_SET) != 0) return false;
    *out_size = (size_t)end;
    return true;
}

bool file_read_at(FILE* f, size_t offset, uint8_t* buf, size_t len, size_t* out_read) {
    if (!f || !buf || len == 0) return false;
    if (fseek(f, (long)offset, SEEK_SET) != 0) return false;
    size_t n = fread(buf, 1, len, f);
    if (out_read) *out_read = n;
    return true;
}

bool file_write_byte(FILE* f, size_t offset, uint8_t value) {
    if (!f) return false;
    if (fseek(f, (long)offset, SEEK_SET) != 0) return false;
    if (fwrite(&value, 1, 1, f) != 1) return false;
    return fflush(f) == 0;
}

