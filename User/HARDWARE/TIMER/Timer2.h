
/********************************************************************
*
*	文件	：	TIMER2.h

*	管脚	：	(NONE)

*	功能	：	10K频率输出控制定时器
*
******************************************************************************/
#ifndef _TIMER2_H
#define _TIMER2_H
#include "main.h"

typedef struct
{
	uint32_t OutputCnt;
}sTimer2_t;
extern sTimer2_t T2;

/*	1minute = 60second = 60*1000millisecond	*/
#define T2_VALUE_MICROperCYCLE	(100)
#define T2_VALUE_MILLIS			(1000 / T2_VALUE_MICROperCYCLE)
#define T2_VALUE_SEC  			(T2_VALUE_MILLIS * 1000)
#define T2_VALUE_MIN  			(T2_VALUE_SEC * 60)

#define ARR_1S		7199
#define PSC_1S		9999
#define ARR_100MS	719
#define PSC_100MS	9999
#define ARR_10MS	719
#define PSC_10MS	999
#define ARR_1MS		719
#define PSC_1MS		99
#define ARR_100US	719
#define PSC_100US	9
#define ARR_50US	359
#define PSC_50US	9
#define ARR_10US	71
#define PSC_10US	9

void Timer2_Init(u32 arr,u16 psc);

#endif
