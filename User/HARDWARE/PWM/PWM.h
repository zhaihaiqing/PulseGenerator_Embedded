
/********************************************************************
*
*	文件	：	PWM.h

*	管脚	：	
				PB8[TIM4 CH3]

*	功能	：	脉宽调制
*
******************************************************************************/
#ifndef __PWM_H
#define __PWM_H
#include "HeaderFiles.h"

void Pwm_Init(void);
void Pwm_WidthSet(uint16_t permillage);

#endif
