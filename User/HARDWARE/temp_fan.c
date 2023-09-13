/**
  ******************************************************************************
  * @file    Project/BSP/
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

uint16_t Read_TMP_Byte(unsigned char SLAVE_Device_Addr,unsigned char Reg_Addr)
{
	uint8_t msb=0,lsb=0;
	uint16_t Reg_Value=0;
	//rt_enter_critical();
	
    i2cb_port_start();
	
	i2cb_port_send_byte( SLAVE_Device_Addr);		//发送器件地址,写数据 	 
	i2cb_port_wait_ack(); 
	
    i2cb_port_send_byte(Reg_Addr);						//发送低地址
	i2cb_port_wait_ack();
	
	i2cb_port_start();
	
	i2cb_port_send_byte(SLAVE_Device_Addr | 0x01);				//进入接收模式
	i2cb_port_wait_ack();
	
    msb=i2cb_port_read_byte();
    i2cb_port_ack();
	//产生一个停止条件
	lsb=i2cb_port_read_byte();
	i2cb_port_nack();
	
	i2cb_port_stop();
	
	//rt_exit_critical();
	
	Reg_Value = (msb<<8) | lsb;
	
	//log_info("temp:msb:0x%x,lsb:0x%x\r\n",msb,lsb);
	
	return Reg_Value;
}




//获取温度 
float GetTempValue(uint8_t addr) 
{  
	uint16_t Result=0; 
	float Temp=0.0;
	//Result=Read_TMP75_Byte(I2C2,LM75_ADDR,LM75_REG_ID);  //读取ID 
	//log_info("LM75_ID:0x%x\r\n",(unsigned char)Result);	

	Result=Read_TMP_Byte(addr,TEMP_REG_TEMP);  //读取温度
	
	if(Result <= 0x7fff)
	{
		Temp = Result*0.0078125;
	}
	else
	{
		Temp = 0-(~Result +1)  *0.0078125;
	}
	
	//log_info("LM75_Result:0x%x  ",Result);
	//log_info("LM75_temp:%.2f℃\r\n",Temp);
//	
	//log_info("addr:0x%x,temp:%f\r\n",addr,Temp);
	return Temp;
}

float get_temp(void)
{
	float temp=0;
	temp = GetTempValue(TEMP_I2C_ADDR);
	log_info("get_temp:%f\r\n",temp);
	return temp;
}




#define TIM9_PWM_FREQ  	1000
#define TIM9_PSC		1



void Timer9_PWM_Init(float Duty)
{
	uint32_t TIM_CLK=0;
	float Freq=0;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  ///使能TIM5时钟
	
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
	/*--------------------------------- GPIO Configuration -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;		//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;		//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);				//初始化该端口
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
	
	/* -----------------------------------------------------------------------
	TIM:32bit
    The TIM clk=84M/(psc+1)= 21 MHz: 
	TIM Freq = TIM5 counter clock/(arr + 1)
	----------------------------------------------------------------------- */
	Freq = TIM9_PWM_FREQ;
	if(Freq <= 0)Freq=1;
	if(Duty < 0)Duty=0;
	if(Duty > 100)Duty=0;
	TIM_CLK = 84000000/(TIM9_PSC+1);
	TIM_TimeBaseInitStructure.TIM_Period = (TIM_CLK / Freq)-1; 				//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=TIM9_PSC;  							//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 				//向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);//初始化TIM
	
	/* ----------PWM Mode configuration: Channe1--------------------------- */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse =  (Duty/100.0) * ((TIM_CLK / Freq)-1) ;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

	TIM_OC1Init(TIM9, &TIM_OCInitStructure);
}

/**********************************************************************************
* Function Name  : Timer9_PWM_OC1_Enable
* PWM启动
**********************************************************************************/
void Timer9_PWM_OC1_Enable(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* GPIOA Configuration: Pin_5 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);
	
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM9,ENABLE);
	 /* TIM enable counter */
	TIM_Cmd(TIM9, ENABLE);
}

/**********************************************************************************
* Function Name  : Timer9_PWM_OC1_Disable
* PWM关闭
**********************************************************************************/
void Timer9_PWM_OC1_Disable(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*--------------------------------- GPIO Configuration -------------------------*/
	/* GPIOA Configuration: Pin 5 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOE,GPIO_Pin_5);
	
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Disable);
	TIM_ARRPreloadConfig(TIM9,DISABLE);
	 /* TIM disable counter */
	TIM_Cmd(TIM9, DISABLE);
}

/**********************************************************************************
* Function Name  : Timer9_PWM_OC1_SetDuty
* 设置占空比，占空比为实际占空比
**********************************************************************************/
void Timer9_PWM_OC1_SetDuty(float Duty)
{
	uint32_t TIM_CLK=0;
	float Freq=0;
	
	Freq = TIM9_PWM_FREQ;
	if(Freq <= 0)Freq=1;
	
	TIM_CLK = 84000000/(TIM9_PSC+1);	//计算TIM9的时钟
	
	TIM_SetCompare1(TIM9,(Duty/100.0) * ((TIM_CLK / Freq)-1));
}











