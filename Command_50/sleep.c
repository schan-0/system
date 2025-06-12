#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number>\n", argv[0]);
        return 1;
    }

    errno = 0;
    long seconds = strtol(argv[1], NULL, 10);

    if (errno != 0 || seconds < 0) {
        fprintf(stderr, "sleep: invalid time interval '%s'\n", argv[1]);
        return 1;
    }

    if (sleep(seconds) != 0) {
        fprintf(stderr, "sleep: sleep interrupted\n");
        return 1;
    }

    return 0;
}
