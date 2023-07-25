
#include  "main.h"

/********************************************************************
*
*	�ļ�	��	TIMER.C

*	�ܽ�	��	(NONE)

*	����	��	�˴�Ӧ��������ʱ����TIM5��TIM3��
*	TIM5: 32bit��ʱ����MAX TIM CLK=84M������������1us-300s

* TIM3: 16bit��ʱ����MAX TIM CLK=84M��������Ƶ�ʣ����1Hz��1s�����30K��33.333ms
******************************************************************************/



sTimer2_t T2 = {0};

/********************************************************************
*	����	��	�жϺ���������������������������ʱ��ģʽ����ҪTIM2���룬����
******************************************************************************/
void TIM2_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)
	{
		Enable_Timer5(tim5_arr);						//�ڶ�ʱ��2�д�һ��TIM5
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
		
		if(UserOperation.fMode == UO_MODE_TRAIN)
		{
			train_count++;
			if(train_count>train_acount)
			{
				train_count=0;
				Process_COMMAND_STOP();
			}
		}
	}	
}

/********************************************************************
*	����	��	��ʼ����Tout = ((arr + 1)*(psc + 1))/Tclk,
				����Tclk = 84MHz��Tout=(arr+1)(psc+1)/84 (us)
*	ʾ��	��	Timer2_Init(719, 1);     
******************************************************************************/
void Timer2_Init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///ʹ��TIM2ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	//TIM_Cmd(TIM2,ENABLE); //ʹ�ܶ�ʱ��3
}








/*****************************************************************************
*	����	��	TIM5�жϺ�����32bit��ʱ���������������
******************************************************************************/
volatile unsigned int TIM5_IRQ_Count=0;
__IO uint32_t tim5_ix=0,tim5_iy=0;	//���ο�Ȳ���,��ֹ�������Ż�

static void tim5_delay_us(uint32_t times)
{
	uint32_t i=0;
	while(times--)
	{
		i=10;
		while(i--);
	}
}
void TIM5_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM5,TIM_IT_Update) != RESET)
	{	
		log_info("in TIM5_IRQHandler \r\n");
		
		if((Wave_type == 0) || (Wave_type == 1))
		{
			Output_VorC(UserOperation.bVC, 0, OUTPUT_ENABLE);
			pTRIGGER_OUT = 0;
			Disable_Timer5();
			if(UserOperation.fMode == UO_MODE_SINGLE)
			{
				SW_CV_OUTPUT = 0;   //�ر����
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
			}
		}
		else
		{
			if(UserOperation.fMode == UO_MODE_EXTBNC)
			{
				TIM5_IRQ_Count++;
				if(TIM5_IRQ_Count == 2)
				{
					if(Wave_type == 2)
					{
						Output_VorC(UserOperation.bVC, (0-pPwmArrayParam[DO_TIM4]->Ampl), OUTPUT_ENABLE);
					}
					else
					{
						Output_VorC(UserOperation.bVC, pPwmArrayParam[DO_TIM4]->Ampl, OUTPUT_ENABLE);
					}
				}
				else if(TIM5_IRQ_Count >= 3)
				{
					if(pluse_Compensate < 40000) tim5_delay_us(9);	//���С��0.04ms,��ִ��
					else tim5_delay_us(30);//���ο�Ȳ���			
					
					Output_VorC(UserOperation.bVC, 0, OUTPUT_ENABLE);
					
					BNCMode_Reflash_LCD_Status = 0;
					Disable_Timer5();
					TIM5_IRQ_Count=0;
				}
			}
			else
			{
				TIM5_IRQ_Count++;
				if(TIM5_IRQ_Count == 2)
				{
					if(Wave_type == 2)
					{
						Output_VorC(UserOperation.bVC, (0-pPwmArrayParam[DO_TIM4]->Ampl), OUTPUT_ENABLE);
					}
					else
					{
						Output_VorC(UserOperation.bVC, pPwmArrayParam[DO_TIM4]->Ampl, OUTPUT_ENABLE);
					}	
				}
				else if(TIM5_IRQ_Count >= 3)
				{
					if(pluse_Compensate < 40000) tim5_delay_us(9);	//���С��0.04ms,��ִ��
					else tim5_delay_us(30);//���ο�Ȳ���
					
					Output_VorC(UserOperation.bVC, 0, OUTPUT_ENABLE);
					pTRIGGER_OUT = 0;
					Disable_Timer5();
					TIM5_IRQ_Count=0;
					
					if(UserOperation.fMode == UO_MODE_SINGLE)
					{
						SW_CV_OUTPUT = 0;   //�ر����						
						pLEDOUTPUT = LED_DIRECTLY_OFF;
						DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
					}
				}
			}
		}
		
		TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
	}	
}




