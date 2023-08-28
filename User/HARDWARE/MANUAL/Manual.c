
#include  "main.h"

//Does it exceed the human safety threshold;


uint8_t is_exceed_human_safety	 = 0;	//定义变量，用于标识是否超过安全阈值（36V或10mA），在输出时进行提醒。
uint8_t is_refash_warn			 = 0;	//定义变量，用于单次刷新警告

void par_check(void);

sUserOperation_t UserOperation = {
									UO_PHASE_UNIPHASE, 
									0, 
									UO_MODE_NONE, 
									UO_PARAM_NONE, 
									UO_PARAM_NONE, 
									0, 
									UO_MODE_NONE, 
									{0}, 
									{0}, 
									{0}, 
									{0}, 
									UO_MODE_NONE, 
									0
								};

sAdditionalData_t sAdditionalData={0};		//附件的参数

sMemData_t	sMemData={0};

uint8_t Wave_type=0;						//波形模式，不改变按键识别架构，单独设置一个标志位用来记录波形类型（0-3,0代表正脉冲，1代表负脉冲，2代表双极性正脉冲在前，3代表双极性负脉冲在前）
//uint64_t bnc_pluse=0;

uint64_t Ceiling[4] = {CEILING_V_AMPLITUDE, CEILING_PULSE, CEILING_FREQUENCY, CEILING_DURATION};
/********************************************************************
*	功能	：	需进行设置的参数相关信息获取
*	流程	：	输出电压/电流--模式--参数--参数值、单位档
******************************************************************************/
static void SUB_SettingInfo_Get(uint8_t IsParamTypeChanged, uint8_t IsUnitSwitched, uint8_t unitbit)
{	
	if(IsParamTypeChanged)
	{
		UserOperation.GetSetupData.pParamNAD = &(UserOperation.GetSetupData.pModeParam->ParamNAD[UserOperation.fParamType]);
		UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType];
		UserOperation.GetSetupData.Ceiling = Ceiling[UserOperation.fParamType];
	}
	else
	{
		
	}
	if(IsUnitSwitched)
	{
		if((UserOperation.fUnitCur & (1 << unitbit)) == 0)
		{
			UserOperation.fUnitCur |= (1 << unitbit);
		}
		else
		{
			UserOperation.fUnitCur &= ~(1 << unitbit);
		}
		UserOperation.GetSetupData.pModeParam->fUnit = UserOperation.fUnitCur;											//保持切换后标识量，以备后续切换模式/参数回调
		
		UserOperation.Modify.bUnitSwitched = !UserOperation.Modify.bUnitSwitched;
	}
	else
	{
		
	}
	
	
	
	UserOperation.GetSetupData.bUnit = ((UserOperation.fUnitCur & (1 << unitbit)) > 0)? 1:0;							//[V88]保持更新设置参数的单位量标识信息，修复只切换参数时单位不匹配bug
	
	if(UserOperation.fParamType == 2)
	{
		UserOperation.GetSetupData.bUnit=1;
	}
	else
	{
		
	}
}
								
#define PARAMTYPECHANGE_N	0
#define PARAMTYPECHANGE_Y	1
#define UNITSWITCH_N		0
#define UNITSWITCH_Y		1
/********************************************************************
*	功能	：	即将设置的参数相关信息获取
*	流程	：	输出电压/电流--模式--参数--参数值、单位档
******************************************************************************/
static void SettingInfo_Get(uint8_t IsParamTypeChanged, uint8_t IsUnitSwitched)
{
	uint8_t unitbit = 0;
	
	if(IsParamTypeChanged == 0 && IsUnitSwitched == 0)
	{
		return;
	}
	else
	{
		
	}
	
	unitbit = UserOperation.fParamType;
	if(UserOperation.bVC == SELECT_VC_V)
	{
		Ceiling[UO_PARAM_AMPL] = CEILING_V_AMPLITUDE;
		
		switch(UserOperation.fMode)
		{
			case UO_MODE_SINGLE:
				UserOperation.GetSetupData.pModeParam = &UserOperation.V_ModeSingle;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_FREERUN:
				UserOperation.GetSetupData.pModeParam = &UserOperation.V_ModeFreeRun;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_TRAIN:
				UserOperation.GetSetupData.pModeParam = &UserOperation.V_ModeTrain;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_EXTBNC:
				UserOperation.GetSetupData.pModeParam = &UserOperation.V_ModeExtBnc;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			default:
				break;
		}
	}
	else
	{
		Ceiling[UO_PARAM_AMPL] = CEILING_C_AMPLITUDE;
		
		if(unitbit == UO_UNIT_BIT_AMPL_V)
		{
			unitbit = UO_UNIT_BIT_AMPL_C;
		}
		else
		{
		
		}
		
		switch(UserOperation.fMode)
		{
			case UO_MODE_SINGLE:
				UserOperation.GetSetupData.pModeParam = &UserOperation.C_ModeSingle;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_FREERUN:
				UserOperation.GetSetupData.pModeParam = &UserOperation.C_ModeFreeRun;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_TRAIN:
				UserOperation.GetSetupData.pModeParam = &UserOperation.C_ModeTrain;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			case UO_MODE_EXTBNC:
				UserOperation.GetSetupData.pModeParam = &UserOperation.C_ModeExtBnc;
				SUB_SettingInfo_Get(IsParamTypeChanged, IsUnitSwitched, unitbit);
				break;
			default:
				break;
		}
	}
	
	
}

