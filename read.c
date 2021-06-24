#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc,char* argv[]){
	if(argc == 4){
		int offset,readbyte,fd;
		char *buffer;

		offset = atoi(argv[2]);
		readbyte = atoi(argv[3]);
		buffer = (char*)malloc(readbyte*sizeof(char));

		fd = open(argv[1],O_RDONLY);
		if(fd == -1){
			puts("복사할 원본 파일이 없습니다");
			exit(1);
		}
		lseek(fd,offset,SEEK_SET);
		read(fd,buffer,readbyte);
		close(fd);
		for(int i=0;i<readbyte;i++)putchar(buffer[i]);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
}
						
