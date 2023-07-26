
#include "main.h"

#define STATUSBAR_CONFIG	(1 << 0)
#define STATUSBAR_RUN		(1 << 1)
#define STATUSBAR_COMPLETE	(1 << 2)
#define STATUSBAR_PAUSE		(1 << 3)
#define STATUSBAR_CONTINUE	(1 << 7)
static uint8_t  statusbar	= 0;																//程序运行进度

/********************************************************************
*	功能	：	运行状态处理函数
******************************************************************************/
static void StatusBar_Progress(uint8_t status)
{
	if(status == STATUSBAR_CONFIG)
	{
		statusbar |= STATUSBAR_CONFIG;
	}
	else if(status == STATUSBAR_RUN)
	{
		statusbar &= ~(STATUSBAR_PAUSE | STATUSBAR_COMPLETE);
		statusbar |= STATUSBAR_RUN;
	}
	else if(status == STATUSBAR_PAUSE)
	{
		statusbar &= ~STATUSBAR_RUN;
		statusbar |= STATUSBAR_PAUSE;
	}
	else if(status == STATUSBAR_CONTINUE)
	{
		statusbar &= ~STATUSBAR_PAUSE;
		statusbar |= STATUSBAR_RUN;
	}
	else if(status == STATUSBAR_COMPLETE)
	{
		statusbar &= ~(STATUSBAR_RUN | STATUSBAR_PAUSE);
		statusbar |= STATUSBAR_COMPLETE;
	}
	else{}
}

//static uint32_t temp_runcnt = 0;																//运行时间暂存
//static uint16_t Tbl_Temp_CCRx[8] = {0};
//static uint16_t Tbl_Temp_CNT[8]  = {0};

