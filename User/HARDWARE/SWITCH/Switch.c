
#include  "main.h"

sSwitch_t Switch = {0x00, 0x00};
/********************************************************************
*	功能	：	初始化
******************************************************************************/
void Switch_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIOB时钟
	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;						//PA7 控制输出继电器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;						//PB12 控制模式继电器
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}

/********************************************************************
*	功能	：	切换操作
******************************************************************************/
void Switch_Ctrl(uint8_t phase)
{
	static uint8_t bphase_pre = 0x00;
	
	if(Switch.ModeCur != Switch.ModePre || bphase_pre != phase)
	{		
		if(phase == UO_PHASE_UNIPHASE)
		{
			if((Switch.ModeCur & (1 << MODE_BIT_VC)) == (SELECT_VC_V << MODE_BIT_VC))   //MODE_BIT_VC		0     SELECT_VC_V		1
			{
				//极性切换，电压正极性
				//Voltage_Ploar_Flag	=	0;
				//Current_Ploar_Flag	=	0;
				log_info("SW Mode to Voltage, Positive voltage!\r\n");
				
				pLEDVC				= 0;
				SW_CV_MODE = 0;     //切换为电压模式
				AD5542_Output(DA_CHNL_CURR,0);//将电流通道DAC设置为0
			}
			else
			{
				//极性切换,电流正极性
				//Voltage_Ploar_Flag	=	0;
				//Current_Ploar_Flag	=	0;
				log_info("SW Mode to Current, Positive current!\r\n");
				pLEDVC				= 1;
				SW_CV_MODE = 1;       //切换为电流模式
				AD5542_Output(DA_CHNL_VOLT,0);//将电压通道DAC设置为0
				
			}
		}
		else if(phase == UO_PHASE_BIPHASE)
		{
			if((Switch.ModeCur & (1 << MODE_BIT_VC)) == (SELECT_VC_V << MODE_BIT_VC))
			{
				//极性切换,电压负极性
				//Voltage_Ploar_Flag	=	1;
				//Current_Ploar_Flag	=	1;
				log_info("SW Mode to Voltage, Negative voltage!\r\n");
				pLEDVC				= 0;
				SW_CV_MODE = 0 ;				//切换为电压模式
				AD5542_Output(DA_CHNL_CURR,0);//将电流通道DAC设置为0
			}
			else
			{
				//极性切换,电流负极性
				//Voltage_Ploar_Flag	=	1;
				//Current_Ploar_Flag	=	1;
				log_info("SW Mode to Current, Negative current!\r\n");
				pLEDVC				= 1;
				SW_CV_MODE = 1 ;			 //切换为电流模式
				AD5542_Output(DA_CHNL_VOLT,0);//将电压通道DAC设置为0
			}
		}
		else
		{
			
		}
		
		Switch.ModePre = Switch.ModeCur;
		bphase_pre = phase;
		Delay_ms(500);
		
	}
	else
	{
		
	}
}
