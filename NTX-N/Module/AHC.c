#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "OLED.h"


/*
Anonymous Host Computer	v7
匿名上位机 v7 通讯
串口通讯的波特率为115200，记得修改
*/


/*
该函数发送惯性传感器数据，也就是发送直接从mpu6050得到的acc，gyro
数据帧格式：
==========================================================================================
		帧头   	目标地址   	功能码ID   	数据长度   	数据内容   	和校验		附加校验
大小	1Byte	1Byte		1Byte		1Byte		nByte		1Byte		1Byte
内容	0xAA	0xFF		0x01		13byte		...			累加		累加
==========================================================================================
数据内容格式：
==========================================================================================
数据类型	int16_t		int16_t		int16_t		int16_t		int16_t		int16_t		int8_t
数据内容	Acc_X		Acc_Y		Acc_Z		Gyro_X		Gyro_Y		Gyro_Z		Shock_STA(震动状态)
==========================================================================================
备注：
1. 帧头、目标地址、功能码均为固定数据
2. 数据长度就是三个Acc和三个Gyro长度，因为Acc和Gyro是int16_t，所以有12个，外加一个震动状态，没有的话就给0x00，共计13个
3. 因为串口只能发送uint8_t的数据，所以要将带符号的int16_t的数据进行拆分，低位在前，高位在后，用无符号的uint8_t进行串口发送即可
4. 和校验就是一直从帧头累加到数据内容结束，共计13+4=17个
5. 附加校验就是又进行了一次和校验，写的时候直接让其与和校验相等即可
*/
void AHC_Send_AG_Data(int16_t AccX, int16_t AccY, int16_t AccZ,
					 int16_t GyroX, int16_t GyroY, int16_t GyroZ)
{
	uint8_t TxData[19];						// 要发送的数据
	int8_t Sum_Check = 0;					// 和校验
	int8_t Add_Check = 0;					// 附加校验
	
	TxData[0] = 0xAA;						// 帧头，固定为0xAA
	TxData[1] = 0xFF;						// 目标地址，固定为0xFF
	TxData[2] = 0x01;						// 功能码ID，0x01表示发送的是直接从mpu6050得到的数据
	TxData[3] = 13;							// 数据的长度（这里不是总包长，数据多长就写多长）
	TxData[4] = AccX & 0xFF;				// 第一个数据的低字节，因为匿名上位机要求低位先行
	TxData[5] = (AccX >> 8) & 0xFF;			// 第一个数据的高字节
	TxData[6] = AccY & 0xFF;
	TxData[7] = (AccY >> 8) & 0xFF;
	TxData[8] = AccZ & 0xFF;
	TxData[9] = (AccZ >> 8) & 0xFF;
	TxData[10] = GyroX & 0xFF;
	TxData[11] = (GyroX >> 8) & 0xFF;
	TxData[12] = GyroY & 0xFF;
	TxData[13] = (GyroY >> 8) & 0xFF;
	TxData[14] = GyroZ & 0xFF;
	TxData[15] = (GyroZ >> 8) & 0xFF;
	TxData[16] = 0x00;						// 最后一个数据，为震动状态，因为不知道如何获取，直接0x00
	
	for (uint8_t i = 0; i < TxData[3] + 4; i++)
	{
		Sum_Check += TxData[i];
		Add_Check += Sum_Check;
	}
	
	TxData[17] = Sum_Check;					// 和校验位
	TxData[18] = Add_Check;					// 附加校验位
	
	Serial_SendArray(TxData, 19);
}


