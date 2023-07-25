/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    22-April-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdint.h>
#include "stdio.h"
#include "math.h"
#include "string.h"

#include "sys.h"
#include "delay.h"
#include "debug_uart.h"



#include "Timer.h"
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
#include "Wdg.h"



/*
*电压输出或电流输出，最电压有两个版本，80V和100V
*/


#define PCB_V14			14			//LZG_WFC,添加小数点此处报错
#define PCB_V15			15
#define PCB_V16			16			//杨工改进输出幅值版本
#define PCB_VERSION		PCB_V15

#define SOFTWARE_VERSION 0x030201		//版本号采用3位管理，xx.xx.xx

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




#define	VOLTAGE_REF_VALUE		4.096		//基准值为4.096V
#define DEBUG_UART_RATE     115200	//定义调试串口波特率=115200bps


#define LED1_ON()	GPIOE->BSRRH = GPIO_Pin_6
#define LED1_OFF()	GPIOE->BSRRL = GPIO_Pin_6

#define APOW_ON()	GPIOF->BSRRL = GPIO_Pin_3
#define APOW_OFF()	GPIOF->BSRRH = GPIO_Pin_3

#define LCD_RST_H()	GPIOE->BSRRL = GPIO_Pin_5
#define LCD_RST_L()	GPIOE->BSRRH = GPIO_Pin_5


#define TEST_SW_CV_Ploar_H()	GPIOA->BSRRL = GPIO_Pin_15
#define TEST_SW_CV_Ploar_L()	GPIOA->BSRRH = GPIO_Pin_15


typedef enum{
	STM32F0,
	STM32F1,
	STM32F2,
	STM32F3,
	STM32F4,
	STM32F7,
	STM32L0,
	STM32L1,
	STM32L4,
	STM32H7,
}MCUTypedef;










void Delay_ms(volatile unsigned int nTime);
void delay_us(uint32_t us);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


extern volatile unsigned int SysTick_Count;   //Systick计数
extern volatile unsigned int SysTick_Count1;   //Systick计数
extern volatile unsigned int TimingDelay;     //延时函数计数

extern	volatile unsigned char UI_TASK_POLL_FLAG;

//extern volatile unsigned char Voltage_Ploar_Flag ; 		//电压极性全局变量，0代表正极性，1代表负极性
//extern volatile unsigned char Current_Ploar_Flag ; 		//电流极性全局变量，0代表正极性，1代表负极性



#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
