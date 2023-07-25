
#include  "main.h"

#if 0              
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
	while((USART1->SR&0X40)==0);
	USART1->DR = (u8) ch;
	return ch;
}
#endif 

//串口操作变量
//sUART1_t U1 = {  
//					0,     //Rflag 
//					{0},   //Rtbl[USART1_REC_LEN]
//					{0}    //Ttbl[USART1_SEND_LEN]
//			  };

/********************************************************************
*	功能	：	将接收数据实时存储，未进行接收数据正确与否的检验处理
******************************************************************************/
//void USART1_IRQHandler(void)
//{
//	u8 res = 0;
//	
//	if(USART1->SR&(1<<5))		//如果是RXE中断，表示接收到1byte数据
//	{		
//		res = USART1->DR;
//		if((U1.Rflag & 0x8000) == 0)
//		{
//			if((U1.Rflag & 0x3fff) < USART1_REC_LEN)
//			{
//				if(U1.Rflag == 0)
//				{
//					U1.Rflag |= 0X4000;
//				}
//				else{}
//				//T6.U1RxTimeOutCnt = 0;
//				U1.Rtbl[U1.Rflag & 0X3FFF] = res;
//				U1.Rflag++;
//			}
//			else{}			
//		}
//		else{}
//	}
//		
//	else if(USART1->SR&(1<<4))	//如果是空闲中断，表示接收到1帧数据
//	{
//		U1.Rflag |= 0X8000;
//		//T6.U1RxTimeOutCnt = 0;
//		
//	}
//	else{}
//}
									 

/********************************************************************
*	功能	：	初始化串口1
******************************************************************************/
//void Uart1_Init(u32 bound)
//{  	 
//	
//	//GPIO端口设置
//    GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//使能USART1时钟
// 
//	//串口6对应引脚复用映射
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOG9复用为USART6 RX
//	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOG14复用为USART6 TX
//	
//	//USART6端口配置
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
//	GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化PA9，PA10

//	//USART1 初始化设置
//	USART_InitStructure.USART_BaudRate = bound;//波特率设置
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
//	USART_Init(USART1, &USART_InitStructure); //初始化串口1
//	
//	USART_Cmd(USART1, ENABLE);  //使能串口1 
//	
//	USART_ClearFlag(USART1, USART_FLAG_TC);
//	
//#if EN_USART1_RX	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);	//开启相关中断
//	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);	//开启空闲中断
//	
//	//Usart1 NVIC 配置
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
//	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

//#endif


////	float temp;
////	u16 mantissa;
////	u16 fraction;	   
////	temp=(float)(pclk1*1000000)/(bound*16);
////	mantissa=temp;				 
////	fraction=(temp-mantissa)*16; 	 
////    mantissa<<=4;
////	mantissa+=fraction; 
////	RCC->APB2ENR|=1<<2;  
////	RCC->APB2ENR|=1<<14;  
////	GPIOA->CRH&=0XFFFFF00F; 
////	GPIOA->CRH|=0X000008B0;
////	GPIOA->ODR|=0X0600;
////		  
////	RCC->APB2RSTR|=1<<14;   
////	RCC->APB2RSTR&=~(1<<14);
//// 	USART1->BRR=mantissa; 
////	USART1->CR1|=0X200C; 
////	 
////#ifdef EN_USART1_RX		  
////	USART1->CR1|=1<<8;    
////	USART1->CR1|=1<<5;    	    	
////	MY_NVIC_Init(1,1,USART1_IRQn,2);
////	while((USART1->SR&0X40)==0);
////#endif
//}

/********************************************************************
*	功能	：	串口发送函数
******************************************************************************/
//void Uart1_Tx(u8 *buf,u8 len)
//{
//	u8 i;
//	
//	for(i=0; i<len; i++)
//	{
//		USART1->DR  = buf[i];
//		while((USART1->SR&0x40) == 0);
//	}
//}

