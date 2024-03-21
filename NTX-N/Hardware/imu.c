#include "stm32f10x.h" 
#include "Delay.h"
#include "math.h"
#include "MPU6050_Hard.h"

/*
ͨ����ת�������ɻ���̬==============================
*/

// ��¼�˴μ�������Ĳ���
float now = 0, lasttime = 0, dt = 0;				// ����΢��ʱ��
float ax_offset = 0, ay_offset = 0, az_offset;		// x��y����ٶ�ƫ����
float gx_offset = 0, gy_offset = 0, gz_offset;		// x��y����ٶ�ƫ����
float k_roll = 0, k_pitch = 0, k_yaw = 0;			// �������˲�����Ƴ����ŽǶȣ����Ź���״̬,��ʼ��0
float v_roll = 0, v_pitch = 0, v_yaw = 0;			// ���㵽x��y���ϵĽ��ٶ�
float g_roll = 0, g_pitch = 0, g_yaw = 0;			// �����ǻ��ּ�����ĽǶȣ�����״̬
float a_roll = 0, a_pitch = 0;						// ���ٶȼƹ۲���ĽǶȣ��۲�״̬
		
float rad2deg = 57.29578;							// ���ȵ��ǶȵĻ���ϵ��
		
float e_P[2][2] = {{1, 0}, {0, 1}};					// Э�������
float k_k[2][2] = {{0, 0}, {0, 0}};					// �������������

/* ����ƫ��������200��ƽ�� */
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
	ax_offset = ax_offset / 200;		// ����x����ٶȵ�ƫ����
	ay_offset = ay_offset / 200;		// ����y����ٶȵ�ƫ����
	gx_offset = gx_offset / 200;		// ����x����ٶȵ�ƫ����
	gy_offset = gy_offset / 200;		// ����y����ٶȵ�ƫ����
	Delay_ms(100);
}


/* ����Roll,Pitch,Yaw */
void Cal_RPY(int16_t AX, int16_t AY, int16_t AZ, int16_t GX, int16_t GY, int16_t GZ, uint32_t Clock, int16_t *Roll_R, int16_t *Pitch_R, int16_t *Yaw_R)
{
	now = Clock;										// ��ǰʱ�䣬��ȡ����ֵ�Ǻ���
	dt = (now - lasttime) / 1000000;					// ΢��ʱ��,s
	lasttime = now;
	
	/* step1:��������״̬ */
	/* ����x��y���ϵĽ��ٶ� */
	v_roll = (GX - gx_offset) + ((sin(k_pitch) * sin(k_roll)) / cos(k_pitch)) * (GY - gy_offset) + ((sin(k_pitch) * cos(k_roll)) / cos(k_pitch)) * GZ; // roll��Ľ��ٶ�
	v_pitch = cos(k_roll) * (GY - gy_offset) - sin(k_roll) * GZ;			// pitch��Ľ��ٶ�
//	v_yaw = (sin(k_roll) / cos(k_pitch)) * (GY - gy_offset) + (cos(k_roll) / cos(k_pitch)) * GZ; 	// yaw��Ľ��ٶ�
	g_roll = k_roll + dt * v_roll;					// ����֤roll�ĽǶ�
	g_pitch = k_pitch + dt * v_pitch;				// ����֤pitch�ĽǶ�
//	g_yaw = k_yaw + dt * v_yaw;						// ��Ϊû�д����ƣ��������������g_yaw�������յ�yaw�����ǲ�̫׼
	
	/* step2:�����������Э�������p */
	e_P[0][0] = e_P[0][0] + 0.0025;					// �����Q������һ���Խ����ҶԽ�Ԫ��Ϊ0.003
	e_P[0][1] = 0;
	e_P[1][0] = 0;
	e_P[1][1] = e_P[1][1] + 0.0025;
	
	/* setp3�����¿���������K* */
	k_k[0][0] = e_P[0][0] / (e_P[0][0] + 0.3);
	k_k[0][1] = 0;
	k_k[1][0] = 0;
	k_k[1][1] = e_P[1][1] / (e_P[1][1] + 0.3);
	
	/* setp4���������Ź���װ�� */
	/* �۲�״̬ */
	// roll,pitch �Ƕ�
	a_roll = atan2((AY - ay_offset) , AZ) * rad2deg;
	a_pitch = -atan((AX - ax_offset) / (sqrt((AY - ay_offset) * (AY - ay_offset) + AZ * AZ))) * rad2deg;
	
	/* ���Ž����״̬ */
	k_roll = k_roll + k_k[0][0] * (a_roll - g_roll);
	k_pitch = k_pitch + k_k[1][1] * (a_pitch - g_pitch);
//	k_yaw = k_yaw + k_k * g_yaw;				// ������Ҫ��e_P,k_kתΪ��ά���󼴿�ʵ��Ŀ�ģ�����û�գ��Ժ���ת
	
	/* step5:����Э�������p */
	e_P[0][0] = (1 - k_k[0][0]) * e_P[0][0];
	e_P[0][1] = 0;
	e_P[1][0] = 0;
	e_P[1][1] = (1- k_k[1][1]) * e_P[1][1];
	
	*Roll_R = k_roll;
	*Pitch_R = k_pitch;
//	*Yaw_R = k_yaw;
	// ���Ľ�� roll: k_roll�� pitch��k_p
}


