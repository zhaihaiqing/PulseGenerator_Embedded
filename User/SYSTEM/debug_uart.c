
#include "sys.h"
#include "debug_uart.h"
#include <stdio.h>

__IO unsigned char Usart6Flag=0;
__IO unsigned short Usart6_rx_len=0;
UART_RBUF_ST	usart6_rbuf	=	{ 0, 0, };


//使用串口6作为debug uart
//PG9=Uart_rx
//PG14=Uart_tx

//串口操作变量
sUSART6_t U6 = {0, {0}, {0}};

              
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
 
void _sys_exit(int x) 
{ 
	x = x; 
} 
 
int fputc(int ch, FILE *f)
{      
	while((USART6->SR&0X40)==0);
	USART6->DR = (u8) ch;      
	return ch;
}


/********************************************************************
*  功能   ：将串口数据存储
******************************************************************************/
void USART6_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data,tmp;	//串口6接收的数据
	UART_RBUF_ST *p = &usart6_rbuf;
	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
	{
		USART_ClearITPendingBit(USART6,USART_IT_RXNE);
		Uart_Get_Data = USART_ReceiveData(USART6);
		//if(!Uart7Flag)		
		//{
			if((p->in - p->out)<UART6_RBUF_SIZE)
			{
				p->buf [p->in & (UART6_RBUF_SIZE-1)] = Uart_Get_Data;	
				p->in++;
			}
			else{}
			Usart6_rx_len  = (p->in - p->out) & (UART6_RBUF_SIZE - 1);//获取数据长度
		//}
		
	}
	else if(USART_GetITStatus(USART6, USART_IT_IDLE) != RESET)
	{
		tmp=USART6->SR;
		tmp=USART6->DR;		
		Usart6Flag = 1;
	}
	else{}	
}
	
#define EMPTY 	0xFFFF
#define TIMEOUT 0xFFFF

/**********************************************************************************
* 串口1接收字符函数，阻塞模式（接收缓冲区中提取）
**********************************************************************************/
uint16_t USART6_GetCharBlock(uint16_t timeout)
{
	UART_RBUF_ST *p = &usart6_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART6_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* 串口1接收字符函数，非阻塞模式（接收缓冲区中提取）
**********************************************************************************/
uint16_t USART6_GetChar(void)
{
	UART_RBUF_ST *p = &usart6_rbuf;			
	if(p->out == p->in) //缓冲区空条件
		return EMPTY;
	else{}
	return USART6_GetCharBlock(1000);
}




//初始化IO 串口6
//bound:波特率
void Usart6_Init(u32 bound)
{
	//GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#if EN_USART6_RX
	NVIC_InitTypeDef NVIC_InitStructure;
#endif	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//使能USART1时钟
 
	//串口6对应引脚复用映射
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9复用为USART6 RX
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14复用为USART6 TX
	
	//USART6端口配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //GPIOA9与GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG,&GPIO_InitStructure); //初始化PA9，PA10

	//USART1 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART6, &USART_InitStructure); //初始化串口1
	
	USART_ClearFlag(USART6, USART_FLAG_TC);
	
	
#if EN_USART6_RX	
	
	USART_ClearFlag(USART6, USART_FLAG_IDLE); //必须先清除IDLE中断，否则会一直进IDLE中断
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//开启相关中断
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);  //开启 UART7 总线空闲中断

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

#endif

	USART_Cmd(USART6, ENABLE);  //使能串口1 
	
}



/********************************************************************
*  功能   ：串口发送函数
******************************************************************************/
void Usart6_Tx(u8 *buf,u8 len)
{
	u8 i;
	
	for(i=0; i<len; i++)
	{
		USART6->DR  = buf[i];
		while((USART6->SR&0x40) == 0);
	}
}
