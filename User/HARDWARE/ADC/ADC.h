
/********************************************************************
*
*	文件	：	ADC.h

*	管脚	：	
				PA0 --ADC_HV
				PA1 --ADC_LV

*	功能	：	电压采集
*
******************************************************************************/
#ifndef __ADC_H
#define __ADC_H
#include "HeaderFiles.h"

#define ADC_INTERVAL	(5 * T_VALUE_MILLISEC)

#define ADC_CHNL_VH		0
#define ADC_CHNL_VL		1

extern float Volt_High, Volt_Low;

void ADC_Init(void);
void ADC_Poll(void);

#endif
