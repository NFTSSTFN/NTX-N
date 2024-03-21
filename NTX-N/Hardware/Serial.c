#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"

uint8_t Serial_RxPacket[4];				// 接收串口发来的数据，包头位0xFF，包尾为0xEE，总共加起来6个，如果格式变了，要去中断函数里改
uint8_t Serial_RxFlag;					// 串口接收标志位

/* 初始化串口通信 */
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		// 开启USART1的时钟，是APB1的外设，其他的都是APB2的外设
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		// 开启GPIO的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;						// 配置发送的GPIO引脚	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				// 配置发送的引脚为推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);						// 初始化GPIOA的时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				// 配置接收的引脚为浮空上拉	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
 	GPIO_Init(GPIOA, &GPIO_InitStructure);						// 初始化GPIOA的时钟
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;						// 配置波特率为115200
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 不适用流控
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		// 选择模式发射/接收
	USART_InitStructure.USART_Parity = USART_Parity_No;					// 选择校验位为无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 选择停止为1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			// 因为不需要校验，所以字长为8位
	USART_Init(USART1, &USART_InitStructure);							// 初始化USART1
	
	/* 
	串口的接收可以分为查询和中断
	查询的方法需要在main函数里不断的查询标志位，如果置1了就证明收到数据了，示例如下：
		uint8_t RxData;
		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
		{
			RxData = ISART_ReceiveData(USART1);  // 接收到的一个字节的数据
		}
	下面这一串是使用中断进行查询的代码
	*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				// 开启中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				// NVIC分组
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			// 中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			// 响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);										// USART1使能
}


/* 
从TX引脚发送一个字节
uint8_t Byte：需要发送的字节
*/
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);									// 发送字节数据
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	// 监听TDR的数据是否转移到移位寄存器
}


/*
发送一个字节数据包
uint8_t *Array：数组首元素的地址
uint8_t Length：数组的长度
*/
void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
}


/*
发送一个字符串
char* String：需要发送的字符串
*/
void Serial_SendString(char* String)
{
	uint8_t i;
	for (i=0;String[i] != '\0';i++)
	{
		Serial_SendByte(String[i]);
	}
}


/*
因为发送数字的时候，例如6000，需要一个一个拆分开发送，所以这个函数就是干这个事的
*/
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;	
}

/*
发送一个数字
uint32_t Number：需要发送的数字
uint8_t Length：数字的长度
*/
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i< Length; i++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) %10 + '0');
	}
}

/*
此函数将需要打印的东西输出到串口，需要导入<stdio.h>，最好在头文件加，这样导入头文件的时候就会直接把它也导进去
移植了printf的方法，使用之前需要打开魔术棒，在Target页面，把Use MicroLIB勾选上
MicroLIB是keil位嵌入式平台优化的一个精简库
printf打印的时候就是调用fputc一个一个打印的，所以将其重定向以后，fprint就会使用这个进行打印，输出到串口
使用的时候直接待用printf();即可，例如printf("Num=%d\r\n", 666);
*/
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

/*
此函数用作多个串口打印
char* format：用来接收格式化字符串
...：用来接收可变参数列表
示例：Serial_Prinf("Num=%d\r\n", 666);
如果需要打印汉字，为了保证不乱码，打开工程选项，C/C++，MiscControls中写上 --no-multibyte-chars
记得打印汉字的时候，译码方式选择UTF8
*/
void Serial_Prinf(char* format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);			// 从format位置接收参数表，放在arg里面
	vsprintf(String, format, arg);
	va_end(arg);					// 释放参数表
	Serial_SendString(String);
}


/*
获取Rx的标志位
*/
uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}


/*
使用中断来接收串口数据的中断函数
*/
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;		// 状态标志位
	static uint8_t pRxPacket = 0;	// 指示接收到哪一个数据了
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		// 判断标志位
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		if (RxState == 0)			// 进入等待包头的程序
		{
			if (RxData == 0xFF)		// 判断包头是不是0xFF
			{
				RxState = 1;
			}
		}
		else if (RxState == 1)						// 接收数据的程序
		{
			Serial_RxPacket[pRxPacket] = RxData;	// 把数据存到RxPacket中
			pRxPacket ++;							// 指向RxPacket的下一位，准备接收下一个数据
			if (pRxPacket >= 4)						// 因为RxPacket只能接收4个数据，所以这里需要将RxState切换到接收包尾的模式
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)						// 接收包尾的程序
		{
			if (RxData == 0xEE)
			{
				RxState = 0;
				Serial_RxFlag = 1;						// 标志位
			}
			
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			// 清除标志位
	}
}

