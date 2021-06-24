#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>

int main(int argc,char* argv[]){
	if(argc == 4){
		char *front,*end;
		int fd,offset,deleteHowMany,endSize;
		offset = atoi(argv[2]);
		deleteHowMany = atoi(argv[3]);
		fd = open(argv[1],O_RDWR,S_IRWXU);
		
		front = (char*)malloc(offset*sizeof(char));
		read(fd,front,offset);
		lseek(fd,deleteHowMany,SEEK_CUR);

		endSize = 0;
		while(1){
			char tmpBuffer[10];
			int tmpSize;
			tmpSize = read(fd,tmpBuffer,10);
			if(tmpSize < 10){
				endSize += tmpSize;
				break;
			}
			endSize += tmpSize;
		}

		lseek(fd,offset + deleteHowMany,SEEK_SET);
		end = (char*)malloc(endSize*sizeof(char));
		read(fd,end,endSize);
		close(fd);

		fd = open(argv[1],O_WRONLY|O_TRUNC);
		write(fd,front,offset);
		write(fd,end,endSize);

		close(fd);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
}
