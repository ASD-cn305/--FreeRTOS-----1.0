#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <FreeRTOS.h>
#include <semphr.h>

extern SemaphoreHandle_t Dma_Tx_Complete_Sem;

void Serial_Init(void);
void Serial_printf_DMA(char * format , ...);	//DMA打印函数
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
uint32_t Serial_Pow(uint32_t X, uint32_t Y);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
int fputc(int ch, FILE *f);
void Serial_Printf(char *format , ...);
uint8_t Serial_GetRxFlag(void);
void USART1_IRQHandler(void);
void DMA1_Channel4_IRQHandler(void);

#endif