/*
LoRa.c
LoRa module
2018-01-02  23:24
Azreal
*/


#include "stm32f10x.h"
#include "mymalloc.h"
#include "stddef.h"

//�ڴ渴��
//Ŀ���ַ  *des
//Դ��ַ	 *src
//������ 	 n
void mymemcpy(void *des,void *src,u32 n)
{
	u8 *xdes=des;
	u8 *xsrc=src;
	while(n--)*xdes++=*xsrc++;
}
//�ڴ渳ֵ
//addr	��ַ
// c 	��ֵ
// n 	����
void mymemset(u8 *addr,u8 c,u32 n)
{
	u8 *xaddr=addr;
	while(n--)*xaddr=c;
}
//��ʼ���ڴ�
void my_mem_init()
{
	mymemset(memmap,0,MEM_ALLOC_TABLE_SIZE);
	mymemset(membase,0,MEM_MAX_SIZE);
	memflag=1;
}
//�ڴ�ʹ����
//����0-100
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
//�ڴ����
//�����С	size(�ֽ�)
//���������ڴ��׵�ַ
u32 my_mem_malloc(u32 size)
{
	u16 i;
	u16 nmemb;
	u16 cmemb=0;
	u16 offset;						//���ݿ���ƫ��
	if(size>MEM_MAX_SIZE)return 0xffffffff;
	if(memflag==0)
	{
		my_mem_init();
	}
	nmemb=size/MEM_BLOCK_SIZE;		//�����ڴ����
	if(size%MEM_BLOCK_SIZE)nmemb++;	//����MEM_BLOCK_SIZE���������ڴ������1 ����nmembʼ�մ��ڵ���1
	for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)
	{
		if(!memmap[offset])cmemb++;				//�Ӻ���ǰ�����ڴ�飬����ڴ����Ϊ0,���1
		else cmemb=0;
		if(cmemb==nmemb)
		{
			for(i=0;i<nmemb;i++)
			{
				memmap[i+offset]=1;				//�ڴ����λΪ1
				return offset*MEM_BLOCK_SIZE; 	//����Ҫ����ĵ�ַ��λ
			}
		}
	}
	return 0xffffffff;
}
//�ͷ��ڴ�
//offset ƫ���׵�ַ
u8 my_mem_free(u32 offset)
{
	u16 nmemb;
	u16 i;
	if(memflag==0)
	{
		return 0;    	//δ���г�ʼ��������
	}
	if(offset<MEM_MAX_SIZE)
	{
		nmemb=offset/MEM_BLOCK_SIZE;
		for(i=nmemb;i<MEM_ALLOC_TABLE_SIZE;i++)
		{
			memmap[i]=0;	//�ڴ����λΪ0,��ʾ��Ҫ�ͷŵ��ڴ�
			return 1;
		}
	}
	return 2;
}
//�ⲿ�����ڴ����
//size Ҫ����Ĵ�С
void *mymalloc(u32 size)
{
	u32 offset;
	offset=my_mem_malloc(size);  	//�����ڴ��׵�ַ
	if(offset==0xffffffff)return NULL;
	else return (void*)((u32)membase+offset);			//����ƫ�Ƶ�ַ+�����׵�ַ
}
//�ͷ��ڴ�
//paddr �ͷ��ڴ���׵�ַ
void myfree(void *paddr)
{
	u32 offset;
	if(paddr==NULL)return;
	offset=(u32)paddr-(u32)membase;   	//�õ�ƫ�Ƶ�ַ
	my_mem_free(offset);				//�ͷ��ڴ�
}

//���·����ڴ�
//paddr ���ڴ���ַ
//size 	�ڴ��С
//����ֵ   ���ڴ���ַ
 void *myrealloc(void *paddr,u32 size)
 {
	 u32 offset;
	 offset=my_mem_malloc(size);		//�ڴ��׵�ַ
	 if(offset==0xffffffff)return NULL;
	 else
	 {
		 mymemcpy((void*)((u32)membase+offset),paddr,size);
		 myfree(paddr);
		 return (void*)(offset+(u32)membase);
	 }
 }
