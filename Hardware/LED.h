#ifndef __LED_H
#define __LED_H

void LED_Init(void);	//模块化声明，表明此函数可被外部调用
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);
//void LED_CONTROL(int LED_C,int LED_L);	//使用函数控制LED

#endif
//用来存放这个驱动程序可以对外提供的函数或变量的声明
