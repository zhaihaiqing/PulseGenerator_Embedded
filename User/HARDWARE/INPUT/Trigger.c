
#include  "main.h"



#define TRIGGER_EXTBNC_ACTIVE		0			//上升沿，高电平进行输出
#define TRIGGER_EXTBNE_INACTIVE		1			//下降沿，低电平停止输出。输入光耦反向，故下降沿时引脚电平为高

#define TRIGGER_PWM_ACTIVE			1
#define TRIGGER_PWM_INACTIVE		0
/********************************************************************
*  功能   ：中断处理
			后面板BNC外部中断可运行于EXTBNC模式，控制输出（屏幕幅值）波形
******************************************************************************/

static void trigger_delay_us(uint32_t times)
{
	uint32_t i=0;
	while(times--)
	{
		i=10;
		while(i--);
	}
}

//__IO unsigned int ex0_count=0;
__IO unsigned int ex_bnc_flag=0;
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		if(UserOperation.fMode == UO_MODE_EXTBNC  && ( (Wave_type==0) || (Wave_type ==1) ) )
		{	
			
			if(UserOperation.bVC == SELECT_VC_V)
			{
				pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL];
				
			}
			else if(UserOperation.bVC == SELECT_VC_C)
			{
				pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL];
			}
			
			trigger_delay_us(2);
			
			if(pTRIGGER_IN == 1)//上升沿
			{
				//log_info("123,%d,%d %d\r\n",UserOperation.fMode,Wave_type,pPwmArrayParam[DO_TIM4]->Ampl);
				
				pLEDOUTPUT = LED_DIRECTLY_ON;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
				BNCMode_Reflash_LCD_Status = 1;
				TIM3_ENABLE();
				
				if(Wave_type==0)
				{
					Output_VorC(UserOperation.bVC, pPwmArrayParam[DO_TIM4]->Ampl, OUTPUT_ENABLE);
				}
				else
				{
					Output_VorC(UserOperation.bVC, (0-pPwmArrayParam[DO_TIM4]->Ampl), OUTPUT_ENABLE);
				}
				pTRIGGER_OUT = 1;
				//log_info("123\r\n");
				
			}
			else if(pTRIGGER_IN == 0)//下降沿
			{
				//log_info("456,%d,%d %d\r\n",UserOperation.fMode,Wave_type,pPwmArrayParam[DO_TIM4]->Ampl);
				
				Output_VorC(UserOperation.bVC, 0, OUTPUT_ENABLE);
				pTRIGGER_OUT = 0;
				
				BNCMode_Reflash_LCD_Status = 0;
			}
			
		}
		
		
		else if(UserOperation.fMode == UO_MODE_EXTBNC  && ( (Wave_type==2) || (Wave_type ==3) ) )
		{
			trigger_delay_us(5);
			
			if(pTRIGGER_IN == 1)
			{
				uint32_t t5_arr=0,pulse=0;
				
				if(UserOperation.bVC == SELECT_VC_V)
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL];
					pulse = UserOperation.V_ModeExtBnc.Param[UO_PARAM_PULSE];
				}
				else if(UserOperation.bVC == SELECT_VC_C)
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL];
					pulse = UserOperation.C_ModeExtBnc.Param[UO_PARAM_PULSE];
				}
				
				//log_info("123,%d,%d %d\r\n",UserOperation.fMode,Wave_type,pPwmArrayParam[DO_TIM4]->Ampl);
				
				if(pulse>0)
				{
					ex_bnc_flag=1;
					
					t5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;
					t5_arr = t5_arr+TIM5_ARR_Compensate2;				
					
					pLEDOUTPUT = LED_DIRECTLY_ON;
					DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
					BNCMode_Reflash_LCD_Status = 1;
					TIM3_ENABLE();
					
					Enable_Timer5(t5_arr);
				}
			}
		}
			
		EXTI_ClearITPendingBit(EXTI_Line0);
			
		}
}



