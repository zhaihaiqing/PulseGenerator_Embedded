
/********************************************************************
*
*	文件	：	HeaderFiles.h

*	管脚	：	(NONE)

*	功能	：	头文件统一包含
*
******************************************************************************/
#ifndef _HEADERFILES_H
#define _HEADERFILES_H

#include "stm32f4xx.h"
#include "stm32f407xx.h"

#include "stdio.h"
#include "math.h"
#include "string.h"

#include "sys.h"
#include "delay.h"
#include "usart2.h"

#define PCB_V14			14			//LZG_WFC,添加小数点此处报错
#define PCB_V15			15
#define PCB_V16			16			//杨工改进输出幅值版本
#define PCB_VERSION		PCB_V15

#define LZG_WFC			0xff

#define TEST_LED		1
#define TEST_AT24CXX	0
#define TEST_LCD		0
#define TEST_OUTPUT		0
#define TEST_KEYBOARD	0
#define TEST_TEMP		1

//读取ID方案，A--直接赋值，容易驱动成功		B--读取寄存器，经常失败，待解决
#define ID_PLAN_A		0
#define ID_PLAN_B		1
#define LCD_ID_PLAN		ID_PLAN_A

#include "Timer6.h"
#include "Led.h"
#include "Beep.h"
#include "Key.h"
#include "DAC.h"
#include "Codec.h"
#include "Switch.h"
#include "Trigger.h"
#include "Manual.h"
#include "AT24C64IIC.h"
#include "AT24C64.h"
#include "AT24C64MemData.h"
#include "Pulse.h"
#include "Usart1.h"
#include "Malloc.h"
#include "Crc.h"
#include "Protocol.h"
#include "PulseTran.h"
#include "SSD1961.h"
#include "UI.h"
#include "Output.h"
#include "Timer2.h"
#include "Wdg.h"




//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_2M		0		//GPIO速度2Mhz
#define GPIO_SPEED_25M		1		//GPIO速度25Mhz
#define GPIO_SPEED_50M		2		//GPIO速度50Mhz
#define GPIO_SPEED_100M		3		//GPIO速度100Mhz

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

//GPIO引脚编号定义
#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 


void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);





#endif
