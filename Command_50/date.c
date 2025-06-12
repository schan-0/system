#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define DEFAULT_FORMAT "%a %b %d %H:%M:%S %Z %Y"
#define RFC2822_FORMAT "%a, %d %b %Y %H:%M:%S %z"
#define ISO8601_FORMAT "%Y-%m-%d"
#define ISO8601_FORMAT_DATE_TIME "%Y-%m-%dT%H:%M:%S%z"

int main(int argc, char *argv[]) {
    int opt;
    char *format_string = DEFAULT_FORMAT;
    time_t current_time;
    struct tm *timeinfo;
    int use_utc = 0;
    int use_date_string = 0;
    int use_reference_file = 0;
    char *date_string_arg = NULL;
    char *reference_file_arg = NULL;
    int iso_format_flag = 0;

    while ((opt = getopt(argc, argv, "d:r:uRI")) != -1) {
        switch (opt) {
            case 'd':
                use_date_string = 1;
                date_string_arg = optarg;
                break;
            case 'r':
                use_reference_file = 1;
                reference_file_arg = optarg;
                break;
            case 'u':
                use_utc = 1;
                break;
            case 'R':
                format_string = RFC2822_FORMAT;
                break;
            case 'I':
                iso_format_flag = 1;
                format_string = ISO8601_FORMAT; 
                break;
            default:
                fprintf(stderr, "Usage: %s [-u] [-R] [-I] [-d STRING] [-r FILE] [+FORMAT]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind < argc && argv[optind][0] == '+') {
        format_string = argv[optind] + 1;
        iso_format_flag = 0;
    }

    if (use_date_string) {
        time(&current_time); 
        
        if (strcmp(date_string_arg, "yesterday") == 0) {
            current_time -= (24 * 60 * 60);
        } else if (strcmp(date_string_arg, "tomorrow") == 0) {
            current_time += (24 * 60 * 60);
        } else {
            fprintf(stderr, "date: invalid date string '%s' (simple parser only supports 'yesterday' and 'tomorrow')\n", date_string_arg);
            exit(EXIT_FAILURE);
        }
    } else if (use_reference_file) {
        struct stat file_stat;
        if (stat(reference_file_arg, &file_stat) == -1) {
            fprintf(stderr, "date: cannot stat '%s': %s\n", reference_file_arg, strerror(errno));
            exit(EXIT_FAILURE);
        }
        current_time = file_stat.st_mtime;
    } else {
        time(&current_time);
    }

    if (use_utc) {
        timeinfo = gmtime(&current_time);
    } else {
        timeinfo = localtime(&current_time);
    }

    char buffer[256];
    strftime(buffer, sizeof(buffer), format_string, timeinfo);
    printf("%s\n", buffer);

    return EXIT_SUCCESS;
}
