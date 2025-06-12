#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int verbose_flag = 0;

int remove_dir(char *path, int parents) {
    if (rmdir(path) == -1) {
        if (errno != ENOTEMPTY && errno != EEXIST) {
            fprintf(stderr, "rmdir: failed to remove '%s': %s\n", path, strerror(errno));
            return -1;
        } else if (errno == ENOTEMPTY) {
            fprintf(stderr, "rmdir: failed to remove '%s': Directory not empty\n", path);
            return -1;
        }
    } else {
        if (verbose_flag) {
            fprintf(stdout, "rmdir: removed directory '%s'\n", path);
        }
    }

    if (parents) {
        char *parent_path = strdup(path);
        if (parent_path == NULL) {
            fprintf(stderr, "rmdir: memory allocation failed\n");
            return -1;
        }

        char *last_slash = strrchr(parent_path, '/');
        if (last_slash != NULL) {
            *last_slash = '\0';
            if (strlen(parent_path) > 0) { 
                if (remove_dir(parent_path, 1) != 0) { 
                    free(parent_path);
                    return -1;
                }
            }
        }
        free(parent_path);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int parents_flag = 0;

    while ((opt = getopt(argc, argv, "pv")) != -1) {
        switch (opt) {
            case 'p':
                parents_flag = 1;
                break;
            case 'v':
                verbose_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-p] [-v] directory...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-p] [-v] directory...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        remove_dir(argv[i], parents_flag);
    }

    return EXIT_SUCCESS;
}
