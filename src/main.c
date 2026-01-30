#include "types.h"
#include "fileio.h"
#include "parser.h"
#include "editbuf.h"
#include "viewer.h"

#include <stdio.h>
#include <string.h>

static void trim_newline(char* s) {
    if (!s) return;
    size_t n = strlen(s);
    while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

static void print_list(const ChangeBuffer* cb) {
    if (!cb || cb->count == 0) {
        printf("No pending changes.\n");
        return;
    }
    printf("Pending changes (%zu):\n", cb->count);
    for (size_t i = 0; i < cb->count; i++) {
        printf("  - offset 0x%zx (%zu) -> 0x%02X\n",
               cb->items[i].offset, cb->items[i].offset, cb->items[i].value);
    }
}

int main(int argc, char** argv) {
    char path[1024] = {0};

    if (argc >= 2) {
        snprintf(path, sizeof(path), "%s", argv[1]);
    } else {
        printf("File path: ");
        if (!fgets(path, sizeof(path), stdin)) return 1;
        trim_newline(path);
        if (path[0] == '\0') {
            fprintf(stderr, "No file given.\n");
            return 1;
        }
    }

    FILE* f = fopen(path, "rb+");
    if (!f) {
        perror("fopen");
        return 1;
    }

    size_t size = 0;
    if (!file_get_size(f, &size)) {
        fprintf(stderr, "Could not read file size.\n");
        fclose(f);
        return 1;
    }

    ViewMode mode = VIEW_ALL;
    size_t base = 0;

    ChangeBuffer pending;
    cb_init(&pending);

    viewer_print_page(f, size, base, mode, &pending);

    char line[256];
    for (;;) {
        printf("\nhexed> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        trim_newline(line);

        if (strcmp(line, "show") == 0) {
            viewer_print_page(f, size, base, mode, &pending);
        } else if (strcmp(line, "n") == 0) {
            size_t step = (mode == VIEW_HEX) ? (16*16) : (16*8);
            if (base + step < size) base += step;
            viewer_print_page(f, size, base, mode, &pending);
        } else if (strcmp(line, "p") == 0) {
            size_t step = (mode == VIEW_HEX) ? (16*16) : (16*8);
            base = (base >= step) ? (base - step) : 0;
            viewer_print_page(f, size, base, mode, &pending);
        } else if (strncmp(line, "g ", 2) == 0) {
            size_t off = 0;
            if (!parse_offset(line + 2, &off) || off >= size) {
                printf("Invalid offset.\n");
            } else {
                base = off;
                viewer_print_page(f, size, base, mode, &pending);
            }
        } else if (strncmp(line, "mode ", 5) == 0) {
            const char* m = line + 5;
            if (strcmp(m, "all") == 0) mode = VIEW_ALL;
            else if (strcmp(m, "hex") == 0) mode = VIEW_HEX;
            else if (strcmp(m, "bin") == 0) mode = VIEW_BIN;
            else printf("Unknown mode.\n");
            viewer_print_page(f, size, base, mode, &pending);
        } else if (strncmp(line, "set ", 4) == 0) {
            // set <offset> <value>
            char off_s[64], val_s[64];
            if (sscanf(line + 4, "%63s %63s", off_s, val_s) != 2) {
                printf("Usage: set <offset> <FF|10101010|'A'>\n");
                continue;
            }
            size_t off = 0; uint8_t val = 0;
            if (!parse_offset(off_s, &off) || off >= size) {
                printf("Invalid offset.\n");
                continue;
            }
            if (!parse_byte_value(val_s, &val)) {
                printf("Invalid value (use hex, 8-bit bin, or 'A').\n");
                continue;
            }
            cb_add(&pending, off, val);
            printf("Pending set: offset 0x%zx -> 0x%02X\n", off, val);
        } else if (strcmp(line, "list") == 0) {
            print_list(&pending);
        } else if (strcmp(line, "clear") == 0) {
            cb_clear(&pending);
            printf("Pending changes cleared.\n");
        } else if (strcmp(line, "w") == 0) {
            if (pending.count == 0) {
                printf("Nothing to write.\n");
                continue;
            }
            printf("Type YES to commit %zu change(s): ", pending.count);
            char ans[32];
            if (!fgets(ans, sizeof(ans), stdin)) continue;
            trim_newline(ans);
// case-insensitive check: YES / yes / Yes ...
for (char* p = ans; *p; ++p) {
    if (*p >= 'a' && *p <= 'z') *p = (char)(*p - 32);
}
if (strcmp(ans, "YES") != 0) {
    printf("Commit aborted.\n");
    continue;
}

            }
            for (size_t i = 0; i < pending.count; i++) {
                if (!file_write_byte(f, pending.items[i].offset, pending.items[i].value)) {
                    printf("Write failed at offset 0x%zx.\n", pending.items[i].offset);
                }
            }
            cb_clear(&pending);
            // size bleibt gleich (wir überschreiben nur bytes)
            printf("Committed.\n");
            viewer_print_page(f, size, base, mode, &pending);
        } else if (strcmp(line, "q") == 0) {
            if (pending.count != 0) {
                printf("Pending changes exist. Use w, clear, or q! to quit without saving.\n");
                continue;
            }
            break;
        } else if (strcmp(line, "q!") == 0) {
            break;
        } else if (strcmp(line, "help") == 0) {
            printf("show | n | p | g <offset> | mode all|hex|bin | set <offset> <FF|10101010|'A'> | list | clear | w | q | q!\n");
        } else {
            printf("Unknown command. Type help.\n");
        }
    }

    cb_free(&pending);
    fclose(f);
    return 0;
}

