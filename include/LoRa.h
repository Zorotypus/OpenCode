#ifndef LORA_H_
#define LORA_H_

#include "stm32f10x.h"

#define lora_head_save	0xC0
#define lora_head_nosave	0xC2
#define lora_local_addr		((uint16_t)0x0002)	//本机lora地址
#define lora_des_addr		((uint16_t)0x0001)	//目标lora地址
#define	lora_des_channel	0x17	//目标lora信道
//默认8N1  9600  传输速率选择
#define lora_speed_3	0x18
#define lora_speed_12	0x19
#define lora_speed_24	0x1A
#define lora_speed_48	0x1B
#define lora_speed_96	0x1C
#define lora_speed_192	0x1D
//默认433Mhz
//#define lora_channel	0x17
//默认 250ms唤醒时间 20dBm  打开FEC开关
//#define lora_option		0xC4


#define lora_m0_port 	GPIOC
#define lora_m0_pin 	GPIO_Pin_13
#define lora_m1_port 	GPIOC
#define lora_m1_pin 	GPIO_Pin_14
#define lora_AUX_port 	GPIOA
#define lora_AUX_pin 	GPIO_Pin_0

#define M0_High()	GPIO_SetBits(lora_m0_port,lora_m0_pin)
#define M1_High()	GPIO_SetBits(lora_m1_port,lora_m1_pin)
#define M0_Low()	GPIO_ResetBits(lora_m0_port,lora_m0_pin)
#define M1_Low()	GPIO_ResetBits(lora_m1_port,lora_m1_pin)

#define frame_len	1020	//每一帧数
#define sub_num		5		//数据块分包数
#define checkdata	0x0aa0
#define lora_block_size (52-8)
//lora 参数配置
typedef struct
{
	u8 lora_id;
	u8 lora_head;
	u8 lora_highaddr;
	u8 lora_lowaddr;
	u8 lora_speed;
	u8 lora_channal;
	u8 lora_option;
	u8 lora_para[6];
}lora_cfg_cmd;

lora_cfg_cmd lora_cfg;

u8 lora_rec_data[6];	//返回的接收参数



void Lora_Init();
void Lora_Mode_Switch(u8 mode);
u8 test_Lora_Send_Data(u8 mode,u8 *pBuff,u16 len,u16 totlelen);
void Lora_Parameter_Read(u8 *send_buff,u8 len);
u8 Lora_Check_Aux_Busy();
void test_lora();
void Lora_Parameter_Write(u8 *send_buff,u8 len);
void Lora_Parameter_Config(u8 mode);
void Lora_Send_Data(u8 *data_buff,u32 len,u16 samplepoint,u8 blockval,u16 data_block);
void Lora_Transmit_Start();
void Transmit_Data(u16 blockval,u16 point);
#endif
