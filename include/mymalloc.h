#ifndef MYMALLOC_H_
#define MYMALLOC_H_

#define MEM_MAX_SIZE			6*1024     //�ڴ��ܴ�С
#define MEM_ALLOC_TABLE_SIZE	6			//�ڴ��ܿ���
#define MEM_BLOCK_SIZE			1024		//�ڴ������С


u8 memmap[MEM_ALLOC_TABLE_SIZE];    //�ڴ����
u8 membase[MEM_MAX_SIZE];			//�ڴ��ܴ�С
static u8 memflag=0;							//��ʼ�����λ

void mymemcpy(void *des,void *src,u32 n);
void mymemset(u8 *addr,u8 c,u32 n);
void my_mem_init();
u8 my_mem_perused();
u32 my_mem_malloc(u32 size);
u8 my_mem_free(u32 offset);
void *mymalloc(u32 size);
void myfree(void *paddr);
void *myrealloc(void *paddr,u32 size);
#endif
