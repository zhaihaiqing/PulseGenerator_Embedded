/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.7.0
  * @date    22-April-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);


volatile unsigned int SysTick_Count = 0;   //Systick����
volatile unsigned int SysTick_Count1 = 0;   //Systick����
volatile unsigned int TimingDelay = 0;     //��ʱ��������

volatile unsigned char UI_TASK_POLL_FLAG	=	0;

//volatile unsigned char Voltage_Ploar_Flag = 0; 		//��ѹ����ȫ�ֱ�����0���������ԣ�1��������
//volatile unsigned char Current_Ploar_Flag = 0; 		//��������ȫ�ֱ�����0���������ԣ�1��������
uint32_t MCU_idAddr[]=
{
	0x1FFFF7AC,		/*STM32F0ΨһID��ʼ��ַ*/
	0x1FFFF7E8,		/*STM32F1ΨһID��ʼ��ַ*/
	0x1FFF7A10,		/*STM32F2ΨһID��ʼ��ַ*/
	0x1FFFF7AC,		/*STM32F3ΨһID��ʼ��ַ*/
	0x1FFF7A10,		/*STM32F4ΨһID��ʼ��ַ*/
	0x1FF0F420,		/*STM32F7ΨһID��ʼ��ַ*/
	0x1FF80050,		/*STM32L0ΨһID��ʼ��ַ*/
	0x1FF80050,		/*STM32L1ΨһID��ʼ��ַ*/
	0x1FFF7590,		/*STM32L4ΨһID��ʼ��ַ*/
	0x1FF0F420		/*STM32H7ΨһID��ʼ��ַ*/
};



