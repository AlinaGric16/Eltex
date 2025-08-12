#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void){
    char *path = "text";
    char *buf_w = "Hi!";

    if(mkfifo(path, 0666) < 0){
        perror("failed mkfifo");
        exit(EXIT_FAILURE);
    }

    int fd = open(path, O_WRONLY);

    if(fd < 0){
        perror("failed open");
        unlink(path);
        exit(EXIT_FAILURE);
    }

    write(fd, buf_w, strlen(buf_w)+1);
    
    close(fd);
    return 0;
}