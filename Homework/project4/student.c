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
	printf("[ReadPage] pagebuf : %02X %02X %02X\n", pagebuf[0], pagebuf[1], pagebuf[2]);
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
	int16_t record_num;
	memcpy(&record_num, pagebuf, 2);

	// last record's offset locatioin in header
	int offset_location = 8 + 2 * record_num;
	
	// record start index
	int16_t record_start;
	memcpy(&record_start, pagebuf + offset_location, 2);

	// write record data in pagebuf
	strncpy(pagebuf + record_start + 1, recordbuf, strlen(recordbuf));
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
	char recordbuf[MAX_RECORD_SIZE];
	char fileheaderbuf[FILE_HEADER_SIZE];
	char pagebuf[PAGE_SIZE];
	
	// pack student struct to record
	pack(recordbuf, s);
	printf("[Insert] recordbuf_size : %lu\n", strlen(recordbuf));	

	// find the last page or make new page
	// read record file's header 
	readFileHeader(fp, fileheaderbuf);

	// find last page's number
	int16_t last_page_num;
	memcpy(&last_page_num, fileheaderbuf, 2);
	// printf("[Insert] fileheaderbuf : %s\n", fileheaderbuf);
	printf("[Insert] fileheaderbuf : %0X %0X %0X %02X ...\n", fileheaderbuf[0], fileheaderbuf[1], fileheaderbuf[2], fileheaderbuf[3]);	
	printf("[Insert] last_page_num : %d\n", last_page_num);

	// read last page
	readPage(fp, pagebuf, last_page_num);

	// compare freespace < recordbuf size	
	int16_t freespace_size;
	memcpy(&freespace_size, pagebuf, 2);
	printf("[Insert] freespace_size : %d\n", freespace_size);


	if (freespace_size < strlen(recordbuf)){
		// allocate new page
		memset(pagebuf, 0, PAGE_SIZE);

		// write page's header
		// #records
		memset(pagebuf, 0, 2);
		// freespace
		memcpy(pagebuf + 2, "448", 2);

		// record file's header : 전체 페이지 개수 수정
		memcpy(fileheaderbuf, (void *)(intptr_t)(last_page_num + 1), 2);	
		writeFileHeader(fp, fileheaderbuf);
	}

	// write record to page buffer
	writeRecToPagebuf(pagebuf, recordbuf);
	printf("[Insert] writeRecToPagebuf\n");


	// page's header : chore #records
	int16_t record_num;
	memcpy(&record_num, pagebuf, 2);
	memcpy(pagebuf, (void *)(intptr_t)(record_num + 1), 2);
		
	// page's header : records' offset
	size_t record_size = strlen(recordbuf);
	int offset_location = 8 + 2 * (record_num - 1);
	int16_t last_offset;
	memcpy(&last_offset, pagebuf + offset_location, 2);
	memcpy(pagebuf + offset_location + 2, (void *)(last_offset + record_size), 2);
		
	// write page buffer to record file
	writePage(fp, pagebuf, last_page_num);
	printf("[Insert] writePage\n");
}


/**file header**/
int readFileHeader(FILE *fp, char *headerbuf){
	int ret;

	fseek(fp, 0, SEEK_SET);
	ret = fread(headerbuf, 1, FILE_HEADER_SIZE, fp);
	printf("[ReadFileHeader] headerbuf : %02X %02X %02X %02X\n", headerbuf[0], headerbuf[1], headerbuf[2], headerbuf[3]);
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
        fp = fopen(argv[2], "w+");
        if (fp == NULL) {
            printf("[*] Error : File Open Error\n");
            exit(1);
        }

		// 파일의 맨 앞 2B를 0으로 초기화
    	fwrite("0", 1, 2, fp);

		// reserved space + 0 page
		fseek(fp, 14 + 512, SEEK_CUR);
    	fwrite("\0", 1, 1, fp);

		// init 0 page
		int16_t init_free_space = 448;
		fseek(fp, 16, SEEK_SET);
		fwrite("0", 1, 2, fp);
		fwrite(&init_free_space, sizeof(init_free_space), 1, fp);
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
