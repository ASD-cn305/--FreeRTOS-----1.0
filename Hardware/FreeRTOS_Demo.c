//使用提醒：请阅读main.c里的注释说明部分

#include "FreeRTOS.h"
#include "task.h"						//任务
#include "LED.h"
#include "OLED.h"
#include "stm32f10x.h"                  // Device header
#include "queue.h"						//队列
#include "Key.h"
#include "USART.h"
#include "semphr.h"						//二值信号量
#include "Timer_Action.h"
#include "Define_List.h"

/*	对Start_Task的各参数配置	*/
void Start_Task(void* pvParameters);	//声明函数
#define START_TASK_STACK_SIZE	128		//xTaskCreate函数的参数的宏定义，此处为堆栈大小
#define START_TASK_PRIORITY		1		//此处为优先级的定义
TaskHandle_t	Start_Task_Handler;		//此处为定义任务句柄

/*	对Task_RecerveData的各参数配置	*/
void Task_ReceiveData(void* pvParameters);
#define TASK_R_STACK_SIZE	256
#define TASK_R_PRIORITY		3
TaskHandle_t	Task_R_Handler;

/*	对Task_TruansmitData的各参数配置	*/
void Task_TruansmitData(void* pvParameters);
#define TASK_T_STACK_SIZE	256
#define TASK_T_PRIORITY		2
TaskHandle_t	Task_T_Handler;

/*	对Task_T_A的各参数配置 */
void Task_T_A(void* pvParameters);
#define TASK_T_A_STACK_SIZE	256
#define TASK_T_A_PRIORITY	2
TaskHandle_t	Task_T_A_Handler;


QueueHandle_t uart_raw_queue; 	//用于串口原始数据缓冲的队列

SemaphoreHandle_t print_mutex;	//创建互斥锁
SemaphoreHandle_t Tim2_sem;		//创建信号量用于TIM中断

//QueueSetHandle_t queueset_handle; 
//QueueHandle_t	queue_handle;
//QueueHandle_t	sempht_handle;

void FreeRTOS_Demo(void)	//用于创建Start_Task，由Start_Task来创建各个任务
{	
	
	xTaskCreate( (TaskFunction_t		) Start_Task,
                 (char * 				) "Start_Task",
                 (configSTACK_DEPTH_TYPE) START_TASK_STACK_SIZE,
                 (void * 				) NULL,
                 (UBaseType_t			) START_TASK_PRIORITY,
                 (TaskHandle_t *		) &Start_Task_Handler );
	vTaskStartScheduler();	//启动任务调度器
}

void Start_Task(void* pvParameters)
{
	
	taskENTER_CRITICAL();	//进入临界区
	
	
	Tim2_sem = xSemaphoreCreateBinary();	//创建二值信号量
	
	uart_raw_queue = xQueueCreate(20,sizeof(uint8_t));	//创建缓冲队列
	
	Dma_Tx_Complete_Sem = xSemaphoreCreateBinary();		//增加DMA同步信号量
	
	print_mutex = xSemaphoreCreateMutex();
	
	if(Tim2_sem != NULL)
	{
		printf("二值信号量创建成功\r\n");
	}
	
	if(uart_raw_queue != NULL)
	{
		printf("缓冲队列创建完成\r\n");
	}
		
	//在Start_Task中创建Task等
	xTaskCreate( (TaskFunction_t		) Task_ReceiveData,
                 (char * 				) "Task_ReceiveData",
                 (configSTACK_DEPTH_TYPE) TASK_R_STACK_SIZE,
                 (void * 				) NULL,
                 (UBaseType_t			) TASK_R_PRIORITY,
                 (TaskHandle_t *		) &Task_R_Handler );
	printf("任务Task_ReceiveData创建成功\r\n");
	
//	xTaskCreate( (TaskFunction_t		) Task_TruansmitData,
//                 (char * 				) "Task_TruansmitData",
//                 (configSTACK_DEPTH_TYPE) TASK_T_STACK_SIZE,
//                 (void * 				) NULL,
//                 (UBaseType_t			) TASK_T_PRIORITY,
//                 (TaskHandle_t *		) &Task_T_Handler );
//	printf("任务Task_TruansmitData创建成功\r\n");
//	
	xTaskCreate( (TaskFunction_t		) Task_T_A,
                 (char * 				) "Task_T_A",
                 (configSTACK_DEPTH_TYPE) TASK_T_A_STACK_SIZE,
                 (void * 				) NULL,
                 (UBaseType_t			) TASK_T_A_PRIORITY,
                 (TaskHandle_t *		) &Task_T_A_Handler );
	printf("任务Task_T_A创建成功\r\n");
				 
	TIM_Cmd(TIM2, ENABLE);
    printf("定时器已启动\r\n");
	
	vTaskDelete(NULL);	//删除任务，当参数为NULL时表示删除当前任务(Start_Task)
				 
	taskEXIT_CRITICAL();
}

//

