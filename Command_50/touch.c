#define _XOPEN_SOURCE
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <utime.h> 
#include <sys/stat.h> 
#include <time.h>    // time_t, time, strptime, mktime
#include <string.h>  // strcmp, memset
#include <getopt.h>  // getopt

extern char *optarg;
extern int optind, opterr, optopt;

void print_usage(const char *prog_name);

int main(int argc, char *argv[]) {
    int opt;
    int set_atime = 1; // Default to update both
    int set_mtime = 1; // Default to update both
    int no_create = 0;
    time_t specific_time = 0;
    char *ref_file = NULL;
    int use_specific_time = 0;
    int use_ref_file = 0;
    int no_dereference = 0; 

    while ((opt = getopt(argc, argv, "acmd:r:t:h")) != -1) {
        switch (opt) {
            case 'a':
                set_mtime = 0;
                break;
            case 'c':
                no_create = 1;
                break;
            case 'm':
                set_atime = 0;
                break;
            case 'd': {
                struct tm tm_info;
                memset(&tm_info, 0, sizeof(struct tm));
                if (strptime(optarg, "%Y-%m-%d %H:%M:%S", &tm_info) == NULL &&
                    strptime(optarg, "%Y-%m-%d", &tm_info) == NULL) { // Simplified date parsing
                    fprintf(stderr, "touch: invalid date format: '%s'\n", optarg);
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
                }
                specific_time = mktime(&tm_info);
                use_specific_time = 1;
                break;
            }
            case 'r':
                ref_file = optarg;
                use_ref_file = 1;
                break;
            case 't': {
                struct tm tm_info;
                memset(&tm_info, 0, sizeof(struct tm));
                if (strptime(optarg, "%Y%m%d%H%M.%S", &tm_info) == NULL && // YYYYMMDDhhmm.ss
                    strptime(optarg, "%Y%m%d%H%M", &tm_info) == NULL) {     // YYYYMMDDhhmm
                    // Add more formats as needed for full compliance, this is simplified.
                    fprintf(stderr, "touch: invalid timestamp format: '%s'\n", optarg);
                    print_usage(argv[0]);
                    return EXIT_FAILURE;
                }
                specific_time = mktime(&tm_info);
                use_specific_time = 1;
                break;
            }
            case 'h':
                no_dereference = 1;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "touch: missing file operand\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = optind; i < argc; i++) {
        const char *filename = argv[i];
        struct utimbuf new_times;
        struct stat st;
        int stat_result;

        if (no_dereference) {
            stat_result = lstat(filename, &st);
        } else {
            stat_result = stat(filename, &st);
        }

        if (stat_result != 0) { // File does not exist
            if (no_create) {
                continue; // -c option, do not create
            }
            // Create empty file
            FILE *f = fopen(filename, "w");
            if (f == NULL) {
                perror(filename);
                continue;
            }
            fclose(f);
        }

        if (use_specific_time) {
            new_times.actime = specific_time;
            new_times.modtime = specific_time;
        } else if (use_ref_file) {
            struct stat ref_st;
            if (stat(ref_file, &ref_st) != 0) {
                perror(ref_file);
                return EXIT_FAILURE;
            }
            new_times.actime = ref_st.st_atime;
            new_times.modtime = ref_st.st_mtime;
        } else {
            new_times.actime = time(NULL);
            new_times.modtime = time(NULL);
        }

        // Adjust times based on -a and -m
        if (stat_result == 0) { // If file existed before (or just created)
            if (!set_atime) { // -m was given
                new_times.actime = st.st_atime;
            }
            if (!set_mtime) { // -a was given
                new_times.modtime = st.st_mtime;
            }
        }

        if (no_dereference) {
            if (utimes(filename, (struct timeval *)&new_times) != 0) { // utimes for symlinks
                perror(filename);
            }
        } else {
            if (utime(filename, &new_times) != 0) { // utime for regular files
                perror(filename);
            }
        }
    }

    return EXIT_SUCCESS;
}

void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-acmh] [-d DATE_STRING] [-r REFERENCE_FILE] [-t STAMP] FILE...\n", prog_name);
}
