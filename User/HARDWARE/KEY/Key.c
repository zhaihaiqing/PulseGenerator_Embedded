
#include  "main.h"



KEY_T s_tBtn[KEY_NUMBER] = {0};
static KEY_FIFO_T s_tKey;
	



/********************************************************************
*  功能	  ：按键初始化
*  说明   ：据需调整
******************************************************************************/
void Btn_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	bsp_InitKeyVar();
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//使能GPIOC时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输出模式
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
}

/********************************************************************
*  功能   ：按键是否按下
*  测试   ：使用淘宝采购8*8矩阵按键FUN 1905，连接方式
			PC0--VER7		PC6--HOR0
			PC1--VER6		PC7--HOR1
			PC2--VER5		PC8--HOR2
			PC3--VER4		PC9--HOR3
			PC4--VER3		PC10-HOR4
			PC5--VER2		PC11-HOR5
*  说明   ：据需调整
******************************************************************************/
static uint8_t IsKeyDown(uint8_t number)
{
	if(number < 6)
	{
		pBTN_C0 = 0;
		pBTN_C1 = 1;
		pBTN_C2 = 1;
		pBTN_C3 = 1;
		pBTN_C4 = 1;
		pBTN_C5 = 1;
		
		if(pBTN_L0 == 0)
		{
			return 0;
		}
		else if(pBTN_L1 == 0)
		{
			return 1;
		}
		else if(pBTN_L2 == 0)
		{
			return 2;
		}
		else if(pBTN_L3 == 0)
		{
			return 3;
		}
		else if(pBTN_L4 == 0)
		{
			return 4;
		}
		else if(pBTN_L5 == 0)
		{
			return 5;
		}
		else{}
	}
	else if(number < 12)
	{
		pBTN_C0 = 1;
		pBTN_C1 = 0;
		pBTN_C2 = 1;
		pBTN_C3 = 1;
		pBTN_C4 = 1;
		pBTN_C5 = 1;
		
		if(pBTN_L0 == 0)
		{
			return 6;
		}
		else if(pBTN_L1 == 0)
		{
			return 7;
		}
		else if(pBTN_L2 == 0)
		{
			return 8;
		}
		else if(pBTN_L3 == 0)
		{
			return 9;
		}
		else if(pBTN_L4 == 0)
		{
			return 10;
		}
		else if(pBTN_L5 == 0)
		{
			return 11;
		}
		else{}
	}
	else if(number < 18)
	{
		pBTN_C0 = 1;
		pBTN_C1 = 1;
		pBTN_C2 = 0;
		pBTN_C3 = 1;
		pBTN_C4 = 1;
		pBTN_C5 = 1;
		
		if(pBTN_L0 == 0)
		{
			return 12;
		}
		else if(pBTN_L1 == 0)
		{
			return 13;
		}
		else if(pBTN_L2 == 0)
		{
			return 14;
		}
		else if(pBTN_L3 == 0)
		{
			return 15;
		}
		else if(pBTN_L4 == 0)
		{
			return 16;
		}
		else if(pBTN_L5 == 0)
		{
			return 17;
		}
		else{}
	}
	else if(number < 24)
	{
		pBTN_C0 = 1;
		pBTN_C1 = 1;
		pBTN_C2 = 1;
		pBTN_C3 = 0;
		pBTN_C4 = 1;
		pBTN_C5 = 1;
		
		if(pBTN_L0 == 0)
		{
			return 18;
		}
		else if(pBTN_L1 == 0)
		{
			return 19;
		}
		else if(pBTN_L2 == 0)
		{
			return 20;
		}
		else if(pBTN_L3 == 0)
		{
			return 21;
		}
		else if(pBTN_L4 == 0)
		{
			return 22;
		}
		else if(pBTN_L5 == 0)
		{
			return 23;
		}
		else{}
	}
	else if(number < 30)
	{
		pBTN_C0 = 1;
		pBTN_C1 = 1;
		pBTN_C2 = 1;
		pBTN_C3 = 1;
		pBTN_C4 = 0;
		pBTN_C5 = 1;
		
		if(pBTN_L0 == 0)
		{
			return 24;
		}
		else if(pBTN_L1 == 0)
		{
			return 25;
		}
		else if(pBTN_L2 == 0)
		{
			return 26;
		}
		else if(pBTN_L3 == 0)
		{
			return 27;
		}
		else if(pBTN_L4 == 0)
		{
			return 28;
		}
		else if(pBTN_L5 == 0)
		{
			return 29;
		}
		else{}
	}
	else if(number < 36)
	{
		pBTN_C0 = 1;
		pBTN_C1 = 1;
		pBTN_C2 = 1;
		pBTN_C3 = 1;
		pBTN_C4 = 1;
		pBTN_C5 = 0;
		
		if(pBTN_L0 == 0)
		{
			return 30;
		}
		else if(pBTN_L1 == 0)
		{
			return 31;
		}
		else if(pBTN_L2 == 0)
		{
			return 32;
		}
		else if(pBTN_L3 == 0)
		{
			return 33;
		}
		else if(pBTN_L4 == 0)
		{
			return 34;
		}
		else if(pBTN_L5 == 0)
		{
			return 35;
		}
		else{}
	}
	else{}
	
	return 0xff;
}


