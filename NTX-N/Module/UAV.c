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
	// 0-ͨѶ 1-���� 2-ǰ�� 3-���� 4-��ص���
	uint8_t RxData[32] = {0};			// ���յ����ݰ���32�ֽ�
	
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