/********************************************************************
*	功能	：	修改参数值
*	流程	：	输入0-9数字或←/→箭头设定参数
******************************************************************************/
void SettingInfo_Modify(uint8_t btn_value)
{
	uint32_t value_expand = VALUE_EXPAND_102;
	
	if(UserOperation.fMode == UO_MODE_NONE || UserOperation.fParamType == UO_PARAM_NONE)
	{
		return;
	}
	else
	{
		
	}	
	
	if(UserOperation.GetSetupData.bUnit == 1 )
	{
		value_expand = VALUE_EXPAND_105;
	}
	else
	{
		
	}
	
	switch(btn_value)
	{
		case KBD_INPUT_0:
		case KBD_INPUT_1:
		case KBD_INPUT_2:
		case KBD_INPUT_3:
		case KBD_INPUT_4:
		case KBD_INPUT_5:
		case KBD_INPUT_6:
		case KBD_INPUT_7:
		case KBD_INPUT_8:
		case KBD_INPUT_9:
			
			if((UserOperation.Modify.fStart == 1) || (UserOperation.Modify.bUnitSwitched == 1))				//首次输入数值或切换一次单位，设置值直接覆盖原值
			{
				UserOperation.Modify.fStart = 2;
				UserOperation.Modify.NumAfterDot = 0;
				
				UserOperation.Modify.bUnitSwitched = 0;
				
				*UserOperation.GetSetupData.pParamNAD = UserOperation.Modify.NumAfterDot;
				UserOperation.GetSetupData.ParamValue = btn_value * value_expand;
			}
			else																							//输入过程中，位数逐步增加，小数点后固定6位有效数字
			{
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					
					if(UserOperation.Modify.NumAfterDot == 4)
					{
						*UserOperation.GetSetupData.pParamNAD = 3;
					}
					else
					{
						*UserOperation.GetSetupData.pParamNAD = UserOperation.Modify.NumAfterDot;
					}					
					switch(UserOperation.Modify.NumAfterDot)
					{
						case 0:												//YYYX.YYYYYY
							UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue * 10 + btn_value * value_expand;
							break;
						
						case 1:												//YYYY.XYYYYY							
							UserOperation.Modify.NumAfterDot++;						
							UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue + btn_value * value_expand / 10;
							break;
						
						case 2:												//YYYY.YXYYYY
							UserOperation.Modify.NumAfterDot++;
							UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue + btn_value * value_expand / 100;
							break;
						
						case 3:						
							UserOperation.Modify.NumAfterDot++;
							UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue + btn_value * value_expand / 1000;
							break;
						
						default:
							break;
					}					
				}
				else
				{			
					UserOperation.Modify.NumAfterDot = 4;
					
					*UserOperation.GetSetupData.pParamNAD = 3;
					UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue * 10 + btn_value * value_expand;
				}
			}
			
			break;
			
		case KBD_INPUT_BACKSPACE:
			
			if(UserOperation.Modify.fStart == 1)
			{
				UserOperation.Modify.fStart = 2;
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					UserOperation.Modify.NumAfterDot = 4;
					
					
					*UserOperation.GetSetupData.pParamNAD = 3;
				}
				else{}
			}
			else{}
			
			if(UserOperation.GetSetupData.bUnit == 1)
			{
				if(UserOperation.Modify.NumAfterDot)
				{				
					UserOperation.Modify.NumAfterDot--;
				}
				else{}
				switch(UserOperation.Modify.NumAfterDot)
				{
					case 0:
						UserOperation.GetSetupData.ParamValue = (UserOperation.GetSetupData.ParamValue - UserOperation.GetSetupData.ParamValue % (value_expand * 10)) / 10;
						break;
					
					case 1:			//需考虑用户删除时的点号处理					
						UserOperation.Modify.NumAfterDot--;														//删除点号
						UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue - UserOperation.GetSetupData.ParamValue % (value_expand  / 10 * 10);
						break;
					
					case 2:
						UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue - UserOperation.GetSetupData.ParamValue % (value_expand  / 100 * 10);
						break;
					
					case 3:
						UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.ParamValue - UserOperation.GetSetupData.ParamValue % (value_expand  / 1000 * 10);
						break;
					
					default:
						break;
				}
				
				*UserOperation.GetSetupData.pParamNAD = UserOperation.Modify.NumAfterDot;
				if(*UserOperation.GetSetupData.pParamNAD)
				{
					*UserOperation.GetSetupData.pParamNAD = *UserOperation.GetSetupData.pParamNAD - 1;
				}
				else{}
			}
			else
			{				
				UserOperation.Modify.NumAfterDot = 4;
				
				*UserOperation.GetSetupData.pParamNAD = 3;
				
				UserOperation.GetSetupData.ParamValue = (UserOperation.GetSetupData.ParamValue - UserOperation.GetSetupData.ParamValue % (value_expand * 10)) / 10;
			}
			
			break;
			
		case KBD_INPUT_CLEAR:
			
			UserOperation.Modify.fStart = 1;
			UserOperation.Modify.NumAfterDot = 0;
		
			*UserOperation.GetSetupData.pParamNAD = UserOperation.Modify.NumAfterDot;
			UserOperation.GetSetupData.ParamValue = 0;
		
			break;
		
		case KBD_INPUT_CODEC_INC_SLOW:
			if(UserOperation.Modify.fStart == 1)				//首次输入数值或切换一次单位，设置值直接覆盖原值
			{
				UserOperation.Modify.fStart = 2;
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					if(*UserOperation.GetSetupData.pParamNAD)	//[V145]，修复直接判断指针导致赋值异常bug
					{
						UserOperation.Modify.NumAfterDot = *UserOperation.GetSetupData.pParamNAD + 1;
					}
					else
					{
						UserOperation.Modify.NumAfterDot = 0;
					}
				}
			}
			else{}
			if(UserOperation.GetSetupData.bUnit == 1)
			{
				switch(*UserOperation.GetSetupData.pParamNAD)
				{
					case 0:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_SLOW * value_expand;
						break;
					
					case 1:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_SLOW * value_expand / 10;
						break;
					
					case 2:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_SLOW * value_expand / 100;
						break;
					
					case 3:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_SLOW * value_expand / 1000;
						break;
					
					default:
						break;
				}				
			}
			else
			{				
				UserOperation.Modify.NumAfterDot = 4;
				
				*UserOperation.GetSetupData.pParamNAD = 3;
				
				UserOperation.GetSetupData.ParamValue += ROTATE_STEP_SLOW * value_expand;
			}
			
			break;
			
		case KBD_INPUT_CODEC_INC_QUICK:
			
			if(UserOperation.Modify.fStart == 1)
			{
				UserOperation.Modify.fStart = 2;
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					if(*UserOperation.GetSetupData.pParamNAD)
					{
						UserOperation.Modify.NumAfterDot = *UserOperation.GetSetupData.pParamNAD + 1;
					}
					else
					{
						UserOperation.Modify.NumAfterDot = 0;
					}
				}
				else{}
			}
			else{}
			
			if(UserOperation.GetSetupData.bUnit == 1)
			{
				switch(*UserOperation.GetSetupData.pParamNAD)
				{
					case 0:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_QUICK * value_expand;
						break;
					
					case 1:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_QUICK * value_expand / 10;
						break;
					
					case 2:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_QUICK * value_expand / 100;
						break;
					
					case 3:
						UserOperation.GetSetupData.ParamValue += ROTATE_STEP_QUICK * value_expand / 1000;
						break;
					
					default:
						break;
				}
			}
			else
			{				
				UserOperation.Modify.NumAfterDot = 4;
				
				*UserOperation.GetSetupData.pParamNAD = 3;
				
				UserOperation.GetSetupData.ParamValue += ROTATE_STEP_QUICK * value_expand;
			}
			
			break;
			
		case KBD_INPUT_CODEC_DEC_SLOW:
			
			if(UserOperation.Modify.fStart == 1)
			{
				UserOperation.Modify.fStart = 2;
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					if(*UserOperation.GetSetupData.pParamNAD)
					{
						UserOperation.Modify.NumAfterDot = *UserOperation.GetSetupData.pParamNAD + 1;
					}
					else
					{
						UserOperation.Modify.NumAfterDot = 0;
					}
				}
				else{}
			}
			else{}
			
			if(UserOperation.GetSetupData.bUnit == 1)
			{
				switch(*UserOperation.GetSetupData.pParamNAD)
				{
					case 0:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_SLOW * value_expand)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_SLOW * value_expand;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 1:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_SLOW * value_expand / 10)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_SLOW * value_expand / 10;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 2:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_SLOW * value_expand / 100)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_SLOW * value_expand / 100;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 3:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_SLOW * value_expand / 1000)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_SLOW * value_expand / 1000;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					default:
						break;
				}				
			}
			else
			{
				if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_SLOW * value_expand)
				{
					UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_SLOW * value_expand;
				}
				else
				{
					UserOperation.GetSetupData.ParamValue = 0;
				}
				UserOperation.Modify.NumAfterDot = 4;
				*UserOperation.GetSetupData.pParamNAD = 3;
			}
			
			break;
			
		case KBD_INPUT_CODEC_DEC_QUICK:
			
			if(UserOperation.Modify.fStart == 1)
			{
				UserOperation.Modify.fStart = 2;
				if(UserOperation.GetSetupData.bUnit == 1)
				{
					if(*UserOperation.GetSetupData.pParamNAD)
					{
						UserOperation.Modify.NumAfterDot = *UserOperation.GetSetupData.pParamNAD + 1;
					}
					else
					{
						UserOperation.Modify.NumAfterDot = 0;
					}
				}
				else{}
			}
			else{}
				
			if(UserOperation.GetSetupData.bUnit == 1)
			{
				switch(*UserOperation.GetSetupData.pParamNAD)
				{
					case 0:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_QUICK * value_expand)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_QUICK * value_expand;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 1:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_QUICK * value_expand / 10)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_QUICK * value_expand / 10;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 2:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_QUICK * value_expand / 100)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_QUICK * value_expand / 100;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					case 3:
						if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_QUICK * value_expand / 1000)
						{
							UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_QUICK * value_expand / 1000;
						}
						else
						{
							UserOperation.GetSetupData.ParamValue = 0;
						}
						break;
					
					default:
						break;
				}				
			}
			else
			{
				if(UserOperation.GetSetupData.ParamValue >= ROTATE_STEP_QUICK * value_expand)
				{
					UserOperation.GetSetupData.ParamValue -= ROTATE_STEP_QUICK * value_expand;
				}
				else
				{
					UserOperation.GetSetupData.ParamValue = 0;
				}
				UserOperation.Modify.NumAfterDot = 4;
				*UserOperation.GetSetupData.pParamNAD = 3;
			}
			
			break;
			
		default:
			break;
	}
	
	if(UserOperation.GetSetupData.ParamValue > UserOperation.GetSetupData.Ceiling)				//判定设置值是否超上限
	{
		UserOperation.GetSetupData.ParamValue = UserOperation.GetSetupData.Ceiling;
	}
	else{}
	
	/*	UserOperation.SettingValue处理完成后返还给设置的参数	*/
	UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
	
	T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
}


