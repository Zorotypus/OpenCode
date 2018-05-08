/*
LoRa.c
LoRa module
2017-12-22  23:45
Azreal
*/
#include "stm32f10x.h"
#include "lora.h"
#include "usart.h"
#include "string.h"
#include "w25qxx.h"
#include "Timer.h"
#include "usart.h"
#include "mymalloc.h"
#include "command.h"
#include "kx122.h"
#include "diag/Trace.h"

u8 lora_default_para[3]={0xC1,0xC1,0xC1};
//u8 lora_write_para_save[6]={lora_head_save,
//							(u8)(lora_local_addr>>8),
//							(u8)lora_local_addr,
//							lora_speed_96,
//							lora_channel,
//							lora_option};
//u8 lora_write_para_nosave[6]={lora_head_nosave,
//							(u8)(lora_des_addr>>8),
//							(u8)lora_des_addr,
//							lora_speed_96,
//							lora_channel,
//							lora_option};
u8 lora_write_para_save[6]={0xC0,0x00,0x02,0x1C,0x17,0xf8};		//本机地址 透传模式 FEC关闭 9.6k
u8 lora_write_para_nosave[6]={0xc2,0x00,0x01,0x1C,0x17,0xC0};	//目标地址 透传模式 FEC关闭 9.6k
u8 lora_addr_channel[3]={(u8)(lora_des_addr>>8),(u8)lora_des_addr,lora_des_channel};
u8 check_head[4]; 	//数据校验头

void Lora_Init()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;     //推完输出
	GPIO_InitStructure.GPIO_Pin=lora_m0_pin|lora_m1_pin;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	GPIO_SetBits(GPIOC,lora_m0_pin|lora_m0_pin);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

//	Lora_Mode_Switch(0);
	while(Lora_Check_Aux_Busy()==0);
}
//lora初始化配置
//缺省配置 	0x00,0xAA,0x1A,0x17,0x44
//模块地址 0x00,0xAA 信道 0x17
void Lora_Parameter_Config(u8 mode)
{
//	Lora_Mode_Switch(mode); 	//切换为休眠模式
//	Lora_Parameter_Write(lora_write_para_save,6);	//先写入本机地址信息 00 02 17
//	Lora_Parameter_Read(lora_default_para,3);
	Lora_Parameter_Write(lora_write_para_save,6);	//配置目标地址信息 掉电不保存
	Lora_Parameter_Read(lora_default_para,3);
#ifdef DEBUG
	trace_printf("write desaddr success!");
#endif
	Lora_Mode_Switch(mode); 				//进入省电模式，需要在模式1的lora模块唤醒
	while(Lora_Check_Aux_Busy()==0);
	//Kx122_Start_Measure();
}

//lora模块参数读取
void Lora_Parameter_Read(u8 *send_buff,u8 len)
{
	u8 i=0;
	Lora_Mode_Switch(3);  //首先进入模式3
	while(Lora_Check_Aux_Busy()==0);
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
		USART_SendData(USART2,send_buff[i]);
	}
	while(1)
	{
		if(command.cmd_len>=1&&command.cmd_len<=6)
		{
			memcpy(lora_rec_data,command.cmd_buff,command.cmd_len);
			command.cmd_len=0;
			break;
		}
	}
}
//lora模块参数写入
void Lora_Parameter_Write(u8 *send_buff,u8 len)
{
	u8 i=0;
	Lora_Mode_Switch(3);  //首先进入模式3
	while(Lora_Check_Aux_Busy()==0);
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
		USART_SendData(USART2,send_buff[i]);
	}
	while(Lora_Check_Aux_Busy()==0);
}
//lora复位
void Lora_Reset()
{
	u8 i=0;
	u8 cmd_reset[3]={0xc4,0xc4,0xc4};
	for(i=0;i<3;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
		USART_SendData(USART2,cmd_reset[i]);
	}
}

