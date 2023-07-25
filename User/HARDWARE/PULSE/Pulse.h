
/********************************************************************
*
*	文件	：	Pulse.h

*	管脚	：	
				PB8		--TIM4 CH3

*	功能	：	脉冲输出引脚
*
******************************************************************************/
#ifndef __PULSE_H
#define __PULSE_H
#include "main.h"

/*	原SYNC_ARR=65455	SYNC_PSC=219经计算确实为导致0.199s周期原因	*/
#define SYNC_ARR 39
#define SYNC_PSC 41
#define SYNC_CCR 0

extern uint8_t Tbl_Flag[8];

typedef enum
{
	DOSTATE_STATUS_RUNNING = 0,
	DOSTATE_STATUS_COMPLETE,
	DOSTATE_STATUS_PAUSE,
}DO_STATUS_E;

__packed typedef struct
{
	uint8_t 	Config;				//普通一到五波形配置标志，bit7~bit0--8路输出是否配置
	
	DO_STATUS_E Status[8];			//运行标志，0--运行，1--完成，2--暂停
	
	uint8_t		ZeroConfig;			//特殊第零种波形配置标志，其与config功能一致，但因实现方式不同，难以统一在一起
	uint16_t    ZeroIndexCnt[8];	//数组索引计数	
}sDOState_t;						//DATA OUTPUT--PWM
extern sDOState_t DOState;

__packed typedef struct
{
	uint16_t Index;					//波形参数数组下标
	uint32_t PulseNum;				//单种占空比波形个数
	uint32_t CycleCnt;				//循环次数
}sTimerRun_t;

/*	定时器通道号对应配置	*/
#define DO_TIM11	0
#define DO_TIM4		1
#define DO_TIM3		2
#define DO_TIM9		3
#define DO_TIM2		4
#define DO_TIM8		5
#define DO_TIM1		6
#define DO_TIM5		7
#define DO_ALL		8
#define DO_NO		0XFF

extern volatile uint32_t *Tbl_TIM_CNT[8];
extern volatile uint32_t *Tbl_TIM_ARR[8];
extern volatile uint16_t *Tbl_TIM_PSC[8];
extern volatile uint32_t *Tbl_TIM_CCRx[8];
extern volatile uint16_t *Tbl_TIM_DIER[8];
extern volatile uint16_t *Tbl_TIM_CR1[8];

extern sTimerRun_t TimerRun[8];

extern void (*Tbl_DOX_TIM_Init[8])(uint16_t arr, uint16_t psc);
extern void DO0_PB9_TIM11_CH1(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
extern void DO1_PB8_TIM4_CH3(uint16_t arr, uint16_t psc);
extern void DO2_PA6_TIM3_CH1(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
extern void DO3_PA2_TIM9_CH1(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
extern void DO4_PB10_TIM2_CH3_F(uint16_t arr, uint16_t psc);	//暂时未使用通道，可供扩展
extern void DO5_PC6_TIM8_CH1(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
extern void DO6_PA11_TIM1_CH4(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
extern void DO7_PA3_TIM5_CH4(uint16_t arr, uint16_t psc);		//暂时未使用通道，可供扩展
void DO_ALL_SYNC_Init(uint8_t do_timx);

//extern unsigned char DOStatue;		//设备运行状态

#endif
