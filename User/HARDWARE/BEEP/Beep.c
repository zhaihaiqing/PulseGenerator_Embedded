
#include  "main.h"

/*
		Beep_Ctrl	PA1
		Beep		PG15
*/


uint8_t bFlag_Beep = 0;
/********************************************************************
*	功能	：	初始化
******************************************************************************/
void Beep_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOA时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//使能GPIOG时钟
	

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	//GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
}

#define BEEP_CYCLE_TIME		1000	//ms
#define BEEP_DISABLE_TIME	600		//ms
/********************************************************************
*	功能	：	蜂鸣器轮询
*	说明	：	与需求功能不一致，故暂不使用该函数
******************************************************************************/
void Beep_Poll(void)
{
	if(bFlag_Beep == 1)
	{
		if(T6.BeepDecCnt == 0)
		{
			T6.BeepDecCnt = BEEP_CYCLE_TIME;
		}
		else{}		
	}
	else							//控制关闭鸣响
	{
		T6.BeepDecCnt = 0;
	}
	
	if(T6.BeepDecCnt % BEEP_CYCLE_TIME <= BEEP_DISABLE_TIME)
	{
		BEEP = BEEP_OFF;
	}
	else
	{
		BEEP = BEEP_ON;
	}
}
