#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char* argv[]){
	if(argc == 4){
		char* buffer;
		int fd,offset,bufferSize;
		offset = atoi(argv[2]);
		fd = open(argv[1],O_RDWR,S_IRWXU);
		if(fd == -1){
			puts("데이터를 끼워 넣을 파일이 없습니다");
			exit(1);
		}

		lseek(fd,offset,SEEK_SET);
		bufferSize = 0;
		while(1){
			char tmpBuffer[10];
			int tmpSize;
			tmpSize = read(fd,tmpBuffer,10);
			if(tmpSize < 10){
				bufferSize += tmpSize;
				break;
			}
			bufferSize += tmpSize;
		}

		lseek(fd,offset,SEEK_SET);
		buffer = (char*)malloc(bufferSize*sizeof(char));
		read(fd,buffer,bufferSize);
		
		lseek(fd,offset,SEEK_SET);
		write(fd,argv[3],strlen(argv[3]));
		write(fd,buffer,bufferSize);
		
		close(fd);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
}