/********************************************************************
*  功能   ：初始化按键变量
*  说明   ：模板框架，无特殊需求不必改动
******************************************************************************/
void bsp_InitKeyVar(void)
{
	uint8_t i = 0;
	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;
	
	for(i=0; i<KEY_NUMBER; i++)
	{
		s_tBtn[i].LongTime 	 = KEY_LONG_TIME;			/* 长按时间 0 表示不检测长按键事件 */
		s_tBtn[i].Count      = KEY_FILTER_TIME;			/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State       = 0;						/* 按键缺省状态，0为未按下 */
		s_tBtn[i].RepeatSpeed = 0;						/* 按键连发的速度，0表示不支持连发 */
		s_tBtn[i].RepeatCount = 0;						/* 连发计数器 */
	}
	
	/* 如果需要单独更改某个按键的参数，可以在此单独重新赋值 */
	/* 比如，我们希望按键1按下超过1秒后，自动重发相同键值 */
//	s_tBtn[KID_K0].LongTime = 100;
//	s_tBtn[KID_K0].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

//	s_tBtn[KID_K1].LongTime = 100;
//	s_tBtn[KID_K1].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */

//	s_tBtn[KID_K2].LongTime = 100;
//	s_tBtn[KID_K2].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */
//	
//	s_tBtn[KID_K3].LongTime = 100;
//	s_tBtn[KID_K3].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */
//	
//	s_tBtn[KID_K4].LongTime = 100;
//	s_tBtn[KID_K4].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */
//	
//	s_tBtn[KID_K5].LongTime = 100;
//	s_tBtn[KID_K5].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */
//	
//	s_tBtn[KID_K6].LongTime = 100;
//	s_tBtn[KID_K6].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */
//	
//	s_tBtn[KID_K7].LongTime = 100;
//	s_tBtn[KID_K7].RepeatSpeed = 5;	/* 每隔50ms自动发送键值 */	
}

/********************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参: _KeyCode : 按键代码
*	说	  明：模板框架，无特殊需求不必改动
********************************************************************************************/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
	else{}
}

/**********************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。使用第1个读指针。
*	返 回 值: 按键代码
*	说    明：模板框架，无特殊需求不必改动
***********************************************************************************************/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		else{}
		return ret;
	}
}

/****************************************************************************
*	函 数 名: bsp_GetKey2
*	功能说明: 从按键FIFO缓冲区读取一个键值。使用第2个读指针。
*	返 回 值: 按键代码
*	说    明：模板框架，无特殊需求不必改动
****************************************************************************************************/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		else{}
		return ret;
	}
}

/*******************************************************************************
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参: _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*	说    明：模板框架，无特殊需求不必改动
*******************************************************************************************************/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/********************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参: _ucKeyID : 按键ID，从0开始
*			  _LongTime : 长按事件时间, 单位为10ms
*			  _RepeatSpeed : 连发速度, 单位为10ms
*	说    明：模板框架，无特殊需求不必改动
********************************************************************************************************/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime    = _LongTime;							//长按时间 0 表示不检测长按键事件
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;						//按键连发的速度，0表示不支持连发
	s_tBtn[_ucKeyID].RepeatCount = 0;									//连发计数器
}

/**********************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	说    明：模板框架，无特殊需求不必改动
********************************************************************************************************/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	说    明：简单对按键按下函数进行对应调整
********************************************************************************************************/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn = &s_tBtn[i];
	
//	if (IsKeyDown(i) != 0xff)											//仅适用于单按键，矩阵按键时会误处理为整排按下[V57]
	if(IsKeyDown(i) == i)												//有按键按下
	{
		if (pBtn->Count < KEY_FILTER_TIME)								//首次检测到按下，赋初值，开始计数
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)						//计数进行中.....
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;										//按键当前状态（按下还是弹起）
				bsp_PutKey((uint8_t)(3 * i + 1));						//发送按钮按下的消息
			}
			else{}

			if (pBtn->LongTime > 0)										//支持长按
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					if (++pBtn->LongCount == pBtn->LongTime)			//发送按钮持续按下的消息
					{
						bsp_PutKey((uint8_t)(3 * i + 3));				//键值放入按键FIFO
					}
					else{}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)							//支持重复写入FIFO
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							bsp_PutKey((uint8_t)(3 * i + 1));			//长按键后，每隔10ms发送1个按键
						}
						else{}
					}
					else{}
				}
			}
			else{}
		}
	}
	else
	{
		if(pBtn->Count > KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count != 0)
		{
			pBtn->Count--;
		}
		else
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;
				bsp_PutKey((uint8_t)(3 * i + 2));						//发送按钮弹起的消息
			}
			else{}
		}

		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。非阻塞，被中断周期性的调用
*	说    明：模板框架，无特殊需求不必改动
********************************************************************************************************/
void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_NUMBER; i++)
	{
		bsp_DetectKey(i);
	}
}
