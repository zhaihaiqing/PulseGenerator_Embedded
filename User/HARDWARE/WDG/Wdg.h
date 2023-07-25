
/********************************************************************
*
*	文件	：	Wdg.h

*	管脚	：	(NONE)

*	功能	：	看门狗
*
******************************************************************************/
#ifndef _WDG_H
#define _WDG_H
#include  "main.h"

void WDG_Init(u8 prer,u32 rlr);
void WDG_Feed(void);

//void PinEmpty_Handle(void);

#endif
