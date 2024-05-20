#include <stdio.h>	
#include "student.h"
#include <string.h>
#include <stdlib.h>

const char* fields[] = {"ID", "NAME", "DEPT", "YEAR", "ADDR", "PHONE", "EMAIL"};


int readPage(FILE *fp, char *pagebuf, int pagenum);
int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum);
void unpack(const char *recordbuf, STUDENT *s);


// methods for save student obj to record file
int writePage(FILE *fp, const char *pagebuf, int pagenum);
void writeRecToPagebuf(char *pagebuf, const char *recordbuf);
void pack(char *recordbuf, const STUDENT *s);


// find record and print the result
void search(FILE *fp, FIELD f, char *keyval);
void printSearchResult(const STUDENT *s, int n);


// add new student record to record file
void insert(FILE *fp, const STUDENT *s);


// switching record's field name to enum FIELD type value
FIELD getFieldID(char *fieldname);


// read or update record file's header
int readFileHeader(FILE *fp, char *headerbuf);
int writeFileHeader(FILE *fp, const char *headerbuf);


// make student struct from input data
void parse_input_to_student(char *argv[], STUDENT *s){
	for (int i = 3; i < 10; i++){
		// find field name
		char* field = strtok(argv[i], "=");
        char* value = strtok(NULL, "=");
        FIELD f = getFieldID(field);

        switch (f){
            case ID: strcpy(s->id, value); break;
            case NAME: strcpy(s->name, value); break;
            case DEPT: strcpy(s->dept, value); break;
            case YEAR: strcpy(s->year, value); break;
            case ADDR: strcpy(s->addr, value); break;
            case PHONE: strcpy(s->phone, value); break;
            case EMAIL: strcpy(s->email, value); break;
        }
	}
}

/**read**/
int readPage(FILE *fp, char *pagebuf, int pagenum){
	// fseek
	int page_location = FILE_HEADER_SIZE + PAGE_SIZE * pagenum;
	fseek(fp, page_location, SEEK_SET);

	// read page data
	int ret;
	ret = fread(pagebuf, PAGE_SIZE, 1, fp);
	// printf("[ReadPage] pagebuf : %C %C %C %C\n", pagebuf[0], pagebuf[1], pagebuf[2], pagebuf[3]);
	printf("[ReadPage] pagebuf : %s\n", pagebuf);

	if (ret == 1) 
		return 1;
	else 
		return 0;
}


int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum){
	// read offset
	int offset = 8 + 2 * recordnum;

	int16_t record_end, record_start;
	memcpy(&record_end, pagebuf + offset, 2);
	memcpy(&record_start, pagebuf + offset - 2, 2);

	int record_len = record_end - record_start;

	if (record_len > 100){
		printf("[*] Error : Invalid Record Length\n");
    	exit(1);
	}

	// read record data
	strncpy(recordbuf, pagebuf + record_start + 1, record_len);
	return 0;
}


void unpack(const char *recordbuf, STUDENT *s){
	int offset = 0;
	while (offset < 7){
		char *field = strtok((char *)recordbuf, "#");
		switch (offset) {
			case 0: strcpy(s->id, field); break;
			case 1: strcpy(s->name, field); break;
			case 2: strcpy(s->dept, field); break;
			case 3: strcpy(s->year, field); break;
			case 4: strcpy(s->addr, field); break;
			case 5: strcpy(s->phone, field); break;
			case 6: strcpy(s->email, field); break;
		}
		offset++;
	}
}


/**write**/
int writePage(FILE *fp, const char *pagebuf, int pagenum){
	// fseek
	int page_location = FILE_HEADER_SIZE + PAGE_SIZE * pagenum;
	fseek(fp, page_location, SEEK_SET);

	// write page data
	int ret;
	ret = fwrite(pagebuf, PAGE_SIZE, 1, fp);
	if (ret == 1) 
		return 1;
	else 
		return 0;
}

void writeRecToPagebuf(char *pagebuf, const char *recordbuf){
	// find page's record num
	uint16_t record_num;
	memcpy(&record_num, pagebuf, 2);
	printf("[WriteRecToPagebuf] recordNum = %hd, pagebuf = %s\n", record_num, pagebuf);

	// last record's offset locatioin in header
	int offset_location = 8 + 2 * record_num;
	
	// record start index
	uint16_t record_start;
	memcpy(&record_start, pagebuf + offset_location, 2);
	printf("[WriteRecToPagebuf] record_start = %hd\n", record_start);

	// write record data in pagebuf
	strncpy(pagebuf + record_start, recordbuf, strlen(recordbuf));
	printf("[WriteRecToPagebuf] recordbuf = %s, pagebuf = %s\n", recordbuf, pagebuf);
}

int pack_filled(char *recordbuf, const char *field, int offset){
	size_t len = strlen(field);
	memcpy(recordbuf + offset, field, len);
	offset += len;
	recordbuf[offset++] = '#';
	return offset;
}

