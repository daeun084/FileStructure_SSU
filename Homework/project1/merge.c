
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int fd1; //file1 descripter
    int fd2; //file2 descripter
    int nfd; //new file descripter
    char c;

    if(argc != 4){
        printf("Invalid ARG SIZE\n");
        return 1;
    }

    fd1 = open(argv[1], O_RDONLY); //파일 오픈
    fd2 = open(argv[2], O_RDONLY); //파일 오픈
    nfd = open(argv[3], O_RDWR|O_CREAT, 0751);	//새로운 파일 생성

    //write
    while (read(fd1, &c, sizeof(c)) > 0) {
        write(nfd, &c, sizeof(c));
    }

    while (read(fd2, &c, sizeof(c)) > 0) {
        write(nfd, &c, sizeof(c));
    }

    close(fd1);
    close(fd2);
    close(nfd);

return 0;
}