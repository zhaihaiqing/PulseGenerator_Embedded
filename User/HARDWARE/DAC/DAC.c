
#include  "main.h"


#define SPI_Wait_TimeOut  		0xffff     //超时常量 0x5000	

/*******************************************************************************
* Function Name  : SPI1_Configuration
* Description    : STM32 SPI1配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);			//打开外设时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		//使能GPIOA时钟
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);  	//设置为外设功能，此处不能合并
	//GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	DA_CS1_H();
	DA_CS2_H();
	DA_LDAC_H();
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;												//设置端口模式，
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                						//外设复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;														//推挽模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;														//默认下拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;													//GPIO翻转速度为25MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																//将配置写入对应的GPIO寄存器中

	//
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;													//设置SPI片选
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//端口输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//推挽模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//默认上拉，禁止AD片选
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;															//GPIO翻转速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//将配置写入对应的GPIO寄存器中 
	
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;	//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;						//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;					//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;							//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;						//串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;							//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;	//定义波特率预分频的值:波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;					//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;							//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  								//根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); 
}


/*******************************************************************************
* Function Name  : SPI1_ReadWriteByte
* Description    : SPI写总线
* Input          : 写字节
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
		
	//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//转换为DAC数据寄存器值	
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

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	
	/*	设置两路DAC输出为0mV  */
	
	DA_CS1_H();
	DA_CS2_H();
	DA_LDAC_H();
	DA_SCLK_H();
	DA_SDIN_L();
	
	__nop();__nop();__nop();__nop();__nop();__nop();
}



//注意，该函数的参数是输出的实际值，在该函数内部进行运算为寄存器值，优点是实现应用与驱动层的剥离。

static void AD5542_WriteA( int64_t val)
{
#if 0
	{
		unsigned short reg_val;
		unsigned char i=0;
		
		//log_info("Write A=%lld\r\n",val);
			
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//转换为DAC数据寄存器值	
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
		
		reg_val=((double)val*8.0+0.5)+0x7fff;//转换为DAC数据寄存器值	
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
		
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//转换为DAC数据寄存器值	
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
		
		//reg_val=(signed short)(val*32768.0/4096.0)+0x8000;	//转换为DAC数据寄存器值	
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
* Description    : AD5542输出设置
* Input          : ch：通道选择，1选择电压通道，2选择电流通道，3两者都选择，value：DAC输出电压，mV
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
	else if(ch == DA_CHNL_CURR)								//[0，6.4V]-----[0, 32mA]			[0,6400mV]-----[0,32000uA]		1mA--0.2V	1uA--0.2mV	5uA--1mV
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






