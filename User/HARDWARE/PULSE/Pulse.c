
#include  "main.h"

#if(PCB_VERSION >= PCB_V15)

/*	各中断相关寄存器规整	*/
volatile uint32_t *Tbl_TIM_CNT[8]						= {&(TIM11->CNT),		&(TIM4->CNT),		&(TIM3->CNT),		&(TIM9->CNT),		&(TIM2->CNT),			&(TIM8->CNT),		&(TIM1->CNT)};
volatile uint32_t *Tbl_TIM_ARR[8]  						= {&(TIM11->ARR),		&(TIM4->ARR),		&(TIM3->ARR),		&(TIM9->ARR),		&(TIM2->ARR),			&(TIM8->ARR),		&(TIM1->ARR)};
volatile uint16_t *Tbl_TIM_PSC[8]  						= {&(TIM11->PSC),		&(TIM4->PSC),		&(TIM3->PSC),		&(TIM9->PSC),		&(TIM2->PSC),			&(TIM8->PSC),		&(TIM1->PSC)};
volatile uint32_t *Tbl_TIM_CCRx[8] 						= {&(TIM11->CCR1),		&(TIM4->CCR3),		&(TIM3->CCR1),		&(TIM9->CCR1),		&(TIM2->CCR3),			&(TIM8->CCR1),		&(TIM1->CCR4)};
	
//volatile uint32_t *Tbl_TIM_CNT[8]						= {&(TIM11->CNT),		&(TIM4->CNT),		&(TIM3->CNT),		&(TIM9->CNT),		&(TIM2->CNT),			&(TIM8->CNT),		&(TIM1->CNT),		&(TIM5->CNT)};
//volatile uint32_t *Tbl_TIM_ARR[8]  						= {&(TIM11->ARR),		&(TIM4->ARR),		&(TIM3->ARR),		&(TIM9->ARR),		&(TIM2->ARR),			&(TIM8->ARR),		&(TIM1->ARR),		&(TIM5->ARR)};
//volatile uint16_t *Tbl_TIM_PSC[8]  						= {&(TIM11->PSC),		&(TIM4->PSC),		&(TIM3->PSC),		&(TIM9->PSC),		&(TIM2->PSC),			&(TIM8->PSC),		&(TIM1->PSC),		&(TIM5->PSC)};
//volatile uint32_t *Tbl_TIM_CCRx[8] 						= {&(TIM11->CCR1),		&(TIM4->CCR3),		&(TIM3->CCR1),		&(TIM9->CCR1),		&(TIM2->CCR3),			&(TIM8->CCR1),		&(TIM1->CCR4),		&(TIM5->CCR4)};
	
//volatile uint16_t *Tbl_TIM_DIER[8]						= {&(TIM11->DIER),		&(TIM4->DIER),		&(TIM3->DIER),		&(TIM9->DIER),		&(TIM2->DIER),			&(TIM8->DIER),		&(TIM1->DIER),		&(TIM5->DIER)};
//volatile uint16_t *Tbl_TIM_CR1[8]						= {&(TIM11->CR1),		&(TIM4->CR1),		&(TIM3->CR1),		&(TIM9->CR1),		&(TIM2->CR1),			&(TIM8->CR1),		&(TIM1->CR1),		&(TIM5->CR1)};
//volatile uint16_t *Tbl_TIM_SR[8]						= {&(TIM11->SR),		&(TIM4->SR),		&(TIM3->SR),		&(TIM9->SR),		&(TIM2->SR),			&(TIM8->SR),		&(TIM1->SR),		&(TIM5->SR)};

//void (*Tbl_DOX_TIM_Init[8])(uint16_t arr, uint16_t psc) = {&DO0_PB9_TIM11_CH1,	&DO1_PB8_TIM4_CH3,	&DO2_PA6_TIM3_CH1,	&DO3_PA2_TIM9_CH1,	&DO4_PB10_TIM2_CH3_F,	&DO5_PC6_TIM8_CH1,	&DO6_PA11_TIM1_CH4,	&DO7_PA3_TIM5_CH4};

