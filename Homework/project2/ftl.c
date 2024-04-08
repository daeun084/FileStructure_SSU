#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flash.h"
#include "flashdevicedriver.c"
// 필요한 경우 헤더파일을 추가한다

FILE *flashfp;	// fdevicedriver.c에서 사용

//
// 이 함수는 FTL의 역할 중 일부분을 수행하는데 물리적인 저장장치 flash memory에 Flash device driver를 이용하여 데이터를
// 읽고 쓰거나 블록을 소거하는 일을 한다.
// flash memory에 데이터를 읽고 쓰거나 소거하기 위해서 fdevicedriver.c에서 제공하는 인터페이스를
// 호출하면 된다. 이때 해당되는 인터페이스를 호출할 때 연산의 단위를 정확히 사용해야 한다.
// 참고로, 읽기와 쓰기는 페이지 단위이며 소거는 블록 단위이다.
//

typedef struct {
    char sector[SECTOR_SIZE];
    char spare[SPARE_SIZE];
} Page;

typedef struct {
    Page pages[BLOCK_SIZE];
} Block;



// 1번: flash file emulator
void create_flash_memory(char* flashFile, char* blocks){
    // create file
    flashfp = fopen(flashFile, "w+");

    // init block
    Block blockbuf = {0xFF, 0xFF};
    memset(&blockbuf, (char)0xFF, BLOCK_SIZE);

    // create n blocks
    for(int i= 0; i < atoi(blocks); i++){
        fwrite(&blockbuf, BLOCK_SIZE, 1, flashfp);
    }

    // close fp
    fclose(flashfp);
}


// 2번: page write
void write_page(char* flashFile, int ppn, char* sectorData, char* spareData, char* pagebuf){
    int pageReadCount = 0;
    int pageWriteCount = 0;
    int blockEraseCount = 0;

    if (ppn == 0 | ppn == 1 | ppn == 2 | ppn == 3){
        printf("Block[0] can't write\n");
        exit(1);
    }

    //open exist file
    flashfp = fopen(flashFile, "r+");
    if (flashfp == NULL){
        printf("file open error\n");
        exit(1);
    }

    // move fp
    fseek(flashfp, PAGE_SIZE*ppn, SEEK_SET);

    // read page
    Page page;
    pagebuf = (char*)&page;

    dd_read(ppn,pagebuf);
    pageReadCount++;

    //if page has data, overwrite
    if (page.sector[0] != (char)0xFF){
        // array for check overwrite page
        int arr[4] = {0};

        // find num in block's first page
        int block_num = ppn / PAGE_NUM;

        // copy data of page in block[0]
        for(int i = 0; i < PAGE_NUM; i++){
            if (i == ppn % PAGE_NUM) continue;

            // mv fp to block's i page
            dd_read(block_num*4 + i, pagebuf);
            pageReadCount++;
            if (page.sector[0] != (char)0xFF){
                // write to block[0]
                dd_write(i, pagebuf);
                pageWriteCount++;
                arr[i] = 1;
            }
        }

        //erase block
        dd_erase(block_num);
        blockEraseCount++;

        //rewrite data block[0]
        for(int i = 0; i < PAGE_NUM; i++){
            if (i == ppn % PAGE_NUM) continue;

            // mv fp to block[0]'s i page
            if(arr[i] == 1) {
                dd_read(i, pagebuf);
                pageReadCount++;

                if (page.sector[0] != (char) 0xFF) {
                    // write data to block's i page
                    dd_write(block_num * 4 + i, pagebuf);
                    pageWriteCount++;
                    arr[i] = 0;
                }
            }
        }

        //erase block[0]
        if(pageWriteCount > 0) {
            dd_erase(0);
            blockEraseCount++;
        }

    }

        // move fp to ppn
        fseek(flashfp, PAGE_SIZE*ppn, SEEK_SET);

        //write new data
        char* sector = page.sector;
        char* spare = page.spare;

        //save sector data
        memcpy(sector, sectorData, SECTOR_SIZE);
        // save 0xFF to sector in 512B
        if (strlen(sectorData) < SECTOR_SIZE) {
            memset(sector + strlen(sectorData), 0xFF, SECTOR_SIZE - strlen(sectorData));
        }

        //save spare data
        memcpy(spare, spareData, SPARE_SIZE);
        // save 0xFF to spare in 16B
        if (strlen(spareData) < SPARE_SIZE) {
            memset(spare + strlen(spareData), 0xFF, SPARE_SIZE - strlen(spareData));
        }

        //call interface
        dd_write(ppn, pagebuf);
        pageWriteCount++;


    // close fp
    fclose(flashfp);

    // print result
    printf("#pagereads=%d #pagewrites=%d #blockerases=%d\n", pageReadCount, pageWriteCount, blockEraseCount);
}


int main(int argc, char *argv[])
{	
	char sectorbuf[SECTOR_SIZE];
	char pagebuf[PAGE_SIZE];
	char *blockbuf;


    if(argv[1][0] == 'c')
        create_flash_memory(argv[2], argv[3]);
    else if(argv[1][0] == 'w')
        write_page(argv[2], atoi(argv[3]), argv[4], argv[5], pagebuf);


	// flash memory 파일 생성: 위에서 선언한 flashfp를 사용하여 flash 파일을 생성한다. 그 이유는 fdevicedriver.c에서 
	//                 flashfp 파일포인터를 extern으로 선언하여 사용하기 때문이다.
	// 페이지 쓰기: pagebuf의 sector 영역과 spare 영역에 각각 데이터를 정확히 저장하고 난 후 해당 인터페이스를 호출한다.
	// 페이지 읽기: pagebuf를 인자로 사용하여 해당 인터페이스를 호출하여 페이지를 읽어 온 후 여기서 sector 데이터와
	//                  spare 데이터를 분리해 낸다
	// memset(), memcpy() 등의 함수를 이용하면 편리하다. 물론, 다른 방법으로 해결해도 무방하다.

	return 0;
}
