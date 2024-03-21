#include "stm32f10x.h"                  // Device header
#include "Control.h"
#include "UAV.h"

#include "OLED.h"
#include "Delay.h"
#include "MPU6050_Hard.h"
#include "Timer.h"
#include "Serial.h"
#include "AHC.h"
#include "test.h"
#include "imu_f.h"

uint64_t Clock;						// ����ʱ��
int16_t AX, AY, AZ, GX, GY, GZ;		// ����������
int16_t Roll, Pitch, Yaw;			// ŷ����

uint8_t Data[32] = {0};		// ���͵����ݰ���32�ֽڣ���Ҫ��������
uint8_t RxData[32] = {0};	// ���յ����ݰ���32�ֽڣ���Ҫ������ʾ




int main(void)
{
//	OLED_Init();					// ��ʼ��OLED
//	MPU6050_Hard_Init();			// ��ʼ��Ӳ����ȡmpu6050����
//	Serial_Init();					// ��ʼ������ģ��
//	Timer1_Init();					// ��ʼ��ʱ�жϣ�������¼ϵͳ��ǰʹ��ʱ��
	test();
	while (1)
	{
//		MPU6050_Hard_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);				// ��ȡ����������
//		Cal_f(AX, AY, AZ, GX, GY, GZ, Clock, &Roll, &Pitch, &Yaw);		// ����ŷ����
//		AHC_Send_EA_Data(Roll, Pitch, 0, 0x00);						// ���ڷ���������������λ��
//		OLED_ShowSignedNum(1, 1, Roll, 5);
//		OLED_ShowSignedNum(1, 9, Pitch, 5);
//		OLED_ShowSignedNum(2, 1, Yaw, 5);
//		OLED_ShowNum(3, 1, Clock, 10);
	}
}


/* ��ʱ�жϴ���TIM1��������ʱ */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)		// ��ȡ�жϱ�־λ
	{
		Clock++;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);			// ����жϱ�־λ
	}
}

