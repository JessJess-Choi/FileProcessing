#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include "person.h"

#define HEADER_RECORD_SIZE 16

// 페이지 단위로 읽고 쓴다.
// 1. readPage(): 주어진 페이지 번호의 페이지 데이터를 프로그램 상으로 읽어와서 pagebuf에 저장
// 2. writePage(): 프로그램 상의 pagebuf의 데이터를 주어진 페이지 번호에 저장
// 레코드 파일에서 기존의 레코드를 읽거나 새로운 레코드를 쓰거나 삭제 레코드를 수정할 때나
// 위의 readPage() 함수를 호출하여 pagebuf에 저장한 후, 여기에 필요에 따라서 새로운 레코드를 저장하거나
// 삭제 레코드 관리를 위한 메타데이터 저장. 이후 writePage() 함수를 호출하여 수정된 pagebuf를 레코드파일에
// 저장한다. 페이지단위로 읽고 쓸 것.
//
// 주의 : 데이터 페이지로부터 레코드(삭제 레코드 포함)를 읽거나 쓸 때 페이지 단위로 I/O를 처리해야 하지만,
// 헤더 레코드의 메타데이터를 저장하거나 수정하는 경우 페이지 단위로 처리하지 않고 직접 레코드 파일을 접근
// 해서 처리한다.
// 

// 페이지 번호에 해당하는 페이지를 주어진 페이지 버퍼에 읽어서 저장한다. 페이지 버퍼는 반드시 페이지 크기와
// 일치해야 한다.
//

typedef struct _index_record{
	char id[13];
	char pagenumber[4];
	char recordnumber[4];
}index_record;

index_record* index_buffer;
int readnumber = 0;
Person print_for_binary;

void unpack(const char *recordbuf, Person *p)
{
	Person tmp;
	int tmp_index=0,buf_index=0;
	while(recordbuf[buf_index] != '#')tmp.id[tmp_index++] = recordbuf[buf_index++];
	tmp.id[tmp_index] = '\0';
	strcpy(p->id,tmp.id);
	tmp_index = 0,buf_index++;

	while(recordbuf[buf_index] != '#')tmp.name[tmp_index++] = recordbuf[buf_index++];
	tmp.name[tmp_index] = '\0';
	strcpy(p->name,tmp.name);
	tmp_index = 0,buf_index++;

	while(recordbuf[buf_index] != '#')tmp.age[tmp_index++] = recordbuf[buf_index++];
	tmp.age[tmp_index] = '\0';
	strcpy(p->age,tmp.age);
	tmp_index = 0,buf_index++;

	while(recordbuf[buf_index] != '#')tmp.addr[tmp_index++] = recordbuf[buf_index++];
	tmp.addr[tmp_index] = '\0';
	strcpy(p->addr,tmp.addr);
	tmp_index = 0,buf_index++;

	while(recordbuf[buf_index] != '#')tmp.phone[tmp_index++] = recordbuf[buf_index++];
	tmp.phone[tmp_index] = '\0';
	strcpy(p->phone,tmp.phone);
	tmp_index = 0,buf_index++;

	while(recordbuf[buf_index] != '#')tmp.email[tmp_index++] = recordbuf[buf_index++];
	tmp.email[tmp_index] = '\0';
	strcpy(p->email,tmp.email);
}

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp,HEADER_RECORD_SIZE + pagenum*PAGE_SIZE,SEEK_SET);
	fread(pagebuf,PAGE_SIZE,1,fp);
}

//
// 페이지 버퍼의 데이터를 주어진 페이지 번호에 해당하는 레코드 파일의 위치에 저장한다.
// 페이지 버퍼는 반드시 페이지 크기와 일치해야 한다.
//


