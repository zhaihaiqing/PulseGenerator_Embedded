
#include "main.h"


#if(TEST_LED)
/********************************************************************
*	功能	：	测试函数
******************************************************************************/
	void test_LED(void)
	{
		pLEDBD = LED_DIRECTLY_ON;
		Delay_ms(500);
		pLEDBD = LED_DIRECTLY_OFF;
		Delay_ms(500);
	}
	
/********************************************************************
*	功能	：	测试函数
*	说明	：	定时器中调用，否则可能导致主函数和定时器不协调，停止某一状态
******************************************************************************/	
	void test_T_LED(void)
	{
		if(T6.LedCtrl == 0)
		{
			T6.LedCtrl = 1000;
		}
		else{}
		
		if(T6.LedCtrl >= 500)
		{
			pLEDBD = LED_DIRECTLY_ON;
		}
		else
		{
			pLEDBD = LED_DIRECTLY_OFF;
		}
	}
#else
/********************************************************************
*	功能	：	检测控制方式
******************************************************************************/
	void CtrlMode_Poll(void)
	{
		static uint8_t ctrlmode_pre = 0;
		if(pCTRLMODE != ctrlmode_pre)
		{
			switch(pCTRLMODE)
			{
				case CTRLMODE_MANNUAL:
					
					if(UserOperation.bVC == SELECT_VC_V)
					{
						UserOperation.fMode = UserOperation.fMode_VPre;
					}
					else if(UserOperation.bVC == SELECT_VC_C)
					{
						UserOperation.fMode = UserOperation.fMode_CPre;
					}
					
					//LZG_WFC,切换显示界面等处理
					
					break;
				
				case CTRLMODE_COMPUTER:
					
					UserOperation.fMode = UO_MODE_COMPUTER;
				
					//LZG_WFC,切换显示界面等处理
				
					break;
				
				default:
					break;
			}
			ctrlmode_pre = pCTRLMODE;
		}
	}
#endif

/********************************************************************
*	功能	：	LED端口初始化
******************************************************************************/
void LED_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOD时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIOE时钟

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化
	

	
	GPIO_SetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12);//设置高，灯灭
	GPIO_SetBits(GPIOB, GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9);//设置高，灯灭
	GPIO_SetBits(GPIOD, GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_12 | GPIO_Pin_13);//设置高，灯灭
	GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4);//设置高，灯灭
	

	pLEDPOWER = LED_DIRECTLY_ON;
}

/********************************************************************
*	功能	：	模式部分LED关闭
******************************************************************************/
void Led_ModePartOff(void)
{
	pLEDSINGLE	= LED_SN74HC240_OFF;
	pLEDFREERUN	= LED_SN74HC240_OFF;
	pLEDTRAIN	= LED_SN74HC240_OFF;
	pLEDEXTBNC	= LED_SN74HC240_OFF;
}

/********************************************************************
*	功能	：	参数部分LED关闭
******************************************************************************/
void Led_ParamPartOff(void)
{
	pLEDAMPL		= LED_SN74HC240_OFF;
	pLEDPULSE		= LED_SN74HC240_OFF;
	pLEDFREQUENCY	= LED_SN74HC240_OFF;
	pLEDDURATION	= LED_SN74HC240_OFF;
}

s_LedShortOn	LedShortOn = {0};
/********************************************************************
*	功能	：	LED短亮部分轮询
******************************************************************************/
void Led_ShortOn_Poll(void)
{
	if(LedShortOn.fSinggleTrigger == LEDSHORTON_BEGIN)
	{
		pLEDSINGLETRIGGER                 = LED_SN74HC240_ON;
		LedShortOn.SingleTriggerCountDown = LEDSHORTONTIME;
		LedShortOn.fSinggleTrigger        = LEDSHORTON_ING;
	}
	else if(LedShortOn.fSinggleTrigger == LEDSHORTON_ING)
	{
		if(LedShortOn.SingleTriggerCountDown == 0)
		{
			pLEDSINGLETRIGGER          = LED_SN74HC240_OFF;
			LedShortOn.fSinggleTrigger = LEDSHORTON_END;
		}
		else{}
	}
	else{}
	
	if(LedShortOn.fVC == LEDSHORTON_BEGIN)
	{
		//pLEDVC                 = LED_SN74HC240_ON;
		LedShortOn.VCCountDown = LEDSHORTONTIME;
		LedShortOn.fVC         = LEDSHORTON_ING;
	}
	else if(LedShortOn.fVC == LEDSHORTON_ING)
	{
		if(LedShortOn.VCCountDown == 0)
		{
			//pLEDVC         = LED_SN74HC240_OFF;
			LedShortOn.fVC = LEDSHORTON_END;
		}
		else{}
	}
	else{}
	
	if(LedShortOn.fUnitSwitch == LEDSHORTON_BEGIN)
	{
		pLEDUNITSWITCH                 = LED_SN74HC240_ON;
		LedShortOn.UnitSwitchCountDown = LEDSHORTONTIME;
		LedShortOn.fUnitSwitch         = LEDSHORTON_ING;
	}
	else if(LedShortOn.fUnitSwitch == LEDSHORTON_ING)
	{
		if(LedShortOn.UnitSwitchCountDown == 0)
		{
			pLEDUNITSWITCH         = LED_SN74HC240_OFF;
			LedShortOn.fUnitSwitch = LEDSHORTON_END;
		}
		else{}
	}
	else{}
}
