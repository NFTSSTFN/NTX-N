#ifndef __NRF24L01_INS_H
#define __NRF24L01_INS_H

/*-------- NRF24L01引脚定义 --------*/
#define IRQ_PORT	GPIOB		// GPIOB_Pin_1
#define CE_PORT		GPIOB		// GPIOB_Pin_0
#define CSN_PORT	GPIOA		// GPIOA_Pin_5
#define SCK_PORT	GPIOA		// GPIOA_Pin_6
#define MOSI_PORT	GPIOB		// GPIOB_Pin_12
#define MISO_PORT	GPIOA		// GPIOA_Pin_7

#define IRQ_PIN		GPIO_Pin_1
#define CE_PIN		GPIO_Pin_0
#define CSN_PIN		GPIO_Pin_5
#define SCK_PIN		GPIO_Pin_6
#define MOSI_PIN	GPIO_Pin_12
#define MISO_PIN	GPIO_Pin_7

/*-------- NRF24L01寄存器指令 --------*/
#define nRF_READ_REG	0x00	// 读配置寄存器
#define nRF_WRITE_REG	0x20	// 写配置寄存器
#define READ_RX_DATA	0x61	// 读RX有效数据，1-32字节，读操作从0字节开始，读完后FIFO寄存器有效数据被清除
#define WRITE_TX_DATA	0xA0	// 写TX有效数据，1-32字节，写操作从0字节开始
#define CLEAR_TX_REG	0xE1	// 清除TX FIFO寄存器，应用于发射模式下
#define CLEAR_RX_REG	0xE2	// 清除RX FIFO寄存器，应用于接收模式下
#define REUSE_TX_DATA	0xE3	// 应用于发射端，重新使用上一包发射的有效数据，当CE=1，数据不断被重新发射，再发射数据包过程中必须禁止数据包重利用功能
#define NOP				0xFF	// 空操作，用来读状态寄存器

/*-------- 寄存器内容及说明 --------*/
#define CONFIG          0x00    // 配置寄存器                      
#define EN_AA           0x01  	// 使能自动应答功能，禁止后可以nRF24L01通讯
#define EN_RXADDR       0x02  	// 接收地址允许
#define SETUP_AW        0x03	// 设置地址宽度（所有数据通道）
#define SETUP_RETR      0x04  	// 建立自动重发
#define RF_CH           0x05  	// 射频通道
#define RF_SETUP        0x06  	// 射频寄存器
#define STATUS          0x07  	// 状态寄存器
#define OBSERVE_TX      0x08  	// 发送检测寄存器
#define CD              0x09  	// CD
#define RX_ADDR_P0      0x0A  	// 数据通道0接收地址
#define RX_ADDR_P1      0x0B  
#define RX_ADDR_P2      0x0C  
#define RX_ADDR_P3      0x0D  
#define RX_ADDR_P4      0x0E  
#define RX_ADDR_P5      0x0F  
#define TX_ADDR         0x10  
#define RX_PW_P0        0x11   
#define RX_PW_P1        0x12   
#define RX_PW_P2        0x13   
#define RX_PW_P3        0x14   
#define RX_PW_P4        0x15  
#define RX_PW_P5        0x16  
#define FIFO_STATUS     0x17  	// FIFO状态寄存器

/*-------- STATUS寄存器定义 --------*/
#define MAX_TX			0x10	// 达到最大收发次数，清除中断，这样才能继续进通讯
#define TX_OK			0x20	// 数据发送完成中断，发送完成此位置1，写1清除中断
#define RX_OK			0x40	// 接收数据完成中断，收到有效数据包后置1，写1清除中断

#endif

