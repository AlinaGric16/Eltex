#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(){

    char *output_buf = "String from file";
    char input_buf[20] = {0};

    int fd = open("output.txt", O_CREAT | O_RDWR, 0644);
    write(fd, output_buf, strlen(output_buf));

    off_t size = lseek(fd, 0, SEEK_END);

    for (int i = 0; i < size; i++) {
        lseek(fd, size - 1 - i, SEEK_SET);
        read(fd, &input_buf[i], 1);
    }

    printf("%s\n", input_buf);

    close(fd);
    return 0;
}