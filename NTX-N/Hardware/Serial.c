#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"

uint8_t Serial_RxPacket[4];				// ���մ��ڷ��������ݣ���ͷλ0xFF����βΪ0xEE���ܹ�������6���������ʽ���ˣ�Ҫȥ�жϺ������
uint8_t Serial_RxFlag;					// ���ڽ��ձ�־λ

/* ��ʼ������ͨ�� */
void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);		// ����USART1��ʱ�ӣ���APB1�����裬�����Ķ���APB2������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		// ����GPIO��ʱ��
	
	GPIO_InitTypeDef GPIO_InitStructure;						// ���÷��͵�GPIO����	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;				// ���÷��͵�����Ϊ�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);						// ��ʼ��GPIOA��ʱ��
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				// ���ý��յ�����Ϊ��������	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;					
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			
 	GPIO_Init(GPIOA, &GPIO_InitStructure);						// ��ʼ��GPIOA��ʱ��
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;						// ���ò�����Ϊ115200
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// ����������
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;		// ѡ��ģʽ����/����
	USART_InitStructure.USART_Parity = USART_Parity_No;					// ѡ��У��λΪ��У��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;				// ѡ��ֹͣΪ1λֹͣλ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;			// ��Ϊ����ҪУ�飬�����ֳ�Ϊ8λ
	USART_Init(USART1, &USART_InitStructure);							// ��ʼ��USART1
	
	/* 
	���ڵĽ��տ��Է�Ϊ��ѯ���ж�
	��ѯ�ķ�����Ҫ��main�����ﲻ�ϵĲ�ѯ��־λ�������1�˾�֤���յ������ˣ�ʾ�����£�
		uint8_t RxData;
		if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
		{
			RxData = ISART_ReceiveData(USART1);  // ���յ���һ���ֽڵ�����
		}
	������һ����ʹ���жϽ��в�ѯ�Ĵ���
	*/
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);				// �����ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				// NVIC����
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;			// �ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	// ��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			// ��Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);										// USART1ʹ��
}


/* 
��TX���ŷ���һ���ֽ�
uint8_t Byte����Ҫ���͵��ֽ�
*/
void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);									// �����ֽ�����
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);	// ����TDR�������Ƿ�ת�Ƶ���λ�Ĵ���
}


/*
����һ���ֽ����ݰ�
uint8_t *Array��������Ԫ�صĵ�ַ
uint8_t Length������ĳ���
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
����һ���ַ���
char* String����Ҫ���͵��ַ���
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
��Ϊ�������ֵ�ʱ������6000����Ҫһ��һ����ֿ����ͣ���������������Ǹ�����µ�
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
����һ������
uint32_t Number����Ҫ���͵�����
uint8_t Length�����ֵĳ���
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
�˺�������Ҫ��ӡ�Ķ�����������ڣ���Ҫ����<stdio.h>�������ͷ�ļ��ӣ���������ͷ�ļ���ʱ��ͻ�ֱ�Ӱ���Ҳ����ȥ
��ֲ��printf�ķ�����ʹ��֮ǰ��Ҫ��ħ��������Targetҳ�棬��Use MicroLIB��ѡ��
MicroLIB��keilλǶ��ʽƽ̨�Ż���һ�������
printf��ӡ��ʱ����ǵ���fputcһ��һ����ӡ�ģ����Խ����ض����Ժ�fprint�ͻ�ʹ��������д�ӡ�����������
ʹ�õ�ʱ��ֱ�Ӵ���printf();���ɣ�����printf("Num=%d\r\n", 666);
*/
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

/*
�˺�������������ڴ�ӡ
char* format���������ո�ʽ���ַ���
...���������տɱ�����б�
ʾ����Serial_Prinf("Num=%d\r\n", 666);
�����Ҫ��ӡ���֣�Ϊ�˱�֤�����룬�򿪹���ѡ�C/C++��MiscControls��д�� --no-multibyte-chars
�ǵô�ӡ���ֵ�ʱ�����뷽ʽѡ��UTF8
*/
void Serial_Prinf(char* format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);			// ��formatλ�ý��ղ���������arg����
	vsprintf(String, format, arg);
	va_end(arg);					// �ͷŲ�����
	Serial_SendString(String);
}


/*
��ȡRx�ı�־λ
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
ʹ���ж������մ������ݵ��жϺ���
*/
void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;		// ״̬��־λ
	static uint8_t pRxPacket = 0;	// ָʾ���յ���һ��������
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)		// �жϱ�־λ
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		if (RxState == 0)			// ����ȴ���ͷ�ĳ���
		{
			if (RxData == 0xFF)		// �жϰ�ͷ�ǲ���0xFF
			{
				RxState = 1;
			}
		}
		else if (RxState == 1)						// �������ݵĳ���
		{
			Serial_RxPacket[pRxPacket] = RxData;	// �����ݴ浽RxPacket��
			pRxPacket ++;							// ָ��RxPacket����һλ��׼��������һ������
			if (pRxPacket >= 4)						// ��ΪRxPacketֻ�ܽ���4�����ݣ�����������Ҫ��RxState�л������հ�β��ģʽ
			{
				RxState = 2;
			}
		}
		else if (RxState == 2)						// ���հ�β�ĳ���
		{
			if (RxData == 0xEE)
			{
				RxState = 0;
				Serial_RxFlag = 1;						// ��־λ
			}
			
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);			// �����־λ
	}
}

