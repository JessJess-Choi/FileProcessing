#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char* argv[]){
	if(argc == 4){
		int fd,offset;
		offset = atoi(argv[2]);
		fd = open(argv[1],O_RDWR,S_IRWXU);
		if(fd == -1){
			puts("복사할 파일이 없습니다");
			exit(1);
		}

		lseek(fd,offset,SEEK_SET);
		write(fd,argv[3],strlen(argv[3]));
		close(fd);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
}
