
#include  "main.h"

/********************************************************************

******************************************************************************/
void WDG_Init(u8 prer,u32 rlr)
{	
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//使能预分频寄存器和重装载寄存器
	IWDG_SetPrescaler(prer);						//设置分频值
	IWDG_SetReload(rlr);							//设置重装载值   Tout=((4X2^prer) X rlr)/40   ms
	IWDG_ReloadCounter();							//重装载初值
	IWDG_Enable();									//使能看门狗
}

/********************************************************************
*  功能   ：喂狗
******************************************************************************/
void WDG_Feed(void)
{
	IWDG_ReloadCounter();
}


