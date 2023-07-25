
/********************************************************************
*
*	文件	：	Switch.h

*	管脚	：	
				PB12--C_OUT_EN
				PC12--OC1
				PC13--OC2
				PC15--V_OUT_EN

*	功能	：	输出切换开关
				OC1与OC2输出真值表
							OC1低、OC2低	OC1高、OC2低	OC1低、OC2高	OC1高、OC2高
				电压刺激	无输出			反向输出		正向输出		正向输出
				电流刺激	正向输出		反向输出		正向输出		反向输出
*
******************************************************************************/
#ifndef __SWTICH_H
#define __SWITCH_H
#include "main.h"

//#define pC_OUT_EN		PBout(12)
//#define pOC1			PCout(12)
//#define pOC2			PCout(13)
//#define pV_OUT_EN		PCout(15)

#define SW_CV_PLOAR		PAout(15)    //正负极性控制
#define SW_CV_MODE      PBout(12)    //输出模式控制
#define SW_CV_OUTPUT    PFout(12)     //输出总开关


#define C_OUT_ENABLE	1
#define C_OUT_DISABLE	0
#define OC_H			1
#define OC_L			0
#define V_OUT_ENABLE	1
#define V_OUT_DISABLE	0

#define MODE_BIT_VC		0
#define MODE_BIT_FLIP	1			//LZG_WFC

#define SELECT_VC_C		0
#define SELECT_VC_V		1
#define SELECT_FLIP_UN	0
#define SELECT_FLIP_EN	1

__packed typedef struct
{
	uint8_t  ModePre;
	uint8_t  ModeCur;					//bit0--V/I		bit1--UnFlip/EnFlip		bit2--VL/VH
}sSwitch_t;
extern sSwitch_t Switch;

void Switch_Init(void);
void Switch_Ctrl(uint8_t phase);

#endif
