#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(2, "usage: sleep <num_seconds>\n");
        exit(1);
    }

    // system call sleep(int)
    sleep(atoi(argv[1]));
}