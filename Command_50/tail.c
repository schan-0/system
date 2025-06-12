#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEFAULT_LINES 10
#define BUFFER_SIZE 4096

typedef enum {
    MODE_LINES,
    MODE_BYTES
} OutputMode;

typedef enum {
    FLAG_QUIET = 1 << 0,
    FLAG_VERBOSE = 1 << 1
} TailFlags;

void process_file(const char *filename, int lines_to_read, long bytes_to_read, OutputMode mode, int flags, int num_files) {
    FILE *fp;
    long file_size;
    long start_offset;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    int lines_found = 0;

    if (strcmp(filename, "-") == 0) {
        fp = stdin;
        if (mode == MODE_BYTES) {
            long total_read = 0;
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                total_read += bytes_read;
            }
            if (bytes_to_read > total_read) bytes_to_read = total_read;
            fprintf(stderr, "tail: -c option on standard input is not fully supported in this simplified version.\n");
            return;
        }
    } else {
        fp = fopen(filename, "rb");
        if (fp == NULL) {
            fprintf(stderr, "tail: cannot open '%s' for reading: %s\n", filename, strerror(errno));
            return;
        }

        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
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
        if (fp == stdin) {
            while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                fwrite(buffer, 1, bytes_read, stdout);
            }
        } else {
            start_offset = file_size;
            for (long i = file_size - 1; i >= 0 && lines_found <= lines_to_read; --i) {
                fseek(fp, i, SEEK_SET);
                char c = fgetc(fp);
                if (c == '\n') {
                    lines_found++;
                }
                start_offset = i;
            }

            if (lines_found > lines_to_read) {
                start_offset++;
            }

            fseek(fp, start_offset, SEEK_SET);

            while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
                fwrite(buffer, 1, bytes_read, stdout);
            }
        }
    } else {
        if (fp == stdin) {
            fprintf(stderr, "tail: -c option on standard input is not fully supported in this simplified version.\n");
            return;
        }
        start_offset = file_size - bytes_to_read;
        if (start_offset < 0) {
            start_offset = 0;
        }
        fseek(fp, start_offset, SEEK_SET);

        while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
            fwrite(buffer, 1, bytes_read, stdout);
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
                    fprintf(stderr, "tail: invalid number of lines: '%s'\n", optarg);
                    return 1;
                }
                mode = MODE_LINES;
                break;
            case 'c':
                bytes_to_read = atol(optarg);
                if (bytes_to_read < 0) {
                    fprintf(stderr, "tail: invalid number of bytes: '%s'\n", optarg);
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
