
/********************************************************************
*
*	文件	：	Crc.h
*	管脚	：	(NONE)
*	功能	：	循环冗余码校验（Cyclic Redundancy Check）
*
******************************************************************************/
#ifndef _CRC_H
#define _CRC_H
#include "main.h"

uint16_t Crc_16bit_Calculate(uint8_t *buf, uint32_t length);

#endif
