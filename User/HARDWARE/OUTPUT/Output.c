
#include  "main.h"

/********************************************************************
*	功能	：输出电压/电流
*	关系	：
				电压-----输入0-4V ，输出0-100V
				电流-----输入0-4V，输出0-32mA
*	备注	：	
				ampl单位是uV/nA,即输出1V时需设置ampl值为1000000
******************************************************************************/
void Output_VorC(uint8_t select_vc, int32_t ampl, uint8_t IsEnable)
{	
	if(IsEnable == OUTPUT_ENABLE)
	{
		//log_info("OUTPUT_ENABLE select_vc:%d  Voltage amp=%d\r\n",select_vc,ampl);
		if(select_vc == SELECT_VC_V)
		{			
			if(UserOperation.bPhase == UO_PHASE_UNIPHASE)						//正极性		
			{
				//log_info("OUTPUT_ENABLE  Voltage amp=%d,%d\r\n",ampl,ampl / 25000);
				
				AD5542_Output(DA_CHNL_VOLT, ampl / 25000 );					//(value/VALUE_EXPAND_105--V)*1000--DAC REGISTER，调整计算顺序，否则超限溢出，输出正极性
			}
			else
			{
				AD5542_Output(DA_CHNL_VOLT, 0 - ampl / 25000);	//输出负极性
			}
		}
		else if(select_vc == SELECT_VC_C)
		{	
			//log_info("SELECT_VC_C select_vc:%d  Voltage amp=%d\r\n",select_vc,ampl);			
			if(UserOperation.bPhase == UO_PHASE_UNIPHASE)					
			{
				AD5542_Output(DA_CHNL_CURR, (ampl / 1000) / 8);					//((value/1000000--mA)/5--V)*1000--DAC REGISTER	输出正极性
			}
			else
			{
				AD5542_Output(DA_CHNL_CURR, 0 - (ampl / 1000) / 8); //输出负极性
			}		
		}
		else{}
	}
	else if(IsEnable == OUTPUT_DISABLE)
	{
		if(select_vc == SELECT_VC_V)
		{
			AD5542_Output(DA_CHNL_VOLT, 0);
		}
		else if(select_vc == SELECT_VC_C)
		{
			AD5542_Output(DA_CHNL_CURR, 0);
		}
		else{}		
		
		
		//pTRIGGER_OUT = 1;
		pLEDOUTPUT = LED_DIRECTLY_OFF;
		BEEP = BEEP_OFF;
	}
	else{}
}











































/********************************************************************
*	功能	：	测试函数
******************************************************************************/
#if(TEST_OUTPUT)
	void test_OUTPUT(uint8_t mode)
	{
		static uint8_t testconf = 0;																//测试函数配置一次参数，然后测试效果
		if(testconf == 0)
		{
//			testconf = 1;
			if(mode == TEST_VOLT_L)
			{
				//电压测试			
//				pSELECT_VC = SELECT_VC_V;
//				pSELECT_VHL = SELECT_VHL_L;
//				pSELECT_FLIP = SELECT_FLIP_UN;
				
				Switch.ModeCur = 0;
				Switch.ModeCur = (SELECT_VC_V << MODE_BIT_VC) | (SELECT_FLIP_UN << MODE_BIT_FLIP) | (SELECT_VHL_L<< MODE_BIT_VHL);
				
				UserOperation.bVC	= SELECT_VC_V;
				UserOperation.fMode	= UO_MODE_FREERUN;
				UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE]	= 1000 * VALUE_EXPAND_105;		//1000ms
				
				/**	AMPL VALUE TEST	--	BEGIN										*/
//				UserOperation.V_ModeFreeRun.Ampl	= 5 * VALUE_EXPAND_105 / 10;	//0.5V
//				UserOperation.V_ModeFreeRun.Ampl	= 3 * VALUE_EXPAND_105 / 10;
//				UserOperation.V_ModeFreeRun.Ampl	= 3 * VALUE_EXPAND_105;			//3V
				UserOperation.V_ModeFreeRun.Ampl	= 5 * VALUE_EXPAND_105;			//5V
//				UserOperation.V_ModeFreeRun.Ampl	= 8 * VALUE_EXPAND_105;			//8V
				/**	AMPL VALUE TEST	--	END											*/
				
				UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ]	= 1 * VALUE_EXPAND_102;			//1Hz
			}
			else if(mode == TEST_VOLT_H)
			{
				//电压测试
//				pSELECT_VC = SELECT_VC_V;
//				pSELECT_VHL = SELECT_VHL_H;
//				pSELECT_FLIP = SELECT_FLIP_UN;
				
				Switch.ModeCur = 0;
				Switch.ModeCur = (SELECT_VC_V << MODE_BIT_VC) | (SELECT_FLIP_UN << MODE_BIT_FLIP) | (SELECT_VHL_H<< MODE_BIT_VHL);
				
				UserOperation.bVC	= SELECT_VC_V;
				UserOperation.fMode	= UO_MODE_FREERUN;
				UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE]	= 1000 * VALUE_EXPAND_105;		//1000ms
				
				/**	AMPL VALUE TEST	--	BEGIN										*/
