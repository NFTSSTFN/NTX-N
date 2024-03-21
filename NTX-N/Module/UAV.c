#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "OLED.h"
#include "MPU6050.h"
#include "Delay.h"
#include "nRF24L01.h"
#include "PWM.h"
#include "PS2.h"
#include "Power.h"


void UAV_Init(void)
{
	// 0-通讯 1-油门 2-前后 3-左右 4-电池电量
	uint8_t RxData[32] = {0};			// 接收的数据包，32字节
	
	NRF24L01_Init();
	PWM_Init();
	Pow_Init();
	
	uint8_t speed = 0;

	while (1)
	{
		if (NRF24L01_GetHex(RxData))
		{
			if (RxData[0] == 1)
			{
				speed = RxData[1];
				PWM_SetCompare1(speed);
				PWM_SetCompare2(speed);
				PWM_SetCompare3(speed);
				PWM_SetCompare4(speed);
			}
			RxData[4] = PowVal[0];
			RxData[1] = speed;
			NRF24L01_SendHex(RxData);
		}
		Delay_ms(200);
	}
}