sTimerRun_t 	   TimerRun[8] 	= {0};
//uint8_t     	   Tbl_Flag[8] 	= {0};				//状态标志位，通过停止按钮停止需要进行最后一个波形处理进行同步
sDOState_t         DOState     	= {0, {DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE, DOSTATE_STATUS_COMPLETE}};

unsigned char DOStatue = DOSTATE_STATUS_COMPLETE;
/********************************************************************
*	功能	：	中断调用函数，中断时间20us，采用计数方式，脉宽=20us * count
******************************************************************************/

//unsigned char TIM4_IRQ_Count=0;

//static void TIMx_IRQHandler_Call(uint8_t do_timx)
//{
//	if(*Tbl_TIM_SR[do_timx] & 0X0001)
//	{		
//		//log_info("TIM4_ARR=%d,TIM4_PSC=%d,pulsewidth=%dus\r\n",TIM4->ARR,TIM4->PSC,(TIM4->ARR+1)*(TIM4->PSC+1)/84);
//		
//		if(DOState.Status[do_timx] == DOSTATE_STATUS_RUNNING)	//运行状态
//		{
//			TimerRun[do_timx].PulseNum++;
//			
//			if(UserOperation.fMode == UO_MODE_SINGLE)		//单次模式
//			{
//				if(Wave_Type <= 1)	//单极性正电平或负电平
//				{
//					if( TimerRun[do_timx].PulseNum >= pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmNum  )
//					{
//						*Tbl_TIM_ARR[do_timx]  = SYNC_ARR;
//						*Tbl_TIM_PSC[do_timx]  = SYNC_PSC;
//						*Tbl_TIM_CCRx[do_timx] = SYNC_CCR;
//						TimerRun[do_timx].Index    = 0;
//						TimerRun[do_timx].PulseNum = 0;
//						TimerRun[do_timx].CycleCnt = 0;
//						DOState.Status[do_timx]    = DOSTATE_STATUS_COMPLETE;
//						Tbl_Flag[do_timx] = 1;
//						*Tbl_TIM_SR[do_timx] = 0;
//						return;	
//					}
//				}
//				else//双极性先正后负或双极性先负后正，需要输出两个脉冲波形（周期减半）
//				{
//					if( TimerRun[do_timx].PulseNum == pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmNum  );
//					
//					else if( TimerRun[do_timx].PulseNum >= 2*pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmNum  )
//					{
//						
//						*Tbl_TIM_ARR[do_timx]  = SYNC_ARR;
//						*Tbl_TIM_PSC[do_timx]  = SYNC_PSC;
//						*Tbl_TIM_CCRx[do_timx] = SYNC_CCR;
//						TimerRun[do_timx].Index    = 0;
//						TimerRun[do_timx].PulseNum = 0;
//						TimerRun[do_timx].CycleCnt = 0;
//						DOState.Status[do_timx]    = DOSTATE_STATUS_COMPLETE;
//						Tbl_Flag[do_timx] = 1;
//						*Tbl_TIM_SR[do_timx] = 0;
//						return;
//					}
//				}
//			}
//			
//			else if(UserOperation.fMode == UO_MODE_FREERUN)	//连续模式
//			{
//				if(Wave_Type==0)	//单极性正电平
//				{
//					
//				}
//				else if(Wave_Type == 1)//单极性负电平
//				{
//					
//				}
//				else if(Wave_Type == 2)//双极性先正后负
//				{
//					
//				}
//				else//双极性先负后正
//				{
//					
//				}
//			}
//			
//			else if(UserOperation.fMode == UO_MODE_TRAIN)	//按时长模式
//			{
//				if(Wave_Type==0)	//单极性正电平
//				{
//					
//				}
//				else if(Wave_Type == 1)//单极性负电平
//				{
//					
//				}
//				else if(Wave_Type == 2)//双极性先正后负
//				{
//					
//				}
//				else//双极性先负后正
//				{
//					
//				}
//			}
//			else if(UserOperation.fMode == UO_MODE_EXTBNC)	//外部触发模式
//			{
//				if(Wave_Type==0)	//单极性正电平
//				{
//					
//				}
//				else if(Wave_Type == 1)//单极性负电平
//				{
//					
//				}
//				else if(Wave_Type == 2)//双极性先正后负
//				{
//					
//				}
//				else//双极性先负后正
//				{
//					
//				}
//			}
//			
//			
//			
////			if((TimerRun[do_timx].PulseNum >= 2*pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmNum)  &&  (UserOperation.fMode != UO_MODE_FREERUN)  )
////			{
////				//TimerRun[do_timx].Index = (TimerRun[do_timx].Index + 1) % pPwmArrayParam[do_timx]->VarietyNum;
////				
////				//log_info("TIM4_count=%d\r\n",TimerRun[do_timx].PulseNum);
////				
////				if(TimerRun[do_timx].Index == 0)	//每次为0时标志完成一个循环
////				{
////					TimerRun[do_timx].CycleCnt++;
////					if(TimerRun[do_timx].CycleCnt >= pPwmArrayParam[do_timx]->CycleNum)
////					{
////						*Tbl_TIM_ARR[do_timx]  = SYNC_ARR;
////						*Tbl_TIM_PSC[do_timx]  = SYNC_PSC;
////						*Tbl_TIM_CCRx[do_timx] = SYNC_CCR;
////						TimerRun[do_timx].Index    = 0;
////						TimerRun[do_timx].PulseNum = 0;
////						TimerRun[do_timx].CycleCnt = 0;
////						DOState.Status[do_timx]    = DOSTATE_STATUS_COMPLETE;
////						Tbl_Flag[do_timx] = 1;
////						*Tbl_TIM_SR[do_timx] = 0;
////						return;
////					}
////				}
////				else if(TimerRun[do_timx].Index == 1 && UserOperation.bPhase == UO_PHASE_BIPHASE)	//如果是单相，此处无需处理；如果是双相，此处需要翻转为负相
////				{
////					Switch_Ctrl(UO_PHASE_BIPHASE);
////				}
////				else if(TimerRun[do_timx].Index == 2 && UserOperation.bPhase == UO_PHASE_BIPHASE)	//如果是单相，此处无需处理；如果是双相，此处需要翻转回正相
////				{
////					Switch_Ctrl(UO_PHASE_UNIPHASE);
////				}
////				
////				*Tbl_TIM_ARR[do_timx]  = pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmARR;
////				*Tbl_TIM_PSC[do_timx]  = pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmPSC;
////				*Tbl_TIM_CCRx[do_timx] = pPwmArrayParam[do_timx]->Pwm[TimerRun[do_timx].Index].PwmDuty;
////			
////				TimerRun[do_timx].PulseNum = 0;
////			}
//		}
//		else if(DOState.Status[do_timx] == DOSTATE_STATUS_COMPLETE)	//已经完成
//		{
//			if(Tbl_Flag[do_timx] == 0)				//停止按钮等非直接完成波形配置引起的输出结束需进行一个周期波形输出保持同步
//			{
//				*Tbl_TIM_ARR[do_timx]  = SYNC_ARR;
//				*Tbl_TIM_PSC[do_timx]  = SYNC_PSC;
//				*Tbl_TIM_CCRx[do_timx] = SYNC_CCR;

