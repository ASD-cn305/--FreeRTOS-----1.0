#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "Task.h"
#include "Timer.h"
#include "USART.h"
#include "Define_List.h"

/*	舵机位置
	
	LF			RF
	
	
	
	
	
	LB			RB

	舵机角度：90表示站立，0表示向前伸直
*/

//站立
void Action_Stand(void)
{
	Action_Mode = 0;
	Step = 0;
	
	Serve_Angle_LF(90);
	Serve_Angle_RF(90);
	vTaskDelay(80);
	Serve_Angle_LB(90);
	Serve_Angle_RB(90);
}

//躺下
void Action_Lay(void)
{
	if(Action_Mode !=1)
	{
		Action_Stand();
		Action_Mode = 1;
	}
	
	Serve_Angle_LF(20);
	Serve_Angle_RF(20);
	vTaskDelay(80);
	Serve_Angle_LB(20);
	Serve_Angle_RB(20);
}

//坐下
void Action_Sit(void)
{
	if(Action_Mode != 2)
	{
		Action_Stand();
		Action_Mode = 2;
	}
	
	Serve_Angle_LF(90);
	Serve_Angle_RF(90);
	vTaskDelay(80);
	Serve_Angle_LB(20);
	Serve_Angle_RB(20);
}

//前进
void Move_Forward(void)
{
	if(Action_Mode != 3)
	{
		Action_Stand();
		Action_Mode = 3;
	}
	
	while(Action_Mode == 3)
	{
		if(Step <= 10)
		{
			Serve_Angle_LF(135);
			Serve_Angle_RB(135);
			vTaskDelay(100);
			Serve_Angle_RF(45);
			Serve_Angle_LB(45);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Step++;
			
			Serve_Angle_RF(135);
			Serve_Angle_LB(135);
			vTaskDelay(100);
			Serve_Angle_LF(45);
			Serve_Angle_RB(45);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Step++;
		}
		else Action_Stand();
	}
	
}

//后退
void Move_Back(void)
{
	if(Action_Mode != 4)
	{
		Action_Stand();
		Action_Mode = 4;
	}
	
	while(Action_Mode == 4)
	{
		if(Step <= 10)
		{
			Serve_Angle_LF(45);
			Serve_Angle_RB(45);
			vTaskDelay(100);
			Serve_Angle_RF(135);
			Serve_Angle_LB(135);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Step++;
			
			Serve_Angle_RF(45);
			Serve_Angle_LB(45);
			vTaskDelay(100);
			Serve_Angle_LF(135);
			Serve_Angle_RB(135);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Step++;
		}
		else Action_Stand();
	}
}

//左转
void Action_LeftTurn(void)
{
	if(Action_Mode != 5)
	{
		Action_Stand();
		Action_Mode = 5;
	}
	
	while(Action_Mode == 5)
	{
		if(Step <= 4)
		{
			Serve_Angle_RF(45);
			Serve_Angle_LB(135);
			vTaskDelay(100);
			Serve_Angle_LF(135);
			Serve_Angle_RB(45);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Step++;
		}
		else Action_Stand();
	}
}

//右转
void Action_RightTurn(void)
{
	if(Action_Mode != 6)
	{
		Action_Stand();
		Action_Mode = 6;
	}
	
	while(Action_Mode == 6)
	{
		if(Step <= 4)
		{
			Serve_Angle_LF(45);
			Serve_Angle_RB(135);
			vTaskDelay(100);
			Serve_Angle_RF(135);
			Serve_Angle_LB(45);
			vTaskDelay(100);
			Serve_Angle_LF(90);
			Serve_Angle_RB(90);
			vTaskDelay(100);
			Serve_Angle_RF(90);
			Serve_Angle_LB(90);
			vTaskDelay(100);
			Step++;
		}
		else Action_Stand();
	}
}

//跳跃
void Action_Jump(void)
{
	if(Action_Mode != 7)
	{
		Action_Stand();
		Action_Mode = 7;
	}
	
	Serve_Angle_LF(110);
	Serve_Angle_RF(110);
	vTaskDelay(100);
	Serve_Angle_LB(20);
	Serve_Angle_RB(20);
	vTaskDelay(100);
	Serve_Angle_LB(110);
	Serve_Angle_RB(110);
	vTaskDelay(100);
	Action_Stand();
}



void Mode_Menu(void)
{
	if(Action_Mode == 0)
	{
		Action_Stand();				//站立
	}		
	else if(Action_Mode == 1)
	{
		Action_Lay();				//躺下
	}		
	else if(Action_Mode == 2)
	{
		Action_Sit();				//坐下
	}		
	else if(Action_Mode == 3)
	{
		Move_Forward();				//前进
	}		
	else if(Action_Mode == 4)
	{
		Move_Back();				//后退
	}		
	else if(Action_Mode == 5)
	{
		Action_LeftTurn();			//左转
	}		
	else if(Action_Mode == 6)
	{
		Action_RightTurn();			//右转
	}		
	else if(Action_Mode == 7)
	{
		Action_Jump();				//跳跃
	}		
}

void Action_Choose(void)
{
	if(Mode == 2)
	{
		switch(Serial_RxData_Hex)
		{
			case 0x00 :
				Action_Mode = 0;
				break;
			case 0x01 :
				Action_Mode = 1;
				break;
			case 0x02 :
				Action_Mode = 2;
				break;
			case 0x03 :
				Action_Mode = 3;
				break;
			case 0x04 :
				Action_Mode = 4;
				break;
			case 0x05 :
				Action_Mode = 5;
				break;
			case 0x06 :
				Action_Mode = 6;
				break;
			case 0x07 :
				Action_Mode = 7;
				break;
			default :
				break;
		}
	}
}