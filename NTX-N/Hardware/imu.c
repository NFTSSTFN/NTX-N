#include "stm32f10x.h" 
#include "Delay.h"
#include "math.h"
#include "MPU6050_Hard.h"

/*
通过旋转矩阵计算飞机姿态==============================
*/

// 记录此次计算所需的参数
float now = 0, lasttime = 0, dt = 0;				// 定义微分时间
float ax_offset = 0, ay_offset = 0, az_offset;		// x，y轴加速度偏移量
float gx_offset = 0, gy_offset = 0, gz_offset;		// x，y轴角速度偏移量
float k_roll = 0, k_pitch = 0, k_yaw = 0;			// 卡尔曼滤波后估计出最优角度，最优估计状态,初始给0
float v_roll = 0, v_pitch = 0, v_yaw = 0;			// 换算到x，y轴上的角速度
float g_roll = 0, g_pitch = 0, g_yaw = 0;			// 陀螺仪积分计算出的角度，先验状态
float a_roll = 0, a_pitch = 0;						// 加速度计观测出的角度，观测状态
		
float rad2deg = 57.29578;							// 弧度到角度的换算系数
		
float e_P[2][2] = {{1, 0}, {0, 1}};					// 协方差矩阵
float k_k[2][2] = {{0, 0}, {0, 0}};					// 卡尔曼增益矩阵

/* 计算偏移量误差，求200次平均 */
void SetUp(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ)
{
	for (int16_t i = 1; i <= 200; i++)
	{
		MPU6050_Hard_GetData(&AX, &AY, &AZ, &GX, &GY, &GZ);
		ax_offset = ax_offset + AX;
		ay_offset = ay_offset + AY;
		gx_offset = gx_offset + GX;
		gy_offset = gy_offset + GY;
	}
	ax_offset = ax_offset / 200;		// 计算x轴加速度的偏移量
	ay_offset = ay_offset / 200;		// 计算y轴加速度的偏移量
	gx_offset = gx_offset / 200;		// 计算x轴剿琴度的偏移量
	gy_offset = gy_offset / 200;		// 计算y轴剿琴度的偏移量
	Delay_ms(100);
}


/* 计算Roll,Pitch,Yaw */
void Cal_RPY(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ, uint32_t Clock, int16_t *Roll_R, int16_t *Pitch_R, int16_t *Yaw_R)
{
	now = Clock;										// 当前时间，获取到的值是毫秒
	dt = (now - lasttime) / 1000000;					// 微分时间,s
	lasttime = now;
	
	/* step1:计算先验状态 */
	/* 计算x，y轴上的角速度 */
	v_roll = (GX - gx_offset) + ((sin(k_pitch) * sin(k_roll)) / cos(k_pitch)) * (GY - gy_offset) + ((sin(k_pitch) * cos(k_roll)) / cos(k_pitch)) * GZ; // roll轴的角速度
	v_pitch = cos(k_roll) * (GY - gy_offset) - sin(k_roll) * GZ;			// pitch轴的角速度
//	v_yaw = (sin(k_roll) / cos(k_pitch)) * (GY - gy_offset) + (cos(k_roll) / cos(k_pitch)) * GZ; 	// yaw轴的角速度
	g_roll = k_roll + dt * v_roll;					// 先验证roll的角度
	g_pitch = k_pitch + dt * v_pitch;				// 先验证pitch的角度
//	g_yaw = k_yaw + dt * v_yaw;						// 因为没有磁力计，所以在这里就是g_yaw就是最终的yaw，但是不太准
	
	/* step2:计算先验误差协方差矩阵p */
	e_P[0][0] = e_P[0][0] + 0.0025;					// 这里的Q矩阵是一个对角阵且对角元均为0.003
	e_P[0][1] = 0;
	e_P[1][0] = 0;
	e_P[1][1] = e_P[1][1] + 0.0025;
	
	/* setp3：更新卡尔曼增益K* */
	k_k[0][0] = e_P[0][0] / (e_P[0][0] + 0.3);
	k_k[0][1] = 0;
	k_k[1][0] = 0;
	k_k[1][1] = e_P[1][1] / (e_P[1][1] + 0.3);
	
	/* setp4：计算最优估计装填 */
	/* 观测状态 */
	// roll,pitch 角度
	a_roll = atan2((AY - ay_offset) , AZ) * rad2deg;
	a_pitch = -atan((AX - ax_offset) / (sqrt((AY - ay_offset) * (AY - ay_offset) + AZ * AZ))) * rad2deg;
	
	/* 最优解估计状态 */
	k_roll = k_roll + k_k[0][0] * (a_roll - g_roll);
	k_pitch = k_pitch + k_k[1][1] * (a_pitch - g_pitch);
//	k_yaw = k_yaw + k_k * g_yaw;				// 这里需要讲e_P,k_k转为三维矩阵即可实现目的，但是没空，以后再转
	
	/* step5:更新协方差矩阵p */
	e_P[0][0] = (1 - k_k[0][0]) * e_P[0][0];
	e_P[0][1] = 0;
	e_P[1][0] = 0;
	e_P[1][1] = (1- k_k[1][1]) * e_P[1][1];
	
	*Roll_R = k_roll;
	*Pitch_R = k_pitch;
//	*Yaw_R = k_yaw;
	// 最后的结果 roll: k_roll， pitch：k_p
}


