
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int offset = atoi(argv[1]);
    int bytes = atoi(argv[2]);
    int fd;

    if(argc != 4){
        printf("Invalid ARG SIZE");
        return 1;
    }

    if(bytes == 0)
        return 0;

    fd = open(argv[3], O_RDWR); //파일 오픈
    int size = lseek(fd, 0, SEEK_END); //파일 크기 확인

    if(bytes > 0){
        if(bytes + offset >= size){
            ftruncate(fd, offset + 1);
        } else {
            int remainByte = size - offset - bytes;
            char *buffer = (char*) malloc(remainByte);

            lseek(fd, offset+bytes+1, SEEK_SET); //포인터 이동
            read(fd, buffer, remainByte); //read remain data

            lseek(fd, offset + 1, SEEK_SET); //포인터 이동
            write(fd, buffer, remainByte); //write remain data
            free(buffer);
            ftruncate(fd, size - bytes);
        }
    } else { //bytes < 0
        int remainByte = size - offset;
        char *buffer = (char*) malloc(remainByte);
        lseek(fd, offset, SEEK_SET); //포인터 이동
        read(fd, buffer, remainByte); //read remain data

        if((bytes + offset) >= 0){
            lseek(fd, offset+bytes, SEEK_SET); //포인터 이동
            write(fd, buffer, remainByte); //write remain data
            lseek(fd, size+bytes, SEEK_SET); //포인터 이동
            ftruncate(fd, size + bytes);
        } else { //절댓값만큼 데이터가 존재하지 않는다면
            lseek(fd, 0, SEEK_SET); //포인터 이동
            write(fd, buffer, remainByte); //write remain data
            lseek(fd, remainByte, SEEK_SET); //포인터 이동
            ftruncate(fd, remainByte);
        }
        free(buffer);
    }

    close(fd);

    return 0;
}