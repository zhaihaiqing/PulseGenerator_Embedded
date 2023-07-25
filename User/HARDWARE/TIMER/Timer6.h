
/********************************************************************
*
*	文件	：	Timer6.h

*	管脚	：	(NONE)

*	功能	：	Timer定时计数器，控制时基计数
*
******************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

typedef struct
{
	uint8_t  U1RxTimeOutCnt;	//串口1接收超时计数
	uint8_t  U2RxTimeOutCnt;	//串口2接收超时计数
	
	uint8_t	 Scan10msCnt;		//10ms轮询计数
	
	uint16_t LedCtrl;
	
	uint16_t BeepDecCnt;
	uint16_t MemoryUpdateCnt;	//存储更新计时
	uint16_t CodecRotateCnt;	//编码器旋速计数，用于检测转动速度控制调节步进值
	
	uint32_t RunCnt;			//运行时长计数
}sTimer6_t;
extern sTimer6_t T6;




//#define ARR_1S		7199
//#define PSC_1S		9999
//#define ARR_100MS	719
//#define PSC_100MS	9999
//#define ARR_10MS	719
//#define PSC_10MS	999
//#define ARR_1MS		719
//#define PSC_1MS		99
#define ARR_100US	719
#define PSC_100US	9
#define ARR_10US	71
#define PSC_10US	9

/*	1minute = 60second = 60*1000millisecond	*/
#define T6_VALUE_MILLISEC	(1)
#define T6_VALUE_SECOND		(T6_VALUE_MILLISEC * 1000)
#define T6_VALUE_MINUTE		(T6_VALUE_SECOND * 60)

void Timer6_Init(u16 arr,u16 psc);

#endif
