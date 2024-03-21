#include "stm32f10x.h"                  // Device header
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS 0xD0

// 硬件I2C通信、B10、B11接口，去查引脚定义表

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
	{
		Timeout--;
		if (Timeout == 0)
		{
			break;
		}
	}
}

void MPU6050_Hard_WriteReg(uint8_t RegAddress, uint8_t Data)
{
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(RegAdress);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(Data);
//	MyI2C_ReceiveAck();
//	MyI2C_Stop();
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);
	
	I2C_SendData(I2C2, Data);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTOP(I2C2, ENABLE);
}

uint8_t MPU6050_Hard_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS);
//	MyI2C_ReceiveAck();
//	MyI2C_SendByte(RegAdress);
//	MyI2C_ReceiveAck();

//	MyI2C_Start();
//	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);
//	MyI2C_ReceiveAck();
//	Data = MyI2C_ReceiveByte();
//	MyI2C_SendAck(1);
//	
//	MyI2C_Stop();
	
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2, RegAddress);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTART(I2C2, ENABLE);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	I2C_GenerateSTOP(I2C2, ENABLE);
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);
	Data = I2C_ReceiveData(I2C2);
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	
	return Data;
}

void MPU6050_Hard_Init(void)
{
//	MyI2C_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_ClockSpeed = 50000;			// 指定SCL频率，标准100KHz，最大400KHz，这里50kHZ
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;	// 时钟频率占空比，再小于100KHZ的情况下没用
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;			// 应答位配置，配置寄存器ACK位
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	// 从机响应几位地址
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;			// STM32作为主机使用，不和其他设备重复
	I2C_Init(I2C2, &I2C_InitStructure);
	
	I2C_Cmd(I2C2, ENABLE);
	
	
	
	
	
	MPU6050_Hard_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
	MPU6050_Hard_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
	
	MPU6050_Hard_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_Hard_WriteReg(MPU6050_CONFIG, 0x06);
	MPU6050_Hard_WriteReg(MPU6050_GYRO_CONFIG, 0x18);		// 配置陀螺仪量程，这里选最小量程
	MPU6050_Hard_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);		// 配置加速度计量程
}

uint8_t MPU6050_Hard_GetID(void)
{
	return MPU6050_Hard_ReadReg(MPU6050_WHO_AM_I);
}

void MPU6050_Hard_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
					 int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ)
{
	uint8_t DataH, DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (DataH << 8) | DataL;
	
	DataH = MPU6050_Hard_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL = MPU6050_Hard_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (DataH << 8) | DataL;
	
}







