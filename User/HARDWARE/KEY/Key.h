
/********************************************************************
*
*  文件   ：Key.h
*  管脚   ：PC0--PC11矩阵按键
*  功能   ：按键驱动程序
*  说明   ：扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*
******************************************************************************/
#ifndef __KEY_H
#define __KEY_H
#include "main.h"

#define KEY_NORMAL				36
#define KEY_SPECIAL				0
#define KEY_NUMBER				(KEY_NORMAL + KEY_SPECIAL)

/*	FP:	引脚定义，配置对应管脚	*/
#define pBTN_L0		PCin(0)				//L--LINE，轮询检测，电阻上拉
#define pBTN_L1		PCin(1)
#define pBTN_L2		PCin(2)
#define pBTN_L3		PCin(3)
#define pBTN_L4		PCin(4)
#define pBTN_L5		PCin(5)
#define pBTN_C0		PCout(6)			//C--COLUME，交替赋值
#define pBTN_C1		PCout(7)
#define pBTN_C2		PCout(8)
#define pBTN_C3		PCout(9)
#define pBTN_C4		PCout(10)
#define pBTN_C5		PCout(11)

/********************************************************************
*	功能	：	按键布局，确定按键功能与按键返回值对应关系
*							第一版布局									*
	RUN			1			2			3
	FLIP		4			5			6
	SINTRI		7			8			9
				.			0			ENTER			←[BACKSPACE]	CLEAR
	VOL/CUR		SINGLE		TRAIN		FREERUN			EXTTRI
	UNIT		PULSE		AMPL		FREQ			DURATION
*							第二版布局									*	
	SINGLE		AMPL		1			2				3				RUN
	FREERUN		PULSE		4			5				6				PAUSE
	TRAIN		FREQ		7			8				9				FLIP
	EXTTRI		DURATION	.			0				ENTER	← →		SINGLETRI
	VOL/CUR		UNIT SWI
******************************************************************************/
#define BTN_RUN				0x0B
#define BTN_PAUSE			0x0A
#define BTN_PHASE			0x09					//BTN_FLIP
#define BTN_SINGLETRIGGER	0x18
#define BTN_VC				0x04
#define BTN_UNITSWITCH		0x1f
#define BTN_SINGLE			0x00
#define BTN_TRAIN			0x02
#define BTN_FREERUN			0x01
#define BTN_EXTBNC			0x03
#define BTN_PULSE			0x22
#define BTN_AMPL			0x23
#define BTN_FREQ			0x21
#define BTN_DURATION		0x20
#define BTN_1				0x1D
#define BTN_2				0x17
#define BTN_3				0x11
#define BTN_4				0x1C
#define BTN_5				0x16
#define BTN_6				0x10
#define BTN_7				0x1B
#define BTN_8				0x15
#define BTN_9				0x0F
#define BTN_0				0x13
#define BTN_DOT				0x19
#define BTN_ENTER			0x0D
#define BTN_BACKSPACE		0x07
#define BTN_CLEAR			0x1E





/* FP:	根据应用程序各按键的功能自行重命名按键宏，便于记忆与调用 */
#define KEY_DOWN_K0				KEY_0_DOWN
#define KEY_UP_K0				KEY_0_UP
#define KEY_LONG_K0				KEY_0_LONG

#define KEY_DOWN_K1				KEY_1_DOWN
#define KEY_UP_K1				KEY_1_UP
#define KEY_LONG_K1				KEY_1_LONG

#define KEY_DOWN_K2				KEY_2_DOWN
#define KEY_UP_K2				KEY_2_UP
#define KEY_LONG_K2				KEY_2_LONG

#define KEY_DOWN_K3				KEY_3_DOWN
#define KEY_UP_K3				KEY_3_UP
#define KEY_LONG_K3				KEY_3_LONG

#define KEY_DOWN_K4				KEY_4_DOWN
#define KEY_UP_K4				KEY_4_UP
#define KEY_LONG_K4				KEY_4_LONG

#define KEY_DOWN_K5				KEY_5_DOWN
#define KEY_UP_K5				KEY_5_UP
#define KEY_LONG_K5				KEY_5_LONG

#define KEY_DOWN_K6				KEY_6_DOWN
#define KEY_UP_K6				KEY_6_UP
#define KEY_LONG_K6				KEY_6_LONG

#define KEY_DOWN_K7				KEY_7_DOWN
#define KEY_UP_K7				KEY_7_UP
#define KEY_LONG_K7				KEY_7_LONG

