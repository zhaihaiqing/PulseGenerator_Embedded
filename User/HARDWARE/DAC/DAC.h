
/********************************************************************
*
*	文件	：	DAC.h

*	管脚	：	
				PA2 --AD5542_CS1
				PA3 --AD5542_CS2
				PA4 --AD5542_LDAC
				PA5 --AD5542_SCLK
				PA6 --AD5542_SDIN
				
				
				
				PA7 --总输出控制（继电器控制）
				PB2 --模式切换，用于切换电压模式、电流模式
				



*	功能	：	模拟信号输出
*	输出关系：	
				双极性电流源-----125Ω电流反馈电阻，输入-4V至+4V对应输出-32mA至+32mA
				双极性电压源-----25倍放大，输入-4V至+4V对应输出-100V至+100V
*
******************************************************************************/
#ifndef __DAC_H
#define __DAC_H
#include "main.h"

#define DA_OUTPUTRANGE_10000MV	10000		//10V
#define DA_OUTPUTRANGE_10800MV	10800		//10.8V
#define DA_OUTPUTRANGE_4096MV	4096		//4.96V
//#define DA_OUTPUTRANGE			DA_OUTPUTRANGE_10800MV
#define DA_OUTPUTRANGE			DA_OUTPUTRANGE_4096MV



#define DA_CS1_H()	GPIOA->BSRRL = GPIO_Pin_2
#define DA_CS1_L()	GPIOA->BSRRH = GPIO_Pin_2

#define DA_CS2_H()	GPIOA->BSRRL = GPIO_Pin_3
#define DA_CS2_L()	GPIOA->BSRRH = GPIO_Pin_3

#define DA_LDAC_H()	GPIOA->BSRRL = GPIO_Pin_4
#define DA_LDAC_L()	GPIOA->BSRRH = GPIO_Pin_4

#define DA_SCLK_H()	GPIOA->BSRRL = GPIO_Pin_5
#define DA_SCLK_L()	GPIOA->BSRRH = GPIO_Pin_5

#define DA_SDIN_H()	GPIOA->BSRRL = GPIO_Pin_7
#define DA_SDIN_L()	GPIOA->BSRRH = GPIO_Pin_7


#define DA_CHNL_VOLT	1
#define DA_CHNL_CURR	2
#define DA_CHNL_ALL		3




void SPI1_Configuration(void);
void SPI1_WriteByte(unsigned char byte);

void AD5542_HSPI_WriteA( int64_t val);






void AD5542_Init(void);
void AD5542_Output(uint8_t ch, int32_t value);





#endif

/**********************************************************************
电压需求：
			输出范围-99.9V至+99.9V连续可调
			输出最大电流50mA,电路可检测（无检测）
			
			输出阻抗：小于1000欧
			脉冲频率10Hz-50KHz连续可调
电流需求：
			输出范围2uA-32mA连续可调
			输出最大电压100V，电路可检测（无检测）
			输出阻抗：小于1000欧
			脉冲频率10Hz-50KHz连续可调
********************************************************************************/
