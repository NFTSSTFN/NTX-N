#ifndef __MPU6050_HARD_H
#define __MPU6050_HARD_H

void MPU6050_Hard_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_Hard_ReadReg(uint8_t RegAddress);

void MPU6050_Hard_Init(void);
uint8_t MPU6050_Hard_GetID(void);
void MPU6050_Hard_GetData(int16_t *AccX, int16_t *AccY, int16_t *AccZ,
					 int16_t *GyroX, int16_t *GyroY, int16_t *GyroZ);


#endif 

