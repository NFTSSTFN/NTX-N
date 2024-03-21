#include "stm32f10x.h"                  // Device header

/*
一些常用的方法技巧
TIM_GetCounter(TIM2)	可以查看计数器的值
*/


/* 初始化TIM1通用定时器，目前0.001s计数一次 */
void Timer1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	// 开启TIM1时钟，位于APB1总线
	
	TIM_InternalClockConfig(TIM1);				// 配置TIM1时钟
	
	/*  
	定时频率 = 72M / (PSC +1) / (ARR + 1)
	定时1s，就是定时频率为1Hz，72M / 7200 / 10000
	定时0.1s，72M / 720 / 10000 或者 72M / 7200 / 1000，就是计数频率比之前短了十倍，所以0.1s就会进一次中断
	因为PSC和ARR都有1个数的偏差，所以要减去1
	PSC和ARR的取值都要在0-65535之间，不能超范围
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// 指定时钟分频，这个可以随便配，这里选择不分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// 配置计数器模式，选择向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 1000 - 1;					// ARR自动重装器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 72 - 1;				// PSC预分频器的值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// 重复计数器的值
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);				// 时基单元配置函数
	
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);							// 因为程序一上电，中断标志位为1，所以会立刻进中断，这里手动清除一下，就解决了
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);						// 使能中断，选择更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);					// NVIC分组，选择分组0
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;				// TIM1在NVIC里的通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// 配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// 配置响应优先级
	NVIC_Init(&NVIC_InitStructure);									// NVIC配置函数，打通中断通道
	
	TIM_Cmd(TIM1, ENABLE);											// 启动TIM1定时器
}

/* 初始化TIM2通用定时器，目前1s计数一次 */
void Timer2_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	// 开启TIM2时钟，位于APB1总线
	
	TIM_InternalClockConfig(TIM2);				// 配置TIM2时钟，选择内部时钟，如果不屑，默认也使用内部时钟
	
	/*  
	定时频率 = 72M / (PSC +1) / (ARR + 1)
	定时1s，就是定时频率为1Hz，72M / 7200 / 10000
	定时0.1s，72M / 720 / 10000 或者 72M / 7200 / 1000，就是计数频率比之前短了十倍，所以0.1s就会进一次中断
	因为PSC和ARR都有1个数的偏差，所以要减去1
	PSC和ARR的取值都要在0-65535之间，不能超范围
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// 指定时钟分频，这个可以随便配，这里选择不分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// 配置计数器模式，选择向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;					// ARR自动重装器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;				// PSC预分频器的值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// 重复计数器的值
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);				// 时基单元配置函数
	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);							// 因为程序一上电，中断标志位为1，所以会立刻进中断，这里手动清除一下，就解决了
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);						// 使能中断，选择更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// NVIC分组，选择分组2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					// TIM2在NVIC里的通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// 配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// 配置响应优先级
	NVIC_Init(&NVIC_InitStructure);									// NVIC配置函数，打通中断通道
	
	TIM_Cmd(TIM2, ENABLE);											// 启动TIM2定时器
}

/* 初始化TIM1通用定时器，目前1s计数一次 */
void Timer3_Init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	// 开启TIM3时钟，位于APB1总线
	
	TIM_InternalClockConfig(TIM3);							// 配置TIM3时钟
	
	/*  
	定时频率 = 72M / (PSC +1) / (ARR + 1)
	定时1s，就是定时频率为1Hz，72M / 7200 / 10000
	定时0.1s，72M / 720 / 10000 或者 72M / 7200 / 1000，就是计数频率比之前短了十倍，所以0.1s就会进一次中断
	因为PSC和ARR都有1个数的偏差，所以要减去1
	PSC和ARR的取值都要在0-65535之间，不能超范围
	*/
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		// 指定时钟分频，这个可以随便配，这里选择不分频
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	// 配置计数器模式，选择向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 10000 - 1;					// ARR自动重装器的值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 - 1;				// PSC预分频器的值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				// 重复计数器的值
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStruct);				// 时基单元配置函数
	
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);							// 因为程序一上电，中断标志位为1，所以会立刻进中断，这里手动清除一下，就解决了
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);						// 使能中断，选择更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// NVIC分组，选择分组2
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;					// TIM3在NVIC里的通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 通道使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// 配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// 配置响应优先级
	NVIC_Init(&NVIC_InitStructure);									// NVIC配置函数，打通中断通道
	
	TIM_Cmd(TIM3, ENABLE);											// 启动TIM3定时器
}


/* 
在startup_stm32f10x_md.s找到TIM2的中断函数 
因为中断里需要执行一些变量操作，所以把它放进主函数里，方便操作
*/ 

/*
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)		// 获取中断标志位
	{
		Clock++;
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);			// 清除中断标志位
	}
}
*/

/*
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)		// 获取中断标志位
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);			// 清除中断标志位
	}
}
*/

/*
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)		// 获取中断标志位
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);			// 清除中断标志位
	}
}
*/


