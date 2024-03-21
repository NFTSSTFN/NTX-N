#include "stm32f10x.h"                  // Device header
#include "nRF24L01.h"
#include "Delay.h"
#include "OLED.h"
	
	

	

void test(void)
{
//	uint8_t Data[32] = {0};		// ���͵����ݰ���32�ֽڣ���Ҫ��������
	uint8_t RxData[32] = {0};	// ���յ����ݰ���32�ֽڣ���Ҫ������ʾ
	OLED_Init();					// ��ʼ��OLED
	NRF24L01_Init();	// ������Ƶ������PA5-PB10

	while (1)
	{
//		// ��������
//		if (NRF24L01_GetHex(RxData))
//		{
//			RxData[0] = RxData[0];
//		}
//		NRF24L01_SendHex(RxData);
//		Delay_ms(10);
		
		
		// ��������
		if (NRF24L01_GetHex(RxData))
		{
			OLED_ShowNum(1, 1, RxData[0], 5);
			OLED_ShowString(4, 1, "Y");
			RxData[0] = RxData[0] + 1;
			NRF24L01_SendHex(RxData);
		}
		else
		{
			OLED_ShowString(4, 1, "N");
		}
		Delay_ms(200);
	}
}
