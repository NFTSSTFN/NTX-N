#include "stm32f10x.h"                  // Device header

uint16_t PowVal[1];		// (��Ҫ��)��
// ���˻�������⣬PA4

void Pow_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		// ��ʼ��ADC1ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		// ��ʼ��GPIOAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			// ��ʼ��DMA1ʱ��
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);							// ����6��Ƶ��72MHz / 6 = 12MHz��ÿ1usת��һ��
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	// (��Ҫ��)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);		// ����ADCͨ��1
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						// ���ö�������ģʽ��Ҳ����˵ֻʹ��һ��ADC
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					// ���ݶ���ģʽ���Ҷ���
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		// ���ô���ģʽΪ�޴�����Ҳ�����������
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						// ��������ת��
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;							// ����ɨ��ģʽ������ɨ��
	ADC_InitStructure.ADC_NbrOfChannel = 1;									// (��Ҫ��)ͨ���ﹲ��4������
	ADC_Init(ADC1, &ADC_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;					// �����ݴ�ADC1�ƶ���DR�Ĵ���
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		// ���ݴ�СΪ1���ֽ�
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// ���üĴ�����ַ������
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PowVal;						// ת�˺�����ݵ�ַ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				// ת�˺�����ݴ�С
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							// ת�˺�������ڴ��ַ����
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;								// ָ������ԴͷΪ����
	DMA_InitStructure.DMA_BufferSize = 1;											// (��Ҫ��)ת�˵����ݴ�С
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									// ����ת��ģʽΪ����ת��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									// ��Ϊ�����赽�Ĵ�����������DISABLE
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;							// ����ת�����ȼ�
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);									// ��ʼ��ͨ��1
	
	DMA_Cmd(DMA1_Channel1, ENABLE);				// ͨ��1ʹ��
	ADC_DMACmd(ADC1, ENABLE);					// ����ADC��DMAת������
	ADC_Cmd(ADC1, ENABLE);						// ADCʹ��
	
	ADC_ResetCalibration(ADC1);									// У׼ADC�Ĵ���
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);			// ��ȡ��ѡADC��λУ׼�Ĵ���״̬��
	ADC_StartCalibration(ADC1);									// ����ѡ��ADCУ׼����
	while (ADC_GetCalibrationStatus(ADC1) == SET);				// ��ȡADCУ׼��״̬
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);						// ����ADC���ת��
}


