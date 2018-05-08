/*
command.c
��λ������
*/

#include "stm32f10x.h"
#include "kx122.h"
#include "command.h"
#include "lora.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "diag/Trace.h"

#define LORA_CONFIG 		0X01
#define START_SAMPLE 		0X02
#define STOP_SAMPLE 		0X03
#define LORA_DEST_CONFIG 	0X04
#define LORA_LOCAL_CONFIG 	0X05
#define LORA_AWAKE			0X06
//extern usart_recevice usart_rec_data;
extern u16 datablock;
//KX122 ֹͣ����
u16 Daq_Stop(u16 block)
{
	command.blockval=block;
	if(datablock>=command.blockval)
	{
		Kx122_Stop_Measure();
		datablock=0;
		return 0;
	}
	return 1;
}
//���ݿ��� һ�����ݿ�4096����   �ܹ�4096*6=24576����  3������
void Daq_Transmit(u16 block,u16 samplepoint)
{
	Transmit_Data(block,samplepoint);
}
//������
//CMD_LORA_START_SAMPLE ��ʼ�����������ɼ�������������
//CMD_LORA_STOP_SAMPLE	�ɼ�����
//%s �ַ���
u8 get_cmd(u8 *command_buff,u8 argv[])
{
	if(command.cmd_len>0)	//���ڽ��յ�������
	{
#ifdef DEBUG
		trace_printf("GET_COMMAND->\n",command.cmd_buff);
#endif
		if(strstr((char*)command.cmd_buff,CMD_LORA_LORA_CONF)!=NULL)
		{
			//���ô�����ID ������ ��������  ����������
			*command_buff=LORA_CONFIG;
			sscanf((char*)command.cmd_buff,"AT+CMDGET_LORA_CONF=%d,%d,%d",
						(int*)&argv[0],(int*)&argv[4],(int*)&argv[8]);
		}
		else if(strstr((char*)command.cmd_buff,CMD_LORA_START_SAMPLE)!=NULL)
		{
			//��������
			*command_buff=START_SAMPLE;
			sscanf((char*)command.cmd_buff,"AT+CMDGET_SAMPLE_START=%d",
								(int*)&argv[0]);
		}
		else if(strstr((char*)command.cmd_buff,CMD_LORA_STOP_SAMPLE)!=NULL)
		{
			//������ID
			*command_buff=STOP_SAMPLE;
			sscanf((char*)command.cmd_buff,"AT+CMDGET_SAMPLE_STOP=%d",
								(int*)&argv[0]);
		}
		else if(strstr((char*)command.cmd_buff,CMD_LORA_DEST_CONFIG)!=NULL)
		{
			//������ID
			*command_buff=LORA_DEST_CONFIG;
			sscanf((char*)command.cmd_buff,"AT+CMDGET_DEST_ADDR=%d",
								(int*)&argv[0]);
		}
		else if(strstr((char*)command.cmd_buff,CMD_LORA_LOCAL_CONFIG)!=NULL)
		{
			//������ID
			*command_buff=LORA_LOCAL_CONFIG;
			sscanf((char*)command.cmd_buff,"AT+CMDGET_LOCAL_ADDR=%d",
								(int*)&argv[0]);
		}
		else if(strstr((char*)command.cmd_buff,CMD_LORA_AWAKE)!=NULL)
		{
			//������ID
			*command_buff=LORA_AWAKE;
//			sscanf((char*)command.cmd_buff,"AT+CMDGET_LORA_AWAKE=%d",(int*)&argv[0]);
		}
		command.cmd_len=0;
		return 1;
	}
	return 0;
}

//ѭ����������
void handle_recv_cmd()
{
	u8 cmd;
	u8 argv[16];
	u8 cfg_flag=0;	//���ñ�־λ
	u8 data_id=0;
	if(get_cmd(&cmd,argv))
	{
		switch(cmd)
		{
		case LORA_CONFIG:
			command.samplerate=*(int*) &argv[4];
			Kx122_Config(*(int*) &argv[4]);
			command.samplepoint=*(int*) &argv[8];
#ifdef DEBUG
			trace_printf("LORA_PARAMETER:  samplerate:%d  samplepoint:%d",*(int*) &argv[4],*(int*) &argv[8]);
#endif
			cfg_flag=1;
			break;
		case START_SAMPLE:
			if(cfg_flag==1)
			{
				Kx122_Start_Measure();
				Daq_Transmit(*(int*) &argv[8]/4096,*(int*) &argv[8]);
				cfg_flag=0;
			}
			else
			{
				Kx122_Start_Measure();
#ifdef DEBUG
				trace_printf("LORA_NOCONFIG! default:  samplerate:3200  samplepoint:4096\n");
#endif
				Daq_Transmit(1,4096);			//û���Ƚ������ã�����ȱʡ���ý��вɼ�
			}
			break;
		case STOP_SAMPLE:
			Kx122_Stop_Measure();
			break;
		case LORA_DEST_CONFIG:
			break;
		case LORA_LOCAL_CONFIG:
			break;
		case LORA_AWAKE:
			//trace_printf("LORA	AWAKE!");
			Lora_Mode_Switch(0);	//�л���һ��ģʽ
			if(cfg_flag==1)
			{
				Kx122_Start_Measure();
				Daq_Transmit(*(int*) &argv[8]/4096,*(int*) &argv[8]);
				cfg_flag=0;
			}
			else
			{
				Kx122_Start_Measure();
#ifdef DEBUG
				trace_printf("LORA_NOCONFIG! default:  samplerate:3200  samplepoint:4096\n");
#endif
				Daq_Transmit(1,4096);			//û���Ƚ������ã�����ȱʡ���ý��вɼ�
			}
			Lora_Mode_Switch(2);	//�л���ʡ��ģʽ
			break;
		}
		clear_cmd_buff();
	}

}
u16 i;
void clear_cmd_buff()
{
	for(i=0;i<CMD_RX_LEN;i++)
	{
		command.cmd_buff[i]=0;
	}
}

