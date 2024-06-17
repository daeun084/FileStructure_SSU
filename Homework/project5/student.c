#include <stdio.h>	
#include "student.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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
int searchByID(FILE *fp, char *keyval, char *recordbuf, int *pagenum, int *recordnum);

// add new student record to record file
void insert(FILE *fp, const STUDENT *s);
void insertNewSpace(FILE *fp, char *recordbuf, char *fileheaderbuf, int page_num);
void allocateNewPage(FILE *fp, char *fileheaderbuf, char *pagebuf, uint16_t last_page_num);
int findRightSize(FILE *fp, int16_t pagenum, int16_t recordnum, int strlen);

// delete record with id
void delete(FILE *fp, char *keyval);

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
	if (ret == 1) 
		return 1;
	else 
		return 0;
}

int getRecStartOffset(const char *pagebuf, int recordnum){
	int record_start;
	int offset_location;

	if (recordnum == 0) 
		return PAGE_HEADER_SIZE;
	else 
		offset_location = 8 + 2 * (recordnum - 1);
	
	memcpy(&record_start, pagebuf + offset_location, 2);
	record_start = record_start % PAGE_SIZE - FILE_HEADER_SIZE;

	printf("[*] RECORD_START=%d, RECORD_NUM=%d, RECORD_OFFSET=%d\n", record_start, recordnum, offset_location);
	return record_start;
}

int getRecLen(const char *pagebuf, int recordnum, int record_start){
	// last record's offset location in page's header
	int offset_location;
	if (recordnum == 0) 
		offset_location = 8;
	else 
		offset_location = 8 + 2 * (recordnum - 1);

	// record start index
	uint16_t record_end;
	memcpy(&record_end, pagebuf + offset_location + 2, 2);
	
	// get record's length
	record_end = record_end % PAGE_SIZE - FILE_HEADER_SIZE;
	
	int record_len = record_end - record_start;
	return record_len;
}


int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum){
	// get record's length
	int record_start = getRecStartOffset(pagebuf, recordnum);
	int record_len = getRecLen(pagebuf, recordnum, record_start);
	printf("[*] RECORD_OFFSET=%d RECORD_LEN=%d\n", record_start, record_len);

	// handle record's length error
	if (record_len > 100){
		printf("[*] Error : Invalid Record Length\n");
    	exit(1);
	}

	// read record data
	if (recordnum == 0) record_start = PAGE_HEADER_SIZE;
	memcpy(recordbuf, pagebuf + record_start, record_len);
	return 0;
}


void unpack(const char *recordbuf, STUDENT *s){
	int offset = 0;
	char *saveptr = NULL;
	char *field = strtok_r((char *)recordbuf, "#", &saveptr);

	while (field != NULL || offset < 7){
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
		field = strtok_r(NULL, "#", &saveptr);
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
	if (ret == 1) return 1;
	else return 0;
}

void writeRecToPagebuf(char *pagebuf, const char *recordbuf){
	// find page's record num
	uint16_t record_num;
	memcpy(&record_num, pagebuf, 2);

	// last record's offset location in header
	int offset_location;
	if (record_num == 0) offset_location = 8;
	else offset_location = 8 + 2 * (record_num - 1);
	
	// record start index
	uint16_t record_start;
	memcpy(&record_start, pagebuf + offset_location, 2);
	
	// write record data in pagebuf
	record_start = record_start % PAGE_SIZE - FILE_HEADER_SIZE;
	memcpy(pagebuf + record_start, recordbuf, strlen(recordbuf));
}

void writeRecToPagebufByRecordNum(char *pagebuf, const char *recordbuf, int recordnum){
	// last record's offset location in header
	int offset_location;
	if (recordnum == 0){
		memcpy(pagebuf + PAGE_HEADER_SIZE, recordbuf, strlen(recordbuf));
		return;
	}
	else offset_location = 8 + 2 * (recordnum - 1);
 
	// record start index
	uint16_t record_start;
	memcpy(&record_start, pagebuf + offset_location, 2);
	
	// write record data in pagebuf
	record_start = record_start % PAGE_SIZE - FILE_HEADER_SIZE;
	memcpy(pagebuf + record_start, recordbuf, strlen(recordbuf));
}

int pack_filled(char *recordbuf, const char *field, int offset){
	size_t len = strlen(field);
	memcpy(recordbuf + offset, field, len);

	// set offset
	offset += len;

	// add dilimiter
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
			return 1;
	}
}


