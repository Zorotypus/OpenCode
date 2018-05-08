#ifndef MYMALLOC_H_
#define MYMALLOC_H_

#define MEM_MAX_SIZE			6*1024     //内存总大小
#define MEM_ALLOC_TABLE_SIZE	6			//内存总块数
#define MEM_BLOCK_SIZE			1024		//内存块数大小


u8 memmap[MEM_ALLOC_TABLE_SIZE];    //内存块标记
u8 membase[MEM_MAX_SIZE];			//内存总大小
static u8 memflag=0;							//初始化标记位

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
