#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

int verbose_flag = 0;

int make_dir(char *path, mode_t mode, int parents) {
    if (parents) {
        char *p = path;
        char *slash = NULL;
        mode_t old_umask = umask(0);

        while (*p != '\0') {
            if (*p == '/') {
                slash = p;
                *slash = '\0';
                if (strlen(path) > 0) {
                    if (mkdir(path, mode) == -1 && errno != EEXIST) {
                        fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", path, strerror(errno));
                        umask(old_umask);
                        return -1;
                    } else if (errno != EEXIST && verbose_flag) {
                        fprintf(stdout, "mkdir: created directory '%s'\n", path);
                    }
                }
                *slash = '/';
            }
            p++;
        }
        
        if (mkdir(path, mode) == -1 && errno != EEXIST) {
            fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", path, strerror(errno));
            umask(old_umask);
            return -1;
        } else if (errno != EEXIST && verbose_flag) {
            fprintf(stdout, "mkdir: created directory '%s'\n", path);
        }
        umask(old_umask);
    } else {
        if (mkdir(path, mode) == -1 && errno != EEXIST) {
            fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", path, strerror(errno));
            return -1;
        } else if (verbose_flag) {
            fprintf(stdout, "mkdir: created directory '%s'\n", path);
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int parents_flag = 0;
    mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;

    while ((opt = getopt(argc, argv, "pm:v")) != -1) {
        switch (opt) {
            case 'p':
                parents_flag = 1;
                break;
            case 'm':
                mode = (mode_t)strtol(optarg, NULL, 8);
                break;
            case 'v':
                verbose_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-p] [-m mode] [-v] directory...\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-p] [-m mode] [-v] directory...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        make_dir(argv[i], mode, parents_flag);
    }

    return EXIT_SUCCESS;
}
