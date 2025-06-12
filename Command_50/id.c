#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

#define OPT_U (1 << 0)
#define OPT_G (1 << 1)
#define OPT_CAP_G (1 << 2)
#define OPT_R (1 << 3)
#define OPT_N (1 << 4)
#define OPT_A (1 << 5)

void print_user_info(uid_t uid, gid_t gid, int options) {
    struct passwd *pw = getpwuid(uid);
    struct group *gr = getgrgid(gid);

    if (!pw) {
        fprintf(stderr, "id: cannot find name for user ID %d\n", uid);
        return;
    }

    if (options == 0 || (options & OPT_A)) {
        printf("uid=%d(%s) gid=%d(%s)", pw->pw_uid, pw->pw_name, gr ? gr->gr_gid : gid, gr ? gr->gr_name : "unknown");

        int ngroups = 0;
        gid_t *groups = NULL;
        if (getgrouplist(pw->pw_name, pw->pw_gid, NULL, &ngroups) == -1) {
            groups = malloc(sizeof(gid_t) * ngroups);
            if (groups) {
                if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups) == -1) {
                }
            }
        }

        if (ngroups > 0) {
            printf(" groups=");
            for (int i = 0; i < ngroups; i++) {
                struct group *g = getgrgid(groups[i]);
                printf("%d(%s)%s", groups[i], g ? g->gr_name : "unknown", (i == ngroups - 1) ? "" : ",");
            }
        }
        printf("\n");
        free(groups);
        return;
    }

    if (options & OPT_U) {
        if (options & OPT_N) {
            printf("%s\n", pw->pw_name);
        } else {
            printf("%d\n", pw->pw_uid);
        }
    } else if (options & OPT_G) {
        if (options & OPT_N) {
            printf("%s\n", gr ? gr->gr_name : "unknown");
        } else {
            printf("%d\n", gr ? gr->gr_gid : gid);
        }
    } else if (options & OPT_CAP_G) {
        int ngroups = 0;
        gid_t *groups = NULL;
        if (getgrouplist(pw->pw_name, pw->pw_gid, NULL, &ngroups) == -1) {
            groups = malloc(sizeof(gid_t) * ngroups);
            if (groups) {
                if (getgrouplist(pw->pw_name, pw->pw_gid, groups, &ngroups) == -1) {
                }
            }
        }

        for (int i = 0; i < ngroups; i++) {
            if (options & OPT_N) {
                struct group *g = getgrgid(groups[i]);
                printf("%s%s", g ? g->gr_name : "unknown", (i == ngroups - 1) ? "" : " ");
            } else {
                printf("%d%s", groups[i], (i == ngroups - 1) ? "" : " ");
            }
        }
        printf("\n");
        free(groups);
    } else if (options & OPT_R) {
        printf("%d\n", geteuid());
    }
}

int main(int argc, char *argv[]) {
    uid_t target_uid = geteuid();
    gid_t target_gid = getegid();
    struct passwd *pw_lookup = NULL;
    int options = 0;
    int opt;

    while ((opt = getopt(argc, argv, "ugGrna")) != -1) {
        switch (opt) {
            case 'u': options |= OPT_U; break;
            case 'g': options |= OPT_G; break;
            case 'G': options |= OPT_CAP_G; break;
            case 'r': options |= OPT_R; break;
            case 'n': options |= OPT_N; break;
            case 'a': options |= OPT_A; break;
            case '?':
                fprintf(stderr, "Usage: %s [-ugGrna] [username]\n", argv[0]);
                return 1;
            default:
                abort();
        }
    }

    if (optind < argc) {
        char *username = argv[optind];
        pw_lookup = getpwnam(username);
        if (pw_lookup) {
            target_uid = pw_lookup->pw_uid;
            target_gid = pw_lookup->pw_gid;
        } else {
            fprintf(stderr, "id: '%s': no such user\n", username);
            return 1;
        }
    }

    print_user_info(target_uid, target_gid, options);

    return 0;
}
