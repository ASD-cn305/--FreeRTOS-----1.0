#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <stdio.h>
#include <string.h>
#include "Define_List.h"

extern QueueHandle_t uart_raw_queue;

SemaphoreHandle_t Dma_Tx_Complete_Sem;

void Serial_Init(void)
{
		//1.开启DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);	
	
		//2.配置通道结构体
	DMA_InitTypeDef DMA1_InitStructure;		//使能DMA1（USART1的TX）
	DMA_DeInit(DMA1_Channel4);	//复位通道4
	
	DMA1_InitStructure.DMA_BufferSize = 0;	//传输数据量，后续赋值
	DMA1_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;		//传输方向：内存->寄存器
	DMA1_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA1_InitStructure.DMA_MemoryBaseAddr = (uint32_t)0;	//内存地址，后续赋值
	DMA1_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	//传输量1字节
	DMA1_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	//内存地址自增
	DMA1_InitStructure.DMA_Mode = DMA_Mode_Normal;	//正常模式
	DMA1_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;	//外设寄存器基地址，选择串口1的DR数据寄存器(TDR)
	DMA1_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;	//1字节，8比特位
	DMA1_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	//外设地址不自增
	DMA1_InitStructure.DMA_Priority = DMA_Priority_Medium;	//中优先级
	
	DMA_Init(DMA1_Channel4,&DMA1_InitStructure);
	
		//3.开启DMA传输完成中断
	DMA_ITConfig(DMA1_Channel4,DMA_IT_TC,ENABLE);
	
		//4.配置DMA中断优先级(需要比任务优先级高)
	NVIC_InitTypeDef NVIC_InitStructure_DMA;
	NVIC_InitStructure_DMA.NVIC_IRQChannel = DMA1_Channel4_IRQn;	//终端通道选择DMA1通道4的中断
	NVIC_InitStructure_DMA.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure_DMA.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure_DMA.NVIC_IRQChannelSubPriority = 0;
	
	NVIC_Init(&NVIC_InitStructure_DMA);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;	//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	//模式选择，Tx表示发送模式，Rx表示接收模式，若同时启用双模式可用|符号
	USART_InitStructure.USART_Parity = USART_Parity_No;	//校验模式，No表示不校验，Odd表示奇教研，Even表示偶校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长
	USART_Init(USART1,&USART_InitStructure);	//
	
	//中断触发串口
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//在RTOS中记得注释掉转而开启主函数的中断分组
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);

	//启动DMA,此函数需在USART_Cmd()后执行，否则会因为USART_Cmd()的执行而复位进而DMA得不到信号导致DMA不运行
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);	

}

//DMA打印函数
void Serial_printf_DMA(char * format , ...)
{
	static char String[200];
	
	va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    
    // 计算要发送的长度
    uint16_t len = strlen(String);
    if(len == 0) return;
	
	DMA_Cmd(DMA1_Channel4,DISABLE);		//强制关闭DMA
    
    // 配置 DMA 的内存地址和数据长度
    DMA1_Channel4->CMAR = (uint32_t)String;      // 内存地址指向 String
	DMA1_Channel4->CNDTR = len;                  // 设置要发送的数据长度，直接操作寄存器
//  DMA_SetCurrDataCounter(DMA1_Channel4,len);   // 设置要发送的数据长度，调用库函数
    
	/*****
	CMAR：内存地址寄存器（Memory Address Register），告诉DMA从哪里取数据。
	CNDTR：数据数量寄存器（Number of Data Transfer），告诉DMA要搬多少个字节。
	*****/
	
    // 开启 DMA 传输通道
    DMA_Cmd(DMA1_Channel4, ENABLE);
    
    // 等待 DMA 发送完成！如果 DMA 没发完，任务会挂起，CPU 去干别的。
    // 这个信号量由 DMA 中断触发 give
	//超时打印提示，1S
    if(xSemaphoreTake(Dma_Tx_Complete_Sem , pdMS_TO_TICKS(1000)) == pdFALSE)
	{
		printf("DMA响应超时，请复位并检查中断配置!\r\n");
	}
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);
	while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
}

//发送数组
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0;i < Length; i++)
	{
		Serial_SendByte(Array[i]);
	}
}

//发送字符串
void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0;String[i] != 0;i++)
	{
		Serial_SendByte(String[i]);
	}
}

//发送字符串形式的数字
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//次方函数
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i=0; i<Length;i++)
	{
		Serial_SendByte(Number / Serial_Pow(10,Length - i - 1) % 10 + '0'); //此处加入'0'的原因是：加0是因为ASCII表里面编码的0是0x30  不加的话  就会从ASCII表的第一位开始算了 输出的就不是数字了
		
	}
}

//printf函数
int fputc(int ch, FILE *f)
{
	Serial_SendByte (ch);
	return ch;
}

//对sprintf函数进行封装,可以打印可变参数
void Serial_Printf(char *format , ...)
{		//此处的...参数用于接收后面的可变参数列表
	char String[100];
	va_list arg;	//前者为类型名，后者为变量名
	va_start(arg,format);
	vsprintf(String,format,arg);	//此处的三个参数分别为：打印位置String，格式化字符串：format，参数表：arg
		//springtf只能接收直接写的参数，对于封装格式需使用vsprintf
	va_end(arg);	//释放参数表
	Serial_SendString(String);	//将string发送出去
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

//对于FreeRTOS来说，中断中不宜进行协议解析等复杂操作，故改写中断服务函数
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
		uint8_t ch = USART_ReceiveData(USART1);
        // 直接回传，用于调试
        USART_SendData(USART1, ch);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
//        uint8_t RxData = USART_ReceiveData(USART1);
//        
//        // 【核心关键】把数据通过队列发送给任务，由任务去解析
//        // 注意：在中断里必须用 FromISR 后缀的函数！
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xQueueSendFromISR(uart_raw_queue, &RxData, &xHigherPriorityTaskWoken);
//        
//        // 如果需要，可以在此唤醒解析任务
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
    }
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

void DMA1_Channel4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC4) == SET)
	{
		//清除中断标志位
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		
		//关闭DMA，准备下次传输
		DMA_Cmd(DMA1_Channel4,DISABLE);
		
		//从ISR中释放信号量，唤醒等待的Task
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(Dma_Tx_Complete_Sem,&xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}