#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "Key.h"
#include "OLED.h"
#include "Delay.h"
#include "nRF24L01.h"
#include "PS2.h"


void Control_Init(void)
{
	AD_Init();
	OLED_Init();		// OLED��ʾ��PB6-PB9
	NRF24L01_Init();	// ������Ƶ������PA5-PB10
	Key_Init();			// PB11����ť���
	

	// 0-ͨѶ 1-���� 2-ǰ�� 3-����
	uint8_t Data[32] = {0};		// ���͵����ݰ���32�ֽڣ���Ҫ��������
	uint8_t RxData[32] = {0};	// ���յ����ݰ���32�ֽڣ���Ҫ������ʾ
	uint8_t flag = 0;
	
	while (1)
	{	
		if (Key_GetNum() == 1)
		{
			if (flag == 0){ flag = 1;}
			else { flag = 0; }
		}
		// ��һ�����������ݣ���ˢ�µ���ʾ����
		if (NRF24L01_GetHex(RxData))
		{
			OLED_ShowString(1, 1, "Pow: ");				// ������ʾ
			OLED_ShowNum(1, 6, RxData[1], 2);
			OLED_ShowString(2, 1, "FBV: ");				// ǰ����ʾ
			OLED_ShowNum(2, 6, RxData[2], 2);
			OLED_ShowString(3, 1, "LRF: ");				// ������ʾ
			OLED_ShowNum(3, 6, RxData[3], 2);
			
			OLED_ShowString(4, 3, "pow: ");				// ������ʾ
			OLED_ShowNum(4, 8, RxData[4], 6);
			
			
			OLED_ShowString(4, 1, "Y");
		}
		else
		{
			OLED_ShowString(4, 1, "N");
		}
		Data[0] = flag;
		Data[1] = (AD_Value[0] / 40.96);		// ����
		Data[2] = (AD_Value[1] / 40.96);		// ǰ��
		Data[3] = (AD_Value[2] / 40.96);		// ����
		
		OLED_ShowNum(1, 11, Data[1], 2);
		OLED_ShowNum(2, 11, Data[0], 2);
		
		NRF24L01_SendHex(Data);
		
		Delay_ms(200);
	}
}
