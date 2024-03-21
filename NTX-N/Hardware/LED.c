#include "stm32f10x.h"                  // Device header
#include "Delay.h"

// 目前是PA0口点灯，蜂鸣器也可以通过这个高电平直接驱动

void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	// 设置推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_0);	// 默认初始化后给低电平，所以这里给高电平让其熄灭
}

void LED_ON_High(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

void LED_OFF_High(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}


void LED_ON_Low(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
}


void LED_OFF_Low(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
}

void LED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	}	
}

// 高电平点灯，闪烁
void LED_LightUp_High(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_0);	// 设置为高电平
	Delay_ms(100);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);	// 设置为低电平
	Delay_ms(100);
	
	GPIO_Write(GPIOA, 1);	// 设置为高电平
	Delay_ms(100);
	GPIO_Write(GPIOA, 0);	// 设置为低电平
	Delay_ms(100);
}

// 低电平点灯，闪烁
void LED_LightUp_Low(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);	// 设置为低电平
	Delay_ms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_0);	// 设置为高电平
	Delay_ms(100);
	
	GPIO_Write(GPIOA, 0);	// 设置为低电平
	Delay_ms(100);
	GPIO_Write(GPIOA, 1);	// 设置为高电平
	Delay_ms(100);
}