#if(PCB_VERSION >= PCB_V15)
void UO_Update(uint8_t updatecmd)
{
	INTX_DISABLE();								//关闭中断，涉及PWM中断相关参数
										
	switch(UserOperation.fMode)
	{
		case UO_MODE_SINGLE:
			
			if(updatecmd == UPDATE_ALL)
			{				
				if(PulseTran_Single(pPwmArrayParam[DO_TIM4]) == RTN_NORMAL)		//计算波形参数
				{
					DOState.Config |= 1 << DO_TIM4;
				}
				else
				{
					DOState.Config &= ~(1 << DO_TIM4);
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
					}
					else{}
				}
			}
			else if(updatecmd == UPDATE_AMPL)
			{
				if (UserOperation.bVC == SELECT_VC_V)
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL];
				}
				else
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL];
				}
				
				if(pPwmArrayParam[DO_TIM4]->Ampl == 0)									//[V188],修复bug，此类应为幅值为0时进行停止输出处理
				{
					DOState.Config &= ~(1 << DO_TIM4);									//[V189],修复bug，幅值为0时先置零配置标志位
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
					}
					else{}
				}
				else{}
			}
			else{}
		
			break;
		
		case UO_MODE_FREERUN:
			
			if(updatecmd == UPDATE_ALL)
			{
				if(PulseTran_FreeRun(pPwmArrayParam[DO_TIM4]) == RTN_NORMAL)
				{
                    DOState.Config |= 1 << DO_TIM4;
				}
				else
				{
					DOState.Config &= ~(1 << DO_TIM4);
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
					}
					else{}					
				}				
			}
			else if(updatecmd == UPDATE_AMPL)
			{
				if (UserOperation.bVC == SELECT_VC_V)
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL];
				}
				else
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL];
				}
				
				if(pPwmArrayParam[DO_TIM4]->Ampl == 0)
				{
					DOState.Config &= ~(1 << DO_TIM4);
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
					}
					else{}					
				}
				else{}
			}
			else{}
			
		
			break;
		
		case UO_MODE_TRAIN:
			
			if(updatecmd == UPDATE_ALL)
			{
				if(PulseTran_Train(pPwmArrayParam[DO_TIM4]) == RTN_NORMAL)
				{
					DOState.Config |= 1 << DO_TIM4;
				}
				else
				{
					DOState.Config &= ~(1 << DO_TIM4);
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();						
					}
					else{}					
				}				
			}
			else if(updatecmd == UPDATE_AMPL)
			{
				if (UserOperation.bVC == SELECT_VC_V)
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL];
				}
				else
				{
					pPwmArrayParam[DO_TIM4]->Ampl = UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL];
				}
				
				if(pPwmArrayParam[DO_TIM4]->Ampl == 0)
				{
					DOState.Config &= ~(1 << DO_TIM4);
					
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
					}
					else{}
				}
				else{}
			}
			else{}
			
		
			break;
		
		case UO_MODE_COMPUTER:					//直接在Protocol中进行了更新等解析，此处无需处理
			
			switch(PulseConf.PulseType)
			{
				case PULSE_TYPE_ZERO:
					
					break;
				
				case PULSE_TYPE_ONE:								
					
					break;
				
				case PULSE_TYPE_TWO:
					
					break;
				
				case PULSE_TYPE_THREE:
					
					break;
				
				case PULSE_TYPE_FOUR:
					
					break;
				
				case PULSE_TYPE_FIVE:
					
					break;
				
				default:
					
					break;
			}
			
			break;
			
		default:
			
			break;
	}
	
	if(updatecmd == UPDATE_ALL)																				//[V188],修复更新存在逻辑漏洞，初始化参数不全面的bug
	{
		*Tbl_TIM_ARR[DO_TIM4]		= pPwmArrayParam[DO_TIM4]->Pwm[TimerRun[DO_TIM4].Index].PwmARR;
		*Tbl_TIM_PSC[DO_TIM4]		= pPwmArrayParam[DO_TIM4]->Pwm[TimerRun[DO_TIM4].Index].PwmPSC;
		*Tbl_TIM_CCRx[DO_TIM4]		= pPwmArrayParam[DO_TIM4]->Pwm[TimerRun[DO_TIM4].Index].PwmDuty;		
		TimerRun[DO_TIM4].Index		= 0;
		TimerRun[DO_TIM4].PulseNum	= 0;
		TimerRun[DO_TIM4].CycleCnt	= 0;
	}
	else{}
	
	INTX_ENABLE();								//打开中断
}


#endif

/********************************************************************
*  功能   ：初始化
******************************************************************************/
void TriggerExIN01_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//PE2 连接到中断线2
	
	/* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //上升沿触发一次 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	//log_info("TriggerExIN01_Init\r\n");
}

/********************************************************************
*  功能   ：初始化
******************************************************************************/
void TriggerExIN23_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//PE2 连接到中断线2
	
	
	/* 配置EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发一次 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
	EXTI_Init(&EXTI_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);//配置
	
	//log_info("TriggerExIN23_Init\r\n");
}

void TriggerOUT_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	pTRIGGER_OUT = 0;
}



/********************************************************************
*  功能   ：初始化
******************************************************************************/
//void TriggerEx_Init(void)
//{
//	GPIO_InitTypeDef  GPIO_InitStructure;
//	NVIC_InitTypeDef   NVIC_InitStructure;
//	EXTI_InitTypeDef   EXTI_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
//	

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
////	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
//	
//	pTRIGGER_OUT = 0;
//	
//	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);//PE2 连接到中断线2
//	
//	
//	/* 配置EXTI_Line0 */
//	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //上升沿触发一次 
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE0
//	EXTI_Init(&EXTI_InitStructure);//配置
//	
//	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部中断0
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure);//配置
//}
