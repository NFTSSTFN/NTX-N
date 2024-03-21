#include "stm32f10x.h" 
#include "Delay.h"
#include "math.h"
#include "imu_f.h"

/*
通过四元数计算飞机姿态
*/

const float M_PI = 3.1415926535;
const float RtA = 57.2957795f;
const float AtR = 0.0174532925f;
const float Gyro_G = 0.03051756f*2;	  	//陀螺仪初始化量程+-2000度每秒于1 / (65536 / 4000) = 0.03051756*2		
const float Gyro_Gr = 0.0005326f*2;     //面计算度每秒,转换弧度每秒则 2*0.03051756	 * 0.0174533f = 0.0005326*2
 
static float NormAcc;
 
int16_t pitch = 0;
int16_t roll = 0;
float now_f, last_f;
float dt_f, halfT; 

 
/* 四元数系数 */
typedef volatile struct {
  float q0;
  float q1;
  float q2;
  float q3;
} Quaternion;
Quaternion NumQ = {1, 0, 0, 0};
 
/* 陀螺仪积分误差 */
struct V{
	float x;
	float y;
	float z;
};	
volatile struct V GyroIntegError = {0};
 
void Cal_f(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, int16_t clock, int16_t* roll, int16_t* pitch, int16_t* yaw)
{		
	
	// 计算当前时间与上次求解的时间差，给四元数微分方程提供时间
	now_f = clock;
	dt_f = (now_f - last_f) / 1000;
	last_f = now_f;
	
	// Gravity 储存三个重力分量
	// Acc 储存加速度归一化后的值
	// AccGravity 储存向量叉乘之后的值
	volatile struct V Gravity,Acc,Gyro,AccGravity;
	
	static  float KpDef = 0.3f;					// 
	static  float KiDef = 0.3f;

	
	float q0_t,q1_t,q2_t,q3_t;
	//float NormAcc;	
	float NormQuat; 
	float HalfTime = dt_f * 0.5f;
 
	//提取等效旋转矩阵中的重力分量 
	Gravity.x = 2*(NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);								
	Gravity.y = 2*(NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);						  
	Gravity.z = 1-2*(NumQ.q1 * NumQ.q1 + NumQ.q2 * NumQ.q2);	
	
	// 加速度归一化
	NormAcc = 1/sqrt(squa(ax)+ squa(ay) +squa(az));
	Acc.x = ax * NormAcc;
	Acc.y = ay * NormAcc;
	Acc.z = az * NormAcc;
	
 	//向量差乘得出的值
	AccGravity.x = (Acc.y * Gravity.z - Acc.z * Gravity.y);
	AccGravity.y = (Acc.z * Gravity.x - Acc.x * Gravity.z);
	AccGravity.z = (Acc.x * Gravity.y - Acc.y * Gravity.x);
	
	//再做加速度积分补偿角速度的补偿值
	GyroIntegError.x += AccGravity.x * KiDef;
	GyroIntegError.y += AccGravity.y * KiDef;
	GyroIntegError.z += AccGravity.z * KiDef;
	
	//角速度融合加速度积分补偿值
	Gyro.x = gx * Gyro_Gr + KpDef * AccGravity.x  +  GyroIntegError.x;	// 弧度值，这里的这个Gyro_Gr待确认
	Gyro.y = gy * Gyro_Gr + KpDef * AccGravity.y  +  GyroIntegError.y;
	Gyro.z = gz * Gyro_Gr + KpDef * AccGravity.z  +  GyroIntegError.z;
	
	// 一阶龙格库塔法, 更新四元数
	q0_t = (-NumQ.q1*Gyro.x - NumQ.q2*Gyro.y - NumQ.q3*Gyro.z) * HalfTime;
	q1_t = ( NumQ.q0*Gyro.x - NumQ.q3*Gyro.y + NumQ.q2*Gyro.z) * HalfTime;
	q2_t = ( NumQ.q3*Gyro.x + NumQ.q0*Gyro.y - NumQ.q1*Gyro.z) * HalfTime;
	q3_t = (-NumQ.q2*Gyro.x + NumQ.q1*Gyro.y + NumQ.q0*Gyro.z) * HalfTime;
	
	NumQ.q0 += q0_t;
	NumQ.q1 += q1_t;
	NumQ.q2 += q2_t;
	NumQ.q3 += q3_t;
	
	// 四元数归一化
	NormQuat = 1/sqrt(squa(NumQ.q0) + squa(NumQ.q1) + squa(NumQ.q2) + squa(NumQ.q3));
	NumQ.q0 *= NormQuat;
	NumQ.q1 *= NormQuat;
	NumQ.q2 *= NormQuat;
	NumQ.q3 *= NormQuat;	
	
	// 四元数转欧拉角
	 

	*yaw = atan2f(2 * NumQ.q1 *NumQ.q2 + 2 * NumQ.q0 * NumQ.q3, 1 - 2 * NumQ.q2 *NumQ.q2 - 2 * NumQ.q3 * NumQ.q3) * RtA;  //yaw

	// 通过另一种方法求yaw
//	float yaw_G = gz * Gyro_G;
//	if((yaw_G > 1.0f) || (yaw_G < -1.0f)) //数据太小可以认为是干扰，不是偏航动作
//	{
//		*yaw  += yaw_G * dt_f;
//	}

	*pitch = asin(2 * NumQ.q0 *NumQ.q2 - 2 * NumQ.q1 * NumQ.q3) * RtA;						

	*roll = atan2(2 * NumQ.q2 *NumQ.q3 + 2 * NumQ.q0 * NumQ.q1, 1 - 2 * NumQ.q1 *NumQ.q1 - 2 * NumQ.q2 * NumQ.q2) * RtA;	//PITCH 			

}




