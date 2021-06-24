#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#define bufferSize 20

int main(int argc,char* argv[]){
	if(argc == 4){
		char buffer[bufferSize];
		int fd_file1,fd_file2,fd_file3;
		fd_file1 = open(argv[1],O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
		fd_file2 = open(argv[2],O_RDONLY);

		if(fd_file2 == -1){
			puts("복사할 원본 파일이 없습니다");
			exit(1);
		}

		while(1){
			memset(buffer,0,sizeof(buffer));
			int check = read(fd_file2,buffer,bufferSize);
			if(check <= 0){
				close(fd_file2);
				break;
			}
			write(fd_file1,buffer,check);
		}
		
		fd_file3 = open(argv[3],O_RDONLY);
		if(fd_file3 == -1){
			puts("복사할 원본 파일이 없습니다");
			exit(1);
		}
		while(1){
			memset(buffer,0,sizeof(buffer));
			int check = read(fd_file3,buffer,bufferSize);
			if(check <= 0){
				close(fd_file3);
				break;
			}
			write(fd_file1,buffer,check);
		}

		close(fd_file1);
		exit(0);
	}
	else{
		puts("입력된 매개변수의 수량이 다릅니다");
		exit(1);
	}
	return 0;
}
