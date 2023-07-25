
#include  "main.h"

uint8_t Rotate_Increase_Cnt = 0, Rotate_Decrease_Cnt = 0;
/********************************************************************
*	功能	：	旋转编码器解码，即设定值

在收到PB13边沿中断时，根据PB14引脚电平来判定转动方向。

A信号与B信号组合可以判定转动方向：
	A下降沿时，B信号为高电平则为顺时针方向旋转
	
	A下降沿时，B信号为低电平则为逆时针方向旋转
******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & (1 << 13))	//检测到PB13中断
	{
		delay_us(100);
		if(PULSE_A != PULSE_B)
		{
			Rotate_Increase_Cnt++;
			if(Rotate_Decrease_Cnt)
			{
				Rotate_Decrease_Cnt--;
			}
			else{}
		}
		else
		{
			Rotate_Decrease_Cnt++;
			if(Rotate_Increase_Cnt)
			{
				Rotate_Increase_Cnt--;
			}
			else{}
		}
		
		if(T6.CodecRotateCnt == 0)
		{
			T6.CodecRotateCnt = 1;
		}
		else{}
		
		EXTI->PR |= 1 << 13;
	}
}

/********************************************************************
*	功能	：	初始化
******************************************************************************/
void Codec_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化

	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);//PB13 连接到中断线13
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);//PB14 连接到中断线14
	
	
	 /* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
  
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	
	
	 /* 配置EXTI_Line15 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line15;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
  
  
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x03;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
  
	
//	RCC->APB2ENR |= 1 << 3;
//	GPIOB->CRH &= 0XF00FFFFF;
//	GPIOB->CRH |= 0X08800000;
//	
//	GPIOB->ODR |= (1 << 13) | (1 << 14);
	
	//Ex_NVIC_Config(GPIO_B, 13, FTIR|RTIR);
	//MY_NVIC_Init(3, 3, EXTI15_10_IRQn, 2);
}
