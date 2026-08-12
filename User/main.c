/*******************************************
	项目名称：基于FreeRTOS的智能小狗
	作者：B站夏洛克福尔摩何		
	
	项目简介：
	本项目基于FreeRTOS，在STM32F103C8T6最小系统板上，通过配置串口，定时器等实现小狗的指令收发和执行
	项目灵感来源与B站up：Sngels_wyh的开源项目：基于STM32F103C8T6单片机的桌面宠物小项目
	目前只实现了比较基本的功能，更多功能作者正在后续开发
	本项目主要用于作者学习和了解mcu开发和FreeRTOS开发，并免费开源分享，欢迎各位大佬指正
	
	特别感谢B站up：江协科技带我入门单片机开发，同时分享开源代码供学习使用
		以及B站up：Sngels_wyh的开源项目给予我的设计灵感，同时在一些代码的实现上提供了思路方法
*******************************************/


#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "FreeRTOS.h"
#include "Task.h"
#include "LED.h"
#include "USART.h"
#include "FreeRTOS_Demo.h"
#include "Timer.h"

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//串口中断函数分组配置，需要时再开启
	LED_Init();
	OLED_Init();
	
	Timer_Init();
	
	Serial_Init();
	
	
	FreeRTOS_Demo();
	
}