#include <stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/time.h>
#define RECORD_LEN 250
//�ʿ��ϸ� header file �߰� ����

int main(int argc, char **argv)
{
	if(argc==2){
		FILE* fp = fopen(argv[1],"rb");
		int how_many_records,time;
		struct timeval start,end;
		char buffer[RECORD_LEN];
		if(fp == NULL){
			puts("error");
			exit(1);
		}

		fread(&how_many_records,sizeof(int),1,fp);
		gettimeofday(&start,NULL);
		for(int i=0;i<how_many_records;i++)
			fread(buffer,sizeof(buffer),1,fp);
		gettimeofday(&end,NULL);
		time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
		printf("#records: %d ",how_many_records);
		printf("elapsed_time: %d us\n",time);
		fclose(fp);
		exit(0);
	}
	else{
		puts("error");
		exit(1);
	}
	//
	// ���ڵ� ���Ϸκ��� ��ü ���ڵ��� ���������� read�� ��
	// �ɸ��� ��ü �ð��� �����ϴ� �ڵ� ����
	//

	return 0;
}
