
#include  "main.h"
#include "Manual.h"

#define 		MEMDATA_LEN    200
static uint8_t  Tbl_MemData[MEMDATA_LEN] = {0};

char Device_SN[18]={"test00110000000001"};
const char Device_Prefix[8] = {"sgszhdgs"};

/********************************************************************
*	功能	：	写入数组【4字节参数】
******************************************************************************/
static void Array_Write(uint32_t addrbase, uint64_t value, uint8_t nbytes)
{
	if(nbytes == 8)																		//PULSE参数可能会出现大于4字节情况
	{
		Tbl_MemData[addrbase]		= value / 256 / 256 / 256 / 256 / 256 / 256 / 256;
		Tbl_MemData[addrbase + 1]	= value / 256 / 256 / 256 / 256 / 256 / 256 % 256;
		Tbl_MemData[addrbase + 2]	= value / 256 / 256 / 256 / 256 / 256 % 256;
		Tbl_MemData[addrbase + 3]	= value / 256 / 256 / 256 / 256 % 256;
		Tbl_MemData[addrbase + 4]	= value / 256 / 256 / 256 % 256;
		Tbl_MemData[addrbase + 5]	= value / 256 / 256 % 256;
		Tbl_MemData[addrbase + 6]	= value / 256 % 256;
		Tbl_MemData[addrbase + 7]	= value % 256;
	}
	else if(nbytes == 4)
	{
		Tbl_MemData[addrbase]		= value / 256 / 256 / 256;
		Tbl_MemData[addrbase + 1]	= value / 256 / 256 % 256;
		Tbl_MemData[addrbase + 2]	= value / 256 % 256;
		Tbl_MemData[addrbase + 3]	= value % 256;
	}
	else if(nbytes == 2)
	{
		Tbl_MemData[addrbase]		= value / 256 % 256;
		Tbl_MemData[addrbase + 1]	= value % 256;
	}
	else if(nbytes == 1)
	{
		Tbl_MemData[addrbase]		= value % 256;
	}
	else{}
}

/********************************************************************
*	功能	：	读取数组【4字节参数】
******************************************************************************/
static uint64_t Array_Read(uint32_t addrbase, uint8_t nbytes)
{
	uint64_t value = 0;	
	if(nbytes == 8)
	{
		value = (uint64_t)Tbl_MemData[addrbase] * 256 * 256 * 256 * 256 * 256 * 256 * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 1] * 256 * 256 * 256 * 256 * 256 * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 2] * 256 * 256 * 256 * 256 * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 3] * 256 * 256 * 256 * 256
				+ (uint64_t)Tbl_MemData[addrbase + 4] * 256 * 256 * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 5] * 256 * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 6] * 256 
				+ (uint64_t)Tbl_MemData[addrbase + 7];
	}
	else if(nbytes == 4)
	{
		value = Tbl_MemData[addrbase] * 256 * 256 * 256 + Tbl_MemData[addrbase + 1] * 256 * 256 + Tbl_MemData[addrbase + 2] * 256 + Tbl_MemData[addrbase + 3];
	}
	else if(nbytes == 2)
	{
		value = Tbl_MemData[addrbase] * 256 + Tbl_MemData[addrbase + 1];
	}
	else if(nbytes == 1)
	{
		value = Tbl_MemData[addrbase];
	}
	else{}
	
	return(value);
}

/********************************************************************
*	功能	：	超限处理
******************************************************************************/
void Memory_OverLimit_Process(uint64_t *value, uint64_t ceiling)
{
	if(*value > ceiling)
	{
		*value = 0;
	}
	else{}
}

