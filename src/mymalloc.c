/*
LoRa.c
LoRa module
2018-01-02  23:24
Azreal
*/


#include "stm32f10x.h"
#include "mymalloc.h"
#include "stddef.h"

//内存复制
//目标地址  *des
//源地址	 *src
//复制数 	 n
void mymemcpy(void *des,void *src,u32 n)
{
	u8 *xdes=des;
	u8 *xsrc=src;
	while(n--)*xdes++=*xsrc++;
}
//内存赋值
//addr	地址
// c 	赋值
// n 	数量
void mymemset(u8 *addr,u8 c,u32 n)
{
	u8 *xaddr=addr;
	while(n--)*xaddr=c;
}
//初始化内存
void my_mem_init()
{
	mymemset(memmap,0,MEM_ALLOC_TABLE_SIZE);
	mymemset(membase,0,MEM_MAX_SIZE);
	memflag=1;
}
//内存使用率
//返回0-100
u8 my_mem_perused()
{
	u16 i;
	u16	used;
	for(i=0;i<MEM_ALLOC_TABLE_SIZE;i++)
	{
		if(memmap[i]==1)used++;
	}
	return used*100/MEM_ALLOC_TABLE_SIZE;
}
//内存分配
//分配大小	size(字节)
//返回申请内存首地址
u32 my_mem_malloc(u32 size)
{
	u16 i;
	u16 nmemb;
	u16 cmemb=0;
	u16 offset;						//数据块数偏移
	if(size>MEM_MAX_SIZE)return 0xffffffff;
	if(memflag==0)
	{
		my_mem_init();
	}
	nmemb=size/MEM_BLOCK_SIZE;		//所需内存块数
	if(size%MEM_BLOCK_SIZE)nmemb++;	//大于MEM_BLOCK_SIZE，则申请内存块数加1 所以nmemb始终大于等于1
	for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)
	{
		if(!memmap[offset])cmemb++;				//从后往前申请内存块，如果内存块标记为0,则加1
		else cmemb=0;
		if(cmemb==nmemb)
		{
			for(i=0;i<nmemb;i++)
			{
				memmap[i+offset]=1;				//内存块标记位为1
				return offset*MEM_BLOCK_SIZE; 	//返回要申请的地址首位
			}
		}
	}
	return 0xffffffff;
}
//释放内存
//offset 偏移首地址
u8 my_mem_free(u32 offset)
{
	u16 nmemb;
	u16 i;
	if(memflag==0)
	{
		return 0;    	//未进行初始化，返回
	}
	if(offset<MEM_MAX_SIZE)
	{
		nmemb=offset/MEM_BLOCK_SIZE;
		for(i=nmemb;i<MEM_ALLOC_TABLE_SIZE;i++)
		{
			memmap[i]=0;	//内存块标记位为0,表示需要释放的内存
			return 1;
		}
	}
	return 2;
}
//外部调用内存分配
//size 要分配的大小
void *mymalloc(u32 size)
{
	u32 offset;
	offset=my_mem_malloc(size);  	//申请内存首地址
	if(offset==0xffffffff)return NULL;
	else return (void*)((u32)membase+offset);			//返回偏移地址+数组首地址
}
//释放内存
//paddr 释放内存的首地址
void myfree(void *paddr)
{
	u32 offset;
	if(paddr==NULL)return;
	offset=(u32)paddr-(u32)membase;   	//得到偏移地址
	my_mem_free(offset);				//释放内存
}

//重新分配内存
//paddr 旧内存块地址
//size 	内存大小
//返回值   新内存块地址
 void *myrealloc(void *paddr,u32 size)
 {
	 u32 offset;
	 offset=my_mem_malloc(size);		//内存首地址
	 if(offset==0xffffffff)return NULL;
	 else
	 {
		 mymemcpy((void*)((u32)membase+offset),paddr,size);
		 myfree(paddr);
		 return (void*)(offset+(u32)membase);
	 }
 }
