
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int offset = atoi(argv[1]);
    int bytes = atoi(argv[2]);
    char c[abs(bytes)+1];
    int fd;

    if(argc != 4){
        printf("Invalid ARG SIZE");
        return 1;
    }

    if(bytes == 0)
        return 0;

    fd = open(argv[3], O_RDONLY); //파일 오픈
    int size = lseek(fd, 0, SEEK_END); //파일 크기 확인

    if(offset < 0) offset = size + offset; //offset 예외처리


    if(bytes > 0){
        lseek(fd, offset+1, SEEK_SET); //포인터 이동
        if(bytes + offset >= size){
            read(fd, &c, size - offset-1);
            c[size-offset-1] = '\0';
        } else {
            read(fd, &c, bytes);
            c[bytes] = '\0';
        }
    } else { //bytes < 0
        if((bytes + offset) >= 0){
            lseek(fd, offset + bytes, SEEK_SET); //포인터 이동
            read(fd, &c, -bytes);
            c[-bytes] = '\0';
        } else { //절댓값만큼 데이터가 존재하지 않는다면
            lseek(fd, 0, SEEK_SET);
            read(fd, &c, offset);
            c[offset] = '\0';
        }
    }
    printf("%s\n", c);
    close(fd);

    return 0;
}
