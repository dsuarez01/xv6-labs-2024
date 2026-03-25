#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    
    char *argv_new[MAXARG];
    char buf[MAXARG][MAXPATH];

    char c;
    int argc_new;

    for (argc_new=0; argc_new<argc-1; ++argc_new) {
        argv_new[argc_new] = argv[argc_new+1];
    }

    int len = 0;
    while (read(0, &c, 1)) {
        if (argc_new >= MAXARG) {
            fprintf(2, "too many args\n");
            exit(1);
        } else if (len >= MAXPATH) {
            fprintf(2, "arg name too long\n");
            exit(1);
        } else if (c == ' ') {
            buf[argc_new][len] = 0;
            argv_new[argc_new] = buf[argc_new];
            argc_new++;
            len = 0;
        } else if (c  == '\n') {
            if (argc_new == argc-1 && len == 0) {
                continue;
            } else if (len > 0) {
                buf[argc_new][len] = 0;
                argv_new[argc_new] = buf[argc_new];
                if (++argc_new >= MAXARG) {
                    fprintf(2, "too many args\n");
                    exit(1);
                }
            }
            argv_new[argc_new] = 0; // null-terminate argv_new arr

            int pid;
            if ((pid = fork()) < 0) {
                fprintf(2, "fork failed\n");
                exit(1);
            }

            if (pid > 0) {
                int cstatus;
                wait(&cstatus);
                if (cstatus != 0) {
                    exit(cstatus);
                }
                // child exec was successful, now reset argv_new
                argc_new = argc-1;
                len = 0;
            } else if (pid == 0) {
                exec(argv_new[0], argv_new);
                fprintf(2, "child: exec failed\n");
                exit(1);
            }
        } else {
            buf[argc_new][len++] = c;
        }
    }
}