__IO uint32_t tim5_arr=0;
__IO uint32_t train_count=0;	//TRAIN模式计数器，
__IO uint32_t train_acount=0;	//TRAIN模式需要计数的总数，例如：10S，Freq=10Hz，则总数=10*10=100个，注意，末尾不满1的舍去
__IO uint32_t pluse_Compensate=0;
/********************************************************************
*	功能	：	命令处理函数
******************************************************************************/
uint8_t  Process_COMMAND_START(void)
{
	if(UserOperation.fMode == UO_MODE_SINGLE)	//单次模式
	{
		
		uint64_t pulse=0;
		
		//计算tim5_arr值
		if(UserOperation.bVC == SELECT_VC_V)
		{
			pulse = UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE];	//单位为ns
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.2ms,则执行
			else{}
		}
		else if(UserOperation.bVC == SELECT_VC_C)
		{
			pulse = UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE];	//单位为ns
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.2ms,则执行
			else{}
		}
		else{}
		
		if( ( (Wave_type == 0) || (Wave_type == 1)) && (pulse < (PULSE_LOWER_LIMIT/2) ) ) return 1;
		if( ( (Wave_type == 2) || (Wave_type == 3)) && (pulse < (PULSE_LOWER_LIMIT) ) ) return 1;
			
			
		if((Wave_type == 0) || (Wave_type == 1))
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;				//计算tim5的装填值，tim5控制周期,(TIM2分频值为21，4M时钟，每记一次数，时间为0.25us，即250ns)
			tim5_arr = tim5_arr+TIM5_ARR_Compensate;				//单极性波形，补偿值
		}
		else
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;
			tim5_arr = tim5_arr+TIM5_ARR_Compensate2;
		}
		
		SW_CV_OUTPUT = 1;   //打开输出
		pLEDOUTPUT = LED_DIRECTLY_ON;
		Delay_ms(15);		//等待继电器闭合
		log_info("SINGLETRIGGER Mode,SW_CV_OUTPUT = 1,Wave_type=%d,tim5_arr=%d\r\n",Wave_type,tim5_arr);
		
		DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
		Enable_Timer5(tim5_arr);
	}
	
	else if(UserOperation.fMode == UO_MODE_FREERUN)				//连续模式下，频率范围1-40KHz，对应周期为1000ms-33.333us，
	{
		uint64_t pulse=0,freq=0;
		
		if(UserOperation.bVC == SELECT_VC_V)
		{
			pulse = UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE];	//单位为ns
			freq = UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ];
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.04ms,则执行
		}
		else if(UserOperation.bVC == SELECT_VC_C)
		{
			pulse = UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE];	//单位为ns
			freq = UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ];
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.2ms,则执行
		}
		
			
		if( ( (Wave_type == 0) || (Wave_type == 1)) && (pulse < (PULSE_LOWER_LIMIT/2) ) ) return 1;
		if( ( (Wave_type == 2) || (Wave_type == 3)) && (pulse < (PULSE_LOWER_LIMIT) ) ) return 1;
		if(freq < FREQ_LOWER_LIMIT) return 1;
		
		if((Wave_type == 0) || (Wave_type == 1))
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;				//计算tim5的装填值，tim5控制周期,(TIM2分频值为21，4M时钟，每记一次数，时间为0.25us，即250ns)
			tim5_arr = tim5_arr+TIM5_ARR_Compensate;				//单极性波形，补偿值
		}
		else
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;
			tim5_arr = tim5_arr+TIM5_ARR_Compensate2;
		}
		log_info("pulse=%lldns,freq=%lldmhz,tim5_arr=%d\r\n",pulse,freq,tim5_arr);
		
		//计算定时器2的周期
		{
			uint32_t tim2_arr = 0,tim2_psc = 0;
			double T = 0;
			tim2_psc = TIM2_FREQ_DIV;
			
			freq = freq/1000;
			
			T = 1000000000000.0/freq;	//ns，计算周期
			tim2_arr = T/TIM2_SINGLE_COUNT_TIME-1;			//计算tim2的装填值，tim2控制周期,(TIM2分频值为42，2M时钟，没记一次数，时间为0.5us，即500ns)
			log_info("T=%lf,tim2_arr=%d\r\n",T,tim2_arr);
			
			if(tim5_arr>tim2_arr)
			{
				tim5_arr = tim2_arr;
			}
			
			SW_CV_OUTPUT = 1;   //打开输出
			pLEDOUTPUT = LED_DIRECTLY_ON;
			Delay_ms(15);		//等待继电器闭合
			
			Timer2_Init(tim2_arr,tim2_psc);
			DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
			TIM_Cmd(TIM2,ENABLE);
		}		
	}
	else if(UserOperation.fMode == UO_MODE_TRAIN)
	{
		uint64_t pulse=0,freq=0,duration=0;
		
		if(UserOperation.bVC == SELECT_VC_V)
		{
			pulse 	 = UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE];	//单位为ns
			freq 	 = UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ];
			duration = UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION];
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.2ms,则执行
			else{}
		}
		else if(UserOperation.bVC == SELECT_VC_C)
		{
			pulse 	 = UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE];	//单位为ns
			freq 	 = UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ];
			duration = UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION];//单位us
			if(pulse < 200000)pluse_Compensate = pulse;	//如果小于0.2ms,则执行
			else{}
		}
		else{}
		
		if( ( (Wave_type == 0) || (Wave_type == 1)) && (pulse < (PULSE_LOWER_LIMIT/2) ) ) return 1;
		if( ( (Wave_type == 2) || (Wave_type == 3)) && (pulse < (PULSE_LOWER_LIMIT) ) ) return 1;
		if(freq < FREQ_LOWER_LIMIT) return 1;
		if(duration < DURATION_LOWER_LIMIT) return 1;
			
			
		if((Wave_type == 0) || (Wave_type == 1))
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;				//计算tim5的装填值，tim5控制周期,(TIM2分频值为21，4M时钟，每记一次数，时间为0.25us，即250ns)
			tim5_arr = tim5_arr+TIM5_ARR_Compensate;				//单极性波形，补偿值
		}
		else
		{
			tim5_arr = pulse/TIM5_SINGLE_COUNT_TIME-1;
			tim5_arr = tim5_arr+TIM5_ARR_Compensate2;
		}
		
		log_info("pulse=%lldns,freq=%lldmhz,tim5_arr=%d,duration=%lld\r\n",pulse,freq,tim5_arr,duration);
		
		//计算定时器2的周期
		{
			uint32_t tim2_arr=0,tim2_psc=0;
			double T=0;
			tim2_psc=TIM2_FREQ_DIV;
			
			freq = freq/1000;
			
			T = 1000000000000.0/freq;	//ns
			tim2_arr = T/TIM2_SINGLE_COUNT_TIME-1;					//计算tim2的装填值，tim2控制周期,(TIM2分频值为42，2M时钟，没记一次数，时间为0.5us，即500ns)
			
			if(tim5_arr>tim2_arr)
			{
				tim5_arr = tim2_arr;
			}
			else{}
			
			train_acount= duration*1000 / T ;
			
			log_info("T=%lf,tim2_arr=%d,train_acount=%d\r\n",T,tim2_arr,train_acount);
			
			SW_CV_OUTPUT = 1;   //打开输出
			pLEDOUTPUT = LED_DIRECTLY_ON;
			Delay_ms(15);		//等待继电器闭合
			//log_info("UO_MODE_TRAIN Mode,SW_CV_OUTPUT = 1,Wave_type=%d,tim5_arr=%d\r\n",Wave_type,tim5_arr);
			
			Timer2_Init(tim2_arr,tim2_psc);
			DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
			TIM_Cmd(TIM2,ENABLE);
			
		}
	}
	else{}
		
	return 0;
}

