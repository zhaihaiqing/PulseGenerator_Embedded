
#include  "main.h"

sPwmArrayParam_t PwmArrayParam[8]   = {0};															//如不定义，每次都会在涉及到动态分配时重启，可能因pPwmArrayParam出现野指针情况
sPwmArrayParam_t *pPwmArrayParam[8] = {&PwmArrayParam[0], &PwmArrayParam[1], &PwmArrayParam[2], &PwmArrayParam[3], &PwmArrayParam[4], &PwmArrayParam[5], &PwmArrayParam[6], &PwmArrayParam[7]};
	
__align(8) static float_t dTempVar = 0;																//临时变量，用于临时存储计算结果，防止计算过程溢出，计算完成后对应赋值
/********************************************************************
*	功能	：	根据配置频率、占空比计算PWM匹配ARR/PSC/DUTY
*	参数	：	
				freq--频率，为实际配置值的100倍，以兼容小数点后两位有效数字,[0.02Hz, 30KHz] * PULSE_FREQ_MAGNIFY
				duty--占空比，基数50000，即满占空比时为50000
				pwm---输出波形结构体指针
*	限值	：	
				psc--[15:0]				arr--[15:0]
*	备注	：	
				对于总线84MHz
				设置输出波形频率50KHz，则输出波形周期 T = 1 / 50KHz = 20us
				①若psc = 1分频则每1us计数为	72M / 1M = 72，则每个波形周期计数	50 * (72 / 1) = 3600
				  若psc = 2分频则每1us计数为	72M / 2 / 1M = 36,每个波形周期计数	50 * (72 / 2) = 1800
				②根据Tout = ((arr+1)*(psc+1))/Tclk
				此时计数上限	arr * psc = 72M / 20K = 3600,即	arr = 72M / 20K / psc
				③如果	(1M / 20KHz) * (72 / psc) > 72M / 20KHz / psc，则波形周期输出全高电平
				  等效	(1 / freq) * (72 / psc) > 72M / freq / psc，则波形周期输出全高电平
******************************************************************************/
static uint8_t PulseTran_Sub_CalcPwmPara(float_t freq, float_t duty, sPwmParam_t *pwm)
{
	uint16_t psc1  = 0, psc2 = 0;
	float_t  temp = 0;
	
#ifdef NEED_FLOAT_CALC
	float_t  arr1 = 0, arr2 = 0;		`																//WFC，待修改为整数，浮点数占用时间过长，第三种波形甚至会触发看门狗
#else
	uint32_t arr1 = 0, arr2 = 0;
#endif
	
	/*	temp--[2.400, 3.600.000.000]	Tout = ((arr+1)*(psc+1))/Tclk	(arr+1)*(psc+1) = Tclk/freq		*/
	temp = 84000000  / ((float_t)freq / 100);															//波形周期，单位计数频率84000000 / 波形频率 = 单个波计数个数。两次乘10为PULSE_FREQ_MAGNIFY，如此处理避免直接相乘溢出或后乘偏差大

	//log_info("temp=%f\r\n",temp);
		
	/*	
		自编小算法，求偏差期望频率较小配置参数，无法使用电脑端可用的穷举法，仍有偏差
		思路：较小时直接配置psc为1，零误差；较大时，判断开方和据需除n个10降低位数配置进行比较，偏差小者作为配置值
	*/
	if(temp <= 65535)
	{
		arr1 = temp;
		psc1 = 1;
	}
	else
	{
		arr1 = sqrt(temp);
		psc1 = temp / arr1;
		
		if(temp > 65535 * 10000)
		{
			arr2 = temp / 60000;
			psc2 = 60000;
		}
		else if(temp > 65535 * 1000)
		{
			arr2 = temp / 10000;																		//除以1000是肯定不合适的，只能除以10000降低位数，误差在0， 999之间
			psc2 = 10000;
		}
		else if(temp > 65535 * 100)
		{
			arr2 = temp / 1000;
			psc2 = 1000;
		}
		else if(temp > 65535 * 10)
		{
			arr2 = temp / 100;
			psc2 = 100;
		}
		else
		{
			arr2 = temp / 10;
			psc2 = 10;
		}
		
		if(temp - arr1 * psc1 > temp - arr2 * psc2)
		{
			arr1 = arr2;
			psc1 = psc2;
		}
		else{}
	}
	
	


	pwm->PwmARR = arr1 - 1;																			//arr，单个波计数个数
	pwm->PwmPSC = psc1 - 1;																			//psc，单位计数频率72000000 / psc / arr，psc更多设计频率分频，通过波形周期进行计算	
	
	if(duty >= PULSE_DUTY_FULL)
	{
		pwm->PwmDuty = pwm->PwmARR + 1;																//100%占空比直接配置为ARR，低频时能捕捉到电平翻转
	}
	else if(duty == 0)
	{
		pwm->PwmDuty = 0;
	}
	else
	{
		if(pwm->PwmARR + 1 < PULSE_DUTY_FULL / duty)												//分频值过大，导致计数值无法满足占空比大于1的特殊情况处理【2018-1-19】
		{
			temp = pwm->PwmARR;
			pwm->PwmARR = PULSE_DUTY_FULL / duty - 1;
			temp = pwm->PwmPSC * temp / pwm->PwmARR;
			if(temp < 1)																			//保证即使波形配置异常，psc不为异常值。此时，输出波形与用户设置有偏差，表现在周期比配置小，但仍有近似波形输出
			{
				temp = 1;
			}
			else{}
			pwm->PwmPSC = temp - 1;
		}
		else{}
		
		temp = (pwm->PwmARR + 1) * duty / PULSE_DUTY_FULL;
		pwm->PwmDuty = (uint16_t)(temp + 0.5);
	}
	
	log_info("arr1=%d,psc1=%d\r\n",pwm->PwmARR,pwm->PwmPSC);
	
	return 0;
}

/***************************************************************************************************
*	功能	：	第一种波形处理
*	特征	：	
				单跳变沿输出
*	处理	：	
				考虑频率过低情况，可以模拟成两个阶段的PWM波形，分别为0占空比和100占空比的阶段
				每个阶段的长度持续时间折合成PWM数量，固定频率10kHz,周期100us
*	参数	：	
				sPulseXXX_t     --串口接收到的配置数据
				sPwmArrayParam_t--规整为可用的参数数组
*******************************************************************************************************************/
eRtn_t PulseTran_One(sPulseOne_t *one, sPwmArrayParam_t *param)
{
	uint8_t  index = 0;
	uint32_t duty  = 0;
	float_t  freq  = 0;
	
	if((one == NULL) || (param == NULL))
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	
	if((one->MsStageFirst == 0) && (one->MsStageSecond == 0))
	{
		return RTN_ABNORMAL_OTHER;
	}
	else{}
	
	if(one->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else
	{
		param->Ampl = one->Ampl * 1000;																//统一到运算使用单位，mV/uA->uV/nA
	}
	
	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * 2);
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	
	freq = PULSE_FREQ_AUXILIARY;
	/*	第一步	*/
	if(one->MsStageFirst)																			//用户设置特殊0值处理
	{		
		duty = (one->IsLowFirst)? 0:PULSE_DUTY_FULL;												//低电平则0%占空比，否则100%
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));	
		dTempVar = one->MsStageFirst * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
		param->Pwm[index].PwmNum = (uint32_t)dTempVar;												//延时较长时，与freq会出现积大于uint32_t情况，且如果ms先除会导致积为0情况
		index++;
		#if 0
			printf("ARR10 = %u\r\n",  	 param->Pwm[0].PwmARR);
			printf("Psc10 = %u\r\n",  	 param->Pwm[0].PwmPSC);
			printf("Duty10 = %u\r\n", 	 param->Pwm[0].PwmDuty);
			printf("PwmNum10 = %u\r\n",  param->Pwm[0].PwmNum);
		#endif
	}
	else{}
	
	/*	第二步	*/
	if(one->MsStageSecond)
	{
		duty = (one->IsLowFirst)? PULSE_DUTY_FULL:0;
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
		dTempVar = one->MsStageSecond * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
		param->Pwm[index].PwmNum = (uint32_t)dTempVar;
		index++;
		#if 0
			printf("ARR11 = %u\r\n",     param->Pwm[1].PwmARR);
			printf("Psc11 = %u\r\n",     param->Pwm[1].PwmPSC);
			printf("Duty11 = %u\r\n",    param->Pwm[1].PwmDuty);
			printf("PwmNum11 = %u\r\n",  param->Pwm[1].PwmNum);
		#endif
	}
	else{}
	
	param->VarietyNum = index;
	param->CycleNum   = 1;
	
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	第二种波形处理
*	特征	：	
				固定波形输出
*	处理	：	
				可以模拟成一个阶段的PWM波形，频率和占空比是有通道参数得出，持续时间转换为PWM个数
				波形个数
					为整数时，正常处理
					否则，根据余数处理
						若大于高电平时长，则波形个数加1后，按照整数波形处理
						若小于高电平时长，根据余数重新折算波形占空比
*******************************************************************************************************************/
eRtn_t PulseTran_Two(sPulseTwo_t *two, sPwmArrayParam_t *param)
{
	uint32_t duty = 0;
	float_t  freq = 0;
	
	if((two == NULL) || (param == NULL))
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	
	if(two->Duty == 0 || two->MsDuration == 0)
	{
		return RTN_ABNORMAL_DUTY;
	}
	else{}
	
	if(two->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else
	{
		param->Ampl = two->Ampl * 1000;
	}

	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * 2);
	if(param->Pwm == NULL)
	{
		#if 0
			printf("PULSETRAN_TWO_p0\r\n");
		#endif
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	#if 0	
		printf("PULSETRAN_TWO_p1\r\n");
	#endif	
	
	if(two->Freq < PULSE_FREQ_LOWERLIMIT)
	{
		two->Freq = PULSE_FREQ_LOWERLIMIT;
	}
	else{}
	freq = two->Freq;
	duty = two->Duty;
	PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[0]));
	dTempVar = two->MsDuration * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
	param->Pwm[0].PwmNum = (uint32_t)dTempVar;														//MsDuration >= 1000/ (freq / PULSE_FREQ_MAGNIFY)
	#if 0
		printf("Freq2 = %u\r\n",   freq);
		printf("duty2 = %u\r\n",   duty);
		printf("PwmNum2 = %u\r\n", param->Pwm[0].PwmNum);
	#endif	

	if(dTempVar - param->Pwm[0].PwmNum == 0)														//整数个波形
	{		
		param->VarietyNum = 1;
	}
	else
	{
		if((dTempVar - param->Pwm[0].PwmNum) * param->Pwm[0].PwmARR >= param->Pwm[0].PwmDuty)		//余数大于高电平时长
		{
			param->Pwm[0].PwmNum += 1;
			
			param->VarietyNum     = 1;
		}
		else if(param->Pwm[0].PwmNum == 0)															//没有整数部分，余数折算波形占空比
		{
			param->Pwm[0].PwmDuty = (dTempVar - param->Pwm[0].PwmNum) * param->Pwm[0].PwmARR;
			param->Pwm[0].PwmNum  = 1;
			
			param->VarietyNum     = 1;
		}
		else if(param->Pwm[0].PwmNum > 0)															//有整数部分，余数折算占空比
		{
			param->Pwm[1].PwmARR  = param->Pwm[0].PwmARR;
			param->Pwm[1].PwmPSC  = param->Pwm[0].PwmPSC;
			param->Pwm[1].PwmDuty = (dTempVar - param->Pwm[0].PwmNum) * param->Pwm[0].PwmARR;
			param->Pwm[1].PwmNum  = 1;
			
			param->VarietyNum     = 2;
		}
		else{}
	}
	param->CycleNum   = 1;
		
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	第三种波形处理
*	特征	：	
				多个阶段，每个阶段有n个波及最后一个波前的延时组成，且延时随阶段线性变化，阶段间固定延时
*	处理	：	
				每个循环阶段可以模拟成五种PWM波形，频率由时间换算，占空比由周期时间和固定高电平时长换算

				第一步--阶段内延长时间前的PWM数量-1【波形个数不包括每个阶段最后一个高电平】
				第二步--（起始/截止类）时间包含的一个波形高电平（因 第三步 时间不一定≥完整波中的低电平时间，兼容＜情况）
				第三步--（起始/截止类）时间减去波形高电平时间
				第四部--（起始/截止类）时间随后的一个波形
				第五步--阶段间暂停时间

				阶段数由初始延长时间、截止延长时间和步长延长时间决定
*******************************************************************************************************************/
eRtn_t PulseTran_Three(sPulseThree_t *three, sPwmArrayParam_t *param)
{
	uint8_t  KindNum = 0;
	uint32_t SectionNum = 0, msDealy = 0;
	uint32_t i = 0, index = 0;
	float_t  freq = 0, duty = 0, three_freq = 0;
	
	if(three == NULL || param == NULL)
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	if(three->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else
	{
		param->Ampl = three->Ampl * 1000;
	}
	
	if(three->MsBegin == three->MsEnd || three->MsStep == 0)									//用户特殊配置值处理
	{
		SectionNum = 1;
	}
	else if(three->MsEnd > three->MsBegin)
	{
		SectionNum = 1 + (three->MsEnd - three->MsBegin) / three->MsStep;						//n*复合运行单元
	}
	else
	{
		SectionNum = 1 + (three->MsBegin - three->MsEnd) / three->MsStep;
	}
	KindNum = (three->Pause100ms!=0)? 5:4;														//复合运行单元中波形种类数,bug修复【2018-2-2】
	#if 0
		printf("SectionNum3 = %u\r\n", SectionNum);
		printf("TABLEINDEX3 = %u\r\n", sizeof(sPwmParam_t) * KindNum * SectionNum);	
	#endif	

	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * KindNum * SectionNum);
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	
	three_freq = PULSE_FREQ_MAGNIFY * 1000 / three->Period;										//	(1000 / three->WaveLenght) * PULSE_UNIT_FREQ调整计算顺序，避免产生较大误差
	for(i=0; i<SectionNum; i++)
	{
		WDG_Feed();																				//耗时较长，不加看门狗则引起重启
		/*	第一步	*/
		freq = three_freq;
		duty = three->HighTime * PULSE_DUTY_FULL / three->Period;
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
		param->Pwm[index].PwmNum = three->PulseNum - 1;
		index++;
		
		/*	第二步	*/
		freq = three_freq * PULSE_DUTY_FULL / duty;												//把单个高电平时间在原来波形基础倍频，视其为一孤立高电平，把高电平当做一个整波，换算出对应频率处理
		duty = PULSE_DUTY_FULL;
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
		param->Pwm[index].PwmNum = 1;
		index++;
		
		/*	第三步	*/
		if(three->MsEnd > three->MsBegin)
		{
			msDealy = three->MsBegin + i * three->MsStep;
		}
		else
		{
			if(three->MsBegin > i*three->MsStep)
			{
				msDealy = three->MsBegin - i * three->MsStep;
			}
			else
			{
				msDealy = 0;
			}
		}

		if(msDealy - three->HighTime > 0)														//间隔时间减去上一个波高电平时间
		{
			if(msDealy - three->HighTime < 50 * 1000)
			{
				freq = 1000 * PULSE_FREQ_MAGNIFY / (msDealy - three->HighTime);
				duty = 0;
				PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
				param->Pwm[index].PwmNum = 1;													//ms内波形个数
				index++;
			}
			else																				//持续时间较长情况处理
			{
				freq = PULSE_FREQ_AUXILIARY;
				duty = 0;
				PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
				
				dTempVar = (msDealy - three->HighTime) * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);	
				param->Pwm[index].PwmNum = (uint32_t)dTempVar;									//ms内波形个数
				index++;
			}
		}
		else{}
		
		/*	第四步	*/
		freq = three_freq;
		duty = three->HighTime * PULSE_DUTY_FULL / three->Period;
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
		param->Pwm[index].PwmNum = 1;
		index++;
		
		/*	第五步	*/
		msDealy = three->Pause100ms * 100;
		if(msDealy != 0)
		{
			freq = PULSE_FREQ_AUXILIARY;
			duty = 0;
			PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
			dTempVar = msDealy * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
			param->Pwm[index].PwmNum = (uint32_t)dTempVar;			
			index++;
		}
		else{}
		#if 0
			printf("PULSETRAN_THREE_p4\r\n");
		#endif
	}
	#if 0	
		printf("PwmNum30 = %u\r\n", param->Pwm[0].PwmNum);
		printf("PwmNum31 = %u\r\n", param->Pwm[1].PwmNum);
		printf("PwmNum32 = %u\r\n", param->Pwm[2].PwmNum);
		printf("PwmNum33 = %u\r\n", param->Pwm[3].PwmNum);
		printf("PULSETRAN_THREE_p3\r\n");
	#endif
	
	if(three->CycleNum == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else{}
	
	param->VarietyNum = index;
	param->CycleNum   = three->CycleNum;
	
	return RTN_NORMAL;
}

/********************************************************************
*	功能	：	计算占空比
*	计算	：	周期改变，高电平时间不变
******************************************************************************/
static uint16_t PulseTran_Four_Sub_CalcDuty(uint16_t hightime, uint32_t curPeriod)
{
    double dutyCur;

	dutyCur = hightime * PULSE_DUTY_FULL / curPeriod;
    return (uint16_t)(dutyCur + 0.5);
} 
  
/***************************************************************************************************
*	功能	：	第四种波形处理
*	特征	：	
				多个阶段，每个阶段有n个高电平时长固定、周期线性变化的波形组成，阶段间固定延时
*	处理	：	
				每个循环阶段可以模拟成二种PWM波形，频率由周期换算，占空比由周期和固定高电平时长换算
				第一步--阶段内参数设置
				第二步--阶段间延时时间
				阶段数由初始周期、截止周期和步长周期决定
*******************************************************************************************************************/
eRtn_t PulseTran_Four(sPulseFour_t *four, sPwmArrayParam_t *param)
{
	uint8_t  KindNum = 0;
	uint32_t i = 0, index = 0;
	uint32_t SectionNum = 0, msDelay = 0;
	float_t  four_period = 0, freq = 0, duty = 0;
	
	#if 0
		printf("PULSETRAN_FOUR_p0\r\n");
	#endif
	if(param == NULL || four == NULL)
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	#if 0
		printf("PULSETRAN_FOUR_p1\r\n");
	#endif
	if(four->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else
	{
		param->Ampl = four->Ampl * 1000;
	}
	
	if((four->PeriodBegin == 0) && (four->PeriodStep == 0))										//用户特殊配置值处理
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	if(four->PeriodBegin == 0)
	{
		four->PeriodBegin += four->PeriodStep;
	}
	else{}
	if(four->PeriodEnd == 0)
	{
		four->PeriodEnd += four->PeriodStep;
	}
	else{}
	
	if(four->PeriodStep == 0)
	{
		SectionNum = 1;																			//步进为0则输出一个阶段
	}
	else if(four->PeriodEnd > four->PeriodBegin)
	{
		SectionNum = (four->PeriodEnd - four->PeriodBegin) / four->PeriodStep;
	}
	else
	{
		SectionNum = (four->PeriodBegin - four->PeriodEnd) / four->PeriodStep;
	}
		
	if(four->PeriodEnd != 0 && four->PeriodBegin != 0)
	{
		SectionNum += 1;
	}
	else{}
	if(SectionNum == 0)																			//[V134]将此语句置后解决步进大于起止周期差只运行一次时出现超出设定周期范围多运行一次情况bug
	{
		SectionNum = 1;
	}
	else{}
	if(four->PulseNum == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else
	{
		KindNum = 2;
	}
	#if 0
		printf("SectionNum4 = %u\r\n", SectionNum);
		printf("TABLEINDEX4 = %u\r\n", sizeof(sPwmParam_t) * KindNum * SectionNum);
	#endif
	
	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * KindNum * SectionNum);
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	#if 0
		printf("PULSETRAN_FOUR_p2\r\n");
	#endif
	
	four_period = four->PeriodBegin;															//避免freq作为差时为负数的情况进行处理
	for(i=0; i<SectionNum; i++)
	{
		WDG_Feed();																				//耗时较长，不加看门狗则引起重启
		/*	第一步	*/
		if(four->PeriodEnd > four->PeriodBegin)
		{
			four_period = four->PeriodBegin + four->PeriodStep * i;
		}
		else
		{
			if(four->PeriodBegin > four->PeriodStep * i)
			{
				four_period = four->PeriodBegin - four->PeriodStep * i;
			}
			else
			{
				four_period = four_period;														//与上一freq值一致，防止负数出现
			}
		}
		freq = 1000 / four_period * PULSE_FREQ_MAGNIFY;
		duty = PulseTran_Four_Sub_CalcDuty(four->HighTime, four_period);
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
		param->Pwm[index].PwmNum = four->PulseNum;
		index++;
		
		/*	第二步	*/
		msDelay = four->Pause100ms * 100;
		if(msDelay != 0)
		{
			if(msDelay > 50 * 1000)
			{
				freq = PULSE_FREQ_AUXILIARY;
				duty = 0;
				PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
				dTempVar = msDelay * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
				param->Pwm[index].PwmNum = (uint32_t)dTempVar;
				index++;
			}
			else
			{
				freq = 1000 * PULSE_FREQ_MAGNIFY / msDelay;										//原顺序1000 / msDelay * PULSE_FREQ_MAGNIFY，整数浮点数除法差异导致输出波形异常
				duty = 0;
				PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
				param->Pwm[index].PwmNum = 1;													//ms内波形个数
				index++;
			}
		}
		else{}
		#if 0	
			printf("PULSETRAN_FOUR_p4\r\n");
			printf("index = %u\r\n", index);
		#endif
	}	
	#if 0	
		printf("PwmNum40 = %u\r\n", param->Pwm[0].PwmNum);
		printf("PwmNum41 = %u\r\n", param->Pwm[1].PwmNum);
		printf("PULSETRAN_FOUR_p3\r\n");
	#endif	
	
	if(four->CycleNum == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else{}
	
	param->VarietyNum = index;
	param->CycleNum   = four->CycleNum;
	
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	第五种波形处理
*	特征	：	
				多个阶段，单个阶段由配置的间隔时间和拍摄时间组成，拍摄时间
				①第八通道，则由n个频率固定、占空比50%的波形组成
				②其它通道，则由n个频率固定、占空比100%的波形组成
*	处理	：	
				每个阶段并无固定关系，故逐个阶段处理解析，每个阶段分两步
				第一步--间隔时间，计算10K波形个数
				第二部--拍摄时间，计算给定参数波形个数
*******************************************************************************************************************/
eRtn_t PulseTran_Five(sPulseFive_t *five, sPwmArrayParam_t *param)
{
	uint16_t index = 0, i = 0;
	uint32_t duty = 0;
	uint32_t freq = 0;
	
	if((five == NULL) || (param == NULL))
	{
		return RTN_ABNORMAL_PARAM;
	}
	else{}
	
	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * 2 * five->ConfigNum);
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	
	if(five->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else
	{
		param->Ampl = five->Ampl * 1000;
	}	
	
	for(i=0; i<five->ConfigNum; i++)
	{
		/*	第一步	*/
		if(five->MsPause[i])
		{
			freq = PULSE_FREQ_AUXILIARY;
			duty = 0;
			PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
			dTempVar = five->MsPause[i] * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
			param->Pwm[index].PwmNum = (uint32_t)dTempVar;
			index++;
		}
		else{}
		
		/*	第二步	*/
		if(five->MsDuration[i])
		{
			freq = five->Freq;
			duty = five->Duty;
			PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
			dTempVar = five->MsDuration[i] * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
			param->Pwm[index].PwmNum = (uint32_t)dTempVar;
			index++;
		}
		else
		{
			freq = five->Freq;
			duty = 0;																			//时长为0时使用占空比为0填充数组[V138]
			PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[index]));
			dTempVar = five->MsDuration[i] * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY);
			param->Pwm[index].PwmNum = (uint32_t)dTempVar;
			index++;
		}
	}
	
	if(index == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else{}
	
	param->VarietyNum = index;
	param->CycleNum   = 1;
	
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	Single波形处理
*	特征	：	
				单个高电平脉冲
*	处理	：	
				分段考虑，目前最长300S，最短，60us（暂定）
				
				1：分频42倍，CLK=2MHz，周期=0.5us，16bit最长计数周期=65535*0.5=32.767ms，即0-32ms段
				2：分频420倍，CLK=200kHz，周期=5us，16bit最长计数周期=65535*5=327.67ms，即32-320ms段
				3：分频4200倍，CLK=20kHz，周期=50us，16bit最长计数周期=65535*50=3276.7ms，即320-3.2s段
				4：分频42000倍，CLK=2kHz，周期=500us，16bit最长计数周期=65535*500=32767ms，即3.2-32s段
				
				每个阶段的长度持续时间折合成PWM数量，固定频率10kHz,周期100us
*	参数	：					
				sPwmArrayParam_t--规整为可用的参数数组
*******************************************************************************************************************/
eRtn_t PulseTran_Single(sPwmArrayParam_t *param)
{
	uint32_t duty  = 0;
	float_t  freq  = 0;
	
	if(param->Pwm != NULL)																//如果已经申请内存，则先释放再重新申请，防止内存泄露和溢出
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * UserOperation.bPhase);
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}	
	
	if (UserOperation.bVC == SELECT_VC_V)
	{
		param->Ampl = UserOperation.V_ModeSingle.Param[UO_PARAM_AMPL];						//获取电压幅值参数
	}
	else
	{
		param->Ampl = UserOperation.C_ModeSingle.Param[UO_PARAM_AMPL];						//获取电流幅值参数
	}
	
	if(param->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else{}
	
	freq = PULSE_FREQ_AUXILIARY;			//辅助频率，50KHz，周期20us
	duty = PULSE_DUTY_FULL;
	PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[0]));
	
	if (UserOperation.bVC == SELECT_VC_V)
	{
		dTempVar = UserOperation.V_ModeSingle.Param[UO_PARAM_PULSE] / 1000 * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY) / 1000;		//计算进入中断的次数
	}
	else
	{
		dTempVar = UserOperation.C_ModeSingle.Param[UO_PARAM_PULSE] / 1000 * ((float_t)freq / 1000 / PULSE_FREQ_MAGNIFY) / 1000;		//计算进入中断的次数
	}
	param->Pwm[0].PwmNum = (uint32_t)dTempVar;
	
	if(UserOperation.bPhase == UO_PHASE_BIPHASE)									//如果双相模式
	{
		PulseTran_Sub_CalcPwmPara(freq, duty, &(param->Pwm[1]));
		param->Pwm[1].PwmNum = (uint32_t)dTempVar;									//暂按双相等宽简化处理
	}
	else{}
	
	if(param->Pwm[0].PwmNum == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else{}

	param->VarietyNum = UserOperation.bPhase;
	param->CycleNum = 1;
	
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	FreeRun波形处理
*	特征	：	
				单种波形脉冲持续输出
*	处理	：	
				
*	参数	：					
				sPwmArrayParam_t--规整为可用的参数数组
*******************************************************************************************************************/
eRtn_t PulseTran_FreeRun(sPwmArrayParam_t *param)
{
	uint32_t duty  = 0;																						//[V176]原本定义uint16_t范围太小导致数据溢出后输出异常
	float_t  freq  = 0;
	
	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * (UserOperation.bPhase + 1));
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	

	if (UserOperation.bVC == SELECT_VC_V)
	{
		param->Ampl = UserOperation.V_ModeFreeRun.Param[UO_PARAM_AMPL];
		
		freq = UserOperation.V_ModeFreeRun.Param[UO_PARAM_FREQ] / 1000;
		duty = UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE] * freq * PULSE_DUTY_FULL / 1000000000;		//UserOperation.V_ModeFreeRun.Param[UO_PARAM_PULSE] / 1000000000 * freq * PULSE_DUTY_FULL;		
	}
	else
	{
		param->Ampl = UserOperation.C_ModeFreeRun.Param[UO_PARAM_AMPL];
		
		freq = UserOperation.C_ModeFreeRun.Param[UO_PARAM_FREQ] / 1000;
		duty = UserOperation.C_ModeFreeRun.Param[UO_PARAM_PULSE] * freq * PULSE_DUTY_FULL / 1000000000;
	}
	
	if(param->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else{}
	if(freq == 0)
	{
		return RTN_ABNORMAL_FREQ;
	}
	else{}
	if(duty == 0)
	{
		return RTN_ABNORMAL_DUTY;
	}
	else{}
	
	PulseTran_Sub_CalcPwmPara(freq * PULSE_FREQ_MAGNIFY, duty, &(param->Pwm[0]));			//[V182]修复bug，为保持与原协议兼容保留了PULSE_FREQ_MAGNIFY，但计算时遗漏
	
	param->Pwm[0].PwmNum = 65535;															//通过输出中断中对比较值不断清零幅值保证输出持续性
	
	param->VarietyNum = 1;
	param->CycleNum = 1;
	
	
	return RTN_NORMAL;
}

/***************************************************************************************************
*	功能	：	Train波形处理
*	特征	：	
				单种波形脉冲持续输出指定时间段
*	处理	：	
				
*	参数	：					
				sPwmArrayParam_t--规整为可用的参数数组
*******************************************************************************************************************/
eRtn_t PulseTran_Train(sPwmArrayParam_t *param)
{
	uint32_t duty  = 0;
	float_t  freq  = 0;
	
	if(param->Pwm != NULL)
	{
		myfree(SRAMIN, param->Pwm);
	}
	else{}
	
	param->Pwm = (sPwmParam_t *)mymalloc(SRAMIN, sizeof(sPwmParam_t) * (UserOperation.bPhase + 1));
	if(param->Pwm == NULL)
	{
		return RTN_ABNORMAL_SRAM;
	}
	else{}
	
	if (UserOperation.bVC == SELECT_VC_V)
	{
		param->Ampl = UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL];
	}
	else
	{
		param->Ampl = UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL];
	}
	
	if(param->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else{}

    if (UserOperation.bVC == SELECT_VC_V)
	{
		param->Ampl = UserOperation.V_ModeTrain.Param[UO_PARAM_AMPL];
		
		freq = UserOperation.V_ModeTrain.Param[UO_PARAM_FREQ] / 1000;						//【现场bug】多乘以个PULSE_FREQ_MAGNIFY，修改过程中处理草率，没有及时整体梳理流程，没有实际详细测试
		duty = UserOperation.V_ModeTrain.Param[UO_PARAM_PULSE] * freq * PULSE_DUTY_FULL / 1000000000;
	}
	else
	{
		param->Ampl = UserOperation.C_ModeTrain.Param[UO_PARAM_AMPL];
		
		freq = UserOperation.C_ModeTrain.Param[UO_PARAM_FREQ] / 1000;
		duty = UserOperation.C_ModeTrain.Param[UO_PARAM_PULSE] * freq * PULSE_DUTY_FULL / 1000000000;
	}
	
	if(param->Ampl == 0)
	{
		return RTN_ABNORMAL_AMPL;
	}
	else{}
	if(freq == 0)
	{
		return RTN_ABNORMAL_FREQ;
	}
	else{}
	if(duty == 0)
	{
		return RTN_ABNORMAL_DUTY;
	}
	else{}

	PulseTran_Sub_CalcPwmPara(freq * PULSE_FREQ_MAGNIFY, duty, &(param->Pwm[0]));
	
	param->Pwm[0].PwmNum = UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION];
	
	if (UserOperation.bVC == SELECT_VC_V)
	{
		param->Pwm[0].PwmNum = UserOperation.V_ModeTrain.Param[UO_PARAM_DURATION] / 1000 * freq / 1000;		//[V190],freq单位为Hz，直接使用即可
	}
	else
	{
		param->Pwm[0].PwmNum = UserOperation.C_ModeTrain.Param[UO_PARAM_DURATION] / 1000 * freq / 1000;
	}
	if(param->Pwm[0].PwmNum == 0)
	{
		return RTN_ABNORMAL_NUM;
	}
	else{}
	
	param->VarietyNum = 1;
	param->CycleNum = 1;
	
	return RTN_NORMAL;
}
