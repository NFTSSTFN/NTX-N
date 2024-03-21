#ifndef __IMU_H
#define	__IMU_H


void SetUp(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ);
void Cal_RPY(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ, uint32_t Clock, int16_t *Roll_R, int16_t *Pitch_R, int16_t *Yaw_R);
 
#endif
