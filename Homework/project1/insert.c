
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int offset = atoi(argv[1]);
    char *data = argv[2];
    int fd;
    int index = 0;

    if(argc != 4){
        printf("Invalid ARG SIZE\n");
        return 1;
    }

    fd = open(argv[3], O_RDWR); //open file
    int size = lseek(fd, 0, SEEK_END); //check file size

    if(size < offset){
        while(data[index] != '\0') {
            write(fd, &data[index], 1); //insert data
            index++;
        }
        close(fd);
        return 0;
    }

    lseek(fd, offset+1, SEEK_SET); //set pointer
    char *backup = (char *)malloc(size - offset);
    read(fd, backup, size - offset-1); //backup

    lseek(fd, offset+1, SEEK_SET); //set pointer
    index = 0;
    while(data[index] != '\0') {
        write(fd, &data[index], 1); //insert data
        index++;
    }

    write(fd, backup, size-offset-1); //backup data

    close(fd);
    return 0;
}
