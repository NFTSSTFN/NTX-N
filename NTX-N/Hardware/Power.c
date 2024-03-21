#include "stm32f10x.h"                  // Device header

uint16_t PowVal[1];		// (需要改)，
// 无人机电量检测，PA4

void Pow_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);		// 初始化ADC1时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		// 初始化GPIOA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);			// 初始化DMA1时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);							// 设置6分频，72MHz / 6 = 12MHz，每1us转运一次
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	// (需要改)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);		// 设置ADC通道1
		
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;						// 设置独立工作模式，也就是说只使用一个ADC
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;					// 数据对齐模式，右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;		// 设置触发模式为无触发，也就是软件触发
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;						// 开启连续转换
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;							// 设置扫描模式，连续扫描
	ADC_InitStructure.ADC_NbrOfChannel = 1;									// (需要改)通道里共有4个数据
	ADC_Init(ADC1, &ADC_InitStructure);
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;					// 将数据从ADC1移动到DR寄存器
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;		// 数据大小为1个字节
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;				// 设置寄存器地址不自增
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)PowVal;						// 转运后的数据地址
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;				// 转运后的数据大小
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;							// 转运后的数据内存地址自增
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;								// 指定数据源头为外设
	DMA_InitStructure.DMA_BufferSize = 1;											// (需要改)转运的数据大小
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;									// 设置转运模式为持续转运
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;									// 因为是外设到寄存器，所以是DISABLE
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;							// 设置转运优先级
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);									// 初始化通道1
	
	DMA_Cmd(DMA1_Channel1, ENABLE);				// 通道1使能
	ADC_DMACmd(ADC1, ENABLE);					// 启用ADC、DMA转运数据
	ADC_Cmd(ADC1, ENABLE);						// ADC使能
	
	ADC_ResetCalibration(ADC1);									// 校准ADC寄存器
	while (ADC_GetResetCalibrationStatus(ADC1) == SET);			// 获取所选ADC复位校准寄存器状态。
	ADC_StartCalibration(ADC1);									// 启用选定ADC校准过程
	while (ADC_GetCalibrationStatus(ADC1) == SET);				// 获取ADC校准的状态
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);						// 启动ADC软件转换
}


