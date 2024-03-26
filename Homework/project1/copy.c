
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int arg, char **argv){
    char c[10]; //file pointer;
    int fd; //file descript
    int nfd; //new file descript
    int size;

    if(arg != 3){
        printf("인자의 개수가 올바르지 않습니다\n");
        return 1;
    }

    fd = open(argv[1], O_RDONLY); //파일 오픈
    nfd = open(argv[2], O_RDWR|O_CREAT, 0751);	//복사본 파일 생성

    while ((size = read(fd, &c, 10)) == 10) //10byte만큼 read
        write(nfd, &c, 10); //new file에 write

    write(nfd, &c, size); //남은 데이터 write

    close(fd);
    close(nfd);
    return 0;
}
