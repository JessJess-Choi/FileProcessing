#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include<sys/time.h>
//�ʿ��ϸ� header file �߰� ����

#define SUFFLE_NUM	10000	// �� ���� ������� ������ �� ����.
#define RECORD_LEN 250

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);
// �ʿ��� �Լ��� ������ �� �߰��� �� ����.

int main(int argc, char **argv)
{
	if(argc==2){
		int *read_order_list;
		int num_of_records; // ���ڵ� ���Ͽ� ����Ǿ� �ִ� ��ü ���ڵ��� ��

		// �� �Լ��� �����ϸ� 'read_order_list' �迭���� �о�� �� ���ڵ� ��ȣ���� �����Ǿ� �����
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
	// 'read_order_list'�� �̿��Ͽ� random �ϰ� read �� �� �ɸ��� ��ü �ð��� �����ϴ� �ڵ� ����
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
