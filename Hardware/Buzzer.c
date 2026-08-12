#include "stm32f10x.h"                  // Device header
void Buzzer_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_12);
}

void Buzzer_ON(void){
	GPIO_ResetBits(GPIOB,GPIO_Pin_12);		//ResetBits置低电平
}

void Buzzer_OFF(void){
	GPIO_SetBits(GPIOB,GPIO_Pin_12);		//SetBits置高电平
}

void Buzzer_Turn(void){		//进行取反操作
	if (GPIO_ReadOutputDataBit(GPIOB,GPIO_Pin_12) == 0){		//读取GPIO_Pin_12的输出状态
		GPIO_SetBits(GPIOB,GPIO_Pin_12);
	}
	else {
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);
	}
}