//				UserOperation.V_ModeFreeRun.Ampl	=  7 * VALUE_EXPAND_105;
//				UserOperation.V_ModeFreeRun.Ampl	= 12 * VALUE_EXPAND_105;		//12V
				UserOperation.V_ModeFreeRun.Ampl	= 18 * VALUE_EXPAND_105;		//18V
//				UserOperation.V_ModeFreeRun.Ampl	= 25 * VALUE_EXPAND_105;		//25V
//				UserOperation.V_ModeFreeRun.Ampl	= 30 * VALUE_EXPAND_105;		//30V
//				UserOperation.V_ModeFreeRun.Ampl	= 50 * VALUE_EXPAND_105;		//50V
//				UserOperation.V_ModeFreeRun.Ampl	= 75 * VALUE_EXPAND_105;		//75V
//				UserOperation.V_ModeFreeRun.Ampl	= 90 * VALUE_EXPAND_105;		//90V
				/**	AMPL VALUE TEST	--	END											*/
				
				UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ]	= 1 * VALUE_EXPAND_102;			//1Hz
			}
			else if(mode == TEST_CURR)
			{
				//电流测试
//				pSELECT_VC = SELECT_VC_C;
//				pSELECT_VHL = SELECT_VHL_L;
//				pSELECT_FLIP = SELECT_FLIP_UN;
				
				Switch.ModeCur = 0;
				Switch.ModeCur = (SELECT_VC_C << MODE_BIT_VC) | (SELECT_FLIP_UN << MODE_BIT_FLIP) | (SELECT_VHL_L<< MODE_BIT_VHL);
				
				UserOperation.bVC	= SELECT_VC_C;
				UserOperation.fMode	= UO_MODE_FREERUN;
				UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE]	= 1000 * VALUE_EXPAND_105;		//1000ms
//				UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE]	= 2 * VALUE_EXPAND_105;			//2ms / 1000ms
				
				/**	AMPL VALUE TEST	--	BEGIN										*/		//current(ma)		volt-input XTR111