/**find record**/
void search(FILE *fp, FIELD f, char *keyval){
	char recordbuf[MAX_RECORD_SIZE] = {0};
	char fileheaderbuf[FILE_HEADER_SIZE] = {0};
	char pagebuf[PAGE_SIZE] = {0};

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

		// page의 레코드 개수
		int16_t record_num;
		memcpy(&record_num, pagebuf, 2);
			
		for (int j = 0; j < record_num; j++){
			// j번째 레코드 읽기
			getRecFromPagebuf(pagebuf, recordbuf, j);

			// record 객체를 student 객체로 변환
			STUDENT s;
			unpack(recordbuf, &s);

			// 각 필드 값 비교
			if (isValueSame(f, keyval, &s) == 0 ){
				// keyval과 s.f의 값이 일치하면 s_list에 추가
				s_list[n++] = s;
			}				
		}
	}
	// 결과 출력
	printSearchResult(s_list, n);
}


// search by id and save in recordbuf
int searchByID(FILE *fp, char *keyval, char *recordbuf, int *pagenum, int *recordnum){
	char fileheaderbuf[FILE_HEADER_SIZE] = {0};
	char pagebuf[PAGE_SIZE] = {0};

	// file header
	readFileHeader(fp, fileheaderbuf);
	printf("[*] READFILEHEAD\n");

	// 전체 페이지 개수
	int16_t page_num;
	memcpy(&page_num, fileheaderbuf, 2);

	for (int16_t i = 0 ; i < page_num; i++){
		// 순차적으로 페이지 read
		readPage(fp, pagebuf, i);
		printf("[*] READ %dPAGE\n", i);

		// page의 레코드 개수
		int16_t record_num;
		memcpy(&record_num, pagebuf, 2);
		printf("[*] SEARCHBYID // PAGENUM=%d, RECORDNUM=%d\n", page_num, record_num);
			
		for (int16_t j = 0; j < record_num; j++){
			// j번째 레코드 읽기
			getRecFromPagebuf(pagebuf, recordbuf, j);
			printf("[*] READ %dRECORD\n", j);

			// record 객체를 student 객체로 변환
			STUDENT s;
			unpack(recordbuf, &s);

			if (strcmp(s.id, keyval) == 0){
				pack(recordbuf, &s);
				*pagenum = i;
				*recordnum = j;
				return 1;
			}				
		}
	}
	return 0;
}


// insert new record data
void insert(FILE *fp, const STUDENT *s){
	char recordbuf[MAX_RECORD_SIZE] = {0};
	char fileheaderbuf[FILE_HEADER_SIZE] = {0};
	char pagebuf[PAGE_SIZE] = {0};
	
	// pack student struct to record
	pack(recordbuf, s);

	// find the last page or make new page
	// read record file's header 
	readFileHeader(fp, fileheaderbuf);

	// header's page num, record num
	uint16_t last_page_num;
	int16_t pagenum;
	int16_t recordnum;
	
	memcpy(&last_page_num, fileheaderbuf, 2);
	memcpy(&pagenum, fileheaderbuf + 2, 2);
	memcpy(&recordnum, fileheaderbuf + 4, 2);

	// find right size in removed record linked list
	int ret = findRightSize(fp, pagenum, recordnum, strlen(recordbuf));
	if (ret == 0){
		// 새롭게 추가
		printf("[*] INSERT NEW SPACE");
		insertNewSpace(fp, recordbuf, fileheaderbuf, last_page_num);
	}
	else {
		printf("[*] INSERT EXISTING SPACE");
		printf("[*] PAGENUM=%d RECORDNUM=%d", pagenum, recordnum);
		readPage(fp, pagebuf, pagenum);

		// fix linked list
		char previousrecordbuf[MAX_RECORD_SIZE] = {0};
		getRecFromPagebuf(pagebuf, previousrecordbuf, recordnum);
		uint16_t previous_page_num, previous_record_num;
		memcpy(&previous_page_num, previousrecordbuf + 1, 2);
		memcpy(&previous_record_num, previousrecordbuf + 3, 2);

		// 기존 레코드의 길이 확인
		int record_start = getRecStartOffset(pagebuf, recordnum);
		int record_len = getRecLen(pagebuf, recordnum, record_start);

		// 페이지 버퍼의 해당 위치에 길이만큼 레코드 데이터 추가
		memcpy(pagebuf + record_start, recordbuf, strlen(recordbuf));
		memset(pagebuf + record_start + strlen(recordbuf), 0xff, record_len - strlen(recordbuf));
		
		writePage(fp, pagebuf, pagenum);
	}
}