/********************************************************************
*	功能	：	加载配置
******************************************************************************/
void Memory_ConfigLoad(void)
{	
	AT24CXX_Read(512, (void *)&sMemData, sizeof(sMemData));
										
	if((sMemData.sCheckdata_A != 0x7e)  || (sMemData.sCheckdata_B != 0x45) )	//读取失败，加载默认值
	{
		log_info("Load par error,set to default value!\r\n");
		
		Switch.ModePre = 0;
		Switch.ModeCur = (SELECT_VC_V << MODE_BIT_VC) | (SELECT_FLIP_UN << MODE_BIT_FLIP);
		Switch_Ctrl(UO_PHASE_UNIPHASE);
		
		bsp_PutKey(BTN_SINGLE * 3 + 1);
		
		UserOperation.V_ModeSingle.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_V) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.V_ModeFreeRun.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_V) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.V_ModeTrain.fUnit		|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_V) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.V_ModeExtBnc.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_V) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.C_ModeSingle.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_C) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.C_ModeFreeRun.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_C) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.C_ModeTrain.fUnit		|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_C) | (1 << UO_UNIT_BIT_DURATION);
		UserOperation.C_ModeExtBnc.fUnit	|= (1 << UO_UNIT_BIT_PULSE) | (1 << UO_UNIT_BIT_AMPL_C) | (1 << UO_UNIT_BIT_DURATION);
		
		
		UserOperation.fMode					= UO_MODE_NONE;								//初始化为无模式后与当前模式比较处理，避免初次上电投入按键值无法正常刷新情况，因轮询执行，先后无关
		UserOperation.fUnitCur				= UserOperation.V_ModeSingle.fUnit;
		
		sMemData.sCheckdata_A = 0x7e;

		sMemData.bPhase 							= UO_PHASE_UNIPHASE;	
		sMemData.bVC 								= (Switch.ModeCur >> MODE_BIT_VC) & 0x01;
		sMemData.fMode_VPre							= UO_MODE_SINGLE;
		sMemData.fMode_CPre							= UO_MODE_SINGLE;

		sMemData.V_ModeSingle_fUnit					= UserOperation.V_ModeSingle.fUnit;
		sMemData.V_ModeSingle_ParamNAD_PULSE		= 0;
		sMemData.V_ModeSingle_ParamNAD_AMPL			= 0;
		sMemData.V_ModeSingle_Param_PULSE			= 10000000;	//10ms
		sMemData.V_ModeSingle_Param_AMPL			= 3000000;	//3V

		sMemData.V_ModeFreeRun_fUnit				= UserOperation.V_ModeFreeRun.fUnit;
		sMemData.V_ModeFreeRun_ParamNAD_PULSE		= 0,
		sMemData.V_ModeFreeRun_ParamNAD_AMPL		= 0,
		sMemData.V_ModeFreeRun_ParamNAD_FREQ		= 0,
		sMemData.V_ModeFreeRun_Param_PULSE			= 10000000,
		sMemData.V_ModeFreeRun_Param_AMPL			= 3000000,
		sMemData.V_ModeFreeRun_Param_FREQ			= 1000000,	//1Hz

		sMemData.V_ModeTrain_fUnit					= UserOperation.V_ModeTrain.fUnit;
		sMemData.V_ModeTrain_ParamNAD_PULSE			= 0;
		sMemData.V_ModeTrain_ParamNAD_AMPL			= 0;
		sMemData.V_ModeTrain_ParamNAD_FREQ			= 0;
		sMemData.V_ModeTrain_ParamNAD_DURATION		= 0;
		sMemData.V_ModeTrain_Param_PULSE			= 10000000;
		sMemData.V_ModeTrain_Param_AMPL				= 3000000;
		sMemData.V_ModeTrain_Param_FREQ				= 1000000;
		sMemData.V_ModeTrain_Param_DURATION			= 10000000;	//10s

		sMemData.V_ModeExtBnc_fUnit					= UserOperation.V_ModeExtBnc.fUnit;
		sMemData.V_ModeExtBnc_ParamNAD_AMPL			= 0;
		sMemData.V_ModeExtBnc_Param_AMPL			= 3000000;

		sMemData.C_ModeSingle_fUnit					= UserOperation.C_ModeSingle.fUnit;
		sMemData.C_ModeSingle_ParamNAD_PULSE		= 0;
		sMemData.C_ModeSingle_ParamNAD_AMPL			= 0;
		sMemData.C_ModeSingle_Param_PULSE			= 10000000;
		sMemData.C_ModeSingle_Param_AMPL			= 1000000;

		sMemData.C_ModeFreeRun_fUnit				= UserOperation.C_ModeFreeRun.fUnit;
		sMemData.C_ModeFreeRun_ParamNAD_PULSE		= 0;
		sMemData.C_ModeFreeRun_ParamNAD_AMPL		= 0;
		sMemData.C_ModeFreeRun_ParamNAD_FREQ		= 0;
		sMemData.C_ModeFreeRun_Param_PULSE			= 10000000;
		sMemData.C_ModeFreeRun_Param_AMPL			= 1000000;
		sMemData.C_ModeFreeRun_Param_FREQ			= 1000000;

		sMemData.C_ModeTrain_fUnit					= UserOperation.C_ModeTrain.fUnit;
		sMemData.C_ModeTrain_ParamNAD_PULSE			= 0;
		sMemData.C_ModeTrain_ParamNAD_AMPL			= 0;
		sMemData.C_ModeTrain_ParamNAD_FREQ			= 0;
		sMemData.C_ModeTrain_ParamNAD_DURATION		= 0;
		sMemData.C_ModeTrain_Param_PULSE			= 10000000;
		sMemData.C_ModeTrain_Param_AMPL				= 1000000;
		sMemData.C_ModeTrain_Param_FREQ				= 1000000;
		sMemData.C_ModeTrain_Param_DURATION			= 10000000;

		sMemData.C_ModeExtBnc_fUnit					= UserOperation.C_ModeExtBnc.fUnit;
		sMemData.C_ModeExtBnc_ParamNAD_AMPL			= 0;
		sMemData.C_ModeExtBnc_Param_AMPL			= 1000000;

		sMemData.V_Wave_type						= 0;
		sMemData.C_Wave_type						= 0;
		sMemData.V_Bnc_Pulse						= 0;
		sMemData.C_Bnc_Pulse						= 0;
		sMemData.MCU_POR_Times						= 0;

		sMemData.sCheckdata_B = 0x45;																													
		
		AT24CXX_Write(512, (void *)&sMemData, sizeof(sMemData));
		
		UserOperation.V_ModeSingle.fUnit						= sMemData.V_ModeSingle_fUnit				;
		UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_PULSE] 	= sMemData.V_ModeSingle_ParamNAD_PULSE		;
		UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeSingle_ParamNAD_AMPL		;
		UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE]		= sMemData.V_ModeSingle_Param_PULSE			;
		UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL]			= sMemData.V_ModeSingle_Param_AMPL			;
		UserOperation.V_ModeFreeRun.fUnit						= sMemData.V_ModeFreeRun_fUnit				;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_PULSE]	= sMemData.V_ModeFreeRun_ParamNAD_PULSE		;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_AMPL] 	= sMemData.V_ModeFreeRun_ParamNAD_AMPL		;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_FREQ]		= sMemData.V_ModeFreeRun_ParamNAD_FREQ		;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE]		= sMemData.V_ModeFreeRun_Param_PULSE		;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL]		= sMemData.V_ModeFreeRun_Param_AMPL			;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ]		= sMemData.V_ModeFreeRun_Param_FREQ			;
		UserOperation.V_ModeTrain.fUnit							= sMemData.V_ModeTrain_fUnit				;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_PULSE] 		= sMemData.V_ModeTrain_ParamNAD_PULSE		;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeTrain_ParamNAD_AMPL		;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_FREQ] 		= sMemData.V_ModeTrain_ParamNAD_FREQ        ;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_DURATION]	= sMemData.V_ModeTrain_ParamNAD_DURATION    ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]			= sMemData.V_ModeTrain_Param_PULSE          ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]			= sMemData.V_ModeTrain_Param_AMPL           ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]			= sMemData.V_ModeTrain_Param_FREQ	        ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]		= sMemData.V_ModeTrain_Param_DURATION       ;
		UserOperation.V_ModeExtBnc.fUnit						= sMemData.V_ModeExtBnc_fUnit               ;
		UserOperation.V_ModeExtBnc.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeExtBnc_ParamNAD_AMPL       ;
		UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL]			= sMemData.V_ModeExtBnc_Param_AMPL          ;
																												
		UserOperation.C_ModeSingle.fUnit						= sMemData.C_ModeSingle_fUnit               ;
		UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_PULSE] 	= sMemData.C_ModeSingle_ParamNAD_PULSE      ;
		UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeSingle_ParamNAD_AMPL       ;
		UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE]		= sMemData.C_ModeSingle_Param_PULSE         ;
		UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL]			= sMemData.C_ModeSingle_Param_AMPL          ;
		UserOperation.C_ModeFreeRun.fUnit						= sMemData.C_ModeFreeRun_fUnit              ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_PULSE] 	= sMemData.C_ModeFreeRun_ParamNAD_PULSE     ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_AMPL] 	= sMemData.C_ModeFreeRun_ParamNAD_AMPL      ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_FREQ] 	= sMemData.C_ModeFreeRun_ParamNAD_FREQ      ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE]		= sMemData.C_ModeFreeRun_Param_PULSE        ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL]		= sMemData.C_ModeFreeRun_Param_AMPL         ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ]		= sMemData.C_ModeFreeRun_Param_FREQ         ;
		UserOperation.C_ModeTrain.fUnit							= sMemData.C_ModeTrain_fUnit                ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_PULSE] 		= sMemData.C_ModeTrain_ParamNAD_PULSE       ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeTrain_ParamNAD_AMPL        ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_FREQ] 		= sMemData.C_ModeTrain_ParamNAD_FREQ        ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_DURATION]	= sMemData.C_ModeTrain_ParamNAD_DURATION    ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE]			= sMemData.C_ModeTrain_Param_PULSE          ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL]			= sMemData.C_ModeTrain_Param_AMPL           ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ]			= sMemData.C_ModeTrain_Param_FREQ           ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION]		= sMemData.C_ModeTrain_Param_DURATION       ;
		UserOperation.C_ModeExtBnc.fUnit						= sMemData.C_ModeExtBnc_fUnit               ;
		UserOperation.C_ModeExtBnc.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeExtBnc_ParamNAD_AMPL       ;
		UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL]			= sMemData.C_ModeExtBnc_Param_AMPL          ;		
	}
	else																					//EEPROM 读取成功
	{		
		UserOperation.bPhase									= sMemData.bPhase 							;
		UserOperation.bVC										= sMemData.bVC 								;
		UserOperation.fMode_VPre								= sMemData.fMode_VPre						;
		UserOperation.fMode_CPre								= sMemData.fMode_CPre						;
		
		UserOperation.V_ModeSingle.fUnit						= sMemData.V_ModeSingle_fUnit				;
		UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_PULSE] 	= sMemData.V_ModeSingle_ParamNAD_PULSE		;
		UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeSingle_ParamNAD_AMPL		;
		UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE]		= sMemData.V_ModeSingle_Param_PULSE			;
		UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL]			= sMemData.V_ModeSingle_Param_AMPL			;
		UserOperation.V_ModeFreeRun.fUnit						= sMemData.V_ModeFreeRun_fUnit				;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_PULSE]	= sMemData.V_ModeFreeRun_ParamNAD_PULSE		;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_AMPL] 	= sMemData.V_ModeFreeRun_ParamNAD_AMPL		;
		UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_FREQ]		= sMemData.V_ModeFreeRun_ParamNAD_FREQ		;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE]		= sMemData.V_ModeFreeRun_Param_PULSE		;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL]		= sMemData.V_ModeFreeRun_Param_AMPL			;
		UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ]		= sMemData.V_ModeFreeRun_Param_FREQ			;
		UserOperation.V_ModeTrain.fUnit							= sMemData.V_ModeTrain_fUnit				;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_PULSE] 		= sMemData.V_ModeTrain_ParamNAD_PULSE		;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeTrain_ParamNAD_AMPL		;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_FREQ] 		= sMemData.V_ModeTrain_ParamNAD_FREQ        ;
		UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_DURATION]	= sMemData.V_ModeTrain_ParamNAD_DURATION    ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]			= sMemData.V_ModeTrain_Param_PULSE          ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]			= sMemData.V_ModeTrain_Param_AMPL           ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]			= sMemData.V_ModeTrain_Param_FREQ	        ;
		UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]		= sMemData.V_ModeTrain_Param_DURATION       ;
		UserOperation.V_ModeExtBnc.fUnit						= sMemData.V_ModeExtBnc_fUnit               ;
		UserOperation.V_ModeExtBnc.ParamNAD[UO_PARAM_AMPL] 		= sMemData.V_ModeExtBnc_ParamNAD_AMPL       ;
		UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL]			= sMemData.V_ModeExtBnc_Param_AMPL          ;
																																																	
		UserOperation.C_ModeSingle.fUnit						= sMemData.C_ModeSingle_fUnit               ;
		UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_PULSE] 	= sMemData.C_ModeSingle_ParamNAD_PULSE      ;
		UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeSingle_ParamNAD_AMPL       ;
		UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE]		= sMemData.C_ModeSingle_Param_PULSE         ;
		UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL]			= sMemData.C_ModeSingle_Param_AMPL          ;
		UserOperation.C_ModeFreeRun.fUnit						= sMemData.C_ModeFreeRun_fUnit              ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_PULSE] 	= sMemData.C_ModeFreeRun_ParamNAD_PULSE     ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_AMPL] 	= sMemData.C_ModeFreeRun_ParamNAD_AMPL      ;
		UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_FREQ] 	= sMemData.C_ModeFreeRun_ParamNAD_FREQ      ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE]		= sMemData.C_ModeFreeRun_Param_PULSE        ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL]		= sMemData.C_ModeFreeRun_Param_AMPL         ;
		UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ]		= sMemData.C_ModeFreeRun_Param_FREQ         ;
		UserOperation.C_ModeTrain.fUnit							= sMemData.C_ModeTrain_fUnit                ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_PULSE] 		= sMemData.C_ModeTrain_ParamNAD_PULSE       ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeTrain_ParamNAD_AMPL        ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_FREQ] 		= sMemData.C_ModeTrain_ParamNAD_FREQ        ;
		UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_DURATION]	= sMemData.C_ModeTrain_ParamNAD_DURATION    ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE]			= sMemData.C_ModeTrain_Param_PULSE          ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL]			= sMemData.C_ModeTrain_Param_AMPL           ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ]			= sMemData.C_ModeTrain_Param_FREQ           ;
		UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION]		= sMemData.C_ModeTrain_Param_DURATION       ;
		UserOperation.C_ModeExtBnc.fUnit						= sMemData.C_ModeExtBnc_fUnit               ;
		UserOperation.C_ModeExtBnc.ParamNAD[UO_PARAM_AMPL] 		= sMemData.C_ModeExtBnc_ParamNAD_AMPL       ;
		UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL]			= sMemData.C_ModeExtBnc_Param_AMPL          ;
		
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]);
		
		
	
		
		#if 1																			//避免读取AT24C64异常导致显示错乱
			Memory_OverLimit_Process(&UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE],		CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL],		CEILING_V_AMPLITUDE);			
			Memory_OverLimit_Process(&UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE],	CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL],		CEILING_V_AMPLITUDE);
			Memory_OverLimit_Process(&UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ],		CEILING_FREQUENCY);			
			Memory_OverLimit_Process(&UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE],		CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL],		CEILING_V_AMPLITUDE);
			Memory_OverLimit_Process(&UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ],		CEILING_FREQUENCY);
			Memory_OverLimit_Process(&UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION],	CEILING_DURATION);			
			Memory_OverLimit_Process(&UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL],		CEILING_V_AMPLITUDE);
			
			Memory_OverLimit_Process(&UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE],		CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL],		CEILING_C_AMPLITUDE);
			Memory_OverLimit_Process(&UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE],	CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL],		CEILING_C_AMPLITUDE);
			Memory_OverLimit_Process(&UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ],		CEILING_FREQUENCY);
			Memory_OverLimit_Process(&UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE],		CEILING_PULSE);
			Memory_OverLimit_Process(&UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL],		CEILING_C_AMPLITUDE);
			Memory_OverLimit_Process(&UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ],		CEILING_FREQUENCY);
			Memory_OverLimit_Process(&UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION],	CEILING_DURATION);
			Memory_OverLimit_Process(&UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL],		CEILING_C_AMPLITUDE);
		#endif
		
		log_info("Read1 UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL]:%lld\r\n",UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL]);
		
		Switch.ModePre = 0;																	//读取后考虑对开关等的处理
		Switch.ModeCur = UserOperation.bVC << MODE_BIT_VC;   //MODE_BIT_VC		0
		Switch_Ctrl(UO_PHASE_UNIPHASE);

		UserOperation.fMode = UO_MODE_NONE;
		
		switch(UserOperation.bVC)															//对参数标志位进行读取
		{
			case SELECT_VC_V:
				switch(UserOperation.fMode_VPre)
				{
					case UO_MODE_SINGLE:
						UserOperation.fUnitCur = UserOperation.V_ModeSingle.fUnit;
						bsp_PutKey(BTN_SINGLE * 3 + 1);
						break;
					
					case UO_MODE_FREERUN:
						UserOperation.fUnitCur = UserOperation.V_ModeFreeRun.fUnit;
						bsp_PutKey(BTN_FREERUN * 3 + 1);
						break;
					
					case UO_MODE_TRAIN:						
						UserOperation.fUnitCur = UserOperation.V_ModeTrain.fUnit;
						bsp_PutKey(BTN_TRAIN * 3 + 1);
						break;
					
					case UO_MODE_EXTBNC:						
						UserOperation.fUnitCur = UserOperation.V_ModeExtBnc.fUnit;
						bsp_PutKey(BTN_EXTBNC * 3 + 1);
						break;
					
					default:
						break;
				}
				break;
			
			case SELECT_VC_C:
				switch(UserOperation.fMode_CPre)
				{
					case UO_MODE_SINGLE:
						UserOperation.fUnitCur = UserOperation.C_ModeSingle.fUnit;	
						bsp_PutKey(BTN_SINGLE * 3 + 1);
						break;
					
					case UO_MODE_FREERUN:
						UserOperation.fUnitCur = UserOperation.C_ModeFreeRun.fUnit;
						bsp_PutKey(BTN_FREERUN * 3 + 1);
						break;
					
					case UO_MODE_TRAIN:
						UserOperation.fUnitCur = UserOperation.C_ModeTrain.fUnit;
						bsp_PutKey(BTN_TRAIN * 3 + 1);
						break;
					
					case UO_MODE_EXTBNC:
						UserOperation.fUnitCur = UserOperation.C_ModeExtBnc.fUnit;
						bsp_PutKey(BTN_EXTBNC * 3 + 1);
						break;
					
					default:
						break;
				}
				break;
		}
	}
	
	
	AT24CXX_Read(230, (void *)&sAdditionalData, sizeof(sAdditionalData));
	
	if(sAdditionalData.V_Wave_type>3)sAdditionalData.V_Wave_type=0;
	else{}
	if(sAdditionalData.C_Wave_type>3)sAdditionalData.C_Wave_type=0;
	else{}
	
	log_info("V_Wave_type:%d,C_Wave_type:%d\r\n",sAdditionalData.V_Wave_type,sAdditionalData.C_Wave_type);
		
	if(UserOperation.bVC == SELECT_VC_V)
	{
		Wave_type = sAdditionalData.V_Wave_type;
		UserOperation.V_ModeExtBnc.Param[UO_PARAM_PULSE] = sAdditionalData.V_Bnc_Pulse;
	}
	else
	{
		Wave_type = sAdditionalData.C_Wave_type;
		UserOperation.C_ModeExtBnc.Param[UO_PARAM_PULSE] = sAdditionalData.C_Bnc_Pulse;
	}
	
	
	#if(TEST_AT24CXX)
		AT24CXX_Read(0, Tbl_MemData, MEMDATA_LEN);
		printf("Tbl_MemData[0] = %d\r\n", Tbl_MemData[0]);
		printf("Tbl_MemData[1] = %d\r\n", Tbl_MemData[1]);
		printf("Tbl_MemData[2] = %d\r\n", Tbl_MemData[2]);
	#endif
}


