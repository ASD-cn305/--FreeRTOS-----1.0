#include "stm32f10x.h"                  // Device header
#include "Define_List.h"

//数据包模式选择，Mode
//0 为Hex数据包
//1 为文本数据包
//2 为不使用数据包，以hex模式发送
//3 为使用固定格式数据包，用于适配语音模块的帧格式
uint8_t Mode = 2;

uint8_t Serial_RxPackte_Hex[4];			//接收的Hex数据包缓冲区，用于串口通信调试
uint8_t Serial_TxPackte_Hex[4];			//发送的Hex数据包缓冲区，用于串口通信调试
uint8_t Serial_RxData_Hex;				//接收的Hex数据缓冲区，用于串口接收小狗指令数据
uint8_t Serial_RxP_Mode_Hex[1];			//接收的Hex数据包缓冲区，用于串口接收小狗指令数据

uint8_t Serial_RxPackte_String[100];	//接收到的文本数据包缓冲区
uint8_t Serial_RxFlag;					//串口传输标志位，置1 表示传输完成

volatile uint8_t Action_Mode;			//动作模式编号
volatile uint8_t Step;					//行动步数，前进后退和转向