uint8_t get_key_code(void)
{
	uint8_t keycode = 0;
	uint8_t i = 0, j = 0;
	
	keycode = bsp_GetKey();
	
	if(keycode != KEY_NONE)
	{
		i = (keycode - 1) / 3;																					//键值
		j = (keycode - 1) % 3;																					//键状态，0--按下 1--弹起 2--长按
		if(j == 0 && i < KEY_NUMBER)
		{
			return i;
		}
		else
		{
			return KEY_NONE;
		}
	}
	else
	{
		return KEY_NONE;
	}	
}


/********************************************************************
*	功能	：	按键轮询
******************************************************************************/
void Manual_Poll(void)
{
	uint8_t keycode = 0;
	uint8_t i = 0, j = 0;
	
	keycode = bsp_GetKey();
	if(keycode != KEY_NONE)
	{
		i = (keycode - 1) / 3;																					//键值
		j = (keycode - 1) % 3;																					//键状态，0--按下 1--弹起 2--长按
		if(j == 0 && i < KEY_NUMBER)
		{
/*******************************************************************	
*
*			1.输出控制部分按键	包含运行、暂停、翻转、单次输出键
*
*
********************************************************************
*/
			
			if(i == BTN_RUN)													//按下运行键
			{		
		
				if(UserOperation.fMode == UO_MODE_SINGLE)														//[V111]，修改输出逻辑，SINGLE模式下，RUN按键无效
				{
					if(DOState.Config & (1 << DO_TIM4))															//[V189],修改输出逻辑，SINGLE模式下，RUN按键停止其输出
					{
						DOState.Config &= ~(1 << DO_TIM4);
					}
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Process_COMMAND_STOP();
				    }
					
					return;
				}
				
				if(UserOperation.fMode == UO_MODE_EXTBNC)														//[V181]，修改输出逻辑，修复EXTBNC开始运行后，一直处于RUN状态而无法切换模式bug
				{
					if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
					{
						Output_VorC(UserOperation.bVC, 0, OUTPUT_DISABLE);
					}
						
					pLEDPAUSE = LED_SN74HC240_OFF;
					DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
					
					return;
				}
				
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				Led_ParamPartOff();
				
				if(DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE)							//运行过程
				{
					if(T6.MemoryUpdateCnt)
					{
						T6.MemoryUpdateCnt = T6_VALUE_MINUTE - T6_VALUE_SECOND * 20;							//参数有变动且触发运行则重新存储,20s储存空隙时间用于优先处理RUN操作
					}
					
					UO_Update(UPDATE_ALL);
					UserOperation.Update = UO_UPDATE_VALID;
					
					//Process_COMMAND_START();												//开始运行
					
					
					if(UserOperation.bVC == SELECT_VC_V)
					{
						log_info("BTN_SINGLETRIGGER Mode Voltage,Ampl=%d\r\n",pPwmArrayParam[DO_TIM4]->Ampl);
						if(pPwmArrayParam[DO_TIM4]->Ampl > SAFE_VOLTAGE)
						{
							is_exceed_human_safety = 1;
							log_info("Exceeding safe voltage\r\n");
						}
						else
						{
							is_exceed_human_safety = 0;
						}
					}
					else
					{
						log_info("BTN_SINGLETRIGGER Mode Current,Ampl=%d\r\n",pPwmArrayParam[DO_TIM4]->Ampl);
						if(pPwmArrayParam[DO_TIM4]->Ampl > SAFE_CURRENT)
						{
							is_exceed_human_safety = 1;
							log_info("Exceeding safe current\r\n");
						}
						else
						{
							is_exceed_human_safety = 0;
						}
					}
					
					if(is_exceed_human_safety == 0)										//如果参数符合安全值，则直接输出
					{
						Process_COMMAND_START();
						//LedShortOn.fSinggleTrigger = LEDSHORTON_BEGIN;
					}
					else																//如果参数不符合安全值，则进行弹窗提醒
					{
						uint8_t key_code=KEY_NONE;
						
						is_refash_warn =0;
						while(1)
						{
							//检测BTN_SINGLETRIGGER和PAUSE按键，按下BTN_SINGLETRIGGER按键，继续输出，按下PAUSE按键，退出输出
							WDG_Feed();
							key_code = get_key_code();
							//log_info("get_key_code:0x%x\r\n",key_code);
							
							if( key_code == BTN_RUN )
							{
								
								UI.fFlush = FLUSH_START;
								Process_COMMAND_START();
								//LedShortOn.fSinggleTrigger = LEDSHORTON_BEGIN;
								break;
							}
							else if( key_code == BTN_PAUSE )
							{
								UI.fFlush = FLUSH_START;
								break;
							}
							
							if(is_refash_warn ==0)
							{
								is_refash_warn = 1;
								UI_Poll(ENABLE_WARNING);	//显示告警信息
							}
						}
					}
				}
				else if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)					//运行完成
				{
					Output_VorC(UserOperation.bVC, 0, OUTPUT_DISABLE);
															
					pLEDPAUSE = LED_SN74HC240_OFF;

					Process_COMMAND_STOP();
				}
			}
			else if(i == BTN_PAUSE)															//按下暂停键
			{
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				Led_ParamPartOff();			
				
				if(DOState.Status[DO_TIM4] == DOSTATE_STATUS_RUNNING)		//如果是run状态，按下pause暂停运行
				{					
					Process_COMMAND_PAUSE();	
					
					Output_VorC(UserOperation.bVC, 0, OUTPUT_DISABLE);			
					pLEDPAUSE = LED_SN74HC240_ON;
				}
				else if(DOState.Status[DO_TIM4] == DOSTATE_STATUS_PAUSE)	//如果是pause状态，按下pause继续运行
				{					
					EXTI->IMR    |= 1<<0;
					
					Process_COMMAND_CONTINUE();					
					pLEDPAUSE = LED_SN74HC240_OFF;
				}
				else if(DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE)
				{
					;
				}
			}
			else if(i == BTN_PHASE)															//按下翻转键
			{
				if(UserOperation.bPhase == UO_PHASE_UNIPHASE)
				{
					UserOperation.bPhase = UO_PHASE_BIPHASE;			//切换为负极性
				}
				else
				{
					UserOperation.bPhase = UO_PHASE_UNIPHASE;			//切换为正极性					
				}
				
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				Led_ParamPartOff();

				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}
			else if(i == BTN_SINGLETRIGGER)													//按下单次输出按钮，
			{				
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				Led_ParamPartOff();
				
				if(UserOperation.fMode == UO_MODE_SINGLE)									//如果设备处在单个波形模式，则执行代码，如果处于其他模式，则不理会
				{					
					UO_Update(UPDATE_ALL);													//在该函数中计算波形参数，获取参数等
					UserOperation.Update = UO_UPDATE_VALID;									//设置一些运行状态
						
					
					//1:打开输出并设置定时器，2：定时器到到设定时间则关闭输出
					Process_COMMAND_START();
					LedShortOn.fSinggleTrigger = LEDSHORTON_BEGIN;
					/*
					if(UserOperation.bVC == SELECT_VC_V)
					{
						log_info("BTN_SINGLETRIGGER Mode Voltage,Ampl=%d\r\n",pPwmArrayParam[DO_TIM4]->Ampl);
						if(pPwmArrayParam[DO_TIM4]->Ampl > SAFE_VOLTAGE)
						{
							is_exceed_human_safety = 1;
							log_info("Exceeding safe voltage\r\n");
						}
						else
						{
							is_exceed_human_safety = 0;
						}
					}
					else
					{
						log_info("BTN_SINGLETRIGGER Mode Current,Ampl=%d\r\n",pPwmArrayParam[DO_TIM4]->Ampl);
						if(pPwmArrayParam[DO_TIM4]->Ampl > SAFE_CURRENT)
						{
							is_exceed_human_safety = 1;
							log_info("Exceeding safe current\r\n");
						}
						else
						{
							is_exceed_human_safety = 0;
						}
					}
					
					if(is_exceed_human_safety == 0)										//如果参数符合安全值，则直接输出
					{
						Process_COMMAND_START();
						LedShortOn.fSinggleTrigger = LEDSHORTON_BEGIN;
					}
					else																//如果参数不符合安全值，则进行弹窗提醒
					{
						uint8_t key_code=KEY_NONE;
						
						is_refash_warn =0;
						while(1)
						{
							//检测BTN_SINGLETRIGGER和PAUSE按键，按下BTN_SINGLETRIGGER按键，继续输出，按下PAUSE按键，退出输出
							WDG_Feed();
							key_code = get_key_code();
							//log_info("get_key_code:0x%x\r\n",key_code);
							
							if( key_code == BTN_SINGLETRIGGER )
							{
								
								UI.fFlush = FLUSH_START;
								Process_COMMAND_START();
								LedShortOn.fSinggleTrigger = LEDSHORTON_BEGIN;
								break;
							}
							else if( key_code == BTN_PAUSE )
							{
								UI.fFlush = FLUSH_START;
								break;
							}
							
							if(is_refash_warn ==0)
							{
								is_refash_warn = 1;
								UI_Poll(ENABLE_WARNING);	//显示告警信息
							}
						}
						
					}
					*/
					
				}
			}
/*******************************************************************	
*
*			2.输出属性部分按键	包含电压电流切换、单位切换
*
*
********************************************************************
*/			
			else if( (i == BTN_VC) &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE) )															//按下电压电流切换键
			{
				if(DOState.Config & (1 << DO_TIM4))																//[V189],修复切换模式VC时，若处于RUN状态，不会自动停止的bug
				{
					DOState.Config &= ~(1 << DO_TIM4);
				}
				else{}
				if(DOState.Status[DO_TIM4] != DOSTATE_STATUS_COMPLETE)
				{
					Process_COMMAND_STOP();
				}
				
				
				
				UserOperation.bVC = !UserOperation.bVC;
				
				LedShortOn.fVC = LEDSHORTON_BEGIN;
				
				UserOperation.fParamType = UO_PARAM_NONE;
				Led_ModePartOff();
				
				Led_ParamPartOff();
				
				UserOperation.Modify.fStart = 0;
				UserOperation.Modify.NumAfterDot = 0;
				
				if(UserOperation.bVC == SELECT_VC_V)
				{
					Switch.ModeCur &= ~(1 << MODE_BIT_VC);    //MODE_BIT_VC		0
					Switch.ModeCur |= SELECT_VC_V << MODE_BIT_VC;  //SELECT_VC_C		0
					
					UserOperation.fMode_VPre = UserOperation.fMode_CPre;										//[V111],处理逻辑调整为切换输出时模式不变
					UserOperation.fMode = UserOperation.fMode_VPre;
					
					Wave_type = sAdditionalData.V_Wave_type;
					UserOperation.V_ModeExtBnc.Param[UO_PARAM_PULSE] = sAdditionalData.V_Bnc_Pulse;
					
					switch(UserOperation.fMode)
					{
						case UO_MODE_SINGLE:
							UserOperation.fUnitCur	= UserOperation.V_ModeSingle.fUnit;
							pLEDSINGLE				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_FREERUN:
							UserOperation.fUnitCur	= UserOperation.V_ModeFreeRun.fUnit;
							pLEDFREERUN				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_TRAIN:
							UserOperation.fUnitCur	= UserOperation.V_ModeTrain.fUnit;
							pLEDTRAIN				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_EXTBNC:
							UserOperation.fUnitCur	= UserOperation.V_ModeExtBnc.fUnit;
							pLEDEXTBNC				= LED_SN74HC240_ON;
							break;
						
						default:
							break;
					}
				}
				else
				{		
					Switch.ModeCur &= ~(1 << MODE_BIT_VC);
					Switch.ModeCur &= SELECT_VC_C << MODE_BIT_VC;
					
					UserOperation.fMode_CPre = UserOperation.fMode_VPre;										//[V111],处理逻辑调整为切换输出时模式不变
					UserOperation.fMode = UserOperation.fMode_CPre;
					
					Wave_type = sAdditionalData.C_Wave_type;
					UserOperation.C_ModeExtBnc.Param[UO_PARAM_PULSE] = sAdditionalData.C_Bnc_Pulse;
					
					switch(UserOperation.fMode)
					{
						case UO_MODE_SINGLE:
							UserOperation.fUnitCur	= UserOperation.C_ModeSingle.fUnit;
							pLEDSINGLE				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_FREERUN:
							UserOperation.fUnitCur	= UserOperation.C_ModeFreeRun.fUnit;
							pLEDFREERUN				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_TRAIN:
							UserOperation.fUnitCur	= UserOperation.C_ModeTrain.fUnit;
							pLEDTRAIN				= LED_SN74HC240_ON;
							break;
						
						case UO_MODE_EXTBNC:
							UserOperation.fUnitCur	= UserOperation.C_ModeExtBnc.fUnit;
							pLEDEXTBNC				= LED_SN74HC240_ON;
							
							break;
						
						default:
							break;
					}
				}
				
			  if((Switch.ModeCur & (1 << MODE_BIT_VC)) != (Switch.ModePre & (1 << MODE_BIT_VC)))   //MODE_BIT_VC  0
			  {
					Switch_Ctrl(UO_PHASE_UNIPHASE);	//切换极性
			   }

				UI.fFlush = FLUSH_START;
				
				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}		
				/***********************************************************************
				*
				*	Wave_Type=0:单极性正电平
				*	Wave_Type=1：单极性负电平
				*	Wave_Type=2：双极性波形1（先正后负，50%）
				*	Wave_Type=3：双极性波形2（先负后正，50%）	
				*
				***********************************************************************/					
			else if( (i == BTN_UNITSWITCH)  &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE) )		//按下单位切换键 改做波形模式切换按键，改按键在何时起效（不RUN的时候）
			{
					pLEDUNITSWITCH		= 1;
				
					Wave_type++;
					if(Wave_type>=4)Wave_type=0;					//Wave_Type=0:单极性正电平，Wave_Type=1：单极性负电平，Wave_Type=2：双极性波形1（先正后负，50%），Wave_Type=3：双极性波形2（先负后正，50%）
						
					log_info("Wave_type=%d\r\n",Wave_type);
					
					if( (Wave_type == 0) || (Wave_type == 1) )
					{
						TriggerExIN01_Init();						//BNC模式下，初始化为上升沿和下降沿触发
					}
					else
					{
						TriggerExIN23_Init();						//BNC模式下，初始化为上升沿触发
					}
					
					
					if(UserOperation.bVC == SELECT_VC_V)				//此处也需要检查脉宽设置参数
					{
						sAdditionalData.V_Wave_type = Wave_type;
					}
					else
					{
						sAdditionalData.C_Wave_type = Wave_type;
					}
						
					AT24CXX_Write(230, (void *)&sAdditionalData, sizeof(sAdditionalData));
					
					{						
						if((Wave_type == 2) || (Wave_type == 3))	//如果波形切换为双极性，则检查脉宽下限
						{
							UserOperation.fParamType = UO_PARAM_PULSE;
							
							if(     UserOperation.GetSetupData.ParamValue < PULSE_LOWER_LIMIT )
							{
								UserOperation.GetSetupData.ParamValue = PULSE_LOWER_LIMIT;
							
								UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
						
								UserOperation.fParamType = UO_PARAM_NONE;
								ParamEdit_RefreshPre();
					
								UO_Update(UPDATE_ALL);
							}
							else{}
							UserOperation.fParamType = UO_PARAM_NONE;
						}
						else{}
					}
									
					log_info("sAdditionalData.V_Wave_type:%d,C_Wave_type:%d\r\n",sAdditionalData.V_Wave_type, sAdditionalData.C_Wave_type);				
					pLEDUNITSWITCH		= 0;
			}

