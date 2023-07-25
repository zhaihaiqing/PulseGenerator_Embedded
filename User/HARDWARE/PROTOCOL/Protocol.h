
/********************************************************************
*
*	文件	：	Protocol.h

*	管脚	：	(NONE)

*	功能	：	串口数据接收及应答
				待后续制定通信协议、规整单位及测试
*
******************************************************************************/
#ifndef __PROTOCOL_H
#define __PROTOCOL_H
#include "main.h"

#define PROTOCOL_FIXED_HEAD			0X36
#define PROTOCOL_FIXED_END			0X32
#define PROTOCOL_FIXED_VERSION		0X01

#define ADDR_OFFSET_HEAD			0
#define ADDR_OFFSET_NUM_H			1
#define ADDR_OFFSET_NUM_L			2
#define ADDR_OFFSET_VERSION			3
#define ADDR_OFFSET_COMMAND			4
#define ADDR_OFFSET_RESERVE			5

#define ADDR_OFFSET_CONTENT_START	6				//若数据帧中有，如波形配置、暂停继续停止、设置定时等信息

#define COMMAND_START	0X01
#define COMMAND_STOP	0X02
#define COMMAND_QUERY	0X03
#define COMMAND_CONFIG	0X10
#define COMMAND_ALARM	0X30
#define COMMAND_PAUSE	0X40

#define BTOL( ray ) ( (((uint32_t) (ray)[0]) <<24) + (((uint32_t)(ray)[1])<<16) + (((uint32_t)(ray)[2])<<8) + (ray)[3] )
#define BTOW( ray ) ( (((uint16_t) (ray)[0]) <<8) + (ray)[1] )

typedef enum EPULSE_DO								//通道号,data output number
{
	PULSE_DO_0 = 0,
	PULSE_DO_1,
	PULSE_DO_2,
	PULSE_DO_3,
	PULSE_DO_4,
	PULSE_DO_5,
	PULSE_DO_6,
	PULSE_DO_7,
	PULSE_DO_ALL = 8,
	PULSE_DO_NO = 0XFF
}ePulseDO_t;

typedef enum EPULSE_TYPE							//波形类型
{
	PULSE_TYPE_ZERO = 0,							//打点实现复杂波形
	PULSE_TYPE_ONE,
	PULSE_TYPE_TWO,
	PULSE_TYPE_THREE,
	PULSE_TYPE_FOUR,
	PULSE_TYPE_FIVE
}ePulseType_t;

#define PULSE_ZERO_TBL_MAX_NUM			1000		//使用动态内存分配比较合适，待后续改进
__packed typedef struct SPULSEZERO
{
	uint16_t IndexNum[8];							//打点数量
	uint32_t *TblTime[8];							//单点持续时长数组(X100us)
	uint32_t *TblAmpl[8];							//单点幅值(mV/uA)
}sPulseZero_t;

__packed typedef struct SPULSEONE
{
	uint8_t  IsLowFirst;							//1-先高后低，0-先低后高
	uint32_t MsStageFirst;							//第一阶段持续时间
	uint32_t MsStageSecond;							//第二阶段持续时间
	uint32_t Ampl;									//幅值(mV/uA)
}sPulseOne_t;

__packed typedef struct SPULSETWO
{	
	uint32_t Freq;									//0.01Hz
	uint32_t Duty;									//50000分之X
	uint32_t MsDuration;							//持续时间
	uint32_t Ampl;									//幅值(mV/uA)
}sPulseTwo_t;

__packed typedef struct SPULSETHREE
{
	uint32_t Period;								//重复输出波的周期ms
	uint32_t HighTime;								//固定不变高电平时长
	uint32_t PulseNum;								//波形连续个数
	uint16_t Pause100ms;							//暂停时间
	uint16_t MsBegin;								//间隔起始时长
	uint16_t MsEnd;									//间隔截止时长
	uint16_t MsStep;								//间隔步进时长
	uint32_t CycleNum;								//循环次数
	uint32_t Ampl;									//幅值(mV/uA)
}sPulseThree_t;

__packed typedef struct SPULSEFOUR
{
	uint32_t PeriodBegin;							//起始周期
	uint32_t HighTime;								//固定不变高电平时长
	uint32_t PulseNum;								//相同周期波形连续个数
	uint16_t Pause100ms;							//暂停时间
	uint32_t PeriodEnd;								//结束周期
	uint32_t PeriodStep;							//步进周期
	uint32_t CycleNum;								//循环次数
	uint32_t Ampl;									//幅值(mV/uA)
}sPulseFour_t;

#define PULSE_FIVE_TBL_MAX_NUM			1000		//使用动态内存分配比较合适，待后续改进
__packed typedef struct SPULSEFIVE
{
	uint32_t Freq;
	uint16_t Duty;									//占空比固定50%，定义此变量便于扩展及兼容
	uint32_t ConfigNum;								//配置数量
	uint16_t MsPause[PULSE_FIVE_TBL_MAX_NUM];		//暂停时间
	uint32_t MsDuration[PULSE_FIVE_TBL_MAX_NUM];	//持续时间
	uint32_t Ampl;									//幅值(mV/uA)
}sPulseFive_t;

__packed typedef union UPULSEPARAM
{
	sPulseZero_t	PulseZero;
	sPulseOne_t		PulseOne;
	sPulseTwo_t		PulseTwo;
	sPulseThree_t	PulseThree;
	sPulseFour_t	PulseFour;
	sPulseFive_t	PulseFive;
}uPulseParam_t;


__packed typedef struct SPULSECONF					//波形配置参数结构体
{
	ePulseDO_t			Channel;
	ePulseType_t		PulseType;					//波形类型
	uPulseParam_t		PulseParam;					//用户配置参数接收，用于解析转化
}sPulseConf_t;
extern sPulseConf_t PulseConf;


extern __IO uint32_t tim5_arr;
extern __IO uint32_t train_count;	//TRAIN模式计数器，
extern __IO uint32_t train_acount;	//TRAIN模式需要计数的总数，例如：10S，Freq=10Hz，则总数=10*10=100个，注意，末尾不满1的舍去
extern __IO uint32_t pluse_Compensate;

uint8_t Process_COMMAND_START(void);
void Process_COMMAND_STOP(void);
void Process_COMMAND_PAUSE(void);
void Process_COMMAND_CONTINUE(void);

#endif
