#include <stdio.h>
#include<time.h>
#include<stdlib.h>
#include<sys/time.h>
#define RECORD_LEN 250
//필요하면 header file 추가 가능

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
	// 레코드 파일로부터 전체 레코들을 순차적으로 read할 때
	// 걸리는 전체 시간을 측정하는 코드 구현
	//

	return 0;
}