// insert new record
void insertNewSpace(FILE *fp, char *recordbuf, char *fileheaderbuf, int page_num){
	char pagebuf[PAGE_SIZE] = {0};

	// insert new record 
	// read last page
	readPage(fp, pagebuf, page_num - 1);

	// compare freespace < recordbuf size	
	uint16_t freespace_size;
	memcpy(&freespace_size, pagebuf + 2, 2);
	printf("[*] PAGENUM=%d FREESPACE=%d\n", page_num, freespace_size);


	// cmp freespace, recordbuf's len
	if (freespace_size < strlen(recordbuf)){
		printf("[*] ALLOCATE NEW PAGE");

		// allocate new page
		allocateNewPage(fp, fileheaderbuf, pagebuf, page_num);
		
		freespace_size = PAGE_SIZE - PAGE_HEADER_SIZE;
		page_num = page_num + 1;
	}

	printf("[*] WRITE REC TO PAGE");

	// write record to page buffer
	writeRecToPagebuf(pagebuf, recordbuf);

	// page's header : chore #records
	uint16_t record_num;
	memcpy(&record_num, pagebuf, 2);	
	record_num = record_num + 1;
	memcpy(pagebuf, &record_num, 2);

	// page's header : records' offset
	int record_size = strlen(recordbuf);
	int offset_location;
	uint16_t new_offset;

	if (record_num == 1){
		// first record
		new_offset = FILE_HEADER_SIZE + PAGE_SIZE * (page_num - 1) + PAGE_HEADER_SIZE +record_size;
		memcpy(pagebuf + 8, &new_offset, 2);
	} else {
		offset_location = 8 + 2 * (record_num - 2);

		uint16_t last_offset;
		memcpy(&last_offset, pagebuf + offset_location, 2);

		new_offset = last_offset + record_size;
		memcpy(pagebuf + offset_location + 2, &new_offset, 2);
	}

	printf("[*] RECORD_ALL_NUM=%d, RECORD_SIZE=%d, OFFSET=%d\n", record_num, record_size, new_offset);


	// page's header : freespace
	uint16_t new_freespace = freespace_size - record_size;
	memcpy(pagebuf + 2, &new_freespace, 2);

	// write page buffer to record file
	writePage(fp, pagebuf, page_num - 1);
}


// allocate new page
void allocateNewPage(FILE *fp, char *fileheaderbuf, char *pagebuf, uint16_t last_page_num){
	// init page
	memset(pagebuf, 0, PAGE_SIZE);

	// init new page's header
	uint16_t num_records = 0;
	uint16_t free_space = PAGE_SIZE - PAGE_HEADER_SIZE;
	uint32_t reserved_space = 0;
	uint16_t first_offset = FILE_HEADER_SIZE + PAGE_SIZE * (last_page_num) + PAGE_HEADER_SIZE;
    		
	memcpy(pagebuf, &num_records, 2);
    memcpy(pagebuf + 2, &free_space, 2);
    memcpy(pagebuf + 4, &reserved_space, 4);
	memcpy(pagebuf + 8, &first_offset, 2);
		
	// record file's header : 전체 페이지 개수 수정		
	last_page_num = last_page_num + 1;
	memcpy(fileheaderbuf, &last_page_num, 2);	
		
	writeFileHeader(fp, fileheaderbuf);
}