void pack(char *recordbuf, const STUDENT *s){
	// init recordbuf
	memset(recordbuf, 0, MAX_RECORD_SIZE);

	// set offset
	int offset = 0;
	offset = pack_filled(recordbuf, s->id, offset);
	offset = pack_filled(recordbuf, s->name, offset);
	offset = pack_filled(recordbuf, s->dept, offset);
	offset = pack_filled(recordbuf, s->year, offset);
	offset = pack_filled(recordbuf, s->addr, offset);
	offset = pack_filled(recordbuf, s->phone, offset);
	offset = pack_filled(recordbuf, s->email, offset);
}


/**switch**/
FIELD getFieldID(char *fieldname){

	for (int i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
		if (strcmp(fieldname, fields[i]) == 0) {
			switch (i) {
				case 0: return ID;
				case 1: return NAME;
				case 2: return DEPT;
				case 3: return YEAR;
				case 4: return ADDR;
				case 5: return PHONE;
				case 6: return EMAIL;
			}
		}
	}
	printf("[*] Error : Invalid Field Name\n");
    exit(1);
}


int isValueSame(FIELD f, char *keyval, const STUDENT *s){
	switch (f){
		case ID:
			return strcmp(s->id, keyval);
		case NAME:
			return strcmp(s->name, keyval);
		case DEPT:
			return strcmp(s->dept, keyval);
		case YEAR:
			return strcmp(s->year, keyval);
		case ADDR:
			return strcmp(s->addr, keyval);
		case PHONE:
			return strcmp(s->phone, keyval);
		case EMAIL:
			return strcmp(s->email, keyval);
		default:
			return 0;
	}
}


/**find record**/
void search(FILE *fp, FIELD f, char *keyval){
	char recordbuf[MAX_RECORD_SIZE];
	char fileheaderbuf[FILE_HEADER_SIZE];
	char pagebuf[PAGE_SIZE];

	// file header
	readFileHeader(fp, fileheaderbuf);

	// 전체 페이지 개수
	int16_t page_num;
	memcpy(&page_num, fileheaderbuf, 2);

	// 출력을 위한 설정
	STUDENT s_list[page_num * 20];
	int n = 0;
		
	for (int i = 0 ; i < page_num; i++){
		// 순차적으로 페이지 read
		readPage(fp, pagebuf, i);

		// read page's header
		char pageheaderbuf[PAGE_HEADER_SIZE];
		memcpy(pageheaderbuf, pagebuf, PAGE_HEADER_SIZE);

		// page의 레코드 개수
		int16_t record_num;
		memcpy(&record_num, pageheaderbuf, 2);
		fseek(fp, 6, SEEK_CUR);
			
		for (int j = 0; j < record_num; j++){
			// j번째 레코드 읽기
			getRecFromPagebuf(pagebuf, recordbuf, j);

			// record 객체를 student 객체로 변환
			STUDENT s;
			unpack(recordbuf, &s);

			// 각 필드 값 비교
			if (isValueSame(f, keyval, &s) == 1 ){
				// keyval과 s.f의 값이 일치하면 s_list에 추가
				s_list[n++] = s;
			}				
		}
	}

	// 결과 출력
	printSearchResult(s_list, n);
}


