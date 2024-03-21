#include "stm32f10x.h"                  // Device header

/*
һЩ���õķ�������
TIM_GetCounter(TIM2)	���Բ鿴��������ֵ
*/


/* ��ʼ��TIM1ͨ�ö�ʱ����Ŀǰ0.001s����һ�� */
void Timer1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	// ����TIM1ʱ�ӣ�λ��APB1����
	
	TIM_InternalClockConfig(TIM1);				// ����TIM1ʱ��
	
	/*  
	��ʱƵ�� = 72M / (PSC +1) / (ARR + 1)
	��ʱ1s�����Ƕ�ʱƵ��Ϊ1Hz��72M / 7200 / 10000
	��ʱ0.1s��72M / 720 / 10000 ���� 72M / 7200 / 1000�����Ǽ���Ƶ�ʱ�֮ǰ����ʮ��������0.1s�ͻ��һ���ж�
	��ΪPSC��ARR����1������ƫ�����Ҫ��ȥ1
	PSC��ARR��ȡֵ��Ҫ��0-65535֮�䣬���ܳ���Χ
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// ָ��ʱ�ӷ�Ƶ�������������䣬����ѡ�񲻷�Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// ���ü�����ģʽ��ѡ�����ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;					// ARR�Զ���װ����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;				// PSCԤ��Ƶ����ֵ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// �ظ���������ֵ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);				// ʱ����Ԫ���ú���
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);							// ��Ϊ����һ�ϵ磬�жϱ�־λΪ1�����Ի����̽��жϣ������ֶ����һ�£��ͽ����
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);						// ʹ���жϣ�ѡ������ж�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);					// NVIC���飬ѡ�����0
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;				// TIM1��NVIC���ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// ������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);									// NVIC���ú�������ͨ�ж�ͨ��
	
	TIM_Cmd(TIM1, ENABLE);											// ����TIM1��ʱ��
}

/* ��ʼ��TIM2ͨ�ö�ʱ����Ŀǰ1s����һ�� */
void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	// ����TIM2ʱ�ӣ�λ��APB1����
	
	TIM_InternalClockConfig(TIM2);				// ����TIM2ʱ�ӣ�ѡ���ڲ�ʱ�ӣ������м��Ĭ��Ҳʹ���ڲ�ʱ��
	
	/*  
	��ʱƵ�� = 72M / (PSC +1) / (ARR + 1)
	��ʱ1s�����Ƕ�ʱƵ��Ϊ1Hz��72M / 7200 / 10000
	��ʱ0.1s��72M / 720 / 10000 ���� 72M / 7200 / 1000�����Ǽ���Ƶ�ʱ�֮ǰ����ʮ��������0.1s�ͻ��һ���ж�
	��ΪPSC��ARR����1������ƫ�����Ҫ��ȥ1
	PSC��ARR��ȡֵ��Ҫ��0-65535֮�䣬���ܳ���Χ
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// ָ��ʱ�ӷ�Ƶ�������������䣬����ѡ�񲻷�Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// ���ü�����ģʽ��ѡ�����ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;					// ARR�Զ���װ����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;				// PSCԤ��Ƶ����ֵ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// �ظ���������ֵ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);				// ʱ����Ԫ���ú���
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);							// ��Ϊ����һ�ϵ磬�жϱ�־λΪ1�����Ի����̽��жϣ������ֶ����һ�£��ͽ����
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);						// ʹ���жϣ�ѡ������ж�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// NVIC���飬ѡ�����2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					// TIM2��NVIC���ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// ������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);									// NVIC���ú�������ͨ�ж�ͨ��
	
	TIM_Cmd(TIM2, ENABLE);											// ����TIM2��ʱ��
}

/* ��ʼ��TIM1ͨ�ö�ʱ����Ŀǰ1s����һ�� */
void Timer3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// ����TIM3ʱ�ӣ�λ��APB1����
	
	TIM_InternalClockConfig(TIM3);							// ����TIM3ʱ��
	
	/*  
	��ʱƵ�� = 72M / (PSC +1) / (ARR + 1)
	��ʱ1s�����Ƕ�ʱƵ��Ϊ1Hz��72M / 7200 / 10000
	��ʱ0.1s��72M / 720 / 10000 ���� 72M / 7200 / 1000�����Ǽ���Ƶ�ʱ�֮ǰ����ʮ��������0.1s�ͻ��һ���ж�
	��ΪPSC��ARR����1������ƫ�����Ҫ��ȥ1
	PSC��ARR��ȡֵ��Ҫ��0-65535֮�䣬���ܳ���Χ
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// ָ��ʱ�ӷ�Ƶ�������������䣬����ѡ�񲻷�Ƶ
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// ���ü�����ģʽ��ѡ�����ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;					// ARR�Զ���װ����ֵ
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;				// PSCԤ��Ƶ����ֵ
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// �ظ���������ֵ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);				// ʱ����Ԫ���ú���
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);							// ��Ϊ����һ�ϵ磬�жϱ�־λΪ1�����Ի����̽��жϣ������ֶ����һ�£��ͽ����
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);						// ʹ���жϣ�ѡ������ж�
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// NVIC���飬ѡ�����2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;					// TIM3��NVIC���ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// ͨ��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// ������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);									// NVIC���ú�������ͨ�ж�ͨ��
	
	TIM_Cmd(TIM3, ENABLE);											// ����TIM3��ʱ��
}


/* 
��startup_stm32f10x_md.s�ҵ�TIM2���жϺ��� 
��Ϊ�ж�����Ҫִ��һЩ�������������԰����Ž���������������
*/ 

/*
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)		// ��ȡ�жϱ�־λ
	{
		Clock++;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);			// ����жϱ�־λ
	}
}
*/

/*
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		// ��ȡ�жϱ�־λ
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			// ����жϱ�־λ
	}
}
*/

/*
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		// ��ȡ�жϱ�־λ
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			// ����жϱ�־λ
	}
}
*/


