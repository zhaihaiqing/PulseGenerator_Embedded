
#include "sys.h"
#include "debug_uart.h"
#include <stdio.h>

__IO unsigned char Usart6Flag=0;
__IO unsigned short Usart6_rx_len=0;
UART_RBUF_ST	usart6_rbuf	=	{ 0, 0, };


//ʹ�ô���6��Ϊdebug uart
//PG9=Uart_rx
//PG14=Uart_tx

//���ڲ�������
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
*  ����   �����������ݴ洢
******************************************************************************/
void USART6_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data,tmp;	//����6���յ�����
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
			Usart6_rx_len  = (p->in - p->out) & (UART6_RBUF_SIZE - 1);//��ȡ���ݳ���
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
* ����1�����ַ�����������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART6_GetCharBlock(uint16_t timeout)
{
	UART_RBUF_ST *p = &usart6_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART6_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* ����1�����ַ�������������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART6_GetChar(void)
{
	UART_RBUF_ST *p = &usart6_rbuf;			
	if(p->out == p->in) //������������
		return EMPTY;
	else{}
	return USART6_GetCharBlock(1000);
}




//��ʼ��IO ����6
//bound:������
void Usart6_Init(u32 bound)
{
	//GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#if EN_USART6_RX
	NVIC_InitTypeDef NVIC_InitStructure;
#endif	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);//ʹ��USART1ʱ��
 
	//����6��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource9,GPIO_AF_USART6); //GPIOG9����ΪUSART6 RX
	GPIO_PinAFConfig(GPIOG,GPIO_PinSource14,GPIO_AF_USART6); //GPIOG14����ΪUSART6 TX
	
	//USART6�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_14; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG,&GPIO_InitStructure); //��ʼ��PA9��PA10

	//USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART6, &USART_InitStructure); //��ʼ������1
	
	USART_ClearFlag(USART6, USART_FLAG_TC);
	
	
#if EN_USART6_RX	
	
	USART_ClearFlag(USART6, USART_FLAG_IDLE); //���������IDLE�жϣ������һֱ��IDLE�ж�
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);//��������ж�
	USART_ITConfig(USART6, USART_IT_IDLE, ENABLE);  //���� UART7 ���߿����ж�

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif

	USART_Cmd(USART6, ENABLE);  //ʹ�ܴ���1 
	
}



/********************************************************************
*  ����   �����ڷ��ͺ���
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
