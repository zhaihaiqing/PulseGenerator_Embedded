
/********************************************************************
*
*	文件	：	AT24C64IIC.h

*	管脚	：	
				PB10--AT24C64_SCK
				PB11--AT24C64_SDA

*	功能	：	IIC协议
*
******************************************************************************/
#ifndef __AT24C64IIC_H
#define __AT24C64IIC_H
#include "main.h"

/*	IO方向设置							*/
#define SDA_IN()  {GPIOB->MODER   &= 0xff3fffff;}									//PB11
#define SDA_OUT() {GPIOB->MODER   &= 0xff3fffff;GPIOB->MODER   |= 0x00400000;}

/*	IO操作函数							*/
#define IIC_SCL		PBout(10)			//SCL
#define IIC_SDA_WR	PBout(11)			//写SDA
#define IIC_SDA_RD	PBin(11)			//读SDA

void IIC_Init(void);					//初始化IIC的IO口				 
void IIC_Start(void);					//发送IIC开始信号
void IIC_Stop(void);					//发送IIC停止信号
void IIC_Send_Byte(u8 txd);				//IIC发送一个字节
u8   IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8   IIC_Wait_Ack(void);				//IIC等待ACK信号
void IIC_Ack(void);						//IIC发送ACK信号
void IIC_NAck(void);					//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8	 IIC_Read_One_Byte(u8 daddr,u8 addr);

#endif