/*******************************************************************	
*
*			3.模式控制按键	包含4个模式切换按键
*
*
********************************************************************
*/				
			else if(i == BTN_SINGLE && (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE || UserOperation.fMode == UO_MODE_EXTBNC))					//按下单脉冲模式键			
			{	
				Process_COMMAND_STOP();
				TIM3_DISABLE();	
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;				
				Led_ParamPartOff();
				
				if(UserOperation.fMode != UO_MODE_SINGLE)
				{
					Led_ModePartOff();
					
					pLEDSINGLE			= LED_SN74HC240_ON;
					
					UserOperation.fMode = UO_MODE_SINGLE;
					
					if(UserOperation.bVC == SELECT_VC_V)
					{
						UserOperation.fMode_VPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.V_ModeSingle.fUnit;
					}
					else
					{
						UserOperation.fMode_CPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.C_ModeSingle.fUnit;
					}

					UI.fFlush = FLUSH_START;
				}
				else{}
				SW_CV_OUTPUT = 0;
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}
			else if(i == BTN_TRAIN && (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE || UserOperation.fMode == UO_MODE_EXTBNC))						//按下按时长运行模式键
			{		
				Process_COMMAND_STOP();
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
				UI.fFlush = FLUSH_END;
				TIM3_DISABLE();
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				
				Led_ParamPartOff();
				
				if(UserOperation.fMode != UO_MODE_TRAIN)
				{
					Led_ModePartOff();
					
					pLEDTRAIN			= LED_SN74HC240_ON;
					
					UserOperation.fMode = UO_MODE_TRAIN;
										
					if(UserOperation.bVC == SELECT_VC_V)
					{
						UserOperation.fMode_VPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.V_ModeTrain.fUnit;
					}
					else
					{
						UserOperation.fMode_CPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.C_ModeTrain.fUnit;
					}

					UI.fFlush = FLUSH_START;
				}
				else{}
				SW_CV_OUTPUT = 0;
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}
			else if(i == BTN_FREERUN && (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE || UserOperation.fMode == UO_MODE_EXTBNC))					//按下持续运行模式键
			{		
				Process_COMMAND_STOP();
				DOState.Status[DO_TIM4] = DOSTATE_STATUS_COMPLETE;
				UI.fFlush = FLUSH_END;
				TIM3_DISABLE();
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				pLEDRUN = LED_DIRECTLY_OFF;
				
				Led_ParamPartOff();
				
				if(UserOperation.fMode != UO_MODE_FREERUN)
				{
					Led_ModePartOff();
					
					pLEDFREERUN			= LED_SN74HC240_ON;
					
					UserOperation.fMode = UO_MODE_FREERUN;
					
					if(UserOperation.bVC == SELECT_VC_V)
					{
						UserOperation.fMode_VPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.V_ModeFreeRun.fUnit;
					}
					else
					{
						UserOperation.fMode_CPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.C_ModeFreeRun.fUnit;
					}

					UI.fFlush = FLUSH_START;
				}
				else{}
				
				SW_CV_OUTPUT = 0;
				pLEDOUTPUT = LED_DIRECTLY_OFF;
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}
			else if(i == BTN_EXTBNC && DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE)					//按下外部触发模式键
			{				
				Led_ParamPartOff();
				
				if(UserOperation.fMode != UO_MODE_EXTBNC)
				{
					Led_ModePartOff();
					
					pLEDEXTBNC = LED_SN74HC240_ON;
					
					UserOperation.fMode = UO_MODE_EXTBNC;
										
					if(UserOperation.bVC == SELECT_VC_V)
					{
						UserOperation.fMode_VPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.V_ModeExtBnc.fUnit;
					}
					else
					{
						UserOperation.fMode_CPre = UserOperation.fMode;
						UserOperation.fUnitCur = UserOperation.C_ModeExtBnc.fUnit;
					}

					UI.fFlush = FLUSH_START;
				}
				else{}
				
				if((Wave_type==0) || (Wave_type==1))
				{
					TriggerExIN01_Init();
				}
				else
				{
					TriggerExIN23_Init();
				}
				
				SW_CV_OUTPUT = 1;			//进入BNC模式后，打开继电器
				Delay_ms(10);
				if(UserOperation.bVC == SELECT_VC_C)
				{
					EN_C_OP();
					Delay_ms(10);
				}
				
				//收到上升沿脉冲后开始输出信号，打开LED指示灯
				
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				T6.MemoryUpdateCnt = MEMORYUPDATE_UPCNT_START;
			}