// find right size in linked list
int findRightSize(FILE *fp, int16_t pagenum, int16_t recordnum, int strlen){
	if (pagenum == -1 || recordnum == -1) return 0;

	char pagebuf[PAGE_SIZE] = {0};
	char recordbuf[MAX_RECORD_SIZE] = {0};
	int16_t previous_page_num = -1, previous_record_num = -1;
	int16_t next_page_num, next_record_num;
	
	while (1){
		readPage(fp, pagebuf, pagenum);

		// get record data
		getRecFromPagebuf(pagebuf, recordbuf, recordnum);
		memcpy(&next_page_num, recordbuf + 1, 2);
		memcpy(&next_record_num, recordbuf + 3, 2);

		printf("[*] ##RECORDBUF## %c %c %c\n", recordbuf[0], recordbuf[1], recordbuf[2]);

		printf("[*] NEXT_PAGE_NUM=%d, NEXT_RECORD_NUM=%d\n", next_page_num, next_record_num);

		// offset 찾기
		int offset_location = PAGE_HEADER_SIZE + 2 * (recordnum - 1); 
		int next_offset_location = offset_location + 2;

		int16_t offset;
		int16_t next_offset;

		memcpy(&offset, pagebuf + offset_location, 2);
		memcpy(&next_offset, pagebuf + next_offset_location, 2);

		// set record len
		int recordlen;
		if (recordnum == 0)
			recordlen = next_offset - pagenum * PAGE_SIZE - PAGE_HEADER_SIZE;
		else 
			recordlen = next_offset - offset;

		printf("[*] OFFSET_LOCATION=%d, NEXT_OFFSET_LOCATION=%d\n", offset_location, next_offset_location);		
		printf("[*] OFFSET=%d, NEXT_OFFSET=%d RECORD_LEN=%d RECORD_NUM=%d\n", offset, next_offset, recordlen, recordnum);

		
		if (recordlen <= strlen){
			// linked list 수정

			if (previous_page_num == -1 || previous_record_num == -1){
				// chore header
				char fileheaderbuf[FILE_HEADER_SIZE] = {0};
				readFileHeader(fp, fileheaderbuf);
				memcpy(fileheaderbuf + 2, &next_page_num, 2);
				memcpy(fileheaderbuf + 4, &next_record_num, 2);
				writeFileHeader(fp, fileheaderbuf);
			} else {
				// chore previous removed 
				readPage(fp, pagebuf, previous_page_num);
				getRecFromPagebuf(pagebuf, recordbuf, previous_record_num);
				memcpy(recordbuf + 1, &next_page_num, 2);
				memcpy(recordbuf + 3, &next_record_num, 2);
				writeRecToPagebufByRecordNum(pagebuf, recordbuf, previous_record_num);
				writePage(fp, pagebuf, previous_page_num);
			}
			return offset;
		}

		// not right size
		previous_page_num = pagenum;
		previous_record_num = recordnum;

		pagenum = next_page_num;
		recordnum = next_record_num;

		if (pagenum == -1 || recordnum == -1) break;
	}
	return 0;
}


