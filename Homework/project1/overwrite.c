
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int offset = atoi(argv[1]);
    char *data = argv[2];
    int fd;

    if(argc != 4){
        printf("Invalid ARG SIZE\n");
        return 1;
    }

    fd = open(argv[3], O_RDWR); //open file
    lseek(fd, offset, SEEK_SET); //set pointer

    int index = 0;
    while (data[index] != '\0') {
        write(fd, &data[index], 1);
        index++;
    }

    close(fd);

    return 0;
}