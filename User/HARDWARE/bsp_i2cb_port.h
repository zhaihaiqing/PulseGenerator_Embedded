
#ifndef __BSP_I2CB_PORT_H
#define __BSP_I2CB_PORT_H

/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */
#define I2CB_SCL_RCC_PORT 		RCC_AHB1Periph_GPIOF		/* GPIO端口时钟 */
#define I2CB_SCL_GPIO_PORT		GPIOF						/* GPIO端口 */
#define I2CB_SCL_GPIO_Pin		GPIO_Pin_6					/* 连接到SCL时钟线的GPIO */
//#define I2CB_SCL_GPIO_Pin_NUM	GPIO_Pin_6_NUM					/* 连接到SCL时钟线的GPIO */

#define I2CB_SDA_RCC_PORT 		RCC_AHB1Periph_GPIOF		/* GPIO端口时钟 */
#define I2CB_SDA_GPIO_PORT		GPIOF						/* GPIO端口 */
#define I2CB_SDA_GPIO_Pin		GPIO_Pin_7					/* 连接到SCL时钟线的GPIO */
//#define I2CB_SDA_GPIO_Pin_NUM	GPIO_Pin_7_NUM					/* 连接到SCL时钟线的GPIO */

/* 定义读写SCL和SDA的宏 */
#define I2CB_SCL_1()  			I2CB_SCL_GPIO_PORT->BSRRL = I2CB_SCL_GPIO_Pin				/* SCL = 1 */
#define I2CB_SCL_0()  			I2CB_SCL_GPIO_PORT->BSRRH = I2CB_SCL_GPIO_Pin				/* SCL = 0 */

#define I2CB_SDA_1()  			I2CB_SDA_GPIO_PORT->BSRRL = I2CB_SDA_GPIO_Pin				/* SDA = 1 */
#define I2CB_SDA_0()  			I2CB_SDA_GPIO_PORT->BSRRH = I2CB_SDA_GPIO_Pin				/* SDA = 0 */

#define I2CB_SCL_READ()  		((I2CB_SCL_GPIO_PORT->IDR & I2CB_SCL_GPIO_Pin) != 0)	/* 读SCL口线状态 */
#define I2CB_SDA_READ()  		((I2CB_SDA_GPIO_PORT->IDR & I2CB_SDA_GPIO_Pin) != 0)	/* 读SDA口线状态 */


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


