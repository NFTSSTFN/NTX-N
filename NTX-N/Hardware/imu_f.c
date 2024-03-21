#include "stm32f10x.h" 
#include "Delay.h"
#include "math.h"
#include "imu_f.h"

/*
ͨ����Ԫ������ɻ���̬
*/

const float M_PI = 3.1415926535;
const float RtA = 57.2957795f;
const float AtR = 0.0174532925f;
const float Gyro_G = 0.03051756f*2;	  	//�����ǳ�ʼ������+-2000��ÿ����1 / (65536 / 4000) = 0.03051756*2		
const float Gyro_Gr = 0.0005326f*2;     //������ÿ��,ת������ÿ���� 2*0.03051756	 * 0.0174533f = 0.0005326*2
 
static float NormAcc;
 
int16_t pitch = 0;
int16_t roll = 0;
float now_f, last_f;
float dt_f, halfT; 

 
/* ��Ԫ��ϵ�� */
typedef volatile struct {
  float q0;
  float q1;
  float q2;
  float q3;
} Quaternion;
Quaternion NumQ = {1, 0, 0, 0};
 
/* �����ǻ������ */
struct V{
	float x;
	float y;
	float z;
};	
volatile struct V GyroIntegError = {0};
 
void Cal_f(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, int16_t clock, int16_t* roll, int16_t* pitch, int16_t* yaw)
{		
	
	// ���㵱ǰʱ�����ϴ�����ʱ������Ԫ��΢�ַ����ṩʱ��
	now_f = clock;
	dt_f = (now_f - last_f) / 1000;
	last_f = now_f;
	
	// Gravity ����������������
	// Acc ������ٶȹ�һ�����ֵ
	// AccGravity �����������֮���ֵ
	volatile struct V Gravity,Acc,Gyro,AccGravity;
	
	static  float KpDef = 0.3f;					// 
	static  float KiDef = 0.3f;

	
	float q0_t,q1_t,q2_t,q3_t;
	//float NormAcc;	
	float NormQuat; 
	float HalfTime = dt_f * 0.5f;
 
	//��ȡ��Ч��ת�����е��������� 
	Gravity.x = 2*(NumQ.q1 * NumQ.q3 - NumQ.q0 * NumQ.q2);								
	Gravity.y = 2*(NumQ.q0 * NumQ.q1 + NumQ.q2 * NumQ.q3);						  
	Gravity.z = 1-2*(NumQ.q1 * NumQ.q1 + NumQ.q2 * NumQ.q2);	
	
	// ���ٶȹ�һ��
	NormAcc = 1/sqrt(squa(ax)+ squa(ay) +squa(az));
	Acc.x = ax * NormAcc;
	Acc.y = ay * NormAcc;
	Acc.z = az * NormAcc;
	
 	//������˵ó���ֵ
	AccGravity.x = (Acc.y * Gravity.z - Acc.z * Gravity.y);
	AccGravity.y = (Acc.z * Gravity.x - Acc.x * Gravity.z);
	AccGravity.z = (Acc.x * Gravity.y - Acc.y * Gravity.x);
	
	//�������ٶȻ��ֲ������ٶȵĲ���ֵ
	GyroIntegError.x += AccGravity.x * KiDef;
	GyroIntegError.y += AccGravity.y * KiDef;
	GyroIntegError.z += AccGravity.z * KiDef;
	
	//���ٶ��ںϼ��ٶȻ��ֲ���ֵ
	Gyro.x = gx * Gyro_Gr + KpDef * AccGravity.x  +  GyroIntegError.x;	// ����ֵ����������Gyro_Gr��ȷ��
	Gyro.y = gy * Gyro_Gr + KpDef * AccGravity.y  +  GyroIntegError.y;
	Gyro.z = gz * Gyro_Gr + KpDef * AccGravity.z  +  GyroIntegError.z;
	
	// һ�����������, ������Ԫ��
	q0_t = (-NumQ.q1*Gyro.x - NumQ.q2*Gyro.y - NumQ.q3*Gyro.z) * HalfTime;
	q1_t = ( NumQ.q0*Gyro.x - NumQ.q3*Gyro.y + NumQ.q2*Gyro.z) * HalfTime;
	q2_t = ( NumQ.q3*Gyro.x + NumQ.q0*Gyro.y - NumQ.q1*Gyro.z) * HalfTime;
	q3_t = (-NumQ.q2*Gyro.x + NumQ.q1*Gyro.y + NumQ.q0*Gyro.z) * HalfTime;
	
	NumQ.q0 += q0_t;
	NumQ.q1 += q1_t;
	NumQ.q2 += q2_t;
	NumQ.q3 += q3_t;
	
	// ��Ԫ����һ��
	NormQuat = 1/sqrt(squa(NumQ.q0) + squa(NumQ.q1) + squa(NumQ.q2) + squa(NumQ.q3));
	NumQ.q0 *= NormQuat;
	NumQ.q1 *= NormQuat;
	NumQ.q2 *= NormQuat;
	NumQ.q3 *= NormQuat;	
	
	// ��Ԫ��תŷ����
	 

	*yaw = atan2f(2 * NumQ.q1 *NumQ.q2 + 2 * NumQ.q0 * NumQ.q3, 1 - 2 * NumQ.q2 *NumQ.q2 - 2 * NumQ.q3 * NumQ.q3) * RtA;  //yaw

	// ͨ����һ�ַ�����yaw
//	float yaw_G = gz * Gyro_G;
//	if((yaw_G > 1.0f) || (yaw_G < -1.0f)) //����̫С������Ϊ�Ǹ��ţ�����ƫ������
//	{
//		*yaw  += yaw_G * dt_f;
//	}

	*pitch = asin(2 * NumQ.q0 *NumQ.q2 - 2 * NumQ.q1 * NumQ.q3) * RtA;						

	*roll = atan2(2 * NumQ.q2 *NumQ.q3 + 2 * NumQ.q0 * NumQ.q1, 1 - 2 * NumQ.q1 *NumQ.q1 - 2 * NumQ.q2 * NumQ.q2) * RtA;	//PITCH 			

}