void Process_COMMAND_STOP(void)
{
	Disable_Timer5();
	TIM_Cmd(TIM2,DISABLE);
	TIM2->CNT = 0;
	
	train_acount = 0;
	train_count = 0;
	
	
	DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
	
	AD5542_Output(DA_CHNL_ALL, 0);
	
	SW_CV_OUTPUT = 0;//关闭输出
	pLEDOUTPUT = LED_DIRECTLY_OFF;
	TIM5_IRQ_Count = 0;	//清除计数器
}

void Process_COMMAND_PAUSE(void)
{
	//暂停定时器2
	pLEDOUTPUT = LED_DIRECTLY_OFF;
	TIM_Cmd(TIM2,DISABLE);
	DOState.Status[DO_TIM4] = DOSTATE_STATUS_PAUSE;
}

void Process_COMMAND_CONTINUE(void)
{
	pLEDOUTPUT = LED_DIRECTLY_ON;
	TIM_Cmd(TIM2,ENABLE);
	DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;

}

/********************************************************************
*	功能	：	串口发送数据
******************************************************************************/
//static void Protocol_Usart1_Tx(uint8_t command)
//{
//	uint8_t  i = 0, j = 0, k = 0, index = 0;
//	uint16_t crcval = 0;
//	
//	U1.Ttbl[ADDR_OFFSET_HEAD]    = PROTOCOL_FIXED_HEAD;		
//	U1.Ttbl[ADDR_OFFSET_VERSION] = PROTOCOL_FIXED_VERSION;										//①
//	U1.Ttbl[ADDR_OFFSET_COMMAND] = command | 0x80;												//②
//	U1.Ttbl[ADDR_OFFSET_RESERVE] = 0X00;
//	
//	index = ADDR_OFFSET_RESERVE;
//	index++;
//	
//	if(command == COMMAND_QUERY)
//	{
//		for(i=0; i<8; i++)
//		{
//			if(DOState.Status[i] == DOSTATE_STATUS_RUNNING)										//运行
//			{
//				j |= 1 << i;
//				StatusBar_Progress(STATUSBAR_RUN);
//				break;
//			}
//			else if(DOState.Status[i] == DOSTATE_STATUS_COMPLETE)								//完成
//			{
//				k++;
//				if(k == 8)
//				{
//					StatusBar_Progress(STATUSBAR_COMPLETE);
//					T6.RunCnt = 0;
//				}
//			}
//			else if(DOState.Status[i] == DOSTATE_STATUS_PAUSE)									//暂停
//			{
//				StatusBar_Progress(STATUSBAR_PAUSE);
//				break;
//			}
//			else{}
//		}
//		U1.Ttbl[index++] = (uint8_t)j;															//③
//		
//		if(temp_runcnt != 0)
//		{
//			U1.Ttbl[index++] = temp_runcnt / 256 / 256 / 256;
//			U1.Ttbl[index++] = temp_runcnt / 256 / 256 % 256;
//			U1.Ttbl[index++] = temp_runcnt / 256 % 256;
//			U1.Ttbl[index++] = temp_runcnt % 256;
//		}
//		else
//		{
//			U1.Ttbl[index++] = T6.RunCnt / 256 / 256 / 256;
//			U1.Ttbl[index++] = T6.RunCnt / 256 / 256 % 256;
//			U1.Ttbl[index++] = T6.RunCnt / 256 % 256;
//			U1.Ttbl[index++] = T6.RunCnt % 256;
//		}		
//		
//		U1.Ttbl[index++] = statusbar;															//运行进度
//	}
//	else{}
//	
//	crcval     			= Crc_16bit_Calculate((uint8_t *)(&U1.Rtbl[1]), index-1);
//	U1.Ttbl[index++] 	= crcval / 256;
//	U1.Ttbl[index++] 	= crcval % 256;	
//	U1.Ttbl[index++] 	= PROTOCOL_FIXED_END;
//	
//	U1.Ttbl[ADDR_OFFSET_NUM_H]	= (index - 4) / 256;
//	U1.Ttbl[ADDR_OFFSET_NUM_L]	= (index - 4) % 256;
//	
//	Uart1_Tx(U1.Ttbl, index);
//}