#define KEY_DOWN_K8				KEY_8_DOWN
#define KEY_UP_K8				KEY_8_UP
#define KEY_LONG_K8				KEY_8_LONG

#define KEY_DOWN_K9				KEY_9_DOWN
#define KEY_UP_K9				KEY_9_UP
#define KEY_LONG_K9				KEY_9_LONG

#define KEY_DOWN_K10			KEY_10_DOWN
#define KEY_UP_K10				KEY_10_UP
#define KEY_LONG_K10			KEY_10_LONG

#define KEY_DOWN_K11			KEY_11_DOWN
#define KEY_UP_K11				KEY_11_UP
#define KEY_LONG_K11			KEY_11_LONG

#define KEY_DOWN_K12			KEY_12_DOWN
#define KEY_UP_K12				KEY_12_UP
#define KEY_LONG_K12			KEY_12_LONG

#define KEY_DOWN_K13			KEY_13_DOWN
#define KEY_UP_K13				KEY_13_UP
#define KEY_LONG_K13			KEY_13_LONG

#define KEY_DOWN_K14			KEY_14_DOWN
#define KEY_UP_K14				KEY_14_UP
#define KEY_LONG_K14			KEY_14_LONG

#define KEY_DOWN_K15			KEY_15_DOWN
#define KEY_UP_K15				KEY_15_UP
#define KEY_LONG_K15			KEY_15_LONG

#define KEY_DOWN_K16			KEY_16_DOWN
#define KEY_UP_K16				KEY_16_UP
#define KEY_LONG_K16			KEY_16_LONG

#define KEY_DOWN_K17			KEY_17_DOWN
#define KEY_UP_K17				KEY_17_UP
#define KEY_LONG_K17			KEY_17_LONG

#define KEY_DOWN_K18			KEY_18_DOWN
#define KEY_UP_K18				KEY_18_UP
#define KEY_LONG_K18			KEY_18_LONG

#define KEY_DOWN_K19			KEY_19_DOWN
#define KEY_UP_K19				KEY_19_UP
#define KEY_LONG_K19			KEY_19_LONG

#define KEY_DOWN_K20			KEY_20_DOWN
#define KEY_UP_K20				KEY_20_UP
#define KEY_LONG_K20			KEY_20_LONG

#define KEY_DOWN_K21			KEY_21_DOWN
#define KEY_UP_K21				KEY_21_UP
#define KEY_LONG_K21			KEY_21_LONG

#define KEY_DOWN_K22			KEY_22_DOWN
#define KEY_UP_K22				KEY_22_UP
#define KEY_LONG_K22			KEY_22_LONG

#define KEY_DOWN_K23			KEY_23_DOWN
#define KEY_UP_K23				KEY_23_UP
#define KEY_LONG_K23			KEY_23_LONG

#define KEY_DOWN_K24			KEY_24_DOWN
#define KEY_UP_K24				KEY_24_UP
#define KEY_LONG_K24			KEY_24_LONG

#define KEY_DOWN_K25			KEY_25_DOWN
#define KEY_UP_K25				KEY_25_UP
#define KEY_LONG_K25			KEY_25_LONG

#define KEY_DOWN_K26			KEY_26_DOWN
#define KEY_UP_K26				KEY_26_UP
#define KEY_LONG_K26			KEY_26_LONG

#define KEY_DOWN_K27			KEY_27_DOWN
#define KEY_UP_K27				KEY_27_UP
#define KEY_LONG_K27			KEY_27_LONG

#define KEY_DOWN_K28			KEY_28_DOWN
#define KEY_UP_K28				KEY_28_UP
#define KEY_LONG_K28			KEY_28_LONG

#define KEY_DOWN_K29			KEY_29_DOWN
#define KEY_UP_K29				KEY_29_UP
#define KEY_LONG_K29			KEY_29_LONG

#define KEY_DOWN_K30			KEY_30_DOWN
#define KEY_UP_K30				KEY_30_UP
#define KEY_LONG_K30			KEY_30_LONG

#define KEY_DOWN_K31			KEY_31_DOWN
#define KEY_UP_K31				KEY_31_UP
#define KEY_LONG_K31			KEY_31_LONG

#define KEY_DOWN_K32			KEY_32_DOWN
#define KEY_UP_K32				KEY_32_UP
#define KEY_LONG_K32			KEY_32_LONG

#define KEY_DOWN_K33			KEY_33_DOWN
#define KEY_UP_K33				KEY_33_UP
#define KEY_LONG_K33			KEY_33_LONG

