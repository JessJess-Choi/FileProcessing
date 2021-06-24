#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include<sys/time.h>
//필요하면 header file 추가 가능

#define SUFFLE_NUM	10000	// 이 값은 마음대로 수정할 수 있음.
#define RECORD_LEN 250

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// 필요한 함수가 있으면 더 추가할 수 있음.

int main(int argc, char **argv)
{
	if(argc==2){
		int *read_order_list;
		int num_of_records; // 레코드 파일에 저장되어 있는 전체 레코드의 수

		// 이 함수를 실행하면 'read_order_list' 배열에는 읽어야 할 레코드 번호들이 나열되어 저장됨
		struct timeval start,end;
		char buffer[RECORD_LEN];
		int how_many_records,time;
		FILE* fp = fopen(argv[1],"rb");
		if(fp == NULL){
			puts("error");
			exit(1);
		}


		fread(&how_many_records,sizeof(int),1,fp);
		read_order_list = (int*)malloc(how_many_records*sizeof(int));
		num_of_records = how_many_records;
		GenRecordSequence(read_order_list, num_of_records);
		gettimeofday(&start,NULL);
		for(int i=0;i<how_many_records;i++){
			fseek(fp,read_order_list[i],SEEK_SET);
			fread(buffer,sizeof(buffer),1,fp);
		}
		gettimeofday(&end,NULL);

		time = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
		printf("#records: %d ",how_many_records);
		printf("elapsed_time: %d us\n",time);
	//
	// 'read_order_list'를 이용하여 random 하게 read 할 때 걸리는 전체 시간을 측정하는 코드 구현
	//
		fclose(fp);
		exit(0);
	}
	else{
		puts("error");
		exit(1);
	}
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}