//TIM5_CH2
void Timer5_Init(u32 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  ///ʹ��TIM5ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; //��ʱ��5�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  	//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);//��ʼ��TIM5
	
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); //����ʱ��5�����ж�
}


void Enable_Timer5(u32 arr)
{
	//���
	uint32_t t5_arr = 0;
	if(Wave_type == 0)	//�����Բ���0����������
	{
		Output_VorC(UserOperation.bVC, pPwmArrayParam[DO_TIM4]->Ampl, OUTPUT_ENABLE);
	}
	else if(Wave_type == 1)	//�����Բ���1���ȸ�����
	{
		Output_VorC(UserOperation.bVC, (0-pPwmArrayParam[DO_TIM4]->Ampl), OUTPUT_ENABLE);
	}
	else if(Wave_type == 2)	//˫���Բ���0�������󸺺���
	{
		Output_VorC(UserOperation.bVC, pPwmArrayParam[DO_TIM4]->Ampl, OUTPUT_ENABLE);
	}
	else					//˫���Բ���1���ȸ���������
	{
		Output_VorC(UserOperation.bVC, (0-pPwmArrayParam[DO_TIM4]->Ampl), OUTPUT_ENABLE);
	}
	
	//TIM5->PSC = TIM5_FREQ_DIV;	//4M
	if(Wave_type <= 1)
	{
		t5_arr = arr+1;
	}
	else
	{
		t5_arr = (arr+1)/2;
	}	
	TIM5->ARR = t5_arr;
	TIM5->CNT	=	0;
	TIM5->SR = (uint16_t)~TIM_IT_Update;
//	
//	log_info("TIM5->ARR:%d TIM5->PSC��%d\r\n",TIM5->ARR,TIM5->PSC);
	
	//Timer5_Init(t5_arr,TIM5_FREQ_DIV);
	TIM5->CR1 |= TIM_CR1_CEN;
	
	pTRIGGER_OUT = 1;
	TIM5_IRQ_Count=1;
}

void Disable_Timer5(void)
{
	TIM5->CR1 &= (uint16_t)~TIM_CR1_CEN;	//�رն�ʱ��
	TIM5->CNT = 0;							//��������Ĵ���,ȷ���´μ����Ǵ�0��ʼ��
	
	if(UserOperation.fMode == UO_MODE_EXTBNC)
	{
		DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
	}
}


//���ڿ�����Ļ״̬ˢ��

void Timer3_Init(u16 arr, u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x02; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	//TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
}

__IO uint8_t BNCMode_Reflash_LCD_Status	=	0;	//����һ��������������BNCģʽ�£�������ʾ״̬ˢ��

__IO uint32_t tim3_count =0;
void TIM3_IRQHandler(void)
{	
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	//if(TIM3->SR&0X0001)
	{
		tim3_count++;
		
		//log_info("%d %d %d\r\n",Wave_type,tim3_count,BNCMode_Reflash_LCD_Status);
		
		if((Wave_type==0) || (Wave_type ==1))
		{
			if((tim3_count % 2) )
			{
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
			}
			if((tim3_count % 2) ==0 )
			{
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
				pLEDOUTPUT = LED_DIRECTLY_ON;
				pLEDRUN = LED_DIRECTLY_ON;
			}
			
			if(BNCMode_Reflash_LCD_Status == 0 )
			{
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
				tim3_count =0;
				TIM3_DISABLE();
			}
		}
		
		else
		{
			
			if((tim3_count % 2) )
			{
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
			}
			if((tim3_count % 2) ==0 )
			{
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_RUNNING;
				pLEDOUTPUT = LED_DIRECTLY_ON;
				pLEDRUN = LED_DIRECTLY_ON;
			}
			
			if((tim3_count>2) && (BNCMode_Reflash_LCD_Status == 0) )
			{
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
				tim3_count =0;
				TIM3_DISABLE();
			}
		}
		
		//log_info("%d %d %d\r\n",Wave_type,tim3_count,BNCMode_Reflash_LCD_Status);
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}	
}

//void Disable_Timer3(void)
//{
//	TIM3->CR1 &= (uint16_t)~TIM_CR1_CEN;	//�رն�ʱ��
//	TIM3->CNT = 0;							//��������Ĵ���,ȷ���´μ����Ǵ�0��ʼ��
//}

//void Enable_Timer3(u32 arr)
//{
//	TIM3->ARR = arr;
//	TIM3->PSC = TIM3_FREQ_DIV;	//4M
//	TIM3->CNT	=	0;
//	TIM3->CR1 |= TIM_CR1_CEN;
//}