sPulseConf_t PulseConf = {(ePulseDO_t)0, (ePulseType_t)0, {0}};
/********************************************************************
*	功能	：	处理串口接收数据
******************************************************************************/
//void Protocol_Usart1_Rx_Poll(void)
//{
//	uint8_t rcvstep = 0;
//	uint16_t datasize = 0, rcvnum = 0, index = 0, i = 0, j = 0;
//	eRtn_t rtn = RTN_NORMAL;
//	
//	if(U1.Rflag & 0x8000)
//	{
//		for(i = 0; i < (U1.Rflag&0X3FFF); i++)
//		{
//			rcvstep = 0;
//			switch(rcvstep)
//			{
//				case 0:
//					
//					if(U1.Rtbl[i] == PROTOCOL_FIXED_HEAD)
//					{
//						rcvstep = 1;
//						rcvnum = i;																//跳入case 1
//					}
//					else
//					{
//						break;
//					}
//					
//				case 1:
//					
//					datasize = U1.Rtbl[rcvnum+1]*256 + U1.Rtbl[rcvnum+2];
//					if(U1.Rtbl[rcvnum+3+datasize] == PROTOCOL_FIXED_END)
//					{
//						rcvstep = 2;															//跳入case 2
//					}
//					else
//					{
//						rcvstep = 0;
//						break;
//					}
//					
//				case 2:
//					
//					rcvstep = 0;																//一组数据可能需识别多个命令
//				
//					if(Crc_16bit_Calculate((uint8_t *)(&U1.Rtbl[rcvnum+ADDR_OFFSET_COMMAND]), datasize-3) == U1.Rtbl[rcvnum+ADDR_OFFSET_COMMAND+datasize-3]*256 + U1.Rtbl[rcvnum+ADDR_OFFSET_COMMAND+datasize-2])
//					{
//						i = rcvnum + 3 + datasize;												//后续可能还有有效数据，不可直接跳出循环
//						
//						switch(U1.Rtbl[rcvnum+ADDR_OFFSET_COMMAND])
//						{
//							case COMMAND_START:
//								
//								Protocol_Usart1_Tx(COMMAND_START);
//							
//								Process_COMMAND_START();

