
#include  "HeaderFiles.h"

/********************************************************************
*	功能	：	响应COMPUTER设置的特殊波形在定时器中断中的处理
******************************************************************************/
void Process_PULSE_TYPE_ZERO(void)
{
	static uint32_t indexcnt = 0;
	
	if(UserOperation.fRun == UO_FRUN_START)													//与按钮模式共用此标识量
	{
		indexcnt = 0;
		CP.PulseTimerOutput.RunTimeCnt = 0;
		CP.PulseTimerOutput.HTimeAreaFloor = 0;
		CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseParam.PulseZero.TblTime[indexcnt];
		
		UserOperation.fRun = UO_FRUN_RUNNING;
	}
	if(UserOperation.fRun == UO_FRUN_RUNNING)
	{
		CP.PulseTimerOutput.RunTimeCnt++;
		if(CP.PulseTimerOutput.RunTimeCnt < CP.PulseTimerOutput.HTimeAreaCeiling)
		{
			Output_VorC(CP.bFlagVC, CP.PulseParam.PulseZero.TblAmpl[indexcnt], OUTPUT_ENABLE);
		}
		else
		{
			if(indexcnt < CP.PulseParam.PulseZero.IndexNum - 1)
			{
				indexcnt++;
				CP.PulseTimerOutput.HTimeAreaCeiling += CP.PulseParam.PulseZero.TblTime[indexcnt];
			}
			else
			{
				indexcnt = 0;
				CP.PulseTimerOutput.RunTimeCnt = 0;				
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
				UserOperation.fRun = UO_FRUN_STOP;
			}
		}
	}
}

void Process_PULSE_TYPE_ONE(void)
{
	if(UserOperation.fRun == UO_FRUN_START)
	{
		CP.PulseTimerOutput.RunTimeCnt = 0;
		UserOperation.fRun = UO_FRUN_RUNNING;
	}
	if(UserOperation.fRun == UO_FRUN_RUNNING)
	{
		CP.PulseTimerOutput.RunTimeCnt++;
		if(CP.PulseParsing.CommonFlag)
		{
			if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseParsing.HighTime)
			{
				Output_VorC(CP.bFlagVC, CP.PulseParam.PulseOne.Ampl, OUTPUT_ENABLE);	//输出设置幅值
			}
			else if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseParsing.HighTime + CP.PulseParsing.LowTime)
			{
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);								//输出0
			}
			else
			{
				CP.PulseTimerOutput.RunTimeCnt = 0;
				UserOperation.fRun = UO_FRUN_STOP;
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
			}
		}
		else
		{
			if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseParsing.LowTime)
			{
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);								//输出0
				
			}
			else if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseParsing.HighTime + CP.PulseParsing.LowTime)
			{
				Output_VorC(CP.bFlagVC, CP.PulseParam.PulseOne.Ampl, OUTPUT_ENABLE);	//输出设置幅值
			}
			else
			{
				CP.PulseTimerOutput.RunTimeCnt = 0;				
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
				UserOperation.fRun = UO_FRUN_STOP;
			}
		}
	}		
}

void Process_PULSE_TYPE_TWO(void)
{
	if(UserOperation.fRun == UO_FRUN_START)
	{
		CP.PulseTimerOutput.RunTimeCnt = 0;
		CP.PulseTimerOutput.HTimeAreaFloor = 0;
		CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseParsing.HighTime;
		UserOperation.fRun = UO_FRUN_RUNNING;
	}
	if(UserOperation.fRun == UO_FRUN_RUNNING)
	{
		CP.PulseTimerOutput.RunTimeCnt++;
		if(CP.PulseTimerOutput.CycleNumCnt < CP.PulseParsing.CycleNum)
		{
			if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaFloor)																					//区间之前，输出低电平
			{
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);												//输出0
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaFloor && CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaCeiling)		//输出高电平区间
			{
				Output_VorC(CP.bFlagVC, CP.PulseParam.PulseTwo.Ampl, OUTPUT_ENABLE);					//输出设置幅值
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaCeiling)																				//超出范围后移寻找新的区间
			{
				CP.PulseTimerOutput.CycleNumCnt++;
				CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.LowTime;
				CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;
			}
		}
		else
		{
			CP.PulseTimerOutput.RunTimeCnt = 0;			
			Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
			UserOperation.fRun = UO_FRUN_STOP;
		}
	}
}

