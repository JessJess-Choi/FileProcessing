#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#define bufferSize 10

int main(int argc,char* argv[]){
	if(argc == 3){
		int fd_original,fd_copy;
		char buffer[bufferSize];

		fd_original = open(argv[1],O_RDONLY);
		fd_copy = open(argv[2],O_WRONLY|O_CREAT,S_IRWXU);
		if(fd_original == -1){
			puts("복사할 원본 파일이 없습니다");
			exit(1);
		}
		while(1){
			memset(buffer,0,sizeof(buffer));
			int tmp = read(fd_original,buffer,bufferSize);
			if(tmp < 10){
				write(fd_copy,buffer,tmp);
				break;
			}
			write(fd_copy,buffer,bufferSize);
		}
		close(fd_original);
		close(fd_copy);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
}
