#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

int main(void) {
    uid_t euid = geteuid();
    struct passwd *pw = getpwuid(euid);

    if (pw == NULL) {
        fprintf(stderr, "whoami: cannot find name for user ID %d: %s\n", euid, strerror(errno));
        return 1;
    }

    printf("%s\n", pw->pw_name);

    return 0;
}
