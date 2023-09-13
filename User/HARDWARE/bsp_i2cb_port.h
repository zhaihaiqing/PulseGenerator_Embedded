
#ifndef __BSP_I2CB_PORT_H
#define __BSP_I2CB_PORT_H

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define I2CB_SCL_RCC_PORT 		RCC_AHB1Periph_GPIOF		/* GPIO�˿�ʱ�� */
#define I2CB_SCL_GPIO_PORT		GPIOF						/* GPIO�˿� */
#define I2CB_SCL_GPIO_Pin		GPIO_Pin_6					/* ���ӵ�SCLʱ���ߵ�GPIO */
//#define I2CB_SCL_GPIO_Pin_NUM	GPIO_Pin_6_NUM					/* ���ӵ�SCLʱ���ߵ�GPIO */

#define I2CB_SDA_RCC_PORT 		RCC_AHB1Periph_GPIOF		/* GPIO�˿�ʱ�� */
#define I2CB_SDA_GPIO_PORT		GPIOF						/* GPIO�˿� */
#define I2CB_SDA_GPIO_Pin		GPIO_Pin_7					/* ���ӵ�SCLʱ���ߵ�GPIO */
//#define I2CB_SDA_GPIO_Pin_NUM	GPIO_Pin_7_NUM					/* ���ӵ�SCLʱ���ߵ�GPIO */

/* �����дSCL��SDA�ĺ� */
#define I2CB_SCL_1()  			I2CB_SCL_GPIO_PORT->BSRRL = I2CB_SCL_GPIO_Pin				/* SCL = 1 */
#define I2CB_SCL_0()  			I2CB_SCL_GPIO_PORT->BSRRH = I2CB_SCL_GPIO_Pin				/* SCL = 0 */

#define I2CB_SDA_1()  			I2CB_SDA_GPIO_PORT->BSRRL = I2CB_SDA_GPIO_Pin				/* SDA = 1 */
#define I2CB_SDA_0()  			I2CB_SDA_GPIO_PORT->BSRRH = I2CB_SDA_GPIO_Pin				/* SDA = 0 */

#define I2CB_SCL_READ()  		((I2CB_SCL_GPIO_PORT->IDR & I2CB_SCL_GPIO_Pin) != 0)	/* ��SCL����״̬ */
#define I2CB_SDA_READ()  		((I2CB_SDA_GPIO_PORT->IDR & I2CB_SDA_GPIO_Pin) != 0)	/* ��SDA����״̬ */


void i2cb_port_init(void);
void i2cb_port_start(void);
void i2cb_port_stop(void);
unsigned char i2cb_port_wait_ack(void);
void i2cb_port_ack(void);
void i2cb_port_nack(void);
void i2cb_port_send_byte(unsigned char txd);
unsigned char i2cb_port_read_byte(void);
unsigned char i2cb_CheckDevice(unsigned char Device_addr);


#endif