#define KEY_DOWN_K34			KEY_34_DOWN
#define KEY_UP_K34				KEY_34_UP
#define KEY_LONG_K34			KEY_34_LONG

#define KEY_DOWN_K35			KEY_35_DOWN
#define KEY_UP_K35				KEY_35_UP
#define KEY_LONG_K35			KEY_35_LONG

/*	FP:	根据实际按键数量，按照既定规则增加或减少按键定义
	定义键值代码, 必须按如下次序定义每个键的按下、弹起和长按事件

	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2) 编译器可帮我们避免键值重复。                          */
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_0_DOWN,				/* 0键按下 */
	KEY_0_UP,				/* 0键弹起 */				//弹起，0x02
	KEY_0_LONG,				/* 0键长按 */
	
	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,				/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,				/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */

	KEY_4_DOWN,				/* 4键按下 */
	KEY_4_UP,				/* 4键弹起 */
	KEY_4_LONG,				/* 4键长按 */

	KEY_5_DOWN,				/* 5键按下 */
	KEY_5_UP,				/* 5键弹起 */
	KEY_5_LONG,				/* 5键长按 */

	KEY_6_DOWN,				/* 6键按下 */
	KEY_6_UP,				/* 6键弹起 */
	KEY_6_LONG,				/* 6键长按 */

	KEY_7_DOWN,				/* 7键按下 */
	KEY_7_UP,				/* 7键弹起 */
	KEY_7_LONG,				/* 7键长按 */

	KEY_8_DOWN,				/* 8键按下 */
	KEY_8_UP,				/* 8键弹起 */
	KEY_8_LONG,				/* 8键长按 */
	
	KEY_9_DOWN,				/* 9键按下 */
	KEY_9_UP,				/* 9键弹起 */		//弹起，0x20
	KEY_9_LONG,				/* 9键长按 */
	
	KEY_10_DOWN,			/* 按下 */
	KEY_10_UP,				/* 弹起 */
	KEY_10_LONG,			/* 长按 */
	
	KEY_11_DOWN,			/* 按下 */
	KEY_11_UP,				/* 弹起 */
	KEY_11_LONG,			/* 长按 */
	
	KEY_12_DOWN,			/* 按下 */
	KEY_12_UP,				/* 弹起 */
	KEY_12_LONG,			/* 长按 */
	
	KEY_13_DOWN,			/* 按下 */
	KEY_13_UP,				/* 弹起 */
	KEY_13_LONG,			/* 长按 */
	
	KEY_14_DOWN,			/* 按下 */
	KEY_14_UP,				/* 弹起 */
	KEY_14_LONG,			/* 长按 */
	
	KEY_15_DOWN,			/* 按下 */
	KEY_15_UP,				/* 弹起 */
	KEY_15_LONG,			/* 长按 */
	
	KEY_16_DOWN,			/* 按下 */
	KEY_16_UP,				/* 弹起 */
	KEY_16_LONG,			/* 长按 */
	
	KEY_17_DOWN,			/* 按下 */
	KEY_17_UP,				/* 弹起 */
	KEY_17_LONG,			/* 长按 */
	
	KEY_18_DOWN,			/* 按下 */
	KEY_18_UP,				/* 弹起 */
	KEY_18_LONG,			/* 长按 */
	
	KEY_19_DOWN,			/* 按下 */
	KEY_19_UP,				/* 弹起 */
	KEY_19_LONG,			/* 长按 */
	
	KEY_20_DOWN,			/* 按下 */
	KEY_20_UP,				/* 弹起 */
	KEY_20_LONG,			/* 长按 */	
	
	KEY_21_DOWN,			/* 按下 */
	KEY_21_UP,				/* 弹起 */
	KEY_21_LONG,			/* 长按 */
	
	KEY_22_DOWN,			/* 按下 */
	KEY_22_UP,				/* 弹起 */
	KEY_22_LONG,			/* 长按 */
	
	KEY_23_DOWN,			/* 按下 */
	KEY_23_UP,				/* 弹起 */
	KEY_23_LONG,			/* 长按 */
	
	KEY_24_DOWN,			/* 按下 */
	KEY_24_UP,				/* 弹起 */
	KEY_24_LONG,			/* 长按 */
	
	KEY_25_DOWN,			/* 按下 */
	KEY_25_UP,				/* 弹起 */
	KEY_25_LONG,			/* 长按 */
	
	KEY_26_DOWN,			/* 按下 */
	KEY_26_UP,				/* 弹起 */
	KEY_26_LONG,			/* 长按 */
	
	KEY_27_DOWN,			/* 按下 */
	KEY_27_UP,				/* 弹起 */
	KEY_27_LONG,			/* 长按 */
	
	KEY_28_DOWN,			/* 按下 */
	KEY_28_UP,				/* 弹起 */
	KEY_28_LONG,			/* 长按 */
	
	KEY_29_DOWN,			/* 按下 */
	KEY_29_UP,				/* 弹起 */
	KEY_29_LONG,			/* 长按 */
	
	KEY_30_DOWN,			/* 按下 */
	KEY_30_UP,				/* 弹起 */
	KEY_30_LONG,			/* 长按 */
	
	KEY_31_DOWN,			/* 按下 */
	KEY_31_UP,				/* 弹起 */
	KEY_31_LONG,			/* 长按 */
	
	KEY_32_DOWN,			/* 按下 */
	KEY_32_UP,				/* 弹起 */
	KEY_32_LONG,			/* 长按 */
	
	KEY_33_DOWN,			/* 按下 */
	KEY_33_UP,				/* 弹起 */
	KEY_33_LONG,			/* 长按 */
	
	KEY_34_DOWN,			/* 按下 */
	KEY_34_UP,				/* 弹起 */
	KEY_34_LONG,			/* 长按 */
	
	KEY_35_DOWN,			/* 按下 */
	KEY_35_UP,				/* 弹起 */
	KEY_35_LONG,			/* 长按 */	
}KEY_ENUM;					//注意按键弹起亦有返回值，单纯（键值-1）/3可能将弹起值默认为按下值处理，引起异常现象

