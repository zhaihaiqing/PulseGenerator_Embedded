
#ifndef __TEMP_FAN_H
#define __TEMP_FAN_H


#define TEMP_I2C_ADDR		0x90

#define TEMP_REG_TEMP        0x00



float get_temp(void);


void Timer9_PWM_Init(float Duty);
void Timer9_PWM_OC1_Enable(void);
void Timer9_PWM_OC1_Disable(void);
void Timer9_PWM_OC1_SetDuty(float Duty);



#endif


