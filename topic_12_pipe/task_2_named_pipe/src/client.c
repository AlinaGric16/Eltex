#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

int main(void){
    char *path = "text";
    char buf_r[5];
    int fd = open(path, O_RDONLY);

    if(fd < 0){
        perror("failed open");
        exit(EXIT_FAILURE);
    }

    if(read(fd, buf_r, sizeof(buf_r)) < 0) {
        perror("failed read");
        exit(EXIT_FAILURE);
    }

    close(fd);
    unlink(path);

    printf("%s\n", buf_r);
    return 0;
}