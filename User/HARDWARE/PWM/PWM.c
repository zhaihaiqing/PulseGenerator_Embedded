
#include  "HeaderFiles.h"

#define PWM_ARR		10000
#define PWM_PSC		720
/********************************************************************
*	功能	：	初始化PWM
*	参数	：	频率 = 72*1000000/((arr+1)*(psc+1))
*				psc--预分频值，决定波形周期
				arr--自动重装载值，一个完整波形计数数量值
******************************************************************************/
void Pwm_Init(void)
{	
	RCC->APB2ENR |= 1 << 3;
	RCC->APB1ENR |= 1 << 2;
	GPIOB->CRH   &= 0XFFFFFFF0;
	GPIOB->CRH   |= 0X0000000B;
	TIM4->ARR     = PWM_ARR-1;		//自动重装载值,ARR
	TIM4->PSC     = PWM_PSC-1;		//预分频值，PSC
	
	TIM4->CCMR2  |= 6 << 4;			//TIM3 CH3,向上计数时，TIMx_CNT<TIMx_CNT_CCRx通道是活动的
	TIM4->CCMR2  |= 1 << 3;
	TIM4->CCER   |= 1 << 8;			//OC3 ENABLE
	TIM4->CR1    |= 0X0080;
	TIM4->CR1    |= 0X01;
	
	TIM4->CCR3    = PWM_ARR;				//初始一个值
}

/********************************************************************
*	功能	：	输出高电平所占千分比
******************************************************************************/
void Pwm_WidthSet(uint16_t permillage)
{
	TIM4->CCR3 = permillage * (PWM_ARR / 1000);
}