void Task_ReceiveData(void* pvParameters)
{
	uint8_t RxData;					//串口数据包数据缓冲，接收到一个数据则用来判断是否满足数据包格式
	static uint16_t RxState = 1;	//状态机状态参数
	static uint16_t pRxState = 0;	//状态机接收数据序号
	
	while(1)
	{
		// 从队列中死等数据
		if(xQueueReceive(uart_raw_queue, &RxData, portMAX_DELAY) == pdPASS)
		{
			
			if(Mode == 0) // Hex 模式
			{
				if(RxState == 1)
				{
					if(RxData == 0xFF)
					{
						RxState = 2;
						pRxState = 0;
					}
				}
				else if(RxState == 2)
				{
					Serial_RxPackte_Hex[pRxState] = RxData;		//pRxState参考本句
					pRxState ++;
					if(pRxState >= 4) RxState = 3;
				}
				else if(RxState == 3)
				{
					if(RxData == 0xFE)
					{
						RxState = 1;
						Serial_RxFlag = 1;
						if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdTRUE)
						{
							//使用DMA进行打印，加快速度同时减少资源占用，下同
							Serial_printf_DMA("从电脑发送数据到串口(接收Hex)：%02X %02X %02X %02X\r\n", 
								   Serial_RxPackte_Hex[0], Serial_RxPackte_Hex[1], 
								   Serial_RxPackte_Hex[2], Serial_RxPackte_Hex[3]);
							
							xSemaphoreGive(print_mutex);
						}
					}
				}
			}
			else if(Mode == 1) // 文本模式
			{
				if(RxState == 1)
				{
					if(RxData == '@' && Serial_RxFlag == 0) RxState = 2, pRxState = 0;
				}
				else if(RxState == 2)
				{
					if(RxData == '\r') RxState = 3;
					else if(pRxState < 99) Serial_RxPackte_String[pRxState++] = RxData;
				}
				else if(RxState == 3)
				{
					if(RxData == '\n')
					{
						RxState = 1;
						Serial_RxPackte_String[pRxState] = '\0';
						Serial_RxFlag = 1;
						if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdTRUE)
						{
							Serial_printf_DMA("从电脑发送数据到串口(接收String)：%s\r\n", Serial_RxPackte_String);
							
							xSemaphoreGive(print_mutex);
						}
					}
					else RxState = 2;
				}
			}
			else if(Mode == 2)	//蓝牙指令交互
			{
				Serial_RxData_Hex = RxData;
				// 直接根据命令设置 Action_Mode
				switch(Serial_RxData_Hex)
				{
					case 0x00: Action_Mode = 0; break;
					case 0x01: Action_Mode = 1; break;
					case 0x02: Action_Mode = 2; break;
					case 0x03: Action_Mode = 3; break;
					case 0x04: Action_Mode = 4; break;
					case 0x05: Action_Mode = 5; break;
					case 0x06: Action_Mode = 6; break;
					case 0x07: Action_Mode = 7; break;
					default: break;
				}
				// 打印确认（加互斥锁保护）
				if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdTRUE)
						{
							Serial_printf_DMA("蓝牙指令已接收：%X，正在执行\r\n", Serial_RxData_Hex);
							xSemaphoreGive(print_mutex);
						}
			}
			
			else if(Mode == 3)	//语音指令交互
			{
				switch(Serial_RxP_Mode_Hex[0])
				{
					case 0x00: Action_Mode = 0; break;
					case 0x01: Action_Mode = 1; break;
					case 0x02: Action_Mode = 2; break;
					case 0x03: Action_Mode = 3; break;
					case 0x04: Action_Mode = 4; break;
					case 0x05: Action_Mode = 5; break;
					case 0x06: Action_Mode = 6; break;
					case 0x07: Action_Mode = 7; break;
					default: break;
				}
				if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdPASS)
					{
						Serial_printf_DMA("语音指令已接收：%X，正在执行\r\n",Serial_RxP_Mode_Hex[0]);
						xSemaphoreGive(print_mutex);
					}
				
			}
			
		}
		Serial_GetRxFlag();
	}
}


//
void Task_TruansmitData(void* pvParameters)
{
//	uint8_t i = 0;
//	while(1)
//	{
//		if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdTRUE)
//		{
//			Serial_printf_DMA("从串口发送数据到电脑：%d\r\n",i);
//			xSemaphoreGive(print_mutex);
//		}
//		i++;
//		vTaskDelay(50);
//	}
}

void Task_T_A(void* pvParameters)
{
	Action_Mode = 0;
	while(1)
	{
		//Action_Choose();
		Mode_Menu();
		//打印数据方便调试
		//加上互斥锁防止在打印过程中被打印数据打乱指令数据格式
		if(xSemaphoreTake(print_mutex,portMAX_DELAY) == pdTRUE)
		{
			if(Action_Mode == 0) Serial_printf_DMA("立正！\r\n");
			else if(Action_Mode == 1) Serial_printf_DMA("这里不让睡觉\r\n");
			else if(Action_Mode == 2) Serial_printf_DMA("那就坐吧\r\n");
			else if(Action_Mode == 3) Serial_printf_DMA("正在前进\r\n");
			else if(Action_Mode == 4) Serial_printf_DMA("倒车请注意\r\n");
			else if(Action_Mode == 5) Serial_printf_DMA("左满舵\r\n");
			else if(Action_Mode == 6) Serial_printf_DMA("右满舵\r\n");
			else if(Action_Mode == 7) Serial_printf_DMA("你跳，我跳！\r\n");
			
			xSemaphoreGive(print_mutex);
		}
		vTaskDelay(20);
	}
}