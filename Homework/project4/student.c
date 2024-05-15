#include <stdio.h>	
#include <student.h>
#include <string.h>

const char* fields[] = {"id", "name", "dept", "year", "addr", "phone", "email"};


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

		for (int j = 0; j < sizeof(fields) / sizeof(fields[0]); j++) {

			if (strcmp(field, fields[j]) == 0) {
				switch (j) {
					case 0: strcpy(s->id, argv[i]); break;
					case 1: strcpy(s->name, argv[i]); break;
					case 2: strcpy(s->dept, argv[i]); break;
					case 3: strcpy(s->year, argv[i]); break;
					case 4: strcpy(s->addr, argv[i]); break;
					case 5: strcpy(s->phone, argv[i]); break;
					case 6: strcpy(s->email, argv[i]); break;
				}
				break;
			}
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


int getRecFromPagebuf(const char *pagebuf, char *recordbuf, int recordnum){
	// read offset
	int offset = 8 + 2 * recordnum;
	int record_end = atoi(pagebuf[offset] + pagebuf[offset + 1]);
	int record_start = atoi(pagebuf[offset - 2] + pagebuf[offset - 1]);
	int record_len = record_end - record_start;

	if (record_len > 100){
		printf("[*] Error : Invalid Record Length\n");
    	exit(1);
	}

	// read record data
	strncpy(recordbuf, pagebuf + record_start + 1, record_len);
}


void unpack(const char *recordbuf, STUDENT *s){
	int offset = 0;
	while (offset < 7){
		char *field = strtok(recordbuf, "#");
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
	int record_num = atoi(pagebuf[0] + pagebuf[1]);

	// last record's offset locatioin in header
	int offset_location = 8 + 2 * record_num;
	
	// record start index
	int record_start = atoi(pagebuf[offset_location] + pagebuf[offset_location + 1]) + 1;

	// write record data in pagebuf
	strncpy(pagebuf + record_start, recordbuf, strlen(recordbuf));
}

int pack_filled(char *recordbuf, char *field, int offset){
	size_t len = strlen(field);
	memcpy(recordbuf + offset, field, len);
	offset += len;
	recordbuf[offset++] = '#';
	return offset;
}

void pack(char *recordbuf, const STUDENT *s){
	int offset = 0;
	offset = (recordbuf, s->id, offset);
	offset = (recordbuf, s->name, offset);
	offset = (recordbuf, s->dept, offset);
	offset = (recordbuf, s->year, offset);
	offset = (recordbuf, s->addr, offset);
	offset = (recordbuf, s->phone, offset);
	offset = (recordbuf, s->email, offset);
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



/**find record**/
void search(FILE *fp, FIELD f, char *keyval){
	char recordbuf[MAX_RECORD_SIZE];
	char fileheaderbuf[FILE_HEADER_SIZE];
	char pagebuf[PAGE_SIZE];
	STUDENT *s_list;

		// file header
		readFileHeader(fp, fileheaderbuf);

		// 전체 페이지 개수
		int page_num = atoi(fileheaderbuf[0] + fileheaderbuf[1]);
		
		for (int i = 0 ; i < page_num; i++){
			// 순차적으로 페이지 read
			readPage(fp, pagebuf, i);

			// read page's header
			char pageheaderbuf[PAGE_HEADER_SIZE];
			memcpy(pageheaderbuf, pagebuf, PAGE_HEADER_SIZE);

			// page의 레코드 개수
			int record_num = atoi(pageheaderbuf[0] + pageheaderbuf[1]);
			fseek(fp, 6, SEEK_CUR);
			
			for (int j = 0; j < record_num; j++){
				// j번째 레코드 읽기
				getRecFromPagebuf(pagebuf, recordbuf, j);

				// record 객체를 student 객체로 변환
				STUDENT s;
				unpack(recordbuf, &s);

				// 각 필드 값 비교


				// 일치하면 s_list에 추가
				

			}
		}

		// 결과 출력
		printSearchResult(s_list, sizeof(s_list));
}


// insert new record data
void insert(FILE *fp, const STUDENT *s){
	char recordbuf[MAX_RECORD_SIZE];
	char fileheaderbuf[FILE_HEADER_SIZE];
	char pagebuf[PAGE_SIZE];
		// pack student struct to record
		pack(recordbuf, &s);

		// find the last page or make new page
		// read record file's header 
		readFileHeader(fp, fileheaderbuf);

		// find last page's number
		int last_page_num = atoi(fileheaderbuf[0] + fileheaderbuf[1]);

		// read last page
		readPage(fp, pagebuf, last_page_num);

		// read header's free space byte
		fseek(fp, 2, SEEK_CUR);

		// compare freespace < recordbuf size	
		if (memcmp(pagebuf, sizeof(recordbuf), 2) < 0){
			// allocate new page
			memset(pagebuf, 0, PAGE_SIZE);

			// write page's header

			// record file's header : 전체 페이지 개수 수정
		}

		// write record to page buffer
		writeRecToPagebuf(pagebuf, recordbuf);

		// page's header : chore #records
		// page's header : records' offset
		
		
		// write page buffer to record file
		writePage(fp, pagebuf, last_page_num);

}


/**file header**/
int readFileHeader(FILE *fp, char *headerbuf){
	int ret;

	ret = fread(headerbuf, FILE_HEADER_SIZE, 1, fp);
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
	fp = fopen(argv[2], 'rw+');
	if (fp == NULL){
        printf("[*] Error : File Open Error\n");
        exit(1);
    }
	
	if (argv[1][0] == 'i'){
		if (argc != 10){
			printf("[*] Error : Invalid argc\n");
			exit(1);
		}

		// make student struct
		STUDENT s;
		parse_input_to_student(argv, &s);

		// insert record data to record file
		insert(fp, &s);

	} else if (argv[1][0] == 's'){
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
