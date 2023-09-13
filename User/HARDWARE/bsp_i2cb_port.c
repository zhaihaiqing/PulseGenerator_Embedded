/**
  ******************************************************************************
  * @file    Project/BSP/bsp_i2c_port.c 
  * @author  zhaihaiqing
  * @version V1.0.0
  * @date    5-April-2020
  * @brief   使用IO模拟I2C
  ******************************************************************************
  */
#include "main.h"

/*
****************************************************************************************
****************************************************************************************
****************************************************************************************
****************************************************************************************
****************************************************************************************
*/


void i2cb_port_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(I2CB_SCL_RCC_PORT, ENABLE);//使能GPIO时钟
	RCC_AHB1PeriphClockCmd(I2CB_SDA_RCC_PORT, ENABLE);//使能GPIO时钟

	//GPIOE11,E10初始化设置
	GPIO_InitStructure.GPIO_Pin = I2CB_SCL_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(I2CB_SCL_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	//GPIOE11,E10初始化设置
	GPIO_InitStructure.GPIO_Pin = I2CB_SDA_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(I2CB_SDA_GPIO_PORT, &GPIO_InitStructure);//初始化
	
	i2cb_port_stop();
}



/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2cb_port_delay(void)
{
	uint16_t i;
	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率 = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 38; i++);
}

//产生IIC起始信号
void i2cb_port_start(void)
{
	/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
	I2CB_SDA_1();
	I2CB_SCL_1();
	i2cb_port_delay();
	I2CB_SDA_0();
	i2cb_port_delay();
	I2CB_SCL_0();
	i2cb_port_delay();
}
//产生IIC停止信号
void i2cb_port_stop(void)
{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
	I2CB_SDA_0();
	I2CB_SCL_1();
	i2cb_port_delay();
	I2CB_SDA_1();					   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
unsigned char i2cb_port_wait_ack(void)
{
	uint8_t re=0;
	uint16_t times=0;
	I2CB_SDA_1();
	i2cb_port_delay();
	I2CB_SCL_1();
	i2cb_port_delay();
	
//	while( I2CB_SDA_READ() )
//	{
//		times++;
//		if(times>100)
//		{
//			i2cb_port_stop();
//			return 1;
//		}
//	}
	
	if( I2CB_SDA_READ() )
	{
		re=1;
	}
	else
	{
		re=0;
	}
	
	I2CB_SCL_0();
	i2cb_port_delay();
	return re; 
} 
//产生ACK应答
void i2cb_port_ack(void)
{
	I2CB_SDA_0();
	i2cb_port_delay();
	I2CB_SCL_1();
	i2cb_port_delay();
	I2CB_SCL_0();
	i2cb_port_delay();
	I2CB_SDA_1();
}
//不产生ACK应答		    
void i2cb_port_nack(void)
{
	I2CB_SDA_1();
	i2cb_port_delay();
	I2CB_SCL_1();
	i2cb_port_delay();
	I2CB_SCL_0();
	i2cb_port_delay();	
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void i2cb_port_send_byte(unsigned char txd)
{                        
    unsigned char t;
	
	I2CB_SCL_0();
	
	for(t=0;t<8;t++)
	{
		if(txd & 0x80)
		{
			I2CB_SDA_1();
		}
		else
		{
			I2CB_SDA_0();
		}
		txd <<= 1;
		i2cb_port_delay();
		I2CB_SCL_1();
		i2cb_port_delay();
		I2CB_SCL_0();			
		i2cb_port_delay();
	}
	
	I2CB_SDA_1();	//释放总线 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
unsigned char i2cb_port_read_byte(void)
{
	unsigned char i=0,receive=0;
	
	for(i=0;i<8;i++)
	{
		I2CB_SCL_1();
		i2cb_port_delay();
		receive <<= 1;
		if( I2CB_SDA_READ() ) receive++;
		I2CB_SCL_0();
		i2cb_port_delay();
	}
//	if(!ack)
//	{
//		i2cb_port_nack();
//	}
//	else
//	{
//		i2cb_port_ack();
//	}
			
	return receive;
}









/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参:  _Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
unsigned char i2cb_CheckDevice(unsigned char Device_addr)
{
	uint8_t ucAck=0;
	
	if( I2CB_SDA_READ() && I2CB_SCL_READ() )
	{
		i2cb_port_start();						/* 发送启动信号 */
		i2cb_port_send_byte(Device_addr | 0);	/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
		ucAck = i2cb_port_wait_ack();			/* 检测设备的ACK应答 */
		
		i2cb_port_stop();						/* 发送停止信号 */
		return ucAck;
	}
	return 1;		//总线异常
}















