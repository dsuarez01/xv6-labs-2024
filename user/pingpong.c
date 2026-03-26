#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {
    
    if (argc != 1) {
        fprintf(2, "usage: pingpong\n");
        exit(1);
    }

    int fds[2];
    if (pipe(fds) < 0) {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid;

    if ((pid = fork()) < 0) {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    char b;

    if (pid > 0) { // parent
        int parent_pid = getpid();
        if (write(fds[1], &b, 1) < 1) {
            fprintf(2, "%d: write failed\n", parent_pid);
            exit(1);
        }
        close(fds[1]);

        int cstatus;
        wait(&cstatus);
        if (cstatus != 0) {
            exit(cstatus);
        }

        int rstatus;
        rstatus = read(fds[0], &b, 1);
        if (rstatus <= 0) {
            fprintf(2, "%d: read failed\n", parent_pid);
            exit(1);
        }

        printf("%d: received pong\n", parent_pid);
        close(fds[0]);
    } else if (pid == 0) { // child
        int child_pid = getpid();
        int rstatus;
        rstatus = read(fds[0], &b, 1);
        if (rstatus <= 0) {
            fprintf(2, "%d: read failed\n", child_pid);
            exit(1);
        }
        close(fds[0]);

        printf("%d: received ping\n", child_pid);
        if (write(fds[1], &b, 1) < 1) {
            fprintf(2, "%d: write failed\n", child_pid);
            exit(1);
        }
        close(fds[1]);
    }
}