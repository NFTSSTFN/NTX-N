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

uint64_t Clock;						// 日历时钟
int16_t AX, AY, AZ, GX, GY, GZ;		// 陀螺仪数据
int16_t Roll, Pitch, Yaw;			// 欧拉角

uint8_t Data[32] = {0};		// 发送的数据包，32字节，主要用来控制
uint8_t RxData[32] = {0};	// 接收的数据包，32字节，主要用来显示




int main(void)
{
//	OLED_Init();					// 初始化OLED
//	MPU6050_Hard_Init();			// 初始化硬件读取mpu6050数据
//	Serial_Init();					// 初始化串口模块
//	Timer1_Init();					// 开始定时中断，用来记录系统当前使用时间
	test();
	while (1)
	{
//		MPU6050_Hard_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);				// 获取陀螺仪数据
//		Cal_f(AX, AY, AZ, GX, GY, GZ, Clock, &Roll, &Pitch, &Yaw);		// 计算欧拉角
//		AHC_Send_EA_Data(Roll, Pitch, 0, 0x00);						// 串口发送数据至匿名上位机
//		OLED_ShowSignedNum(1, 1, Roll, 5);
//		OLED_ShowSignedNum(1, 9, Pitch, 5);
//		OLED_ShowSignedNum(2, 1, Yaw, 5);
//		OLED_ShowNum(3, 1, Clock, 10);
	}
}


/* 定时中断代码TIM1，用来计时 */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)		// 获取中断标志位
	{
		Clock++;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);			// 清除中断标志位
	}
}