//				TimerRun[do_timx].Index    = 0;
//				TimerRun[do_timx].PulseNum = 0;
//				TimerRun[do_timx].CycleCnt = 0;
//				Tbl_Flag[do_timx]		   = 1;
//			}
//			else if(Tbl_Flag[do_timx] == 1)			//正常输出完成，初始化相关变量
//			{
//				*Tbl_TIM_CCRx[do_timx] 	 = 0;

//				TimerRun[do_timx].Index     = 0;
//				TimerRun[do_timx].PulseNum  = 0;
//				TimerRun[do_timx].CycleCnt  = 0;
//				
//				*Tbl_TIM_DIER[do_timx]   &= ~(1 << 0);
//				*Tbl_TIM_CR1[do_timx]    &= ~0X01;
//				Tbl_Flag[do_timx] 		  = 0;
//				
//				//SW_CV_PLOAR = 0 ;    //输出正极性
//				//log_info("SW_CV_OUTPUT = 0\r\n");
//				SW_CV_OUTPUT = 0;   //关闭输出
//				
//				//log_info("TIM4_ARR=%d,TIM4_PSC=%d,pulsewidth=%dus\r\n",TIM4->ARR,TIM4->PSC,(TIM4->ARR+1)*(TIM4->PSC+1)/84);
//				
//			}
//		}
//	}
//	*Tbl_TIM_SR[do_timx] = 0;
//}

