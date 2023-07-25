
#include  "main.h"

/********************************************************************
*	功能	：	初始化
******************************************************************************/
void IIC_Init(void)
{	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟

	//GPIOB11,E10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	IIC_SDA_WR = 1;
	IIC_SCL    = 1;
	
	
	
	
// 	RCC->APB2ENR |= 1<<3;		//先使能外设IO PORT时钟
//	GPIOB->CRH   &= 0XFFFF00FF;	//推挽输出
//	GPIOB->CRH   |= 0X00003300;
//	GPIOB->ODR   |= 3<<10;		//输出高
}

/*********************************************************************
*	功能	：	产生IIC起始信号
********************************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();					//sda线输出
	IIC_SDA_WR = 1;
	IIC_SCL    = 1;
	delay_us(8);
 	IIC_SDA_WR = 0;				//START:when CLK is high,DATA change form high to low
	delay_us(8);
	IIC_SCL    = 0;				//钳住I2C总线，准备发送或接收数据
}	  

/*********************************************************************
*	功能	：	产生IIC停止信号
********************************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();					//sda线输出
	IIC_SCL    = 0;
	IIC_SDA_WR = 0;				//STOP:when CLK is high DATA change form low to high
 	delay_us(8);
	IIC_SCL    = 1;	
	delay_us(8);
	IIC_SDA_WR = 1;				//发送I2C总线结束信号
}

/*********************************************************************
*	功能	：	等待应答信号到来
*	返回	：	1，接收应答失败    
				0，接收应答成功
********************************************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	SDA_IN();					//SDA设置为输入
	IIC_SDA_WR   = 1;
	delay_us(2);
	IIC_SCL      = 1;
	delay_us(2);
	while(IIC_SDA_RD)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			IIC_Stop();
			return 1;
		}
		else{}
	}
	IIC_SCL      = 0;			//时钟输出0
	return 0;  
} 

/*********************************************************************
*	功能	：	产生ACK应答
********************************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL    = 0;
	SDA_OUT();
	IIC_SDA_WR = 0;
	delay_us(4);
	IIC_SCL    = 1;
	delay_us(4);
	IIC_SCL    = 0;
}

/*********************************************************************
*	功能	：	不产生ACK应答
********************************************************************************************/
void IIC_NAck(void)
{
	IIC_SCL    = 0;
	SDA_OUT();
	IIC_SDA_WR = 1;
	delay_us(4);
	IIC_SCL    = 1;
	delay_us(4);
	IIC_SCL    = 0;
}					 	

/*********************************************************************
*	功能	：	IIC发送一个字节
********************************************************************************************/
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL = 0;				//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        IIC_SDA_WR = (txd&0x80)>>7;
        txd      <<= 1;
		delay_us(4);			//对TEA5767这三个延时都是必须的
		IIC_SCL    = 1;
		delay_us(4);
		IIC_SCL    = 0;
		delay_us(4);
    }	 
} 	    

/*********************************************************************
*	功能	：	读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
********************************************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();					//SDA设置为输入
    for(i=0; i<8; i++ )
	{
        IIC_SCL   = 0;
        delay_us(4);
		IIC_SCL   = 1;
        receive <<= 1;
        if(IIC_SDA_RD)
			receive++;
		delay_us(2);
    }					 
    if (!ack)
        IIC_NAck();				//发送nACK
    else
        IIC_Ack();				//发送ACK   
    return receive;
}
