#include <stdio.h>
#include "person.h"
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#define HEADER_RECORD_SIZE 16

void readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp,HEADER_RECORD_SIZE + pagenum*PAGE_SIZE,SEEK_SET);
	fread(pagebuf,PAGE_SIZE,1,fp);
}


void writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp,HEADER_RECORD_SIZE + pagenum*PAGE_SIZE,SEEK_SET);
	fwrite(pagebuf,PAGE_SIZE, 1,fp);
}

void pack(char *recordbuf, const Person *p)
{
	sprintf(recordbuf,"%s#%s#%s#%s#%s#%s#",p->id,p->name,p->age,p->addr,p->phone,p->email);
}


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

void add(FILE *fp, const Person *p)
{
	char pagebuf[PAGE_SIZE];
	fseek(fp,0,SEEK_SET);
	int all_page_number,all_record_number,page_number,record_number;
	char recordbuf[MAX_RECORD_SIZE];
	int page_record_number=1,data_header = 0;
	memset(pagebuf,0,sizeof(pagebuf));
	memset(recordbuf,0,sizeof(recordbuf));
	fread(&all_page_number,sizeof(int),1,fp);
	fread(&all_record_number,sizeof(int),1,fp);
	fread(&page_number,sizeof(int),1,fp);
	fread(&record_number,sizeof(int),1,fp);

	if(all_page_number == 0 && all_record_number == 0){		//맨 처음
		fseek(fp,0,SEEK_SET);
		all_page_number = all_record_number = 1;
		fwrite(&all_page_number,sizeof(int),1,fp);
		fwrite(&all_record_number,sizeof(int),1,fp);
		pack(recordbuf,p);
		int data_header_len = strlen(recordbuf);
		memcpy(pagebuf,&page_record_number,sizeof(int));
		memcpy(pagebuf+sizeof(int),&data_header,sizeof(int));
		memcpy(pagebuf+sizeof(int)*2,&data_header_len,sizeof(int));
		memcpy(pagebuf+HEADER_AREA_SIZE,recordbuf,data_header_len);
		writePage(fp,pagebuf,0);
		return;
	}
	else{
		fseek(fp,0,SEEK_SET);
		fread(&all_page_number,sizeof(int),1,fp);
		fread(&all_record_number,sizeof(int),1,fp);
		fread(&page_number,sizeof(int),1,fp);
		fread(&record_number,sizeof(int),1,fp);
		if(page_number == -1 && record_number == -1){	//삭제 레코드가 없는 경우
				memset(pagebuf,0,sizeof(pagebuf));
				memset(recordbuf,0,sizeof(recordbuf));
				readPage(fp,pagebuf,all_page_number-1);
				int record;
				memcpy(&record,pagebuf,sizeof(int));
				if(record < DATA_AREA_SIZE / MAX_RECORD_SIZE){		//페이지에 레코드 쓰기 가능한 경우
					pack(recordbuf,p);
					int offset,record_len, len;
					memcpy(&offset,pagebuf+sizeof(int)+((record-1)*2*sizeof(int)),sizeof(int));
					memcpy(&len,pagebuf+2*sizeof(int)+((record-1)*2*sizeof(int)),sizeof(int));
					record_len = strlen(recordbuf);
					record++;
					len += offset;

					memcpy(pagebuf+sizeof(int)+((record-1)*2*sizeof(int)),&len,sizeof(int));
					memcpy(pagebuf+sizeof(int)+((record-1)*2*sizeof(int)) + sizeof(int),&record_len,sizeof(int));
					memcpy(pagebuf+HEADER_AREA_SIZE+len,recordbuf,record_len);
					memcpy(pagebuf,&record,sizeof(int));

					all_record_number++;
					fseek(fp,sizeof(int),SEEK_SET);
					fwrite(&all_record_number,sizeof(int),1,fp);
					writePage(fp,pagebuf,all_page_number-1);
					return;
				}
				else{									//페이지가 꽉 찬 경우
					fseek(fp,0,SEEK_SET);
					fread(&all_page_number,sizeof(int),1,fp);
					fread(&all_record_number,sizeof(int),1,fp);
					all_page_number++;
					all_record_number++;
					fseek(fp,0,SEEK_SET);
					fwrite(&all_page_number,sizeof(int),1,fp);
					fwrite(&all_record_number,sizeof(int),1,fp);
					memset(pagebuf,0,sizeof(pagebuf));
					memset(recordbuf,0,sizeof(recordbuf));
					int record = 1,len,start=0;
					pack(recordbuf,p);
					len = strlen(recordbuf);
					memcpy(pagebuf,&record,sizeof(int));
					memcpy(pagebuf+sizeof(int),&start,sizeof(int));
					memcpy(pagebuf+sizeof(int)*2,&len,sizeof(int));
					memcpy(pagebuf+HEADER_AREA_SIZE,recordbuf,sizeof(recordbuf));
					writePage(fp,pagebuf,all_page_number-1);
					return;
				}
		}
		else{												//삭제 레코드가 있는 경우
			int before_page=page_number,before_record=record_number;
			while(page_number != -1 && record_number != -1){
				memset(pagebuf,0,sizeof(pagebuf));
				readPage(fp,pagebuf,page_number);
				int offset,len;
				memcpy(&offset,pagebuf+(2*record_number+1)*sizeof(int),sizeof(int));
				memcpy(&len,pagebuf+(2*record_number+2)*sizeof(int),sizeof(int));
				memset(recordbuf,0,sizeof(recordbuf));
				pack(recordbuf,p);			
				if(strlen(recordbuf) <= len){					//쓸수 있는 경우
					int next_page_number,next_record_number;
					memcpy(&next_page_number,pagebuf+HEADER_AREA_SIZE+offset+sizeof(char),sizeof(int));
					memcpy(&next_record_number,pagebuf+HEADER_AREA_SIZE+offset+sizeof(char)+sizeof(int),sizeof(int));
					memcpy(pagebuf+HEADER_AREA_SIZE+offset,recordbuf,strlen(recordbuf));
					writePage(fp,pagebuf,page_number);

					if(before_page == page_number && before_record == record_number){
						fseek(fp,sizeof(int)*2,SEEK_SET);
						fwrite(&next_page_number,sizeof(int),1,fp);
						fwrite(&next_record_number,sizeof(int),1,fp);
					}
					else{
						readPage(fp,pagebuf,before_page);
						memcpy(&offset,pagebuf+(2*before_record+1)*sizeof(int),sizeof(int));
						memcpy(pagebuf+HEADER_AREA_SIZE+offset+sizeof(char),&next_page_number,sizeof(int));
						memcpy(pagebuf+HEADER_AREA_SIZE+offset+sizeof(char)+sizeof(int),&next_record_number,sizeof(int));
						writePage(fp,pagebuf,before_page);
					}
					return;
				}
				else{										//못쓰는 경우
					before_page = page_number;
					before_record = record_number;
					memcpy(&page_number,pagebuf+HEADER_AREA_SIZE+offset+sizeof(char),sizeof(int));
					memcpy(&record_number,pagebuf+HEADER_AREA_SIZE+offset+sizeof(char)+sizeof(int),sizeof(int));
				}
			}

			//레코드를 새로운 장소에 기록 -> 페이지를 새로 만드는 경우, 그냥 쓸수 있는 경우
			fseek(fp,0,SEEK_SET);
			fread(&all_page_number,sizeof(int),1,fp);
			fread(&all_record_number,sizeof(int),1,fp);
			all_record_number++;
			fseek(fp,sizeof(int),SEEK_SET);
			fwrite(&all_record_number,sizeof(int),1,fp);

			int tmp_record,offset,len;
			readPage(fp,pagebuf,all_page_number-1);
			memcpy(&tmp_record,pagebuf,sizeof(int));

//			printf("%d %d\n",all_page_number,tmp_record);
			if(tmp_record < DATA_AREA_SIZE / MAX_RECORD_SIZE){	//페이지에 기록 가능
				memcpy(&offset,pagebuf+(2*(tmp_record-1)+1)*sizeof(int),sizeof(int));
				memcpy(&len,pagebuf+(2*(tmp_record-1)+2)*sizeof(int),sizeof(int));
				memset(recordbuf,0,sizeof(recordbuf));

				int tmp_offset = offset + len,tmp_len;
				pack(recordbuf,p);
				memcpy(pagebuf+(2*(tmp_record-1)+3)*sizeof(int),&tmp_offset,sizeof(int));
				tmp_len = strlen(recordbuf);
				memcpy(pagebuf+(2*(tmp_record-1)+4),&tmp_len,sizeof(int));
				memcpy(pagebuf+HEADER_AREA_SIZE+tmp_offset,recordbuf,tmp_len);

				writePage(fp,pagebuf,all_page_number-1);
//				printf("%d %d %d %d\n",offset,len,tmp_offset,tmp_len);
			}
			else{					//페이지를 새로 만드는 경우
				all_page_number++;
				fseek(fp,0,SEEK_SET);
				fwrite(&all_page_number,sizeof(int),1,fp);

				memset(pagebuf,0,sizeof(pagebuf));
				memset(recordbuf,0,sizeof(recordbuf));
				int record = 1,len,start=0;
				pack(recordbuf,p);
				len = strlen(recordbuf);
				memcpy(pagebuf,&record,sizeof(int));
				memcpy(pagebuf+sizeof(int),&start,sizeof(int));
				memcpy(pagebuf+sizeof(int)*2,&len,sizeof(int));
				memcpy(pagebuf+HEADER_AREA_SIZE,recordbuf,sizeof(recordbuf));
				writePage(fp,pagebuf,all_page_number-1);
			}
			return;
		}
	}
}