//--------------------------------------------------------uA----------------------------------------------------------------------//
//				UserOperation.C_ModeFreeRun.Ampl	=   5 * VALUE_EXPAND_102;			//uA--[0.021,0.024]		--2.7mV  3.1mv				
//				UserOperation.C_ModeFreeRun.Ampl	=   6 * VALUE_EXPAND_102;			//uA--[0.023,0.024]		--3.1mv
//				UserOperation.C_ModeFreeRun.Ampl	=   7 * VALUE_EXPAND_102;			//uA--[0.019,0.019]		--2.7mv
//				UserOperation.C_ModeFreeRun.Ampl	=   8 * VALUE_EXPAND_102;			//uA--[0.024,0.024]		--3.1
//				UserOperation.C_ModeFreeRun.Ampl	=   9 * VALUE_EXPAND_102;			//uA--[0.024,0.024]		--3.1
//				UserOperation.C_ModeFreeRun.Ampl	=  10 * VALUE_EXPAND_102;			//uA--[0.025,0.026]0.028--4.1
//				UserOperation.C_ModeFreeRun.Ampl	=  11 * VALUE_EXPAND_102;			//uA--[0.028,0.029]		--4.1
//				UserOperation.C_ModeFreeRun.Ampl	=  12 * VALUE_EXPAND_102;			//uA--[0.029,0.029]		--4.1
//				UserOperation.C_ModeFreeRun.Ampl	=  13 * VALUE_EXPAND_102;			//uA--[0.027,0.028]		--4.1
//				UserOperation.C_ModeFreeRun.Ampl	=  14 * VALUE_EXPAND_102;			//uA--[0.028,0.028]		--4.1
//				UserOperation.C_ModeFreeRun.Ampl	=  15 * VALUE_EXPAND_102;			//uA--[0.032,0.032]		--5.0
//				UserOperation.C_ModeFreeRun.Ampl	=  16 * VALUE_EXPAND_102;			//uA--[0.032,0.032]		--5.0
//				UserOperation.C_ModeFreeRun.Ampl	=  17 * VALUE_EXPAND_102;			//uA--[0.032,0.032]		--5.0
//				UserOperation.C_ModeFreeRun.Ampl	=  18 * VALUE_EXPAND_102;			//uA--[0.032,0.032]		--5.0
//				UserOperation.C_ModeFreeRun.Ampl	=  19 * VALUE_EXPAND_102;			//uA--[0.032,0.032]		--5.0
//				UserOperation.C_ModeFreeRun.Ampl	=  20 * VALUE_EXPAND_102;			//uA--[0.035,0.036]		--6.0
//				UserOperation.C_ModeFreeRun.Ampl	=  30 * VALUE_EXPAND_102;			//uA--[0.043,0.042]0.046--8.0
//				UserOperation.C_ModeFreeRun.Ampl	=  40 * VALUE_EXPAND_102;			//uA--[0.053,0.054]		--9.9
//				UserOperation.C_ModeFreeRun.Ampl	=  50 * VALUE_EXPAND_102;			//uA--[0.063,0.064]		--11.9
//				UserOperation.C_ModeFreeRun.Ampl	=  80 * VALUE_EXPAND_102;			//uA--[0.091,0.092]		--17.8
//				UserOperation.C_ModeFreeRun.Ampl	= 100 * VALUE_EXPAND_102;			//uA--[0.108,0.109]		--21.9
//				UserOperation.C_ModeFreeRun.Ampl	= 150 * VALUE_EXPAND_102;			//uA--[0.157,0.158]0.160--31.8
//				UserOperation.C_ModeFreeRun.Ampl	= 200 * VALUE_EXPAND_102;			//uA--[0.209,0.210]0.212--41.9
//--------------------------------------------------------mA----------------------------------------------------------------------//
//				UserOperation.C_ModeFreeRun.Ampl	=   1 * VALUE_EXPAND_105;			//mA--[1.008,1.009]1.013--202mV
//				UserOperation.C_ModeFreeRun.Ampl	=   3 * VALUE_EXPAND_105;
				UserOperation.C_ModeFreeRun.Ampl	=   5 * VALUE_EXPAND_105;			//mA--[4.98X,4.99X]		--1002mV
//				UserOperation.C_ModeFreeRun.Ampl	=  15 * VALUE_EXPAND_105;
//				UserOperation.C_ModeFreeRun.Ampl	=  25 * VALUE_EXPAND_105;
				/**	AMPL VALUE TEST	--	END											*/
				
				UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ]	= 1 * VALUE_EXPAND_102;			//1Hz
			}
			UserOperation.fRun = UO_FRUN_START;
		}
	}
#endif