/********************************************************************
*	功能	：	中断函数
******************************************************************************/
//void TIM4_IRQHandler(void)
//{	
//	TIMx_IRQHandler_Call(DO_TIM4);
//}

/********************************************************************
*	功能	：	初始化
******************************************************************************/
//void DO0_PB9_TIM11_CH1(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO1_PB8_TIM4_CH3(uint16_t arr, uint16_t psc)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	NVIC_InitTypeDef   NVIC_InitStructure;	
//	
//	
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能GPIOB时钟
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
//	
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//AF输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	
//	
//	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4);//PB8
//	
//	
////	if(UserOperation.bPhase == UO_PHASE_UNIPHASE)	
////	{
////		TIM4->ARR     = arr;		//装载值
////		TIM4->PSC     = psc;		//分频系数	84M/(psc+1)
////	}
////	else
////	{
////		TIM4->ARR     = arr/2;		//装载值
////		TIM4->PSC     = psc;		//分频系数	84M/(psc+1)
////	}
//	
//	TIM4->ARR     = arr;		//装载值
//	TIM4->PSC     = psc;		//分频系数	84M/(psc+1)
//	
//	TIM4->CCMR2  |= 6 << 4;		//输出比较模式3，与硬件电路有无反相器直接相关配置
//	TIM4->CCMR2  |= 1 << 3;		//输出比较3预装载使能
//	TIM4->CCER   |= 1 << 8;		//捕获/比较3输出使能
//	TIM4->CR1     = 0X0080;		//设置为单脉冲模式
//	
//	TIM4->DIER   &= ~(1 << 0);	//禁止更新中断
//	TIM4->CR1    &= ~0X01;		//禁止计数器
//	
//	
//	NVIC_InitStructure.NVIC_IRQChannel=TIM4_IRQn; //定时器3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//}

//void DO2_PA6_TIM3_CH1(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO3_PA2_TIM9_CH1(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO4_PB10_TIM2_CH3_F(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO5_PC6_TIM8_CH1(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO6_PA11_TIM1_CH4(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO7_PA3_TIM5_CH4(uint16_t arr, uint16_t psc)
//{
//	;
//}

//void DO_ALL_SYNC_Init(uint8_t do_timx)
//{
//	uint8_t i = 0;
//	
//	if(do_timx == DO_ALL)
//	{
//		for(i=0; i<8; i++)
//		{
//			(*Tbl_DOX_TIM_Init[i])(SYNC_ARR, SYNC_PSC);
//			*Tbl_TIM_CCRx[i] = SYNC_CCR;
//		}
//	}
//	else
//	{
//		(*Tbl_DOX_TIM_Init[do_timx])(SYNC_ARR, SYNC_PSC);
//		*Tbl_TIM_CCRx[do_timx] = SYNC_CCR;
//	}
//}

#endif