/*	FP:	根据实际按键数量，按照既定规则增加或减少按键定义
	按键ID, 主要用于bsp_KeyState()函数的入口参数 		*/
typedef enum
{
	KID_K0 = 0,
	KID_K1,
	KID_K2,
	KID_K3,
	KID_K4,
	KID_K5,
	KID_K6,
	KID_K7,
	KID_K8,
	KID_K9,
	KID_K10,
	KID_K11,
	KID_K12,
	KID_K13,
	KID_K14,
	KID_K15,
	KID_K16,
	KID_K17,
	KID_K18,
	KID_K19,
	KID_K20,
	KID_K21,
	KID_K22,
	KID_K23,
	KID_K24,
	KID_K25,
	KID_K26,
	KID_K27,
	KID_K28,
	KID_K29,
	KID_K30,
	KID_K31,
	KID_K32,
	KID_K33,
	KID_K34,
	KID_K35,	
}KEY_ID_E;

/*	FP:	可根据实际效果自行调整
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件  	*/
#define KEY_FILTER_TIME   5				/* 单位10ms， 按键滤波时间50ms		*/
#define KEY_LONG_TIME     100			/* 单位10ms， 持续1秒，认为长按事件 */

/*	FP:	处理框架，无需修改	每个按键对应1个全局的结构体变量。	*/
typedef struct
{
	uint8_t  Count;						/* 滤波器计数器 */
	uint16_t LongCount;					/* 长按计数器 */
	uint16_t LongTime;					/* 按键按下持续时间, 0表示不检测长按，超过时长认为长按 */
	uint8_t  State;						/* 按键当前状态（按下还是弹起） */
	uint8_t  RepeatSpeed;				/* 连续识别按键周期，超过此时长再次将按键按下写入FIFO */
	uint8_t  RepeatCount;				/* 连续按键计数器 */
}KEY_T;
extern KEY_T s_tBtn[KEY_NUMBER];

/*	FP:	处理框架，无需修改	按键FIFO用到变量 */
#define KEY_FIFO_SIZE	50
typedef struct							/* FP:	处理框架，无需修改*/
{
	uint8_t Buf[KEY_FIFO_SIZE];			/* 键值缓冲区 */
	uint8_t Read;						/* 缓冲区读指针1 */
	uint8_t Write;						/* 缓冲区写指针 */
	uint8_t Read2;						/* 缓冲区读指针2 */
}KEY_FIFO_T;



/*	FP:	配置控制芯片引脚	*/
void Btn_Init(void);
/*	FP:	无需修改 	供外部调用的函数声明。	*/
void bsp_InitKeyVar(void);				/* 初始化按键变量 */
void bsp_KeyScan(void);					/* 轮询按键，定时器中调用 */
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);				/* 获取按键值 */
//uint8_t bsp_GetKey2(void);
//uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
//void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
//void bsp_ClearKey(void);

#endif
