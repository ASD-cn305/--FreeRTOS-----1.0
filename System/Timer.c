#include "stm32f10x.h"                  // Device header
#include "USART.h"
#include "FreeRTOS.h"
#include <semphr.h>
#include "Define_List.h"

extern SemaphoreHandle_t Tim2_sem;		//创建信号量用于TIM中断

void Timer_Init(void)
{
	//定时器使用步骤
	
	//1.开启外部时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//2.时基单元时钟选择，此处选择内部时钟
	TIM_InternalClockConfig(TIM2);	//切换为内部定时2
	
	//3.初始化时基单元
	TIM_TimeBaseInitTypeDef TIM_InitStructre;
	
		//配置为ARR=20000，CNT=72
	TIM_InitStructre.TIM_ClockDivision = TIM_CKD_DIV1;	//
	TIM_InitStructre.TIM_CounterMode = TIM_CounterMode_Up;	//
	TIM_InitStructre.TIM_Period = 20000 - 1;	//ARR,自动重装寄存器的值
	TIM_InitStructre.TIM_Prescaler = 72 - 1;	//PSC，预分频器
	TIM_InitStructre.TIM_RepetitionCounter = 0;	//重复寄存器，用于设置CNT溢出几次后在触发中断等
	
	TIM_TimeBaseInit(TIM2,&TIM_InitStructre);
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	
	//PWM
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;	//CCR值
	TIM_OC1Init(TIM2,&TIM_OCInitStructure);	//开启四个复用通道
	TIM_OC2Init(TIM2,&TIM_OCInitStructure);
	TIM_OC3Init(TIM2,&TIM_OCInitStructure);
	TIM_OC4Init(TIM2,&TIM_OCInitStructure);

	TIM_Cmd(TIM2,ENABLE);	//使能放在Task_Start中
	
}

void PWM_LF_SetCCR(uint16_t CCR)
{
	TIM_SetCompare1(TIM2,CCR);	//左前舵机CCR1值设置
}

void PWM_RF_SetCCR(uint16_t CCR)
{
	TIM_SetCompare2(TIM2,CCR);	//右前舵机CCR2值设置
}

void PWM_LB_SetCCR(uint16_t CCR)
{
	TIM_SetCompare3(TIM2,CCR);	//左后舵机CCR3值设置
}

void PWM_RB_SetCCR(uint16_t CCR)
{
	TIM_SetCompare4(TIM2,CCR);	//右后舵机CCR4值设置
}

void Serve_Angle_LF(float Angle)
{
	PWM_LF_SetCCR(Angle / 180 * 2000 + 500);	//左前舵机角度设置
}

void Serve_Angle_RF(float Angle)
{
	PWM_RF_SetCCR((180 - Angle) / 180 * 2000 + 500);	//右前舵机角度设置
}

void Serve_Angle_LB(float Angle)
{
	PWM_LB_SetCCR(Angle / 180 * 2000 + 500);	//左后舵机角度设置
}

void Serve_Angle_RB(float Angle)
{
	PWM_RB_SetCCR((180 - Angle) / 180 * 2000 + 500);	//右后舵机角度设置
}