/*******************************************************************	
*
*			4.参数类型按键	包含设置幅值、脉宽、频率、运行时长
*
*
********************************************************************
*/			
			else if( (i == BTN_PULSE) &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE))																				//按下设置脉宽键
			{	
				par_check();
				
				if(UserOperation.fMode == UO_MODE_SINGLE || UserOperation.fMode == UO_MODE_FREERUN || UserOperation.fMode == UO_MODE_TRAIN || UserOperation.fMode == UO_MODE_EXTBNC)
				{
					Led_ParamPartOff();
					
					if(UserOperation.fParamType != UO_PARAM_PULSE)
					{
						pLEDPULSE = LED_SN74HC240_ON;
						
						if(UserOperation.Update == UO_UPDATE_VALID)
						{							
							UserOperation.Update = UO_UPDATE_SETTING;
						}

						UserOperation.fParamType = UO_PARAM_PULSE;
						
						SettingInfo_Get(PARAMTYPECHANGE_Y, UNITSWITCH_N);
						
						UserOperation.Modify.fStart = 1;
						UserOperation.Modify.bUnitSwitched = 0;
						UserOperation.Modify.NumAfterDot = 0;
					}
					else
					{
						if(UserOperation.fParamType == UO_PARAM_PULSE)						//脉宽下限值检查
						{	
							if((Wave_type == 0) || (Wave_type == 1))
							{
								if(     UserOperation.GetSetupData.ParamValue < (PULSE_LOWER_LIMIT/2) )
								{
									UserOperation.GetSetupData.ParamValue = (PULSE_LOWER_LIMIT/2);
								}
								else{}
								UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
							}
							else
							{
								if(     UserOperation.GetSetupData.ParamValue < PULSE_LOWER_LIMIT )
								{
									UserOperation.GetSetupData.ParamValue = PULSE_LOWER_LIMIT;
								}
								else{}
								UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
							}
						}
						else{}
					
						if(UserOperation.Update == UO_UPDATE_SETTING)
						{
							UserOperation.Update = UO_UPDATE_INVALID;
					
							UO_Update(UPDATE_ALL);

							UserOperation.Update = UO_UPDATE_VALID;
						}
						else{}
						UserOperation.fParamType = UO_PARAM_NONE;
						
						UserOperation.Modify.fStart = 0;
						UserOperation.Modify.NumAfterDot = 0;
					}
					
					ParamEdit_RefreshPre();
				}
				else{}
			}
			else if((i == BTN_AMPL) &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE))																		//按下设置幅值键
			{
				par_check();
				
				Led_ParamPartOff();
				
				if(UserOperation.fParamType != UO_PARAM_AMPL)
				{
					pLEDAMPL = LED_SN74HC240_ON;
					
					if(UserOperation.Update == UO_UPDATE_VALID)
					{							
					   UserOperation.Update = UO_UPDATE_SETTING;
					}
					else{}
					
					UserOperation.fParamType = UO_PARAM_AMPL;
					
					SettingInfo_Get(PARAMTYPECHANGE_Y, UNITSWITCH_N);
					
					UserOperation.Modify.fStart = 1;
					UserOperation.Modify.bUnitSwitched = 0;
					UserOperation.Modify.NumAfterDot = 0;
				}
				else
				{					
					if(UserOperation.Update == UO_UPDATE_SETTING)
					{
						UserOperation.Update = UO_UPDATE_INVALID;
					
						UO_Update(UPDATE_ALL);

						UserOperation.Update = UO_UPDATE_VALID;
					}
					else{}
						
					UserOperation.fParamType = UO_PARAM_NONE;
					
					UserOperation.Modify.fStart = 0;
					UserOperation.Modify.NumAfterDot = 0;
				}
				
				ParamEdit_RefreshPre();
			}
			else if((i == BTN_FREQ) &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE))																		//按下设置频率键
			{	
				par_check();
				
				if(UserOperation.fMode == UO_MODE_FREERUN || UserOperation.fMode == UO_MODE_TRAIN)
				{
					Led_ParamPartOff();
					
					if(UserOperation.fParamType != UO_PARAM_FREQ)
					{
						pLEDFREQUENCY = LED_SN74HC240_ON;
						
						if(UserOperation.Update == UO_UPDATE_VALID)
						{							
							UserOperation.Update = UO_UPDATE_SETTING;
						}
						else{}
						
						UserOperation.fParamType = UO_PARAM_FREQ;
						
						SettingInfo_Get(PARAMTYPECHANGE_Y, UNITSWITCH_N);
						
						UserOperation.Modify.fStart = 1;
						UserOperation.Modify.bUnitSwitched = 0;
						UserOperation.Modify.NumAfterDot = 0;						
					}
					else
					{
						if(UserOperation.fParamType == UO_PARAM_FREQ)						//频率下限值再按下enter键后检查
						{
							if(     UserOperation.GetSetupData.ParamValue < FREQ_LOWER_LIMIT )
							{
								UserOperation.GetSetupData.ParamValue = FREQ_LOWER_LIMIT;
							}
							UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
						}
						
						if(UserOperation.Update == UO_UPDATE_SETTING)
						{
							UserOperation.Update = UO_UPDATE_INVALID;
					
							UO_Update(UPDATE_ALL);

							UserOperation.Update = UO_UPDATE_VALID;
						}
							
						UserOperation.fParamType = UO_PARAM_NONE;
						
						UserOperation.Modify.fStart = 0;
						UserOperation.Modify.NumAfterDot = 0;
					}
					
					ParamEdit_RefreshPre();
				}
			}
			else if((i == BTN_DURATION) &&  (DOState.Status[DO_TIM4] == DOSTATE_STATUS_COMPLETE))																	//按下设置运行时长键
			{	
				par_check();
				
				if(UserOperation.fMode == UO_MODE_TRAIN)
				{
					Led_ParamPartOff();
					
					if(UserOperation.fParamType != UO_PARAM_DURATION)
					{
						pLEDDURATION = LED_SN74HC240_ON;
						
						if(UserOperation.Update == UO_UPDATE_VALID)
						{							
							UserOperation.Update = UO_UPDATE_SETTING;
						}
						
						UserOperation.fParamType = UO_PARAM_DURATION;
						
						SettingInfo_Get(PARAMTYPECHANGE_Y, UNITSWITCH_N);
						
						UserOperation.Modify.fStart = 1;
						UserOperation.Modify.bUnitSwitched = 0;
						UserOperation.Modify.NumAfterDot = 0;
					}
					else
					{
						if(UserOperation.fParamType == UO_PARAM_DURATION)						//运行时间下限值再按下enter键后检查
						{
							if(     UserOperation.GetSetupData.ParamValue < DURATION_LOWER_LIMIT )
							{
								UserOperation.GetSetupData.ParamValue = DURATION_LOWER_LIMIT;
							}
							UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
						}
					
						if(UserOperation.Update == UO_UPDATE_SETTING)
						{
							UserOperation.Update = UO_UPDATE_INVALID;
							
							UO_Update(UPDATE_ALL);

							UserOperation.Update = UO_UPDATE_VALID;
						}
						
						UserOperation.fParamType = UO_PARAM_NONE;
						
						UserOperation.Modify.fStart = 0;
						UserOperation.Modify.NumAfterDot = 0;
					}
					
					ParamEdit_RefreshPre();
				}
			}