//
// 주어진 레코드 파일(recordfp)을 이용하여 심플 인덱스 파일(idxfp)을 생성한다.
void createIndex(FILE *idxfp, FILE *recordfp)
{
	char pagebuf[PAGE_SIZE];
	int all_page_number,all_record_number,page_number,record_number;
	char recordbuf[MAX_RECORD_SIZE];
	int page_record_number=1,data_header = 0;
	int index = 0;
	fseek(recordfp,0,SEEK_SET);
	memset(pagebuf,0,sizeof(pagebuf));
	memset(recordbuf,0,sizeof(recordbuf));

	fread(&all_page_number,sizeof(int),1,recordfp);
	fread(&all_record_number,sizeof(int),1,recordfp);
	index_buffer = (index_record*)malloc(sizeof(index_record)*all_record_number);
	Person p;

	for(int i=0;i<all_page_number;i++){
		readPage(recordfp,pagebuf,i);
		memcpy(&record_number,pagebuf,sizeof(int));
		for(int j=0;j<record_number;j++){
			int offset,length;
			memset(recordbuf,0,sizeof(recordbuf));
			memcpy(&offset,pagebuf+sizeof(int)*(2*j+1),sizeof(int));
			memcpy(&length,pagebuf+sizeof(int)*(2*j+2),sizeof(int));
			memcpy(recordbuf,pagebuf+HEADER_AREA_SIZE+offset,length);
			if(strlen(recordbuf)){
				unpack(recordbuf,&p);
				memset(index_buffer[index].id,0,sizeof(index_buffer[index].id));
				memset(index_buffer[index].pagenumber,0,sizeof(index_buffer[index].pagenumber));
				memset(index_buffer[index].recordnumber,0,sizeof(index_buffer[index].recordnumber));

				memcpy(index_buffer[index].id,p.id,strlen(p.id));
				memcpy(&index_buffer[index].pagenumber,&i,sizeof(int));
				memcpy(&index_buffer[index].recordnumber,&j,sizeof(int));
				index++;
			}

		}
	}
	int count = 0,tmp_index=0;
	index_record tmp[all_record_number];

	for(int i=0;i<index;i++)
		if(strstr(index_buffer[i].id,"*"))
			count++;
		else
			tmp[tmp_index++] = index_buffer[i];

	index -= count;
	for(int i=0;i<index;i++)
		for(int j=0;j<i;j++)
			if(atoll(tmp[i].id) < atoll(tmp[j].id)){
				index_record buf = tmp[i];
				tmp[i] = tmp[j];
				tmp[j] = buf;
			}
	fwrite(&index,sizeof(int),1,idxfp);
	for(int i=0;i<index;i++)
		fwrite(&tmp[i],sizeof(tmp[i]),1,idxfp);
}

//
// 주어진 심플 인덱스 파일(idxfp)을 이용하여 주민번호 키값과 일치하는 레코드의 주소, 즉 페이지번호와
// 레코드 번호를 찾는다. 이때, 반드시 이진 검색 알고리즘을 사용해야 한다.
//
void binarysearch(FILE *idxfp, const char *id, int *pageNum, int *recordNum)
{
	int start = 0,end;
	fseek(idxfp,0,SEEK_SET);
	fread(&end,sizeof(int),1,idxfp);
	end--;
	while(1){
		readnumber++;
		char now[13];
		int mid = (start + end) / 2;
		if(start > end)break;
		fseek(idxfp,sizeof(int) + 21 * mid,SEEK_SET);
		fread(now,sizeof(now),1,idxfp);
		if(atoll(now) == atoll(id)){
			fread(pageNum,sizeof(int),1,idxfp);
			fread(recordNum,sizeof(int),1,idxfp);
			break;
		}
		else if(atoll(now) > atoll(id))end = mid - 1;
		else
			start = mid + 1;
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;  // 레코드 파일의 파일 포인터
	fp = fopen(argv[2],"rb");
	if(!strcmp(argv[1],"i")){
		FILE *index = fopen(argv[3],"wb+");
		createIndex(index,fp);
	}
	else if(!strcmp(argv[1],"b")){
		FILE *index = fopen(argv[3],"rb");
		int pagenumber = -1,recordnumber = -1;
		binarysearch(index,argv[4],&pagenumber,&recordnumber);
		printf("#reads:%d\n",readnumber);
		if(pagenumber == -1 && recordnumber == -1)
			printf("no persons\n");
		else{
			char pagebuf[PAGE_SIZE];
			char recordbuf[MAX_RECORD_SIZE];
			readPage(fp,pagebuf,pagenumber);
			int offset,length;
			memset(recordbuf,0,sizeof(recordbuf));
			memcpy(&offset,pagebuf+sizeof(int)*(2*recordnumber+1),sizeof(int));
			memcpy(&length,pagebuf+sizeof(int)*(2*recordnumber+2),sizeof(int));
			memcpy(recordbuf,pagebuf+HEADER_AREA_SIZE+offset,length);
			unpack(recordbuf,&print_for_binary);

			printf("id=%s\n",print_for_binary.id);
			printf("name=%s\n",print_for_binary.name);
			printf("age=%s\n",print_for_binary.age);
			printf("addr=%s\n",print_for_binary.addr);
			printf("phone=%s\n",print_for_binary.phone);
			printf("email=%s\n",print_for_binary.email);
		}
	}
	return 0;
}