//								StatusBar_Progress(STATUSBAR_RUN);
//							
//								break;
//							
//							case COMMAND_PAUSE:
//								
//								if(U1.Rtbl[rcvnum+ADDR_OFFSET_CONTENT_START] == 0x00)
//								{
//									Process_COMMAND_PAUSE();
//									
//									StatusBar_Progress(STATUSBAR_PAUSE);
//								}
//								else if(U1.Rtbl[rcvnum+ADDR_OFFSET_CONTENT_START] == 0x01)
//								{
//									Process_COMMAND_CONTINUE();
//									
//									StatusBar_Progress(STATUSBAR_CONTINUE);
//								}
//								else{}
//								
//								Protocol_Usart1_Tx(COMMAND_PAUSE);
//								
//								break;
//							
//							case COMMAND_STOP:
//								
//								Process_COMMAND_STOP();
//							
//								Protocol_Usart1_Tx(COMMAND_STOP);
//							
//								StatusBar_Progress(STATUSBAR_COMPLETE);
//							
//								break;
//							
//							case COMMAND_ALARM:
//								
////								T6.AlarmValue = U1.Rtbl[rcvnum+ADDR_OFFSET_CONTENT_START] * 256 + U1.Rtbl[rcvnum+ADDR_OFFSET_CONTENT_START+1];
//								Protocol_Usart1_Tx(COMMAND_ALARM);
//							
//								break;
//							
//							case COMMAND_QUERY:
//								
//								Protocol_Usart1_Tx(COMMAND_QUERY);
//							
//								break;
//							
//							case COMMAND_CONFIG:
//								
//								INTX_DISABLE();													//关闭中断，涉及PWM中断相关参数
//								
//								index = rcvnum + ADDR_OFFSET_CONTENT_START;
//							
////								PulseConf.Channel = (ePulseDO_t)(U1.Rtbl[index] - 1);			//通道号，上位机【1，8】，位于单片机【0，7】协调，故而减一
//								PulseConf.Channel = (ePulseDO_t)DO_TIM4;
//								index++;
//							
//								UserOperation.bVC = U1.Rtbl[index++];
//								if(UserOperation.bVC == SELECT_VC_V)
//								{
//									Switch.ModeCur &= ~(1 << MODE_BIT_VC);
//									Switch.ModeCur |= SELECT_VC_V << MODE_BIT_VC;
//								}
//								else
//								{
//									Switch.ModeCur &= ~(1 << MODE_BIT_VC);
//									Switch.ModeCur |= SELECT_VC_C << MODE_BIT_VC;
//								}
//								
//								UserOperation.bPhase = (UO_PHASE_E)U1.Rtbl[index++];
//							
//								if(pPwmArrayParam[PulseConf.Channel]->Pwm != NULL)
//								{
//									myfree(SRAMIN, pPwmArrayParam[PulseConf.Channel]->Pwm);
//									pPwmArrayParam[PulseConf.Channel]->Pwm = NULL;
//									WDG_Feed();
//								}
//								else{}
//								DOState.Config |= 1 << PulseConf.Channel;
//								DOState.ZeroConfig &= ~(1 << PulseConf.Channel);
//								
//								PulseConf.PulseType = (ePulseType_t)U1.Rtbl[index++];
//								switch(PulseConf.PulseType)
//								{
//									case PULSE_TYPE_ZERO:													//在100us定时器中断中执行此波形处理
//										
//										DOState.Config &= ~(1 << PulseConf.Channel);						//在ZeroConfig置标志位
//										PulseConf.PulseParam.PulseZero.IndexNum[PulseConf.Channel] = BTOW(U1.Rtbl + index);	//待协议中添加打点数量字节
//										index += 2;
//										PulseConf.PulseParam.PulseZero.TblTime[PulseConf.Channel] = (uint32_t *)mymalloc(SRAMIN, sizeof(uint32_t) * PulseConf.PulseParam.PulseZero.IndexNum[PulseConf.Channel]);
//										if(PulseConf.PulseParam.PulseZero.TblTime[PulseConf.Channel] == NULL)
//										{											
//											if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//											{
//												Process_COMMAND_STOP();					//若正在运行时修改参数，则停止输出
//											}
//											else{}
//											
//											break;
//										}
//										else{}
//										PulseConf.PulseParam.PulseZero.TblAmpl[PulseConf.Channel] = (uint32_t *)mymalloc(SRAMIN, sizeof(uint32_t) * PulseConf.PulseParam.PulseZero.IndexNum[PulseConf.Channel]);
//										if(PulseConf.PulseParam.PulseZero.TblAmpl[PulseConf.Channel] == NULL)
//										{										
//											if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//											{
//												Process_COMMAND_STOP();
//											}
//											else{}
//											
//											break;
//										}
//										else{}
//										
//										for(j=0; j<PulseConf.PulseParam.PulseZero.IndexNum[PulseConf.Channel]; j++)
//										{
//											PulseConf.PulseParam.PulseZero.TblTime[PulseConf.Channel][j] = BTOL(U1.Rtbl + index);
//											index += 4;
//											PulseConf.PulseParam.PulseZero.TblAmpl[PulseConf.Channel][j] = BTOL(U1.Rtbl + index) * 1000;
//											index += 4;
//										}
//										
//										DOState.ZeroConfig |= 1 << PulseConf.Channel;
//										
//										break;
//																			
//									case PULSE_TYPE_ONE:
//										
//										PulseConf.PulseParam.PulseOne.IsLowFirst    = U1.Rtbl[index++];
//										PulseConf.PulseParam.PulseOne.MsStageFirst	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseOne.MsStageSecond	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseOne.Ampl			= BTOL(U1.Rtbl + index);
//										index += 4;
//									
//										rtn = PulseTran_One(&(PulseConf.PulseParam.PulseOne), pPwmArrayParam[PulseConf.Channel]);
//										if(rtn != RTN_NORMAL)
//										{
//											if(PulseConf.Channel == DO_TIM4 || (PulseConf.Channel != DO_TIM4 && rtn != RTN_ABNORMAL_AMPL))	//模拟通道 或 数字通道非幅值异常 时
//											{
//												DOState.Config &= ~(1 << PulseConf.Channel);	//用户设置参数为特殊值0或其它原因引起异常时，视为未配置处理
//											
//												if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//												{
//													Process_COMMAND_STOP();	//若正在运行时修改参数，则停止输出
//												}
//												else{}
//											}
//											else{}
//										}
//										else{}
//										
//										break;
//									
//									case PULSE_TYPE_TWO:
//										
//										PulseConf.PulseParam.PulseTwo.Freq			= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseTwo.Duty			= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseTwo.MsDuration	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseTwo.Ampl			= BTOL(U1.Rtbl + index);
//										index += 4;
//								
//										rtn = PulseTran_Two(&(PulseConf.PulseParam.PulseTwo), pPwmArrayParam[PulseConf.Channel]);
//										if(rtn != RTN_NORMAL)
//										{
//											if(PulseConf.Channel == DO_TIM4 || (PulseConf.Channel != DO_TIM4 && rtn != RTN_ABNORMAL_AMPL))
//											{
//												DOState.Config &= ~(1 << PulseConf.Channel);
//											
//												if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//												{
//													Process_COMMAND_STOP();
//												}
//												else{}
//											}
//											else{}
//										}
//										else{}
//										
//										break;
//									
//									case PULSE_TYPE_THREE:
//										
//										PulseConf.PulseParam.PulseThree.Period		= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseThree.HighTime	= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseThree.PulseNum	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseThree.Pause100ms	= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseThree.MsBegin		= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseThree.MsEnd		= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseThree.MsStep		= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseThree.CycleNum	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseThree.Ampl		= BTOL(U1.Rtbl + index);
//										index += 4;
//																		
//										rtn = PulseTran_Three(&(PulseConf.PulseParam.PulseThree), pPwmArrayParam[PulseConf.Channel]);
//										if(rtn != RTN_NORMAL)
//										{
//											if(PulseConf.Channel == DO_TIM4 || (PulseConf.Channel != DO_TIM4 && rtn != RTN_ABNORMAL_AMPL))
//											{
//												DOState.Config &= ~(1 << PulseConf.Channel);
//											
//												if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//												{
//													Process_COMMAND_STOP();
//												}
//												else{}
//											}
//											else{}
//										}
//										else{}
//										
//										break;
//										
//									case PULSE_TYPE_FOUR:
//										
//										PulseConf.PulseParam.PulseFour.PeriodBegin	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseFour.HighTime		= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseFour.PulseNum		= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseFour.Pause100ms	= BTOW(U1.Rtbl + index);
//										index += 2;
//										PulseConf.PulseParam.PulseFour.PeriodEnd	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseFour.PeriodStep	= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseFour.CycleNum		= BTOL(U1.Rtbl + index);
//										index += 4;
//										PulseConf.PulseParam.PulseFour.Ampl			= BTOL(U1.Rtbl + index);
//										index += 4;
//									
//										rtn = PulseTran_Four(&(PulseConf.PulseParam.PulseFour), pPwmArrayParam[PulseConf.Channel]);
//										if(rtn != RTN_NORMAL)
//										{
//											if(PulseConf.Channel == DO_TIM4 || (PulseConf.Channel != DO_TIM4 && rtn != RTN_ABNORMAL_AMPL))
//											{
//												DOState.Config &= ~(1 << PulseConf.Channel);
//											
//												if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//												{
//													Process_COMMAND_STOP();
//												}
//												else{}
//											}
//											else{}
//										}
//										else{}
//																				
//										break;
//										
//									case PULSE_TYPE_FIVE:
//										
//										PulseConf.PulseParam.PulseFive.Freq					= BTOL(U1.Rtbl + index);
//										index += 4;
//									
//										if(PulseConf.Channel == 0x07)							//最后一通道波形输出处理
//										{
//											PulseConf.PulseParam.PulseFive.Duty				= PULSE_DUTY_FULL / 2;		//非接收，约定值
//										}
//										else													//其它通道波形输出处理
//										{
//											PulseConf.PulseParam.PulseFive.Duty				= PULSE_DUTY_FULL;			//非接收，约定值
//										}
//										
//										PulseConf.PulseParam.PulseFive.ConfigNum			= BTOL(U1.Rtbl + index);
//										index += 4;
//										
//										if(PulseConf.PulseParam.PulseFive.ConfigNum > PULSE_FIVE_TBL_MAX_NUM)
//										{
//											PulseConf.PulseParam.PulseFive.ConfigNum		= PULSE_FIVE_TBL_MAX_NUM;
//										}
//										else{}
//										for(j = 0; j < PulseConf.PulseParam.PulseFive.ConfigNum; j++)
//										{
//											PulseConf.PulseParam.PulseFive.MsPause[j]		= BTOW(U1.Rtbl + index);
//											index += 2;
//											PulseConf.PulseParam.PulseFive.MsDuration[j]	= BTOL(U1.Rtbl + index);
//											index += 4;
//										}
//										
//										rtn = PulseTran_Five(&(PulseConf.PulseParam.PulseFive), pPwmArrayParam[PulseConf.Channel]);
//										if(rtn != RTN_NORMAL)
//										{
//											if(PulseConf.Channel == DO_TIM4 || (PulseConf.Channel != DO_TIM4 && rtn != RTN_ABNORMAL_AMPL))
//											{
//												DOState.Config &= ~(1 << PulseConf.Channel);
//											
//												if(DOState.Status[PulseConf.Channel] == DOSTATE_STATUS_RUNNING)
//												{
//													Process_COMMAND_STOP();
//												}
//												else{}
//											}
//											else{}
//										}
//										else{}										
//										
//										break;
//									
//									default:
//										
//										break;
//								}
//								
//								if(DOState.Config)
//								{
//									Protocol_Usart1_Tx(COMMAND_CONFIG);
//									
//									StatusBar_Progress(STATUSBAR_CONFIG);
//								}
//								else{}
//																
//								INTX_ENABLE();													//打开中断
//								
//								break;
//								
//							default:
//								
//								break;
//						}
//					}
//					else{}
//					
//					break;
//					
//				default:
//					break;
//			}
//		}
//		U1.Rflag = 0;
//	}
//	else{}
//}
