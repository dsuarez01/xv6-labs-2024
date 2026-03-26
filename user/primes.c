#include "kernel/types.h"
#include "user/user.h"

void
filter(int read_fd) {

    int parent_pid = getpid();
    int rstatus, p;

    if ((rstatus = read(read_fd, &p, sizeof(p))) == 0) {
        return;
    } else if (rstatus < 0) {
        fprintf(2, "%d: read failed\n", parent_pid);
        exit(1);
    }

    printf("prime %d\n", p);

    // pipe
    int fds[2];
    if (pipe(fds) < 0) {
        fprintf(2, "%d: pipe failed\n", parent_pid);
        exit(1);
    }

    // fork
    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "%d: fork failed\n", parent_pid);
        exit(1);
    }

    if (pid > 0) {
        close(fds[0]);
        
        int n, wstatus;
        while((rstatus = read(read_fd, &n, sizeof(n))) > 0) {
            if (n%p != 0) {
                wstatus = write(fds[1], &n, sizeof(n));
                if (wstatus < 0 || wstatus != sizeof(n)) {
                    fprintf(2, "%d: failed to write %d\n", parent_pid, n);
                    exit(1);
                }
            }
        }

        if (rstatus < 0) {
            fprintf(2, "%d: read failed\n", parent_pid);
            exit(1);
        }

        close(read_fd);
        close(fds[1]);

        int cstatus;
        wait(&cstatus);
        if (cstatus != 0) {
            exit(cstatus);
        }
    } else if (pid == 0) {
        close(read_fd);
        close(fds[1]);
        filter(fds[0]);
        close(fds[0]);
    }
}

int 
main(int argc, char *argv[]) {

    if (argc != 1) {
        fprintf(2, "usage: primes\n");
        exit(1);
    }

    int parent_pid = getpid();

    int fds[2];

    if (pipe(fds) < 0) {
        fprintf(2, "%d: pipe failed\n", parent_pid);
        exit(1);
    }

    int pid;
    if ((pid = fork()) < 0) {
        fprintf(2, "%d: fork failed\n", parent_pid);
        exit(1);
    }

    if (pid > 0) {

        close(fds[0]); // parent doesn't use this read fd

        // send [2..280] over write pipe
        int i, wstatus;
        for (i=2; i<=280; ++i) {
            wstatus = write(fds[1], &i, sizeof(i));
            if (wstatus < 0 || wstatus != sizeof(i)) {
                fprintf(2, "%d: write failed at %d\n", parent_pid, i);
                exit(1);
            }
        }
        close(fds[1]);

        int cstatus;
        wait(&cstatus);
        if (cstatus != 0) {
            fprintf(2, "%d: error, exiting with status %d\n", parent_pid, cstatus);
            exit(cstatus);
        }
    } else if (pid == 0) {
        close(fds[1]); // child doesn't use this write fd
        filter(fds[0]);
        close(fds[0]);
    }
}