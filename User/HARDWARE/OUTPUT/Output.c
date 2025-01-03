
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
		
		//pTRIGGER_OUT = 1;
		pLEDOUTPUT = LED_DIRECTLY_OFF;
		BEEP = BEEP_OFF;
	}
}