/*******************************************************************	
*
*			5.数值输入按键	包含设置0-9数字、小数点、enter、回退、清除
*
*
********************************************************************
*/				
			else if(i == BTN_0)											//按下0键
			{
				SettingInfo_Modify(KBD_INPUT_0);
			}
			else if(i == BTN_1)											//按下1键
			{
				SettingInfo_Modify(KBD_INPUT_1);
			}
			else if(i == BTN_2)											//按下2键
			{
				SettingInfo_Modify(KBD_INPUT_2);
			}
			else if(i == BTN_3)											//按下3键
			{
				SettingInfo_Modify(KBD_INPUT_3);
			}
			else if(i == BTN_4)											//按下4键
			{
				SettingInfo_Modify(KBD_INPUT_4);
			}
			else if(i == BTN_5)											//按下5键
			{
				SettingInfo_Modify(KBD_INPUT_5);
			}
			else if(i == BTN_6)											//按下6键
			{
				SettingInfo_Modify(KBD_INPUT_6);
			}
			else if(i == BTN_7)											//按下7键
			{
				SettingInfo_Modify(KBD_INPUT_7);
			}
			else if(i == BTN_8)											//按下8键
			{
				SettingInfo_Modify(KBD_INPUT_8);
			}
			else if(i == BTN_9)											//按下9键
			{
				SettingInfo_Modify(KBD_INPUT_9);
			}
			else if(i == BTN_BACKSPACE)									//按下回退键
			{
				SettingInfo_Modify(KBD_INPUT_BACKSPACE);
			}
			else if(i == BTN_CLEAR)										//按下清除键
			{
				SettingInfo_Modify(KBD_INPUT_CLEAR);
			}
			else if(i == BTN_DOT)										//按下小数点键
			{
				if(UserOperation.Modify.NumAfterDot == 0)
				{
					UserOperation.Modify.NumAfterDot = 1;
				}
				else{}
			}
			else if(i == BTN_ENTER)										//按下enter键
			{
				Led_ParamPartOff();
				
				par_check();	
				
				UserOperation.fParamType = UO_PARAM_NONE;
				ParamEdit_RefreshPre();
				
				UserOperation.Modify.fStart = 0;
				UserOperation.Modify.NumAfterDot = 0;
				
				if(UserOperation.Update == UO_UPDATE_SETTING)
				{
					UserOperation.Update = UO_UPDATE_INVALID;
					
					UO_Update(UPDATE_ALL);

					UserOperation.Update = UO_UPDATE_VALID;
				}
				else{}
			}
			else{}
		}
		else{}
	}
	else{}
}

