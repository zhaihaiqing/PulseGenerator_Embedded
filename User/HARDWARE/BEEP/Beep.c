
#include  "main.h"

/*
		Beep_Ctrl	PA1
		Beep		PG15
*/


uint8_t bFlag_Beep = 0;
/********************************************************************
*	����	��	��ʼ��
******************************************************************************/
void Beep_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOAʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��GPIOGʱ��
	

	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	//GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	
}

#define BEEP_CYCLE_TIME		1000	//ms
#define BEEP_DISABLE_TIME	600		//ms
/********************************************************************
*	����	��	��������ѯ
*	˵��	��	�������ܲ�һ�£����ݲ�ʹ�øú���
******************************************************************************/
void Beep_Poll(void)
{
	if(bFlag_Beep == 1)
	{
		if(T6.BeepDecCnt == 0)
		{
			T6.BeepDecCnt = BEEP_CYCLE_TIME;
		}
		else{}		
	}
	else							//���ƹر�����
	{
		T6.BeepDecCnt = 0;
	}
	
	if(T6.BeepDecCnt % BEEP_CYCLE_TIME <= BEEP_DISABLE_TIME)
	{
		BEEP = BEEP_OFF;
	}
	else
	{
		BEEP = BEEP_ON;
	}
}
