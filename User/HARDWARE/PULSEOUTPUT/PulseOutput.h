
/********************************************************************
*
*	文件	：	PulseOutput.h

*	管脚	：	(NONE)

*	功能	：	特殊波形输出处理，中断中被调用
				包括除SINGLE TRAIN FREERUN EXTBNC四种波形以外波形输出处理
				待后续规整单位及测试
*
******************************************************************************/
#ifndef _PULSEOUTPUT_H
#define _PULSEOUTPUT_H
#include "HeaderFiles.h"

void Process_PULSE_TYPE_ZERO(void);
void Process_PULSE_TYPE_ONE(void);
void Process_PULSE_TYPE_TWO(void);
void Process_PULSE_TYPE_THREE(void);
void Process_PULSE_TYPE_FOUR(void);

#endif
