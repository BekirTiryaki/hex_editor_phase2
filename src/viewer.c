#include "viewer.h"
#include "fileio.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include "editbuf.h"
#include <stdbool.h>

static void byte_to_bin(uint8_t b, char out[9]) {
    for (int i = 7; i >= 0; i--) out[7 - i] = ((b >> i) & 1) ? '1' : '0';
    out[8] = '\0';
}

static char printable(uint8_t b) {
    return isprint((unsigned char)b) ? (char)b : '.';
}

void viewer_print_page(FILE* f, size_t file_size, size_t base_offset,
                       ViewMode mode, const ChangeBuffer* pending) {
    const size_t rows = 16;
    const size_t cols = (mode == VIEW_HEX) ? 16 : 8; // passend zur Terminalbreite (dein Konzept)
    printf("\n--- Offset 0x%08zx (%zu) | size=%zu bytes | mode=%s | pending=%zu ---\n",
           base_offset, base_offset, file_size,
           (mode==VIEW_ALL?"ALL":(mode==VIEW_HEX?"HEX":"BIN")),
           pending ? pending->count : 0UL);

    for (size_t r = 0; r < rows; r++) {
        size_t row_off = base_offset + r * cols;
        printf("0x%08zx | ", row_off);

        // HEX oder BIN Spalten
        for (size_t c = 0; c < cols; c++) {
            size_t off = row_off + c;
            if (off >= file_size) {
                if (mode != VIEW_BIN) printf("   ");
                if (mode == VIEW_BIN) printf("         ");
                continue;
            }

            uint8_t b = 0;
            size_t got = 0;
            (void)file_read_at(f, off, &b, 1, &got);

            uint8_t pb = b;
            if (pending && cb_get(pending, off, &pb)) {
                // pending overlay
                b = pb;
            }

            if (mode != VIEW_BIN) {
uint8_t orig = 0; size_t got2 = 0;
(void)file_read_at(f, off, &orig, 1, &got2);
uint8_t overlay = orig;
bool changed = (pending && cb_get(pending, off, &overlay));
b = changed ? overlay : orig;

printf("%c%02X ", changed ? '*' : ' ', b);
           
 } else {
                char bin[9];
                byte_to_bin(b, bin);
                printf("%s ", bin);
            }
        }

        // BIN extra in ALL
        if (mode == VIEW_ALL) {
            printf("| ");
            for (size_t c = 0; c < cols; c++) {
                size_t off = row_off + c;
                if (off >= file_size) { printf("         "); continue; }
                uint8_t b = 0; size_t got = 0;
                (void)file_read_at(f, off, &b, 1, &got);
                if (pending) cb_get(pending, off, &b);
                char bin[9]; byte_to_bin(b, bin);
                printf("%s ", bin);
            }
        }

        // CHAR
        printf("| ");
        for (size_t c = 0; c < cols; c++) {
            size_t off = row_off + c;
            if (off >= file_size) { putchar(' '); continue; }
            uint8_t b = 0; size_t got = 0;
            (void)file_read_at(f, off, &b, 1, &got);
            if (pending) cb_get(pending, off, &b);
            putchar(printable(b));
        }

        putchar('\n');
    }

    printf("\nCommands: show | n | p | g <offset> | mode all|hex|bin | set <offset> <FF|10101010|'A'> | list | clear | w | q | q!\n");
}

