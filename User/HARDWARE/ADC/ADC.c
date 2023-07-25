
#include  "HeaderFiles.h"

float Volt_High = 0, Volt_Low = 0;

/********************************************************************
*	功能	：	初始化
******************************************************************************/
void ADC_Init(void)
{
	RCC->APB2ENR  |= 1<<2;	
	GPIOA->CRL    &= 0XFFFFFF00;
	
	RCC->APB2ENR  |= 1<<9;
	RCC->APB2RSTR |= 1<<9;
	RCC->APB2RSTR &= ~(1<<9);
	RCC->CFGR &= ~(3<<14);
	RCC->CFGR |= 2<<14;
	ADC1->CR1 &= 0XF0FFFF;
	ADC1->CR1 |= 0<<16;
	ADC1->CR1 &= ~(1<<8);
	ADC1->CR2 &= ~(1<<1);
	ADC1->CR2 &= ~(7<<17);
	ADC1->CR2 |= 7<<17;
	ADC1->CR2 |= 1<<20;
	ADC1->CR2 &= ~(1<<11);
	ADC1->SQR1  &= ~(0XF<<20);
	ADC1->SQR1  &= 0<<20;
	ADC1->SMPR2 &= ~(7<<18);
	ADC1->SMPR2 |= 7<<18;
	ADC1->SMPR2 &= ~(7<<21);
	ADC1->SMPR2 |= 7<<21;
	ADC1->CR2 |= 1<<0;				  
	ADC1->CR2 |= 1<<3;
	while(ADC1->CR2 & 1<<3);
	ADC1->CR2 |= 1<<2;
	while(ADC1->CR2 & 1<<2);
}

/********************************************************************
*  功能   ：获取ch通道ADC值
******************************************************************************/
static uint16_t ADC_Get(uint8_t ch)
{
	ADC1->SQR3 &= 0XFFFFFFE0;
	ADC1->SQR3 |= ch;
	ADC1->CR2  |= 1 << 22;
	while(!(ADC1->SR & 1 << 1));	   
	return ADC1->DR;
}

/********************************************************************
*  功能   ：获取ch通道多次采集平均ADC值[0, 4095]
******************************************************************************/
static void ADC_Value(uint8_t times)
{
	uint8_t  step = 0, count = 0;
	uint32_t sum = 0;
	
	switch(step)
	{
		case 0:																										//初始化
			count = 0;
			sum   = 0;
			step  = 1;
			if(T6.AdcGetCnt == 0)
			{				
				T6.AdcGetCnt = 1;
			}
			break;
		case 1:																										//溢流值获取
			if(T6.AdcGetCnt >= ADC_INTERVAL)
			{
				sum += ADC_Get(ADC_CHNL_VH);
				count++;
				if(count >= times)
				{
					Volt_High = sum / count * 3.3 * 10 / 4096;														//10倍值，便于处理，溢流检测，临界值3.3 / 2 * 10 ≈17V
					count = 0;					
					sum   = 0;
					step  = 2;
				}
				T6.AdcGetCnt = 1;
			}
			break;
		case 2:																										//电压值获取
			if(T6.AdcGetCnt >= ADC_INTERVAL)
			{
				sum += ADC_Get(ADC_CHNL_VL);
				count++;
				if(count >= times)
				{
					Volt_Low = sum / count * 3.3 * 10 / 4096 * 6;													//10倍值，便于处理，当前电源电压，10K与2K电阻分压采集
					count = 0;					
					sum   = 0;
					step  = 1;
				}
				T6.AdcGetCnt = 1;
			}
			break;
	}
}

/********************************************************************
*  功能   ：主函数轮询函数
******************************************************************************/
void ADC_Poll(void)
{
	ADC_Value(8);
}
