#include "editbuf.h"
#include <stdlib.h>

void cb_init(ChangeBuffer* cb) {
    cb->items = NULL;
    cb->count = 0;
    cb->cap = 0;
}

void cb_free(ChangeBuffer* cb) {
    if (!cb) return;
    free(cb->items);
    cb->items = NULL;
    cb->count = 0;
    cb->cap = 0;
}

void cb_clear(ChangeBuffer* cb) {
    if (!cb) return;
    cb->count = 0;
}

static bool ensure_cap(ChangeBuffer* cb, size_t need) {
    if (cb->cap >= need) return true;
    size_t newcap = cb->cap ? cb->cap * 2 : 8;
    while (newcap < need) newcap *= 2;
    Change* p = (Change*)realloc(cb->items, newcap * sizeof(Change));
    if (!p) return false;
    cb->items = p;
    cb->cap = newcap;
    return true;
}

bool cb_add(ChangeBuffer* cb, size_t offset, uint8_t value) {
    if (!cb) return false;
    // überschreiben, wenn Offset schon existiert
    for (size_t i = 0; i < cb->count; i++) {
        if (cb->items[i].offset == offset) {
            cb->items[i].value = value;
            return true;
        }
    }
    if (!ensure_cap(cb, cb->count + 1)) return false;
    cb->items[cb->count++] = (Change){ .offset = offset, .value = value };
    return true;
}

bool cb_get(const ChangeBuffer* cb, size_t offset, uint8_t* out_value) {
    if (!cb) return false;
    for (size_t i = 0; i < cb->count; i++) {
        if (cb->items[i].offset == offset) {
            if (out_value) *out_value = cb->items[i].value;
            return true;
        }
    }
    return false;
}

