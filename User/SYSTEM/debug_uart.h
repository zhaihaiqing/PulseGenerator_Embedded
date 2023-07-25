
/********************************************************************
*
*  文件   ：usart2.h
*  管脚   ：PA2/USART2_TX     PA3/USART2_RX
*  功能   ：
*
******************************************************************************/

#ifndef __DEBUG_UART_H
#define __DEBUG_UART_H
#include  "main.h"



//定义log_info
#define debug
#ifdef debug
		//#define log_info(...)    printf(__VA_ARGS__)
		 #define log_info(...)     printf(__VA_ARGS__)
#else
		#define log_info(...)
#endif



#define USART6_REC_LEN		5
#define USART6_SEND_LEN		5
#define USART6_PCLK			36
#define USART6_BAUD			9600
#define USART6_RecOverTime	2400*16/USART6_BAUD+1		//16ms是波特率2400时计算所得接收超时，可作为基准

typedef struct 
{
	u16 Rflag;											//接收标志【bit15--接收完成  bit14--接收到第一个字节  bit5~0--接收到有效字节数目】
	u8 Rtbl[USART6_REC_LEN];							//接收数组
	u8 Ttbl[USART6_SEND_LEN];							//发送数组
}sUSART6_t;


#define UART6_RBUF_SIZE   64		//要求：2 的整次幂
typedef struct uart_rbuf_st
{
	unsigned int in;							//输入
	unsigned int out;							//输出
	unsigned char  buf [UART6_RBUF_SIZE];		//缓冲区空间
}UART_RBUF_ST;


extern __IO unsigned char Usart6Flag;
extern __IO unsigned short Usart6_rx_len;
extern UART_RBUF_ST	usart6_rbuf	;


extern sUSART6_t U6;

#define EN_USART6_RX  1
void Usart6_Init(u32 bound); 
void Usart6_Tx(u8 *buf,u8 len);

uint16_t USART6_GetChar(void);
#endif	 
