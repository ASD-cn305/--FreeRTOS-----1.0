#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "FreeRTOS.h"
#include "task.h"

void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GOIO_InitStructure;
	GOIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GOIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11 ;
	GOIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GOIO_InitStructure);
	
}

uint8_t Key_GetNum(void){
	uint8_t KeyNum = 0;
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0){
		vTaskDelay(20);
		KeyNum = 1;
	}
	
	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0){
		vTaskDelay(20);
		KeyNum = 2;
	}
//	//下面两个按键用于测试队列出入
//	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0){
//		vTaskDelay(20);
//		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12) == 0);
//		vTaskDelay(20);
//		KeyNum = 3;
//	}
//	
//	if (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == 0){
//		vTaskDelay(20);
//		while (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13) == 0);
//		vTaskDelay(20);
//		KeyNum = 4;
//	}
	
	return KeyNum;
}
