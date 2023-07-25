
/********************************************************************
*
*	文件	：	Trigger.h

*	管脚	：	
				PB0 --TRIGGER_IN
				PB1 --TRIGGER_OUT
				PB15--TRIGGER_PWM

*	功能	：	外部触发信号，TTL形式，中断处理
				脉冲输出信号，TTL形式，同步输出高电平脉冲
				单片机PWM触发中断，TTL形式
*
******************************************************************************/
#ifndef __TRIGGER_H
#define __TRIGGER_H
#include "main.h"

#define pTRIGGER_IN		PBin(0)
#define pTRIGGER_OUT	PBout(1)

#if(PCB_VERSION >= PCB_15)
	//#define pTRIGGER_PWM	PBin(15)
	#define pTRIGGER_PWM	PBin(2)
	
	#define UPDATE_NO	0	//不更新
	#define UPDATE_ALL	1	//更新全部参数，包括不论是否在运行状态的PSC ARR CCRX AMPL等参数
	#define UPDATE_AMPL	2	//只更新AMPL参数
	void UO_Update(uint8_t updatecmd);
	
	void PWM_EXTI15_IRQHandler(void);
#endif

extern __IO unsigned int ex_bnc_flag;



void TriggerExIN01_Init(void);
void TriggerExIN23_Init(void);
void TriggerOUT_Init(void);

void TriggerEx_Init(void);



#endif
