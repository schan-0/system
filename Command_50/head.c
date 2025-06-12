#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEFAULT_LINES 10

typedef enum {
    MODE_LINES,
    MODE_BYTES
} OutputMode;

typedef enum {
    FLAG_QUIET = 1 << 0,
    FLAG_VERBOSE = 1 << 1
} HeadFlags;

void process_file(const char *filename, int lines_to_read, long bytes_to_read, OutputMode mode, int flags, int num_files) {
    FILE *fp;
    char buffer[4096];
    size_t bytes_read;
    int lines_count = 0;
    long total_bytes_read = 0;

    if (strcmp(filename, "-") == 0) {
        fp = stdin;
    } else {
        fp = fopen(filename, "r");
        if (fp == NULL) {
            fprintf(stderr, "head: cannot open '%s' for reading: %s\n", filename, strerror(errno));
            return;
        }
    }

    if (num_files > 1 && !(flags & FLAG_QUIET)) {
        if (flags & FLAG_VERBOSE) {
            printf("==> %s <==\n", filename);
        } else {
            printf("==> %s <==\n", filename);
        }
    } else if (num_files == 1 && (flags & FLAG_VERBOSE)) {
        printf("==> %s <==\n", filename);
    }

    if (mode == MODE_LINES) {
        while (lines_count < lines_to_read && (bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            for (size_t i = 0; i < bytes_read; ++i) {
                putchar(buffer[i]);
                if (buffer[i] == '\n') {
                    lines_count++;
                    if (lines_count >= lines_to_read) {
                        break;
                    }
                }
            }
        }
    } else { // MODE_BYTES
        while (total_bytes_read < bytes_to_read && (bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            long remaining_bytes = bytes_to_read - total_bytes_read;
            size_t write_bytes = (bytes_read < remaining_bytes) ? bytes_read : (size_t)remaining_bytes;
            fwrite(buffer, 1, write_bytes, stdout);
            total_bytes_read += write_bytes;
        }
    }

    if (fp != stdin) {
        fclose(fp);
    }
}

int main(int argc, char *argv[]) {
    int lines_to_read = DEFAULT_LINES;
    long bytes_to_read = -1;
    OutputMode mode = MODE_LINES;
    int flags = 0;
    int opt;

    while ((opt = getopt(argc, argv, "n:c:qv")) != -1) {
        switch (opt) {
            case 'n':
                lines_to_read = atoi(optarg);
                if (lines_to_read < 0) {
                    fprintf(stderr, "head: invalid number of lines: '%s'\n", optarg);
                    return 1;
                }
                mode = MODE_LINES;
                break;
            case 'c':
                bytes_to_read = atol(optarg);
                if (bytes_to_read < 0) {
                    fprintf(stderr, "head: invalid number of bytes: '%s'\n", optarg);
                    return 1;
                }
                mode = MODE_BYTES;
                break;
            case 'q':
                flags |= FLAG_QUIET;
                break;
            case 'v':
                flags |= FLAG_VERBOSE;
                break;
            case '?':
                fprintf(stderr, "Usage: %s [-n lines | -c bytes] [-qv] [FILE...]\n", argv[0]);
                return 1;
            default:
                abort();
        }
    }

    int num_files = argc - optind;

    if (num_files == 0) {
        process_file("-", lines_to_read, bytes_to_read, mode, flags, 1);
    } else {
        for (int i = optind; i < argc; i++) {
            process_file(argv[i], lines_to_read, bytes_to_read, mode, flags, num_files);
        }
    }

    return 0;
}
