#include "stm32f10x.h"                  // Device header
//存放驱动程序的主体代码
void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//	GPIO_Init(GPIOA,&GPIO_InitStructure);
//	
	GPIO_SetBits(GPIOA,GPIO_Pin_1 | GPIO_Pin_2);
}

void LED1_ON(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);		//ResetBits置低电平
}

void LED1_OFF(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_1);		//SetBits置高电平
}

void LED1_Turn(void){		//LED进行取反操作，使按键操作达到案件按一下亮再按一下灭
	if (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_1) == 0){		//读取LED的GPIO_Pin_1的输出状态
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
	}
	else {
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	}
}

void LED2_ON(void){
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);		
}

void LED2_OFF(void){
	GPIO_SetBits(GPIOA,GPIO_Pin_2);		
}

void LED2_Turn(void){
	if (GPIO_ReadOutputDataBit(GPIOA,GPIO_Pin_2) == 0){
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
	}
	else {
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}
/*
void LED_CONTROL(int LED_C,int LED_L){
	if (LED_C == 1){
		if (LED_L == 1){
			GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		}
		else if (LED_L == 0){
			GPIO_SetBits(GPIOA,GPIO_Pin_1);
		}
	}
	else if (LED_C == 2){
		if (LED_L == 1){
			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		}
		else if (LED_L == 0){
			GPIO_SetBits(GPIOA,GPIO_Pin_2);
		}
	}
}
*/