void delete(FILE *fp, const char *id)
{
	int remove_page_number,remove_record_number;
	fseek(fp,sizeof(int)*2,SEEK_SET);
	fread(&remove_page_number,sizeof(int),1,fp);
	fread(&remove_record_number,sizeof(int),1,fp);
	char pagebuffer[PAGE_SIZE];
	char recordbuffer[MAX_RECORD_SIZE];
	char record_header[HEADER_AREA_SIZE];
	Person p;
	memset(pagebuffer,0,sizeof(pagebuffer));
	memset(recordbuffer,0,sizeof(recordbuffer));

	if(remove_page_number == -1 && remove_record_number == -1){		//맨 처음 지울때
		int all_pagenumber,all_recordnumber;
		bool check = false;
		fseek(fp,0,SEEK_SET);
		fread(&all_pagenumber,sizeof(int),1,fp);
		fread(&all_recordnumber,sizeof(int),1,fp);

		for(int i=0;i<all_pagenumber;i++){
			readPage(fp,pagebuffer,i);
			int record_number;
			memcpy(&record_number,pagebuffer,sizeof(int));
			for(int j=0;j<record_number;j++){
				int offset,length;
				memset(recordbuffer,0,sizeof(recordbuffer));
				memcpy(&offset,pagebuffer+sizeof(int)*(2*j+1),sizeof(int));
				memcpy(&length,pagebuffer+sizeof(int)*(2*j+2),sizeof(int));
				memcpy(recordbuffer,pagebuffer+HEADER_AREA_SIZE+offset,length);
				
				if(strlen(recordbuffer))
					unpack(recordbuffer,&p);
				if(!strcmp(p.id,id)){
					fseek(fp,sizeof(int)*2,SEEK_SET);
					fwrite(&i,sizeof(int),1,fp);
					fwrite(&j,sizeof(int),1,fp);

					pagebuffer[HEADER_AREA_SIZE+offset] = '*';			
					int tmp = -1;
					memcpy(pagebuffer+HEADER_AREA_SIZE+offset+sizeof(char),&tmp,sizeof(int));
					memcpy(pagebuffer+HEADER_AREA_SIZE+offset+sizeof(char)+sizeof(int),&tmp,sizeof(int));
					writePage(fp,pagebuffer,i);
					check = true;
					break;
				}
				if(check)break;
			}
		}
	}
	else{									//먼저 지운 내역이 있는 경우
		int all_pagenumber,all_recordnumber;
		bool check = false;
		fseek(fp,0,SEEK_SET);
		fread(&all_pagenumber,sizeof(int),1,fp);
		fread(&all_recordnumber,sizeof(int),1,fp);
		fread(&remove_page_number,sizeof(int),1,fp);
		fread(&remove_record_number,sizeof(int),1,fp);

		for(int i=0;i<all_pagenumber;i++){
			readPage(fp,pagebuffer,i);
			int record_number,offset,length;
			memcpy(&record_number,pagebuffer,sizeof(int));

			for(int j=0;j<record_number;j++){
				memset(recordbuffer,0,sizeof(recordbuffer));
				memcpy(&offset,pagebuffer+sizeof(int)*(2*j+1),sizeof(int));
				memcpy(&length,pagebuffer+sizeof(int)*(2*j+2),sizeof(int));
				memcpy(recordbuffer,pagebuffer+HEADER_AREA_SIZE+offset,length);
			
				if(strlen(recordbuffer))
					unpack(recordbuffer,&p);
				if(!strcmp(p.id,id)){
					fseek(fp,sizeof(int)*2,SEEK_SET);
					fwrite(&i,sizeof(int),1,fp);
					fwrite(&j,sizeof(int),1,fp);			//헤더에 현재 위치 기록

					pagebuffer[HEADER_AREA_SIZE+offset] = '*';
					int tmp = -1;
					memcpy(pagebuffer+HEADER_AREA_SIZE+offset+sizeof(char),&remove_page_number,sizeof(int));
					memcpy(pagebuffer+HEADER_AREA_SIZE+offset+sizeof(char)+sizeof(int),&remove_record_number,sizeof(int));
					writePage(fp,pagebuffer,i);
					check = true;		
					break;
				}
				if(check)break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	FILE *fp;
	int page_number=-1,record_number=-1,all_page_number=0,all_record_number=0;
	if((fp = fopen(argv[2],"rb+")) == NULL){
		fp = fopen(argv[2],"wb+");
		int tmp1=0, tmp2= -1;
		fwrite(&tmp1,sizeof(int),1,fp);
		fwrite(&tmp1,sizeof(int),1,fp);
		fwrite(&tmp2,sizeof(int),1,fp);
		fwrite(&tmp2,sizeof(int),1,fp);
	}

	if(!strcmp(argv[1],"a")){
		Person person;
		strcpy(person.id,argv[3]);
		strcpy(person.name,argv[4]);
		strcpy(person.age,argv[5]);
		strcpy(person.addr,argv[6]);
		strcpy(person.phone,argv[7]);
		strcpy(person.email,argv[8]);
		add(fp,&person);
	}
	else if(!strcmp(argv[1],"d")){
		delete(fp,argv[3]);
	}
	fclose(fp);
	return 1;
}