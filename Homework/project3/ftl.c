// 주의사항
// 1. blockmapping.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmapping.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmapping.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmapping.h"
// #include "flashdevicedriver.c"

// prototype
void ftl_print();


typedef struct {
    char sector[SECTOR_SIZE];
    char spare[SPARE_SIZE];
} Page;

typedef struct {
    Page pages[BLOCK_SIZE];
} Block;

static int free_block_pbn = 0;

static int address_mapping_table[DATABLKS_PER_DEVICE][2];

static int free_block_list[BLOCKS_PER_DEVICE];


void ftl_open()
{
	// init free block's pbn
	free_block_pbn = 0;

	// init address mapping table
	for (int i = 0 ; i < DATABLKS_PER_DEVICE ; i++){
		address_mapping_table[i][0] = i;
		address_mapping_table[i][1] = -1;
	}

	// init free_block_list
	for (int i = 0 ; i< BLOCKS_PER_DEVICE ; i++){
		// the blocks are free
		free_block_list[i] = -1; 
	}

	return;
}


void ftl_read(int lsn, char *sectorbuf)
{

	if (lsn > DATAPAGES_PER_DEVICE){
		printf("[Error] : invalid lsn\n");
		exit(1);
	}

	// ppn
	int lbn = lsn / PAGES_PER_BLOCK;
	int ppn = lsn % PAGES_PER_BLOCK;
	int pbn = address_mapping_table[lbn][1];

	if (pbn == -1){
		printf("[Error] : invalid pbn\n");
		exit(1);
	}


	// read page's data
	char * pagebuf;
	pagebuf = (char *)malloc(PAGE_SIZE);

	int ret = dd_read(pbn * PAGES_PER_BLOCK + ppn, pagebuf);
	if (ret == -1){
		printf("[Error] : fail to read data\n");
		exit(1);
	}


	// copy sector data to sectorbuf
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);
	free(pagebuf);

	return;
}


void ftl_write(int lsn, char *sectorbuf)
{
	if (lsn >= DATAPAGES_PER_DEVICE){
		printf("[Error] : invalid lsn\n");
		exit(1);
	}

	// ppn
	int lbn = lsn / PAGES_PER_BLOCK;
	int ppn = lsn % PAGES_PER_BLOCK;
	int pbn = address_mapping_table[lbn][1];


	// init pagebuf
	char *pagebuf;
	pagebuf = (char *)malloc(PAGE_SIZE);
	// copy sector data
	memcpy(pagebuf, sectorbuf, strlen(sectorbuf));
	memset(pagebuf + strlen(sectorbuf), 0xFF, SECTOR_SIZE - strlen(sectorbuf));
	// copy lsn data
	char sparebuf[5];
	sprintf(sparebuf, "%d", lsn);
	memcpy(pagebuf + SECTOR_SIZE, sparebuf, 4);
	memset(pagebuf + SECTOR_SIZE + 4, 0xFF, SPARE_SIZE - 4);	
	

	// 새로운 데이터 작성
	if (pbn == -1){
		// allocate new block
		for (int i = 0 ; i < BLOCKS_PER_DEVICE ; i++){
			if (free_block_list[i] == -1 && i != free_block_pbn){
				// update pbn
				pbn = i;
				// update address mapping table
				address_mapping_table[lbn][1] = pbn;
				// check that the block is allocated
				free_block_list[i] = 0;
				break;
			}
		}

		if (pbn == -1){
			printf("[Error] : fail to allocate new free block\n");
			exit(1);
		}

		// write data
		int ret = dd_write(pbn * PAGES_PER_BLOCK + ppn, pagebuf);
		if (ret == -1){
			printf("[Error] : fail to write data\n");
			exit(1);
		}

	}
	else {	
		// overwrite
		char *overwrite_pagebuf;
		overwrite_pagebuf = (char *)malloc(PAGE_SIZE);

		 // find num in block's first page number
        int block_first_page_num = pbn * PAGES_PER_BLOCK;

        // copy data of page in free block
        for(int i = 0; i < PAGES_PER_BLOCK; i++){
            if (i == ppn) continue;

			// read block's i page
			int ret = dd_read(block_first_page_num + i, overwrite_pagebuf);
			if (ret == -1){
				printf("[Error] : fail to read page data\n");
				exit(1);
			}


			// read page's spare data
			char spare[4];
			for (int i = SECTOR_SIZE ; i < SECTOR_SIZE + 4; i++){
				spare[i-SECTOR_SIZE] = overwrite_pagebuf[i];
			}


			// if the page is not empty, copy data
			if (memcmp(spare, "\xff\xff\xff\xff", 4) != 0)
				dd_write(free_block_pbn * PAGES_PER_BLOCK + i, overwrite_pagebuf);
			
		}
			// write new data
			dd_write(free_block_pbn * PAGES_PER_BLOCK + ppn, pagebuf);
		
		// 기존 block erase
		dd_erase(pbn);

		// 기존 block을 free block으로 update
		free_block_list[pbn] = -1;
		free_block_list[free_block_pbn] = 0;

		address_mapping_table[lbn][1] = free_block_pbn;
		free_block_pbn = pbn;


		// free buf
		free(overwrite_pagebuf);
	}

		// free buf
		free(pagebuf);

	return;
}


void ftl_print()
{
	printf("lbn pbn\n");

	for (int i = 0 ; i < DATABLKS_PER_DEVICE; i++){
		printf("%d %d\n", address_mapping_table[i][0], address_mapping_table[i][1]);
	}

	printf("free block=%d\n", free_block_pbn);

	return;
}