// delete record
void delete(FILE *fp, char *keyval){
	char recordbuf[MAX_RECORD_SIZE] = {0};
	char fileheaderbuf[FILE_HEADER_SIZE] = {0};
	char pagebuf[PAGE_SIZE] = {0};

	int pagenum; 
	int recordnum;

	// id로 레코드 파일 찾아서 버퍼에 저장
	int ret = searchByID(fp, keyval, recordbuf, &pagenum, &recordnum);
	if (ret == 0){
		STUDENT s;
		printSearchResult(&s, 0);
		return;
	}

	printf("[*] PAGENUM=%d, RECORDNUM=%d\n", pagenum, recordnum);

	// last removed record data
	int16_t last_page_num ;
	int16_t last_record_num ;

	readFileHeader(fp, fileheaderbuf);
	memcpy(&last_page_num, fileheaderbuf + 2, 2);
	memcpy(&last_record_num, fileheaderbuf + 4, 2);

	printf("[*] LASTPAGENUM=%d, LASTRECORDNUM=%d\n", last_page_num, last_record_num);


	// fix file header data
	memcpy(fileheaderbuf + 2, &pagenum, 2 ); // page num
	memcpy(fileheaderbuf + 4, &recordnum, 2 ); // record num
	writeFileHeader(fp, fileheaderbuf);
				
	// fix record data
	char removedrecordbuf[MAX_RECORD_SIZE] = {0};
	memcpy(removedrecordbuf, recordbuf, strlen(recordbuf));
	memcpy(removedrecordbuf, "*", 1);
	memcpy(removedrecordbuf + 1, &last_page_num, 2); // page_num
	memcpy(removedrecordbuf + 3, &last_record_num, 2); // record_num



	int16_t a, b, c;
	memcpy(&a, removedrecordbuf + 1, 2);
	memcpy(&b, removedrecordbuf + 3, 2);
	memcpy(&c, removedrecordbuf + 5, 2);



	printf("[*] REMOVEDRECORDBUF : %c %d %d %d\n", removedrecordbuf[0], a, b, c);

	// write record buf to page buf by record number
	readPage(fp, pagebuf, pagenum);
	writeRecToPagebufByRecordNum(pagebuf, removedrecordbuf, recordnum);
	writePage(fp, pagebuf, pagenum);

	// print result
	STUDENT s;
	unpack(recordbuf, &s);
	printSearchResult(&s, 1);
}


/**file header**/
int readFileHeader(FILE *fp, char *headerbuf){
	// fseek
	fseek(fp, 0, SEEK_SET);

	int ret;
	ret = fread(headerbuf, FILE_HEADER_SIZE, 1, fp);
	
	if (ret == 1) return 1;
	else return 0;
}

int writeFileHeader(FILE *fp, const char *headerbuf){
	// fseek
	fseek(fp, 0, SEEK_SET);

	int ret;
	ret = fwrite(headerbuf, FILE_HEADER_SIZE, 1, fp);
	if (ret == 1) return 1;
	else return 0;
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

		// 파일의 맨 앞 2B를 01으로 초기화
		char fileheaderbuf[FILE_HEADER_SIZE] = {0};
		uint16_t num_pages = 1;
    	uint32_t reserved_space = 0;
		int16_t init_num = -1;

		memcpy(fileheaderbuf, &num_pages, 2);
		// linked_list : header's page_num, record_num init
		memcpy(fileheaderbuf + 2, &init_num, 2);
		memcpy(fileheaderbuf + 4, &init_num, 2);
		writeFileHeader(fp, fileheaderbuf);

		// init 0 page
		char pagebuf[PAGE_SIZE] = {0};

		uint16_t num_records = 0;
    	uint16_t free_space = PAGE_SIZE - PAGE_HEADER_SIZE;
		uint16_t first_offset = PAGE_HEADER_SIZE + FILE_HEADER_SIZE;

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
		char* keyval = strtok(NULL, "=");
		FIELD f = getFieldID(fieldname);

		// search record
		search(fp, f, keyval);

	} else if (argv[1][1] == 'd'){
		if (argc != 4){
			printf("[*] Error : Invalid argc\n");
			exit(1);
		}

		// get field name
		char* fieldname = strtok(argv[3], "=");
		char* keyval = strtok(NULL, "=");

		if (strcmp(fieldname, "ID") != 0){
			printf("[*] Error : Invalid argc. Field Name is not 'ID'\n");
			exit(1);
		}

		// delete record
		delete(fp, keyval);
	
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

