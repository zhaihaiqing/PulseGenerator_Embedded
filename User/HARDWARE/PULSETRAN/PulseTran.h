
/********************************************************************
*
*	文件	：	PulseTran.h

*	管脚	：	(NONE)

*	功能	：	对接收到的数据进行解析、规整
				待后续规整单位及测试
*
******************************************************************************/
#ifndef _PULSETRAN_H
#define _PULSETRAN_H
#include "main.h"

#if(PCB_VERSION >= PCB_V15)

#define PULSE_FREQ_MAGNIFY 		  100.0f		// 频率基数，小数点后两位有效数字	freq = freq / 100.0		100 = 1hz
//#define PULSE_UNIT_DUTY   	 1000.0f		// 占空比基数						duty = duty / 1000.0	500 = 50%，old version
#define PULSE_DUTY_FULL			50000.0f		// 新版占空比协议					duty = duty / 50000  	50000 = 100%
#define PULSE_FREQ_LOWERLIMIT	    2.0f		// 频率有效范围【0.02Hz， 30KHz】，因PULSE_FREQ_MAGNIFY故此处为2

//#define PULSE_FREQ_5K	  		 500000
//#define PULSE_FREQ_10K  		1000000 		// 10khz  10 * 1000 * 100，当需要只输出低电平或高电平时用的频率基准
//#define PULSE_FREQ_20K  		2000000
//#define PULSE_FREQ_30K		3000000
#define PULSE_FREQ_50K			5000000
#define PULSE_FREQ_AUXILIARY	PULSE_FREQ_50K	//辅助频率，并非用户配置频率，而是根据用户配置参数辅助波形设置频率

__packed typedef struct
{
	uint32_t PwmNum;							//同一占空比波形连续个数
	uint16_t PwmARR;							//当前单个波形周期
	uint16_t PwmPSC;							//分频
	uint16_t PwmDuty;							//占空比
}sPwmParam_t;

__packed typedef struct
{
	uint32_t	Ampl;							//输出幅值
	
	uint32_t	CycleNum;						//循环次数
	uint32_t	VarietyNum;						//PWM种类数
	sPwmParam_t	*Pwm;							//单种PWM参数
}sPwmArrayParam_t;
extern sPwmArrayParam_t *pPwmArrayParam[8];

typedef enum
{
	RTN_NORMAL			= 0,					//返回正常
	RTN_ABNORMAL_AMPL	= 1,					//幅值（为0）异常
	RTN_ABNORMAL_FREQ	= 2,					//频率（为0）异常
	RTN_ABNORMAL_DUTY	= 3,					//占比（为0）异常
	RTN_ABNORMAL_NUM	= 4,					//个数（为0）异常
	RTN_ABNORMAL_SRAM	= 5,					//存储（为0）异常
	RTN_ABNORMAL_PARAM	= 6,					//参数（为0）异常
	RTN_ABNORMAL_OTHER	= 7,					//其它异常
}eRtn_t;

eRtn_t PulseTran_Zero(sPulseZero_t *zero, sPwmArrayParam_t *param);
eRtn_t PulseTran_One(sPulseOne_t *one, sPwmArrayParam_t *param);
eRtn_t PulseTran_Two(sPulseTwo_t *two, sPwmArrayParam_t *param);
eRtn_t PulseTran_Three(sPulseThree_t *three, sPwmArrayParam_t *param);
eRtn_t PulseTran_Four(sPulseFour_t *four, sPwmArrayParam_t *param);
eRtn_t PulseTran_Five(sPulseFive_t *five, sPwmArrayParam_t *param);

eRtn_t PulseTran_Single(sPwmArrayParam_t *param);
eRtn_t PulseTran_FreeRun(sPwmArrayParam_t *param);
eRtn_t PulseTran_Train(sPwmArrayParam_t *param);

#endif

#endif
