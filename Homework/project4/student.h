#ifndef _STUDENT_H_
#define _STUDENT_H_

#define MAX_RECORD_SIZE	100		//id(8) + name(13) + department(16) + year(1) + address(20) + phone(15) +  email(20) + 7 delimeters
#define PAGE_SIZE		512		// 512 Bytes
#define PAGE_HEADER_SIZE	64		// 64 Bytes
#define FILE_HEADER_SIZE	16		// 16 Bytes

typedef enum {ID=0, NAME, DEPT, YEAR,  ADDR, PHONE, EMAIL} FIELD;

typedef struct _STUDENT
{
	char id[9];			
	char name[14];		
	char dept[17];		
	char year[2];		
	char addr[21];		
	char phone[16];		
	char email[21];		
} STUDENT;

#endif