/*
该函数发送飞控姿态，欧拉角
数据帧格式：
==========================================================================================
		帧头   	目标地址   	功能码ID   	数据长度   	数据内容   	和校验		附加校验
大小	1Byte	1Byte		1Byte		1Byte		nByte		1Byte		1Byte
内容	0xAA	0xFF		0x03		7byte		...			累加		累加
==========================================================================================
数据内容格式：
==========================================================================================
数据类型	int16_t		int16_t		int16_t		uint8_t
数据内容	Roll*100	Pitch*100	Yaw*100		Fusion_STA(融合状态)
==========================================================================================
*/
void AHC_Send_EA_Data(int16_t Roll, int16_t Pitch, int16_t Yaw, uint8_t Fusion_STA)
{
	Roll = Roll * 100;			// 这里的比例待定，有点问题，需要实际数据来进行调试
	Pitch = Pitch * 100;
	Yaw = Yaw * 100;
	
	uint8_t TxData[13];						// 要发送的数据
	int8_t Sum_Check = 0;					// 和校验
	int8_t Add_Check = 0;					// 附加校验
	
	TxData[0] = 0xAA;						// 帧头，固定为0xAA
	TxData[1] = 0xFF;						// 目标地址，固定为0xFF
	TxData[2] = 0x03;						// 功能码ID，0x01表示发送的是直接从mpu6050得到的数据
	TxData[3] = 7;							// 数据的长度（这里不是总包长，数据多长就写多长）
	TxData[4] = Roll & 0xFF;				// 第一个数据的低字节，因为匿名上位机要求低位先行
	TxData[5] = (Roll >> 8) & 0xFF;			// 第一个数据的高字节
	TxData[6] = Pitch & 0xFF;
	TxData[7] = (Pitch >> 8) & 0xFF;
	TxData[8] = Yaw & 0xFF;
	TxData[9] = (Yaw >> 8) & 0xFF;
	TxData[10] = 0x00;						// 最后一个数据，为震动状态，因为不知道如何获取，直接0x00
	
	for (uint8_t i = 0; i < TxData[3] + 4; i++)
	{
		Sum_Check += TxData[i];
		Add_Check += Sum_Check;
	}
	
	TxData[11] = Sum_Check;					// 和校验位
	TxData[12] = Add_Check;					// 附加校验位
	
	Serial_SendArray(TxData, 13);
}


/*
该函数发送飞控姿态，四元数
数据帧格式：
==========================================================================================
		帧头   	目标地址   	功能码ID   	数据长度   	数据内容   	和校验		附加校验
大小	1Byte	1Byte		1Byte		1Byte		nByte		1Byte		1Byte
内容	0xAA	0xFF		0x04		9byte		...			累加		累加
==========================================================================================
数据内容格式：
==========================================================================================
数据类型	int16_t		int16_t		int16_t		int16_t		uint8_t
数据内容	q0*10000	q1*10000	q3*10000	q4*10000	Fusion_STA(融合状态)
==========================================================================================
*/
void AHC_Send_QA_Data(int16_t q0, int16_t q1, int16_t q2, int16_t q3, uint8_t Fusion_STA)
{
	q0 = q0 * 10000;
	q1 = q1 * 10000;
	q2 = q2 * 10000;
	q3 = q3 * 10000;
	
	uint8_t TxData[15];						// 要发送的数据
	int8_t Sum_Check = 0;					// 和校验
	int8_t Add_Check = 0;					// 附加校验
	
	TxData[0] = 0xAA;						// 帧头，固定为0xAA
	TxData[1] = 0xFF;						// 目标地址，固定为0xFF
	TxData[2] = 0x04;						// 功能码ID，0x01表示发送的是直接从mpu6050得到的数据
	TxData[3] = 9;							// 数据的长度（这里不是总包长，数据多长就写多长）
	TxData[4] = q0 & 0xFF;					// 第一个数据的低字节，因为匿名上位机要求低位先行
	TxData[5] = (q0 >> 8) & 0xFF;			// 第一个数据的高字节
	TxData[6] = q1 & 0xFF;
	TxData[7] = (q1 >> 8) & 0xFF;
	TxData[8] = q2 & 0xFF;
	TxData[9] = (q2 >> 8) & 0xFF;
	TxData[10] = q3 & 0xFF;
	TxData[11] = (q3 >> 8) & 0xFF;
	TxData[12] = 0x00;						// 最后一个数据，为震动状态，因为不知道如何获取，直接0x00
	
	for (uint8_t i = 0; i < TxData[3] + 4; i++)
	{
		Sum_Check += TxData[i];
		Add_Check += Sum_Check;
	}
	
	TxData[13] = Sum_Check;					// 和校验位
	TxData[14] = Add_Check;					// 附加校验位
	
	Serial_SendArray(TxData, 15);
}
