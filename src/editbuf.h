#ifndef EDITBUF_H
#define EDITBUF_H

#include "types.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void cb_init(ChangeBuffer* cb);
void cb_free(ChangeBuffer* cb);
bool cb_add(ChangeBuffer* cb, size_t offset, uint8_t value);
bool cb_get(const ChangeBuffer* cb, size_t offset, uint8_t* out_value);
void cb_clear(ChangeBuffer* cb);

#endif

