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
	// ����ڷκ��� �Է� ���� ���ڵ� ����ŭ�� ���ڵ带 ���ڵ� ���Ͽ� �����ϴ� �ڵ带 ����
	//
}
