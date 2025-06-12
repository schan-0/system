#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 4096

#define OPT_N (1 << 0)
#define OPT_B (1 << 1)
#define OPT_E (1 << 2)
#define OPT_T (1 << 3)
#define OPT_A (1 << 4)
#define OPT_S (1 << 5)
#define OPT_V (1 << 6)

void process_file(const char *filename, int flags) {
    FILE *fp;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int line_number = 1;
    int prev_char = '\n';
    int blank_lines_count = 0;

    if (strcmp(filename, "-") == 0) {
        fp = stdin;
    } else {
        fp = fopen(filename, "r");
        if (fp == NULL) {
            fprintf(stderr, "cat: %s: %s\n", filename, strerror(errno));
            return;
        }
    }

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        for (size_t i = 0; i < bytes_read; ++i) {
            unsigned char current_char = buffer[i];

            if (flags & OPT_S) {
                if (current_char == '\n') {
                    blank_lines_count++;
                } else {
                    blank_lines_count = 0;
                }
                if (blank_lines_count > 1) {
                    prev_char = current_char;
                    continue;
                }
            }

            if (prev_char == '\n') {
                if ((flags & OPT_N) && !(flags & OPT_B)) {
                    printf("%6d\t", line_number++);
                } else if ((flags & OPT_B) && current_char != '\n') {
                    printf("%6d\t", line_number++);
                }
            }

            if (current_char == '\t') {
                if (flags & OPT_T) {
                    fputs("^I", stdout);
                } else {
                    putchar(current_char);
                }
            } else if (current_char == '\n') {
                if (flags & OPT_E) {
                    putchar('$');
                }
                putchar(current_char);
            } else if (flags & OPT_V) {
                if (iscntrl(current_char)) {
                    if (current_char == 127) {
                        fputs("^?", stdout);
                    } else {
                        putchar('^');
                        putchar(current_char + '@');
                    }
                } else if (current_char >= 128) {
                    putchar('M');
                    putchar('-');
                    if (iscntrl(current_char - 128)) {
                        if ((current_char - 128) == 127) {
                            fputs("^?", stdout);
                        } else {
                            putchar('^');
                            putchar((current_char - 128) + '@');
                        }
                    } else {
                        putchar(current_char - 128);
                    }
                } else {
                    putchar(current_char);
                }
            } else {
                putchar(current_char);
            }
            prev_char = current_char;
        }
    }

    if (fp != stdin) {
        fclose(fp);
    }
}

int main(int argc, char *argv[]) {
    int flags = 0;
    int opt;

    while ((opt = getopt(argc, argv, "nbETAsvet")) != -1) {
        switch (opt) {
            case 'n': flags |= OPT_N; break;
            case 'b': flags |= OPT_B; break;
            case 'E': flags |= OPT_E; break;
            case 'T': flags |= OPT_T; break;
            case 'A': flags |= OPT_A; break;
            case 's': flags |= OPT_S; break;
            case 'v': flags |= OPT_V; break;
            case 'e': flags |= (OPT_E | OPT_V); break;
            case 't': flags |= (OPT_T | OPT_V); break;
            case '?':
                fprintf(stderr, "Usage: %s [-nbeTAs] [FILE...]\n", argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (flags & OPT_A) {
        flags |= (OPT_V | OPT_E | OPT_T);
    }
    if ((flags & OPT_B) && (flags & OPT_N)) {
        flags &= ~OPT_N;
    }

    if (optind == argc) {
        process_file("-", flags);
    } else {
        for (int i = optind; i < argc; i++) {
            process_file(argv[i], flags);
        }
    }

    return 0;
}