// insert new record data
void insert(FILE *fp, const STUDENT *s){
	char recordbuf[MAX_RECORD_SIZE] = {0};
	char fileheaderbuf[FILE_HEADER_SIZE] = {0};
	char pagebuf[PAGE_SIZE] = {0};
	
	// pack student struct to record
	pack(recordbuf, s);
	printf("[Insert] recordbuf_size : %lu\n", strlen(recordbuf));	

	// find the last page or make new page
	// read record file's header 
	readFileHeader(fp, fileheaderbuf);

	// find last page's number
	uint16_t last_page_num;
	memcpy(&last_page_num, fileheaderbuf, 2);
	printf("[Insert] last_page_num : %u\n", last_page_num - 1);

	// read last page
	readPage(fp, pagebuf, last_page_num - 1);

	// compare freespace < recordbuf size	
	uint16_t freespace_size;
	memcpy(&freespace_size, pagebuf + 2, 2);
	printf("[Insert] freespace_size : %d\n", freespace_size);

	if (freespace_size < strlen(recordbuf)){
		printf("[Insert] --allocate new page--\n");
		// allocate new page
		memset(pagebuf, 0, PAGE_SIZE);

		// write page's header
		// #records
		uint16_t num_records = 0;
		memcpy(pagebuf, &num_records, 2);

		// freespace
    	uint16_t free_space = PAGE_SIZE - PAGE_HEADER_SIZE;
		memcpy(pagebuf + 2, &free_space, 2);

		// reserved_space
    	uint32_t reserved_space = 0;
    	memcpy(pagebuf + 4, &reserved_space, 4);

		// record file's header : 전체 페이지 개수 수정		
		uint16_t num_pages = last_page_num + 1;
		memcpy(fileheaderbuf, &num_pages, 2);	
		writeFileHeader(fp, fileheaderbuf);
	}

	// write record to page buffer
	writeRecToPagebuf(pagebuf, recordbuf);
	printf("[Insert] writeRecToPagebuf\n");

	// page's header : chore #records
	uint16_t record_num;
	memcpy(&record_num, pagebuf, 2);	
	printf("[Insert] recordnum = %d\n", record_num);
	record_num += 1;
	memcpy(pagebuf, &record_num, 2);
		
	printf("[Insert] after chore #records / pagebuf = %s\n", pagebuf);

	// page's header : records' offset
	size_t record_size = strlen(recordbuf);
	int offset_location = 8 + 2 * (record_num - 1);

	uint16_t last_offset;
	memcpy(&last_offset, pagebuf + offset_location, 2);

	uint16_t new_offset = last_offset + record_size;
	memcpy(pagebuf + offset_location + 2, &new_offset, 2);
		
	printf("[Insert] after chore offsets / pagebuf = %s, last_offset = %d, new_offset = %d\n", pagebuf, last_offset, new_offset);

	// page's header : freespace
	uint16_t new_freespace = freespace_size - strlen(recordbuf);
	memcpy(pagebuf + 2, &new_freespace, 2);

	printf("[Insert] after chore freespace / pagebuf = %s, new_freespace=%d\n", pagebuf, new_freespace);

	// write page buffer to record file
	writePage(fp, pagebuf, last_page_num);
	printf("[Insert] writePage\n");
}


/**file header**/
int readFileHeader(FILE *fp, char *headerbuf){
	int ret;

	fseek(fp, 0, SEEK_SET);
	ret = fread(headerbuf, FILE_HEADER_SIZE, 1, fp);
	printf("[ReadFileHeader] headerbuf : %s\n", headerbuf);
	if (ret == 1)
		return 1;
	else
		return 0;
}

int writeFileHeader(FILE *fp, const char *headerbuf){
	int ret;

	ret = fwrite(headerbuf, FILE_HEADER_SIZE, 1, fp);
	if (ret == 1)
		return 1;
	else
		return 0;
}



int main(int argc, char *argv[])
{
	// use c lib op
	FILE *fp;

	if (argc < 3){
		printf("[*] Error : Invalid argc\n");
		exit(1);
	}

	// open record file
	fp = fopen(argv[2], "r+");
    if (fp == NULL) {
        // 파일이 존재하지 않으면 "w+" 모드로 파일을 새로 생성
		printf("new file\n");
        fp = fopen(argv[2], "w+");
        if (fp == NULL) {
            printf("[*] Error : File Open Error\n");
            exit(1);
        }

		// 파일의 맨 앞 2B를 01으로 초기화
		char fileheaderbuf[FILE_HEADER_SIZE] = {0};
		uint16_t num_pages = 1;
    	uint32_t reserved_space = 0;
		memcpy(fileheaderbuf, &num_pages, 2);
		memcpy(fileheaderbuf + 2, &reserved_space, 14);
		writeFileHeader(fp, fileheaderbuf);

		// init 0 page
		char pagebuf[PAGE_SIZE] = {0};

		uint16_t num_records = 0;
    	uint16_t free_space = PAGE_SIZE - PAGE_HEADER_SIZE;
		uint16_t first_offset = 8;
    	memcpy(pagebuf, &num_records, 2);
    	memcpy(pagebuf + 2, &free_space, 2);
    	memcpy(pagebuf + 4, &reserved_space, 4);
		memcpy(pagebuf + 8, &first_offset, 2);
		writePage(fp, pagebuf, 0);
    }

    
	if (argv[1][1] == 'i'){
		if (argc != 10){
			printf("[*] Error : Invalid argc\n");
			exit(1);
		}

		// make student struct
		STUDENT s;
		parse_input_to_student(argv, &s);

		// insert record data to record file
		insert(fp, &s);

	} else if (argv[1][1] == 's'){
		if (argc != 4){
			printf("[*] Error : Invalid argc\n");
			exit(1);
		}
		
		// get field name
		char* fieldname = strtok(argv[3], "=");
		FIELD f = getFieldID(fieldname);
		// search record
		search(fp, f, argv[3]);

	} else {
		printf("[*] Error : Invalid argument\n");
        exit(1);
	}

	fclose(fp);

	return 1;
}

void printSearchResult(const STUDENT *s, int n)
{
	int i;

	printf("#Records = %d\n", n);
	
	for(i=0; i<n; i++)
	{
		printf("%s#%s#%s#%s#%s#%s#%s\n", s[i].id, s[i].name, s[i].dept, s[i].year, s[i].addr, s[i].phone, s[i].email);
	}
}
