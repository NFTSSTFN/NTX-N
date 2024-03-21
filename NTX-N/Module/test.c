#include "stm32f10x.h"                  // Device header
#include "nRF24L01.h"
#include "Delay.h"
#include "OLED.h"
	
	

	

void test(void)
{
//	uint8_t Data[32] = {0};		// 发送的数据包，32字节，主要用来控制
	uint8_t RxData[32] = {0};	// 接收的数据包，32字节，主要用来显示
	OLED_Init();					// 初始化OLED
	NRF24L01_Init();	// 无线射频开启，PA5-PB10

	while (1)
	{
//		// 发送数据
//		if (NRF24L01_GetHex(RxData))
//		{
//			RxData[0] = RxData[0];
//		}
//		NRF24L01_SendHex(RxData);
//		Delay_ms(10);
		
		
		// 接收数据
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
