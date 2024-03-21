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
	OLED_Init();		// OLED显示，PB6-PB9
	NRF24L01_Init();	// 无线射频开启，PA5-PB10
	Key_Init();			// PB11，按钮检测
	

	// 0-通讯 1-油门 2-前后 3-左右
	uint8_t Data[32] = {0};		// 发送的数据包，32字节，主要用来控制
	uint8_t RxData[32] = {0};	// 接收的数据包，32字节，主要用来显示
	uint8_t flag = 0;
	
	while (1)
	{	
		if (Key_GetNum() == 1)
		{
			if (flag == 0){ flag = 1;}
			else { flag = 0; }
		}
		// 第一步：接收数据，并刷新到显示屏上
		if (NRF24L01_GetHex(RxData))
		{
			OLED_ShowString(1, 1, "Pow: ");				// 油门显示
			OLED_ShowNum(1, 6, RxData[1], 2);
			OLED_ShowString(2, 1, "FBV: ");				// 前后显示
			OLED_ShowNum(2, 6, RxData[2], 2);
			OLED_ShowString(3, 1, "LRF: ");				// 左右显示
			OLED_ShowNum(3, 6, RxData[3], 2);
			
			OLED_ShowString(4, 3, "pow: ");				// 左右显示
			OLED_ShowNum(4, 8, RxData[4], 6);
			
			
			OLED_ShowString(4, 1, "Y");
		}
		else
		{
			OLED_ShowString(4, 1, "N");
		}
		Data[0] = flag;
		Data[1] = (AD_Value[0] / 40.96);		// 油门
		Data[2] = (AD_Value[1] / 40.96);		// 前后
		Data[3] = (AD_Value[2] / 40.96);		// 左右
		
		OLED_ShowNum(1, 11, Data[1], 2);
		OLED_ShowNum(2, 11, Data[0], 2);
		
		NRF24L01_SendHex(Data);
		
		Delay_ms(200);
	}
}
