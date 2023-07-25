
#include  "main.h"


#define SPI_Wait_TimeOut  		0xffff     //��ʱ���� 0x5000	

/*******************************************************************************
* Function Name  : SPI1_Configuration
* Description    : STM32 SPI1����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);			//������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		//ʹ��GPIOAʱ��
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);  	//����Ϊ���蹦�ܣ��˴����ܺϲ�
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	DA_CS1_H();
	DA_CS2_H();
	DA_LDAC_H();
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;												//���ö˿�ģʽ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                						//���踴��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;														//����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;														//Ĭ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;													//GPIO��ת�ٶ�Ϊ25MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																//������д���Ӧ��GPIO�Ĵ�����

	//
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;													//����SPIƬѡ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//�˿����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//Ĭ����������ֹADƬѡ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;															//GPIO��ת�ٶ�Ϊ2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//������д���Ӧ��GPIO�Ĵ����� 
	
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  								//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI1, ENABLE); 
}


/*******************************************************************************
* Function Name  : SPI1_ReadWriteByte
* Description    : SPIд����
* Input          : д�ֽ�
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_WriteByte(unsigned char byte)
{
  unsigned int timeout = SPI_Wait_TimeOut;
 
  //while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) && (timeout--)); 	/*!< Loop while DR register in not emplty */
  //SPI_I2S_SendData(SPI1, byte);/*!< Send byte through the SPI2 peripheral */

  while(  ( (SPI1->SR & SPI_I2S_FLAG_TXE) == RESET )  && (timeout--) );
  SPI1->DR = byte;
  SPI1->SR&=(1<<0);
	
  timeout = SPI_Wait_TimeOut;
  
  while(  ( (SPI1->SR & SPI_I2S_FLAG_BSY) == SPI_I2S_FLAG_BSY )  && (timeout--) );
  //while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET) && (timeout--));/*!< Wait to receive a byte */
 
  //return SPI_I2S_ReceiveData(SPI2); /*!< Return the byte read from the SPI bus */
}



void AD5542_HSPI_WriteA( int64_t val)
{
	unsigned short reg_val;
	//unsigned char dat[2]={0};
	//log_info("Write A=%lld\r\n",val);
		
	//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//ת��ΪDAC���ݼĴ���ֵ	
	reg_val=(signed short)((double)val*8.0+0.5)+0x7fff;
	
	DA_LDAC_H();
	DA_CS1_L();
	DA_CS2_H();
	
	//dat[0] = (unsigned char)((reg_val&0xff00)>>8);
	//dat[1] = (unsigned char)(reg_val&0xff);
	
	SPI1_WriteByte( (reg_val>>8) );
	SPI1_WriteByte( reg_val);        
	      
	DA_LDAC_L();
	DA_CS1_H();
	DA_CS2_H();
	
	//log_info("AD5542_HSPI_WriteA val:%llx,,,,,,reg_val:0x%x %d,,,,,dat[0]:0x%x,dat[1]:0x%x\r\n",val,reg_val,reg_val,dat[0],dat[1]);
}








void AD5542_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	
	/*	������·DAC���Ϊ0mV  */
	
	DA_CS1_H();
	DA_CS2_H();
	DA_LDAC_H();
	DA_SCLK_H();
	DA_SDIN_L();
	
	__nop();__nop();__nop();__nop();__nop();__nop();
}



//ע�⣬�ú����Ĳ����������ʵ��ֵ���ڸú����ڲ���������Ϊ�Ĵ���ֵ���ŵ���ʵ��Ӧ����������İ��롣

