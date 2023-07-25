
#include  "main.h"

sTimer6_t T6 = {0}; 

/********************************************************************
*	功能	：	编码器处理
******************************************************************************/
static void T_Codec_Process(void)
{
	if(T6.CodecRotateCnt > 0 && T6.CodecRotateCnt < T6_VALUE_SECOND)
	{
		T6.CodecRotateCnt++;
		if(T6.CodecRotateCnt > ROTATE_SPEED_BOUNDARY_MS * T6_VALUE_MILLISEC)
		{
			if(Rotate_Increase_Cnt)
			{
				if(Rotate_Increase_Cnt >= ROTATE_BOUNDARY_TIMES)
				{
					SettingInfo_Modify(KBD_INPUT_CODEC_INC_QUICK);
				}
				else
				{
					SettingInfo_Modify(KBD_INPUT_CODEC_INC_SLOW);
				}
				Rotate_Increase_Cnt = 0;
				T6.CodecRotateCnt   = 0;
			}
			else{}
			if(Rotate_Decrease_Cnt)
			{
				if(Rotate_Decrease_Cnt >= ROTATE_BOUNDARY_TIMES)
				{
					SettingInfo_Modify(KBD_INPUT_CODEC_DEC_QUICK);
				}
				else
				{
					SettingInfo_Modify(KBD_INPUT_CODEC_DEC_SLOW);
				}
				Rotate_Decrease_Cnt = 0;
				T6.CodecRotateCnt   = 0;
			}
			else{}
		}
		else{}
	}
	else{}
}

/********************************************************************
*	功能	：	自减函数
******************************************************************************/
static void T_SelfSubtraction(uint16_t *param)
{
	if(*param > 0)
	{
		(*param)--;
	}
	else{}
}

/********************************************************************
*	功能	：	自加函数
******************************************************************************/
static void T_SelfAddition(uint16_t *param, uint16_t upperlimit)
{
	if(*param > 0 && *param < upperlimit)
	{
		(*param)++;
	}
	else{}
}

/********************************************************************
*  功能   ：每1ms检测执行一次
******************************************************************************/
static void Timer_Execute_Per1ms(void)
{
	T_Codec_Process();

	#if(TEST_LED)
		T_SelfSubtraction(&T6.LedCtrl);
		//test_T_LED();
	#endif

	T_SelfSubtraction(&T6.BeepDecCnt);
	T_SelfSubtraction(&LedShortOn.SingleTriggerCountDown);
	T_SelfSubtraction(&LedShortOn.VCCountDown);
	T_SelfSubtraction(&LedShortOn.UnitSwitchCountDown);
	T_SelfAddition(&T6.MemoryUpdateCnt, T6_VALUE_MINUTE+10);

	T6.RunCnt = (T6.RunCnt < T6_VALUE_MINUTE * 60 * 24) ? (T6.RunCnt + 1) : 0;
}

/********************************************************************
*  功能   ：每10ms检测执行一次
******************************************************************************/
static void Timer_Execute_Per10ms(void)
{
	bsp_KeyScan();													//按键轮询
}

/********************************************************************
*	功能	：定时器中断服务程序，2ms进入一次中断
******************************************************************************/
void TIM6_DAC_IRQHandler(void)
{
	if(TIM6->SR&0X0001)												//TIM6溢出中断
	{
		
		//GPIOG->ODR ^= GPIO_Pin_10;
		
		Timer_Execute_Per1ms();									//
		
		if(++T6.Scan10msCnt >= 10 * T6_VALUE_MILLISEC)
		{
			Timer_Execute_Per10ms();								//按键轮询代码
			T6.Scan10msCnt = 0;
		}
		else{}
		
		TIM6->SR&=~(1<<0);											//清除TIM6中断标志位
	}
	else{}
}

/********************************************************************
*	功能	：	初始化
*	周期	：	Tout = ((arr + 1)*(psc + 1))/Tclk,
				其中Tclk = 72MHz，Tout=(arr+1)(psc+1)/72 (us)
*	举例	：	TimerX_Init(719, 1);     //72MHZ,500KHZ溢出频率,20us
******************************************************************************/
void Timer6_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  ///使能TIM6时钟
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM6,ENABLE); //使能定时器3
	
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



