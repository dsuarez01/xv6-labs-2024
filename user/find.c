#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "user/user.h"

char*
basename(char *path)
{
  static char buf[MAXPATH];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  memmove(buf, p, strlen(p)+1);
  return buf;
}

void
find(char *path, char *name) {
    char buf[MAXPATH], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        exit(1);
    }

    // if it's a DEVICE, do nothing
    // if it's a FILE and basename matches name, print full path you're at
    // if it's a DIR, call find on every path w/ basename isn't "." or ".."

    switch(st.type) {
    case T_DEVICE:
    case T_FILE:
        if (strcmp(basename(path), name) == 0) {
            printf("%s\n", path);
        }
        break;
    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > MAXPATH) {
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        
        int rstatus;
        while((rstatus = read(fd, &de, sizeof(de))) == sizeof(de)) {
            if (
                de.inum == 0 || 
                strcmp(de.name, ".") == 0 || 
                strcmp(de.name, "..") == 0
            ) {
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find(buf, name);
        }

        if (rstatus < 0) {
            printf("find: failed to read entry, skipping rest of directory...\n");
        }

        break;
    }
    close(fd);
}

int
main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(2, "usage: find <dir> <name>\n");
        exit(1);
    }

    find(argv[1], argv[2]);
}