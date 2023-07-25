
/********************************************************************
*
*	文件	：	Beep.h

*	管脚	：	
				PB5

*	功能	：	蜂鸣器
*
******************************************************************************/
#ifndef __BEEP_H
#define __BEEP_H
#include "main.h"

extern uint8_t bFlag_Beep;		//过流报警

//#define BEEPCTRL	PAin(1)
#define BEEP 		PBout(5)

#define BEEPCTRL_EN	0
#define BEEPCTRL_UN	1
#define BEEP_ON		1
#define BEEP_OFF	0

void Beep_Init(void);
void Beep_Poll(void);

#endif
