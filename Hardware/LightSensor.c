#include "stm32f10x.h"                  // Device header

void LightSensor_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GOIO_InitStructure;
	GOIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GOIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GOIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GOIO_InitStructure);
	
}

uint8_t LightSensor_Get(void){
	return GPIO_ReadInputDataBit (GPIOB,GPIO_Pin_13);
}