void IO_Init()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOEʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOEʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_15;
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	
	
	GPIO_SetBits(GPIOA, GPIO_Pin_2 | GPIO_Pin_3 );
	GPIO_ResetBits(GPIOA, GPIO_Pin_15 );
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_12 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	pLEDPOWER			= 0;
	pLEDOUTPUT			= 0;
	pLEDOVERLOAD		= 0;
	pLEDRUN				= 1;
	pLEDPAUSE			= 1;
	pLEDFLIP			= 1;
	pLEDSINGLETRIGGER	= 1;
	
	pLEDUNITSWITCH		= 1;
	pLEDSINGLE			= 1;
	pLEDFREERUN			= 1;
	pLEDTRAIN			= 1;
	pLEDEXTBNC			= 1;
	pLEDAMPL			= 1;
	pLEDPULSE			= 1;
	pLEDFREQUENCY		= 1;
	pLEDDURATION		= 1;
	pLEDVC				= 1;
	
	
}
void MCU_RESET_Log(void)
{
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
	{
		log_info("RCC_FLAG_PORRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
	{
		log_info("RCC_FLAG_PINRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) != RESET)
	{
		log_info("RCC_FLAG_SFTRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		log_info("RCC_FLAG_IWDGRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{
		log_info("RCC_FLAG_WWDGRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) != RESET)
	{
		log_info("RCC_FLAG_LPWRRST\r\n");
	}
	else{}
	if(RCC_GetFlagStatus(RCC_FLAG_BORRST) != RESET)
	{
		log_info("RCC_FLAG_BORRST\r\n");
	}
	else{}
	RCC_ClearFlag();	//����жϸ�λ��־
}

void get_MCU_Type(MCUTypedef type)
{
	uint32_t id[3]={0};
	uint32_t id1[3]={0};
	id[0] = (uint32_t)DBGMCU->IDCODE & DBGMCU_IDCODE_DEV_ID;
	
	log_info("MCU id[0]:0x%x\r\n",id[0]);
	
	id[0]=*(uint32_t*)(MCU_idAddr[type]);
	id[1]=*(uint32_t*)(MCU_idAddr[type]+4);
	id[2]=*(uint32_t*)(MCU_idAddr[type]+8);
	
	log_info("MCU id1[0]:0x%x\r\n",id1[0]);
	log_info("MCU id1[1]:0x%x\r\n",id1[1]);
	log_info("MCU id1[2]:0x%x\r\n",id1[2]);
}


void get_RCCCLOCK(void)
{
	//char str[32]={0};
	RCC_ClocksTypeDef  RCC_Clocks;
	RCC_GetClocksFreq(&RCC_Clocks);
	
	log_info("HCLK_Frequency:%.3fMHz\r\n",RCC_Clocks.HCLK_Frequency/1000000.0);
	log_info("PCLK1_Frequency:%.3fMHz\r\n",RCC_Clocks.PCLK1_Frequency/1000000.0);
	log_info("PCLK2_Frequency:%.3fMHz\r\n",RCC_Clocks.PCLK2_Frequency/1000000.0);
	log_info("SYSCLK_Frequency:%.3fMHz\r\n",RCC_Clocks.SYSCLK_Frequency/1000000.0);
}

void get_device_sn(void)
{
	AT24CXX_Read(512, (void *)&Device_SN, 18);
	log_info("Device_SN:%s\r\n",Device_SN);
}

/*******************************************************************************
* Function Name  : Delay function
* Description    : ��ʱ����������ʱ����sleepģʽ
* Input          : ��ʱʱ����ms��
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_ms(volatile unsigned int nTime)
{ 
	TimingDelay = nTime;
	while(TimingDelay != 0);//__WFI;//�ȴ������н���sleepģʽ
}


void delay_us(uint32_t us)
{
    uint32_t delta;
    /* �����ʱ������ tick �� */
    us = us * (SysTick->LOAD/(1000000/1000));
    /* ��õ�ǰʱ�� */
    delta = SysTick->VAL;
    /* ѭ����õ�ǰʱ�䣬ֱ���ﵽָ����ʱ����˳�ѭ�� */
    while (delta - SysTick->VAL< us);
}

/*******************************************************************************
* Function Name  : SysTick init function
* Description    : ���ö�ʱ���ȼ�SysTick���ȼ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(SystemCoreClock / 1000); //���ö�ʱ���ȣ�1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick�ж����ȼ�����
}

void GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOAʱ��
}




/*******************************************************************************
* Function Name  : Init_Devices function
* Description    : ���Ӳ���豸��������ģ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Devices(void)
{
	u8 test=0;
	u8 tbuff[5]={0};
	u16 T2_ARR=0,T2_PSC=0;
	u16 T6_ARR=0,T6_PSC=0;
	//��ɻ���Ӳ����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�ж����ȼ�����	
	
	Init_SysTick();//����SysTick��ʱ��
	
	WDG_Init(5,4000);
	
	IO_Init();
	Usart6_Init(DEBUG_UART_RATE);
	
	log_info("\r\n");
	log_info("****************************************************************\r\n");
	log_info("MCU Start!\r\n");
	WDG_Feed();
	get_MCU_Type(STM32F4);
	get_RCCCLOCK();
	MCU_RESET_Log();
	log_info("Debgu_Uart init OK! baud rate=%d bit/s\r\n\r\n",DEBUG_UART_RATE);
	
	
/*
*	��ʱ��ʱ��84M��16bitװ��ֵ
*	1��1000ms����ʱ����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����10000��Ϊ1000ms
*	2��100ms����ʱ����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����1000��Ϊ100ms
*	3��10ms����ʱ����Ƶϵ��840������84M/840=100Khz�ļ���Ƶ�ʣ�����1000��Ϊ10ms
*	4��1ms��ʱ����Ƶϵ��84������84M/84=1000Khz�ļ���Ƶ�ʣ�����1000��Ϊ1ms
*	5��100us��ʱ����Ƶϵ��84������84M/84=1000Khz�ļ���Ƶ�ʣ�����100��Ϊ100us
*/
	Delay_ms(100);	//�ȴ�
	WDG_Feed();
	APOW_ON();		//��ģ���Դ
		log_info("Anglog power has open!\r\n");
		log_info("Wait for analog power to stabilize......!\r\n");
		Delay_ms(300);	//�ȴ�ģ���Դ�ȶ�
	WDG_Feed();
	LED_Init();
		log_info("LED_Init OK!\r\n");
	Beep_Init();
		log_info("Beep_Init OK!\r\n");
	
	Get_MCU_POR_Times();	//������I2C��ʼ��֮��
		log_info("MCU_POR_Times:%d\r\n",sAdditionalData.MCU_POR_Times);
	get_device_sn();
	WDG_Feed();
	Btn_Init();		//����������ͨ����ʱ������⣬ÿ��10msִ��һ�μ��
		log_info("Btn_Init OK!\r\n");
	//AD5542_Init();
	SPI1_Configuration();
	AD5542_Output(DA_CHNL_ALL, 0);
		log_info("AD5542_Init OK,Set AD5542 Output to zero!\r\n");
	Codec_Init();
		log_info("Codec_Init OK!\r\n");
	WDG_Feed();
	TriggerExIN01_Init();
	TriggerOUT_Init();
		log_info("TriggerExIN12_Init OK!\r\n");
	//Uart1_Init(USART1_BAUD);
		//log_info("Uart1_Init OK! baud rate=%d bit/s\r\n",USART1_BAUD);
	My_Mem_Init(SRAMIN);
	Switch_Init();
	//pLEDOUTPUT=1;
	WDG_Feed();
	
	LCDRST_Init();
	WDG_Feed();
		log_info("Reset LCD!\r\n");
	Delay_ms(600);	
	WDG_Feed();
	
	LCD_Init();
		log_info("LCD OK! LCDDEV_ID=0x%x\r\n",lcddev.id);
	Delay_ms(500);							//��ʱ��������Ļһ�����������ٵ�������ʱ=400ms��Ļ��ʼ��ʾ�ַ��������쳣���ƣ�=500ms��Ļ��ʾ������// longzhigu:  [V194]	
	WDG_Feed();
	AT24CXX_Init();
		log_info("AT24CXX_Init OK!\r\n");
	test_eeprom();
	WDG_Feed();
	Delay_ms(50);	
	
	Memory_ConfigLoad();
		log_info("Get Par OK\r\n");
		
		
	WDG_Feed();
	UI_DeskTopInit();
	
	
	WDG_Feed();
	T6_ARR=999;
	T6_PSC=83;
	Timer6_Init(T6_ARR,T6_PSC);		//1msһ���ж�
	log_info("Init Timer6 OK!\r\n");
	log_info("Timer6 ARR=%d,PSC=%d\r\n",T6_ARR,T6_PSC);
	log_info("Timer6 Overflow_time=%dus\r\n",(T6_ARR+1)*(T6_PSC+1)/84);
	WDG_Feed();
	
	//��ʱ��5��PWM1ģʽ��
	Timer5_Init(10000-1,TIM5_FREQ_DIV);	//�̶���Ƶ��0.5us����ʱ��Ƶ��=0.5us*(arr+1)
	Timer3_Init(1000, TIM3_FREQ_DIV);
	
	Delay_ms(50);	
	WDG_Feed();
	
	SW_CV_OUTPUT = 0;
	
	if((Wave_type==0) || (Wave_type==1))
	{
		TriggerExIN01_Init();
	}
	else
	{
		TriggerExIN23_Init();
	}
	WDG_Feed();
	
	pLEDPOWER			= 0;
	pLEDOUTPUT			= 1;
	pLEDOVERLOAD		= 1;
	pLEDRUN				= 0;
	pLEDPAUSE			= 0;
	pLEDFLIP			= 0;
	pLEDSINGLETRIGGER	= 0;
						  
	pLEDUNITSWITCH		= 0;
	pLEDSINGLE			= 0;
	pLEDFREERUN			= 0;
	pLEDTRAIN			= 0;
	pLEDEXTBNC			= 0;
	pLEDAMPL			= 0;
	pLEDPULSE			= 0;
	pLEDFREQUENCY		= 0;
	pLEDDURATION		= 0;
	pLEDVC				= 0;
	
	{
		//SOFTWARE_VERSION
		log_info("software_version:%d.%d.%d\r\n",(uint8_t)(SOFTWARE_VERSION>>16),(uint8_t)((SOFTWARE_VERSION>>8)&0xff),(uint8_t)SOFTWARE_VERSION);
		
	}
	log_info("Hardware init OK! Sys running......\r\n\r\n");
}










/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */

int main(void)
{	
	Init_Devices();
	
	

//#if 1
//	
//	{
//		int32_t xx=0;
//		
//		while(1)
//		{
//			AD5542_Output(DA_CHNL_VOLT, 0);
//			__nop();
//			__nop();
//			
//			AD5542_Output(DA_CHNL_VOLT, 3300);
//			__nop();
//			__nop();
//			
////			AD5542_Output(DA_CHNL_VOLT, xx);
////			xx += 100;
////			if(xx>3300) xx = -3300;
//			WDG_Feed();
////			Delay_ms(50);
//			//log_info("123\r\n");
//		}
//	}

//#else
//	
//	{
//		int32_t xx=0;
//		
//		SPI1_Configuration();
//		//Delay_ms(50);
//		while(1)
//		{
//			WDG_Feed();
//			AD5542_HSPI_WriteA(xx);
//			xx += 100;
//			if(xx>3300) xx = -3300;
//			Delay_ms(50);
//		}
//	}
//	
//#endif
     
	/* Infinite loop */
	while (1)
	{
		Manual_Poll();
		WDG_Feed();
		Memory_Poll();
		WDG_Feed();
		if(UI_TASK_POLL_FLAG )
		{
			UI_TASK_POLL_FLAG = 0;
			
			UI_Poll();
			WDG_Feed();			
			//log_info("UI_TASK_POLL_FLAG:%d\r\n",UI_TASK_POLL_FLAG);
		}
		Led_ShortOn_Poll();
		WDG_Feed();
		
		if(Usart6Flag)			
		{
			if( Usart6_rx_len == 28)	//����SN��,��ǰ��֧��һ���ض����ȵ�ָ�����ֱ�����жϳ���
			{
				uint16_t ix = 0;
				uint16_t crc = 0;
				char uart_buf[UART6_RBUF_SIZE] = {0};
				char crc_mask[2]={"AA"};
				
				memset(uart_buf,0xff,UART6_RBUF_SIZE);	//���㻺����
				for(ix=0;ix<Usart6_rx_len;ix++) 
				{
					uart_buf[ix] =USART6_GetChar();//���������ݷŵ�ָ��buf
				}
				//ִ��CRCУ��
				//Device_Prefix
				
				crc = CRC16_Check((uint8_t *)uart_buf,Usart6_rx_len-2 );
				
				if( (( crc == ( (uart_buf[Usart6_rx_len - 1] << 8) |    uart_buf[Usart6_rx_len - 2])        )) \
				||  ( ( uart_buf[Usart6_rx_len - 1] == crc_mask[0]    )  && (uart_buf[Usart6_rx_len - 2] == crc_mask[1]))  )	//"A"
				{
					if(strncmp(Device_Prefix,uart_buf,8) == 0)	//ǰ׺��ͬ
					{
						log_info("Usart6Flag=1,Usart6_rx_len:%d,uart_buf:%-28s,SN:%-18s\r\n",Usart6_rx_len,uart_buf,&uart_buf[8]);
						AT24CXX_Write(512, (void *)&uart_buf[8], 18);
					}
					else
					{
						;
					}					
				}
				else
				{
					;
				}
				Usart6Flag = 0;
				Usart6_rx_len = 0;
				
				AT24CXX_Read(512, (void *)&Device_SN, 18);			
			}
			else
			{
				;
			}
			
		}
		else
		{
			;
		}
		WDG_Feed();
		//Delay_ms(1);
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