//数据格式：head+datalen+did+data1..datan+datasum
//数据头+数据长度+数据ID号+数据+数据和
/*void Lora_Send_Data(u8 *data_buff,u32 len,u16 samplepoint,u8 blockval,u8 mode)
{
	u16 i,m,n,temp;
	u32	j;
	u32 setval;
	u8 lora_block_size=53;
	u16 nmemb;					//数据包数量
	u16 data_block_pack;
	u16 did=0;					//数据包ID号
	u16 datasum=0;				//每块数据总和
	data_buff=mymalloc(len);	//包括传输地址信道
	nmemb=blockval*samplepoint*6/lora_block_size;
	if(blockval*samplepoint*6%lora_block_size)nmemb++;
	data_block_pack=len/lora_block_size;
	if(len%lora_block_size)data_block_pack++;
	temp=nmemb;
	nmemb=nmemb/data_block_pack;
	if(temp%data_block_pack)nmemb++;
	while(Daq_Stop(nmemb)==SET);	//等待采样结束
	Lora_Mode_Switch(0);		//模式0 串口发送
	while(Lora_Check_Aux_Busy()==0);
	for(j=0;j<nmemb;j++)
	{
		//每次读取1020个字节，循环从4096个sector中读取
		W25qxx_Read_Page(data_buff,(Flash_KX122_WRITE_ADDR+4096*j),len);
		//循环5次，每次1020/5=210个字节
		while(len>lora_block_size)
		{
			for(i=0;i<data_block_pack;i++)
			{
				did++;
				check_head[0]=0x0A;
				check_head[1]=0xA0;
				check_head[2]=len/data_block_pack;
				check_head[3]=did;
				for(n=0;n<3;n++)
				{
					while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
					USART_SendData(USART2,lora_addr_channel[n]);
				}
				for(m=0;m<4;m++)
				{
					while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
					USART_SendData(USART2,check_head[m]);
				}
				for(setval=0;setval<(len/sub_num);setval++)
				{
					datasum+=data_buff[setval+i*len/sub_num];
					while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
					USART_SendData(USART2,data_buff[setval+i*len/sub_num]);
				}
				while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
				USART_SendData(USART2,(u8)datasum);
				while(Lora_Check_Aux_Busy()==0);
			}
			len-=lora_block_size;
		}
	}
	myfree(data_buff);
}*/

void Lora_Send_Data(u8 *data_buff,u32 len,u16 samplepoint,u8 blockval,u16 data_block)
{
	u8 *tmp;
	u8 i;
	u8 j=0;
	u8 checksum=0;	//校验和
	tmp=mymalloc(512);	 	//申请一块内存以发送数据
	if(tmp==NULL)
	{
#ifdef DEBUG
	trace_printf("malloc failed!");
#endif
	}
	W25qxx_Read_Page(data_buff,(Flash_KX122_WRITE_ADDR+4096*data_block),len);
	*tmp=lora_des_addr>>8;
	*(tmp+1)=(u8)lora_des_addr;
	*(tmp+2)=0x17;
	*(tmp+3)=(u8)(checkdata>>8);
	*(tmp+4)=(u8)(checkdata);
	*(tmp+5)=data_block;
	*(tmp+6)=lora_block_size;
	while(len>lora_block_size)
	{
		memcpy(tmp+7,data_buff+j*lora_block_size,lora_block_size);
		for(i=0;i<lora_block_size;i++)
		{
			checksum+=data_buff[j*lora_block_size+i];
		}
		*(tmp+7+lora_block_size)=(u8)checksum;
		while(Lora_Check_Aux_Busy()==0);
		USART2_SEND_DATA(tmp,lora_block_size+8);
		len-=lora_block_size;
		checksum=0;
		j++;
	}
//	len=lora_block_size-len;
	checksum=0;
	memcpy(tmp+8,data_buff+j*lora_block_size,len);
	for(i=0;i<len;i++)
	{
		checksum+=data_buff[j*lora_block_size+i];
	}
	*(tmp+7+lora_block_size)=(u8)checksum;
	while(Lora_Check_Aux_Busy()==0);
	USART2_SEND_DATA(tmp,lora_block_size+8);

	myfree(tmp);
}
//开始发送数据
void Transmit_Data(u16 blockval,u16 point)
{
	u8 lora_send_buff[1024];
	u8 i;
	u16 did;
//	lora_send_buff=mymalloc(1024);
	if(lora_send_buff==NULL)
	{
		trace_printf("malloc sizo of lora buff error!");
	}
	did=point*6/1020;
	if(point*6%1020)did++;
	while(Daq_Stop(did)==SET);	//等待采样结束
	Lora_Mode_Switch(0);		//模式0 串口发送
	while(Lora_Check_Aux_Busy()==0);
	for(i=0;i<did;i++)
	{
		Lora_Send_Data(lora_send_buff,frame_len,point,blockval,i);
	}
}

//检测AUX脚电平
u8 Lora_Check_Aux_Busy()
{
	timer_sleep(10);
	while(GPIO_ReadInputDataBit(lora_AUX_port,lora_AUX_pin)!=SET)
	{
		return 0;		//AUX为低
	}
	return 1;			//AUX为高
}

//lora模式选择
//0:一般模式 接收来自模式0,1的数据包
//1:唤醒模式 添加唤醒码  0,1,2收到
//2:省电模式  只接收来自模式1的数据包
//3:休眠模式 配置参数
void Lora_Mode_Switch(u8 mode)
{
	switch(mode)
	{
	case 0:
		M0_Low();
		M1_Low();
		break;
	case 1:
		M0_High();
		M1_Low();
		break;
	case 2:
		M0_Low();
		M1_High();
		break;
	case 3:
		M0_High();
		M1_High();
		break;
	}
	//while(Lora_Check_Aux_Busy()==0);
}

