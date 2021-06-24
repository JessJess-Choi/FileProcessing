#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#define RECORD_LEN 250

int main(int argc, char **argv)
{
	if(argc==3){
		char arr[RECORD_LEN];
		for(int i=0;i<RECORD_LEN;i++)arr[i]='a';
		int header_record = atoi(argv[1]);
		FILE *fp = fopen(argv[2],"wb");
		fwrite(&header_record,sizeof(header_record),1,fp);
		for(int i=0;i<header_record;i++)
			fwrite(arr,sizeof(char),sizeof(arr),fp);
		fclose(fp);
		exit(0);
	}
	else{
		puts("error");
		exit(1);
	}
	//
	// 사용자로부터 입력 받은 레코드 수만큼의 레코드를 레코드 파일에 저장하는 코드를 구현
	//
}