static void AD5542_WriteA( int64_t val)
{
#if 0
	{
		unsigned short reg_val;
		unsigned char i=0;
		
		//log_info("Write A=%lld\r\n",val);
			
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//ת��ΪDAC���ݼĴ���ֵ	
		reg_val=(signed short)((double)val*8.0+0.5)+0x7fff;
		
		//log_info("Write A:val:%llx,,,,%x,,%d\r\n",val,reg_val,reg_val);
		
		DA_LDAC_H();
		DA_CS1_L();
		DA_CS2_H();
		
		for(i=0;i<16;i++)
		{
			if((reg_val<<i)&0x8000) 
			{
				DA_SDIN_H();
			}
			else
			{
				DA_SDIN_L();
			}
				//__nop();
				
				DA_SCLK_L();
				__nop();
				
				DA_SCLK_H();
				//__nop();
				
		}
			//__nop();
	       DA_LDAC_L();
	       DA_CS1_H();
	       DA_CS2_H();
	}
#else
	{
		unsigned short reg_val;
		//unsigned char dat[2]={0};
		//log_info("Write A=%lld\r\n",val);
		
		reg_val=((double)val*8.0+0.5)+0x7fff;//ת��ΪDAC���ݼĴ���ֵ	
		//reg_val=(signed short)((double)val*8.0+0.5)+0x7fff;
	
		DA_LDAC_H();
		DA_CS1_L();
	
		//dat[0] = (unsigned char)((reg_val&0xff00)>>8);
		//dat[1] = (unsigned char)(reg_val&0xff);
	
		SPI1_WriteByte( (reg_val>>8) );
		SPI1_WriteByte( reg_val);        
	      
		DA_LDAC_L();
		DA_CS1_H();
		DA_CS2_H();
	}
#endif
}

static void AD5542_WriteB(int64_t val)
{
#if 0
	{
		unsigned short reg_val;
		unsigned char i=0;
		
		//log_info("Write B=%lld\r\n",val);
		
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//ת��ΪDAC���ݼĴ���ֵ	
		reg_val=(signed short)(val*8)+0x8000;
		
		DA_LDAC_H();
		DA_CS1_H();
		DA_CS2_L();
		
		for(i=0;i<16;i++)
		{
			if((reg_val<<i)&0x8000) 
			{
				DA_SDIN_H();
			}
			else
			{
				DA_SDIN_L();
			}
				//__nop();
			
				DA_SCLK_L();
			
				__nop();
			
				DA_SCLK_H();
			
				//__nop();
		}
	
	      // __nop();
	       
	       DA_LDAC_L();
	       DA_CS1_H();
	       DA_CS2_H();
	}
#else
	{
		unsigned short reg_val;
		//unsigned char dat[2]={0};
		//log_info("Write A=%lld\r\n",val);
		
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//ת��ΪDAC���ݼĴ���ֵ	
		reg_val=((double)val*8.0+0.5)+0x7fff;
		
		//log_info("AD5542_WriteB reg_val:0x%x\r\n",reg_val);
		
		DA_LDAC_H();
		DA_CS2_L();
	
		//dat[0] = (unsigned char)((reg_val&0xff00)>>8);
		//dat[1] = (unsigned char)(reg_val&0xff);
	
		SPI1_WriteByte( (reg_val>>8) );
		SPI1_WriteByte( reg_val);        
	      
		DA_LDAC_L();
		DA_CS1_H();
		DA_CS2_H();
	}
#endif
}





//AD5542_Output value=mV
/*******************************************************************************
* Function Name  : AD5542_Output
* Description    : AD5542�������
* Input          : ch��ͨ��ѡ��1ѡ���ѹͨ����2ѡ�����ͨ����3���߶�ѡ��value��DAC�����ѹ��mV
* Output         : None
* Return         : None
*******************************************************************************/
void AD5542_Output(uint8_t ch, int32_t value)
{
	//static uint8_t  ch_pre = 0xff;
	//static int64_t value_pre = 0xff;
	
	
	//log_info("ch=%d value=%d\r\n",ch,value);
	
	if(ch == DA_CHNL_VOLT)
	{
		AD5542_WriteA(value);
	}
	else if(ch == DA_CHNL_CURR)								//[0��6.4V]-----[0, 32mA]			[0,6400mV]-----[0,32000uA]		1mA--0.2V	1uA--0.2mV	5uA--1mV
	{
		//log_info("value:%d\r\n",value);
		AD5542_WriteB(value);
	}
	else if(ch == DA_CHNL_ALL)
	{
		
		AD5542_WriteA(value);
		AD5542_WriteB(value);
	}
	else{}
}