void par_check(void)
{
	if(UserOperation.fParamType == UO_PARAM_PULSE)						//脉宽下限值再按下enter、切换参数键后检查
	{	
		if((Wave_type == 0) || (Wave_type == 1))
		{
			if(     UserOperation.GetSetupData.ParamValue < (PULSE_LOWER_LIMIT/2) )
			{
				UserOperation.GetSetupData.ParamValue = (PULSE_LOWER_LIMIT/2);
			}
			UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
		}
		else
		{
			if(     UserOperation.GetSetupData.ParamValue < PULSE_LOWER_LIMIT )
			{
				UserOperation.GetSetupData.ParamValue = PULSE_LOWER_LIMIT;
			}
			
			UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
		}
	}
	
	if(UserOperation.fParamType == UO_PARAM_FREQ)						//频率下限值再按下enter、切换参数键后检查
	{
		if(     UserOperation.GetSetupData.ParamValue < FREQ_LOWER_LIMIT )
		{
			UserOperation.GetSetupData.ParamValue = FREQ_LOWER_LIMIT;
		}
		UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
	}
	
	if(UserOperation.fParamType == UO_PARAM_DURATION)						//运行时间下限值再按下enter、切换参数键后检查
	{
		if(     UserOperation.GetSetupData.ParamValue < DURATION_LOWER_LIMIT )
		{
			UserOperation.GetSetupData.ParamValue = DURATION_LOWER_LIMIT;
		}
		UserOperation.GetSetupData.pModeParam->Param[UserOperation.fParamType] = UserOperation.GetSetupData.ParamValue;
	}
}




