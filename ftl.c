// 주의사항
// 1. blockmap.h에 정의되어 있는 상수 변수를 우선적으로 사용해야 함
// 2. blockmap.h에 정의되어 있지 않을 경우 본인이 이 파일에서 만들어서 사용하면 됨
// 3. 필요한 data structure가 필요하면 이 파일에서 정의해서 쓰기 바람(blockmap.h에 추가하면 안됨)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include "blockmap.h"
// 필요한 경우 헤더 파일을 추가하시오.

//
// flash memory를 처음 사용할 때 필요한 초기화 작업, 예를 들면 address mapping table에 대한
// 초기화 등의 작업을 수행한다. 따라서, 첫 번째 ftl_write() 또는 ftl_read()가 호출되기 전에
// file system에 의해 반드시 먼저 호출이 되어야 한다.
//

int address_mapping_table[BLOCKS_PER_DEVICE];

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);
bool checking[BLOCKS_PER_DEVICE];

void ftl_open()
{
	int ppn,lbn;
	char pagebuf[PAGE_SIZE];
	for(int i=0;i<DATABLKS_PER_DEVICE;i++)
		address_mapping_table[i] = -1;
	for(int i=0;i<BLOCKS_PER_DEVICE;i++){
		memset(pagebuf,(char)0xFF,sizeof(pagebuf));
		dd_read(i * PAGES_PER_BLOCK,pagebuf);
		memcpy(&lbn,pagebuf+SECTOR_SIZE,4);
		if(lbn != -1){
			address_mapping_table[lbn] = i;
			checking[i] = true;
		}
		else{
			address_mapping_table[DATABLKS_PER_DEVICE] = i;
		}
		
	}
	//
	// address mapping table 초기화 또는 복구
	// free block's pbn 초기화
    	// address mapping table에서 lbn 수는 DATABLKS_PER_DEVICE 동일
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_read(int lsn, char *sectorbuf)
{
	int offset = lsn % PAGES_PER_BLOCK;
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = address_mapping_table[lbn];
	int ppn = pbn * PAGES_PER_BLOCK + offset;
	char tmp[PAGE_SIZE];
	dd_read(ppn,tmp);
	memcpy(sectorbuf,tmp,SECTOR_SIZE);
//	for(int i=0;i<PAGE_SIZE;i++)
//		printf("%c",tmp[i]);
//	puts("tmp");
	return;
}

//
// 이 함수를 호출하는 쪽(file system)에서 이미 sectorbuf가 가리키는 곳에 512B의 메모리가 할당되어 있어야 함
// (즉, 이 함수에서 메모리를 할당 받으면 안됨)
//
void ftl_write(int lsn, char *sectorbuf)
{
	char pagebuf[PAGE_SIZE];
	int offset = lsn % PAGES_PER_BLOCK;
	int lbn = lsn / PAGES_PER_BLOCK;
	int pbn = address_mapping_table[lbn];
	int ppn = pbn * PAGES_PER_BLOCK + offset;
	if(pbn == -1){								//처음 쓰기
		for(int i=0;i<BLOCKS_PER_DEVICE;i++){
			if(!checking[i]){
				address_mapping_table[lbn] = i;
				checking[i] = true;
				break;
			}
		}
		
		pbn = address_mapping_table[lbn];
		ppn = pbn * PAGES_PER_BLOCK + offset;
		
		memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
		memcpy(pagebuf+SECTOR_SIZE,&lbn,sizeof(int));
		memcpy(pagebuf+SECTOR_SIZE+sizeof(int),&lsn,sizeof(int));

		dd_write(ppn,pagebuf);

		if(offset != 0){
			memset(pagebuf,0xff,sizeof(pagebuf));
			memcpy(pagebuf+SECTOR_SIZE,&lbn,sizeof(int));
			ppn = pbn * PAGES_PER_BLOCK;
			dd_write(ppn,pagebuf);
		}

	}
	else{		
		char pagebuf[PAGE_SIZE];
		int offset = lsn % PAGES_PER_BLOCK;
		int lbn = lsn / PAGES_PER_BLOCK;
		int pbn = address_mapping_table[lbn];
		int psn = pbn * PAGES_PER_BLOCK + offset;				
		int tmp,new_pbn,new_ppn;

		dd_read(ppn,pagebuf);
		memcpy(&tmp,pagebuf+SECTOR_SIZE+sizeof(int),sizeof(int));

		if(tmp >= 0){			//overwrite
			char tmpbuf[PAGE_SIZE];

			new_pbn = address_mapping_table[DATABLKS_PER_DEVICE];

			for(int i=0;i<PAGES_PER_BLOCK;i++){
				psn = pbn * PAGES_PER_BLOCK + i;
				new_ppn = new_pbn * PAGES_PER_BLOCK + i;
				dd_read(psn,tmpbuf);
				if(i != offset)
					dd_write(new_ppn,tmpbuf);
				else{
					memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
					memcpy(pagebuf+SECTOR_SIZE,&lbn,sizeof(int));
					memcpy(pagebuf+SECTOR_SIZE+sizeof(int),&lsn,sizeof(int));
					dd_write(new_ppn,pagebuf);
				}
			}


			address_mapping_table[lbn] = address_mapping_table[DATABLKS_PER_DEVICE];
			address_mapping_table[DATABLKS_PER_DEVICE] = pbn;
			dd_erase(pbn);
		}
		else{
			memcpy(pagebuf,sectorbuf,SECTOR_SIZE);
			memcpy(pagebuf+SECTOR_SIZE,&lbn,sizeof(int));
			memcpy(pagebuf+SECTOR_SIZE+sizeof(int),&lsn,sizeof(int));
			dd_write(ppn,pagebuf);
		}
	}
	return;
}

void ftl_print()
{
	printf("lbn pbn\n");
	for(int i=0;i<DATABLKS_PER_DEVICE;i++)
		printf("%d %d\n",i,address_mapping_table[i]);
	printf("free block : %d\n",address_mapping_table[DATABLKS_PER_DEVICE]);
	return;
}