void Get_MCU_POR_Times(void)
{
	AT24CXX_Read(230, (void *)&sAdditionalData, sizeof(sAdditionalData));
	sAdditionalData.MCU_POR_Times++;
	AT24CXX_Write(230, (void *)&sAdditionalData, sizeof(sAdditionalData));
}



void test_eeprom(void)
{
	uint16_t ix=0;
	sMemData_t	sMemData_tt={0};
	
	uint16_t len=0;
	
	len=sizeof(sMemData_tt);
	
	AT24CXX_Read(512, (void *)&sMemData_tt, len);
	
	log_info("test_eeprom\r\n");
	log_info("sMemData_tt.C_ModeExtBnc_ParamNAD_AMPL:%d\r\n",sMemData_tt.C_ModeExtBnc_ParamNAD_AMPL);
	log_info("sMemData_tt.C_ModeExtBnc_Param_AMPL:%lld\r\n",sMemData_tt.C_ModeExtBnc_Param_AMPL);
}



/********************************************************************
*	功能	：	轮询保存用户数据
******************************************************************************/
void Memory_Poll(void)
{
	if(T6.MemoryUpdateCnt >= T6_VALUE_MINUTE)												//如有数据更新则60s后刷新存储
	{																															
		sMemData.sCheckdata_A = 0x7e;

		sMemData.bPhase 							= UserOperation.bPhase;		
		sMemData.bVC 								= UserOperation.bVC;
		sMemData.fMode_VPre							= UserOperation.fMode_VPre;
		sMemData.fMode_CPre							= UserOperation.fMode_CPre;

		sMemData.V_ModeSingle_fUnit					= UserOperation.V_ModeSingle.fUnit;
		sMemData.V_ModeSingle_ParamNAD_PULSE		= UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_PULSE];
		sMemData.V_ModeSingle_ParamNAD_AMPL			= UserOperation.V_ModeSingle.ParamNAD[UO_PARAM_AMPL];
		sMemData.V_ModeSingle_Param_PULSE			= UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE];
		sMemData.V_ModeSingle_Param_AMPL			= UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL];

		sMemData.V_ModeFreeRun_fUnit				= UserOperation.V_ModeFreeRun.fUnit;
		sMemData.V_ModeFreeRun_ParamNAD_PULSE		= UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_PULSE];
		sMemData.V_ModeFreeRun_ParamNAD_AMPL		= UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_AMPL];
		sMemData.V_ModeFreeRun_ParamNAD_FREQ		= UserOperation.V_ModeFreeRun.ParamNAD[UO_PARAM_FREQ];
		sMemData.V_ModeFreeRun_Param_PULSE			= UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE];
		sMemData.V_ModeFreeRun_Param_AMPL			= UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL];
		sMemData.V_ModeFreeRun_Param_FREQ			= UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ];

		sMemData.V_ModeTrain_fUnit					= UserOperation.V_ModeTrain.fUnit;
		sMemData.V_ModeTrain_ParamNAD_PULSE			= UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_PULSE];
		sMemData.V_ModeTrain_ParamNAD_AMPL			= UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_AMPL];
		sMemData.V_ModeTrain_ParamNAD_FREQ			= UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_FREQ];
		sMemData.V_ModeTrain_ParamNAD_DURATION		= UserOperation.V_ModeTrain.ParamNAD[UO_PARAM_DURATION];
		sMemData.V_ModeTrain_Param_PULSE			= UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE];
		sMemData.V_ModeTrain_Param_AMPL				= UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL];
		sMemData.V_ModeTrain_Param_FREQ				= UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ];
		sMemData.V_ModeTrain_Param_DURATION			= UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION];

		sMemData.V_ModeExtBnc_fUnit					= UserOperation.V_ModeExtBnc.fUnit;
		sMemData.V_ModeExtBnc_ParamNAD_AMPL			= UserOperation.V_ModeExtBnc.ParamNAD[UO_PARAM_AMPL];
		sMemData.V_ModeExtBnc_Param_AMPL			= UserOperation.V_ModeExtBnc.Param[UO_PARAM_AMPL];

		sMemData.C_ModeSingle_fUnit					= UserOperation.C_ModeSingle.fUnit;
		sMemData.C_ModeSingle_ParamNAD_PULSE		= UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_PULSE];
		sMemData.C_ModeSingle_ParamNAD_AMPL			= UserOperation.C_ModeSingle.ParamNAD[UO_PARAM_AMPL];
		sMemData.C_ModeSingle_Param_PULSE			= UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE];
		sMemData.C_ModeSingle_Param_AMPL			= UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL];

		sMemData.C_ModeFreeRun_fUnit				= UserOperation.C_ModeFreeRun.fUnit;
		sMemData.C_ModeFreeRun_ParamNAD_PULSE		= UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_PULSE];
		sMemData.C_ModeFreeRun_ParamNAD_AMPL		= UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_AMPL];
		sMemData.C_ModeFreeRun_ParamNAD_FREQ		= UserOperation.C_ModeFreeRun.ParamNAD[UO_PARAM_FREQ];
		sMemData.C_ModeFreeRun_Param_PULSE			= UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE];
		sMemData.C_ModeFreeRun_Param_AMPL			= UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL];
		sMemData.C_ModeFreeRun_Param_FREQ			= UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ];

		sMemData.C_ModeTrain_fUnit					= UserOperation.C_ModeTrain.fUnit;
		sMemData.C_ModeTrain_ParamNAD_PULSE			= UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_PULSE];
		sMemData.C_ModeTrain_ParamNAD_AMPL			= UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_AMPL];
		sMemData.C_ModeTrain_ParamNAD_FREQ			= UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_FREQ];
		sMemData.C_ModeTrain_ParamNAD_DURATION		= UserOperation.C_ModeTrain.ParamNAD[UO_PARAM_DURATION];
		sMemData.C_ModeTrain_Param_PULSE			= UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE];
		sMemData.C_ModeTrain_Param_AMPL				= UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL];
		sMemData.C_ModeTrain_Param_FREQ				= UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ];
		sMemData.C_ModeTrain_Param_DURATION			= UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION];

		sMemData.C_ModeExtBnc_fUnit					= UserOperation.C_ModeExtBnc.fUnit;
		sMemData.C_ModeExtBnc_ParamNAD_AMPL			= UserOperation.C_ModeExtBnc.ParamNAD[UO_PARAM_AMPL];
		sMemData.C_ModeExtBnc_Param_AMPL			= UserOperation.C_ModeExtBnc.Param[UO_PARAM_AMPL];

		sMemData.V_Wave_type						= 0;
		sMemData.C_Wave_type						= 0;
		sMemData.V_Bnc_Pulse						= 0;
		sMemData.C_Bnc_Pulse						= 0;
		sMemData.MCU_POR_Times						= 0;

		sMemData.sCheckdata_B = 0x45;																													
		
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ]);
		log_info("UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]:%lld\r\n",UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION]);
		
		log_info("UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE]:%lld\r\n",UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE]);
		log_info("UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL]:%lld\r\n",UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL]);
		log_info("UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ]:%lld\r\n",UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ]);
		log_info("UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION]:%lld\r\n",UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION]);
		
		
		AT24CXX_Write(512, (void *)&sMemData, sizeof(sMemData));
		
		T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_CLEAR;
	}
}