void Process_PULSE_TYPE_THREE(void)
{
	if(UserOperation.fRun == UO_FRUN_START)
	{
		CP.PulseTimerOutput.RunTimeCnt = 0;
		CP.PulseTimerOutput.HTimeAreaFloor = 0;
		CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseParsing.HighTime;
		UserOperation.fRun = UO_FRUN_RUNNING;
	}
	if(UserOperation.fRun == UO_FRUN_RUNNING)
	{
		CP.PulseTimerOutput.RunTimeCnt++;
		if(CP.PulseTimerOutput.CycleNumCnt < CP.PulseParsing.CycleNum)
		{
			if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaFloor)
			{
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);												//输出0
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaFloor && CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaCeiling)
			{
				Output_VorC(CP.bFlagVC, CP.PulseParam.PulseThree.Ampl, OUTPUT_ENABLE);					//输出设置幅值
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaCeiling)
			{
				if(CP.PulseTimerOutput.PulseNumCnt < CP.PulseParsing.PulseNum - 1)						//LZG_WFC,CP.PulseParsing.PulseNum判断需多次测试
				{
					CP.PulseTimerOutput.PulseNumCnt++;
					CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.LowTime;
					CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;				
				}
				else if(CP.PulseTimerOutput.PulseNumCnt == CP.PulseParsing.PulseNum - 1)
				{
					CP.PulseTimerOutput.PulseNumCnt++;
					CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.BaseTime + CP.PulseParsing.StepTime * CP.PulseParsing.StepNum;
					CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;				
				}
				else if(CP.PulseTimerOutput.PulseNumCnt == CP.PulseParsing.PulseNum)
				{
					CP.PulseTimerOutput.PulseNumCnt = 0;
					CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.PauseTime;
					CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;
					
					CP.PulseTimerOutput.StepNumCnt++;
					if(CP.PulseTimerOutput.StepNumCnt >= CP.PulseParsing.StepNum)
					{
						CP.PulseTimerOutput.RunTimeCnt = 0;
						CP.PulseTimerOutput.StepNumCnt = 0;
						CP.PulseTimerOutput.CycleNumCnt++;
					}
				}
			}
		}
		else
		{
			CP.PulseTimerOutput.RunTimeCnt = 0;
			Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
			UserOperation.fRun = UO_FRUN_STOP;
		}		
	}	
}

void Process_PULSE_TYPE_FOUR(void)
{
	if(UserOperation.fRun == UO_FRUN_START)
	{
		CP.PulseTimerOutput.RunTimeCnt = 0;
		CP.PulseTimerOutput.HTimeAreaFloor = 0;
		CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseParsing.HighTime;
		UserOperation.fRun = UO_FRUN_RUNNING;
	}
	if(UserOperation.fRun == UO_FRUN_RUNNING)
	{
		CP.PulseTimerOutput.RunTimeCnt++;
		if(CP.PulseTimerOutput.CycleNumCnt < CP.PulseParsing.CycleNum)
		{
			if(CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaFloor)
			{
				Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);												//输出0
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaFloor && CP.PulseTimerOutput.RunTimeCnt <= CP.PulseTimerOutput.HTimeAreaCeiling)
			{
				Output_VorC(CP.bFlagVC, CP.PulseParsing.Ampl, OUTPUT_ENABLE);							//输出设置幅值
			}
			else if(CP.PulseTimerOutput.RunTimeCnt > CP.PulseTimerOutput.HTimeAreaCeiling)
			{
				if(CP.PulseTimerOutput.PulseNumCnt < CP.PulseParsing.PulseNum)
				{
					CP.PulseTimerOutput.PulseNumCnt++;
					CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.BaseTime + CP.PulseParsing.StepTime * CP.PulseParsing.StepNum;
					CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;
				}
				else
				{
					CP.PulseTimerOutput.PulseNumCnt = 0;
					CP.PulseTimerOutput.HTimeAreaFloor += CP.PulseParsing.HighTime + CP.PulseParsing.PauseTime;
					CP.PulseTimerOutput.HTimeAreaCeiling = CP.PulseTimerOutput.HTimeAreaFloor + CP.PulseParsing.HighTime;
					
					CP.PulseTimerOutput.StepNumCnt++;
					if(CP.PulseTimerOutput.StepNumCnt >= CP.PulseParsing.StepNum)
					{
						CP.PulseTimerOutput.RunTimeCnt = 0;
						CP.PulseTimerOutput.StepNumCnt = 0;
						CP.PulseTimerOutput.CycleNumCnt++;
					}
				}
			}
		}
		else
		{
			CP.PulseTimerOutput.RunTimeCnt = 0;			
			Output_VorC(CP.bFlagVC, 0, OUTPUT_DISABLE);
			UserOperation.fRun = UO_FRUN_STOP;
		}
	}	
}

