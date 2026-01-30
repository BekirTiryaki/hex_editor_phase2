#ifndef VIEWER_H
#define VIEWER_H

#include "types.h"
#include <stdio.h>

void viewer_print_page(FILE* f, size_t file_size, size_t base_offset,
                       ViewMode mode, const ChangeBuffer* pending);

#endif

