#include "stm32f10x.h"                  // Device header
#include "nRF24L01_Ins.h"
#include "Delay.h"
#include "math.h"

#define TX_ADR_WIDTH	5		// 5字节地址宽度
#define RX_ADR_WIDTH	5		// 5字节地址宽度
#define TX_REG_WIDTH	32		// 32字节有效数据宽度
#define RX_REG_WIDTH	32		// 32字节有效数据宽度 

const uint8_t TX_ADDRESS[TX_ADR_WIDTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// 发射地址
const uint8_t RX_ADDRESS[TX_ADR_WIDTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF};	// 接收地址

void W_SS(uint8_t BitValue)
{
	GPIO_WriteBit(CSN_PORT, CSN_PIN, (BitAction)BitValue);
} 

void W_CE(uint8_t BitValue)
{
	GPIO_WriteBit(CE_PORT, CE_PIN, (BitAction)BitValue);
} 

void W_SCK(uint8_t BitValue)
{
	GPIO_WriteBit(SCK_PORT, SCK_PIN, (BitAction)BitValue);
} 

void W_MOSI(uint8_t BitValue)
{
	GPIO_WriteBit(MOSI_PORT, MOSI_PIN,(BitAction)BitValue);
} 

uint8_t R_MISO(void)
{
	return GPIO_ReadInputDataBit(MISO_PORT, MISO_PIN);
}

uint8_t R_IRQ(void)
{
	return GPIO_ReadInputDataBit(IRQ_PORT, IRQ_PIN);
}

// 初始化GPIO脚
void NRF24L01_Pin_Init(void)	
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CSN_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCK_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MOSI_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = CE_PIN;  
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(CE_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = MISO_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MISO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = IRQ_PIN;  			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IRQ_PORT, &GPIO_InitStructure);
}

// SPI寄存器数据交换
uint8_t SPI_SwapByte(uint8_t Byte)
{
	uint8_t i;
	for(i = 0; i < 8; i ++) 
	{
		if((uint8_t)(Byte & 0x80) == 0x80)
		{
			W_MOSI(1);
		} 		
		else 
		{
			W_MOSI(0);
		}			
		Byte = (Byte << 1);			
		W_SCK(1);					
		Byte |= R_MISO();	        
		W_SCK(0);					
	}
	return Byte;
}

// NRF24L01写寄存器，Reg--寄存器地址，Value--写入的数据，return--寄存器的地址
uint8_t NRF24L01_Write_Reg(uint8_t Reg, uint8_t Value)
{
	uint8_t Status;

	W_SS(0);                  
  	Status = SPI_SwapByte(Reg);	
	SPI_SwapByte(Value);	
	W_SS(1);                 

	return Status;
}

// NRF24L01读寄存器，Reg--寄存器地址，return--寄存器里读出的数据
uint8_t NRF24L01_Read_Reg(uint8_t Reg)
{
 	uint8_t Value;

	W_SS(0);              
  	SPI_SwapByte(Reg);		
	Value = SPI_SwapByte(NOP);
	W_SS(1);             	

	return Value;
}

// 一次读NRF24L01寄存器多个字节，Reg--寄存器地址，*Buf--寄存器字节读出后存储的数组，Len--要读出的字节个数
// return--读取的状态，
uint8_t NRF24L01_Read_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	W_SS(0);                    	
	Status =SPI_SwapByte(Reg);		
 	for(i = 0; i < Len; i ++)
	{
		Buf[i] = SPI_SwapByte(NOP);	
	}
	W_SS(1);                 		
	return Status;        			
}

// 一次写NRF24L01寄存器多个字节，Reg--寄存器地址，*Buf--寄存器字节写入后存储的数组，Len--要写入的字节个数
// return--读取的状态，
uint8_t NRF24L01_Write_Buf(uint8_t Reg, uint8_t *Buf, uint8_t Len)
{
	uint8_t Status, i;
	W_SS(0); 
	Status = SPI_SwapByte(Reg);		
	for(i = 0; i < Len; i ++)
	{
		SPI_SwapByte(*Buf ++); 		
	}
	W_SS(1); 
	return Status;          		
}	

// 读出接收到的数据，为多个字节，return--状态值，1--成功，0--失败
uint8_t NRF24L01_GetRxBuf(uint8_t *Buf)
{
	uint8_t State;
	State = NRF24L01_Read_Reg(STATUS);						// 读取STATUS寄存器第0个字节，1-已满，0-未满			
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State);		// 在STATUS寄存器写入state，相当于读出来再写进去
	if(State & RX_OK)								
	{                                                       
		W_CE(1);											
		NRF24L01_Read_Buf(READ_RX_DATA, Buf, RX_REG_WIDTH);	// 读出32字节有效数据
		NRF24L01_Write_Reg(CLEAR_RX_REG, NOP);				// 清除RX FIFO寄存器，应用于接收模式下
		W_CE(1);				
		Delay_us(150);
		return 1; 
	}	   
	return 0;
}

// 写入发射数据
uint8_t NRF24L01_SendTxBuf(uint8_t *Buf)
{
	uint8_t State;
   
	W_CE(0);												
  	NRF24L01_Write_Buf(WRITE_TX_DATA, Buf, TX_REG_WIDTH);		// 发射寄存器写入数据
 	W_CE(1);												
	while(R_IRQ() == 1);										// 读IRQ
	State = NRF24L01_Read_Reg(STATUS);  						// 读取STATUS第一字节，看看寄存器数据是否已满
	NRF24L01_Write_Reg(nRF_WRITE_REG + STATUS, State); 			// 把刚刚读出来的写进入
	if(State&MAX_TX)				 						
	{
		NRF24L01_Write_Reg(CLEAR_TX_REG, NOP);					// 如果达到最大发射/接收次数，清空中断
		return MAX_TX; 
	}
	if(State & TX_OK)	
	{
		return TX_OK;
	}
	return NOP;	
}

// 检查NRF24L01是否存在，存在返回0，失败返回1
uint8_t NRF24L01_Check(void)
{
	uint8_t check_in_buf[5] = {0x11 ,0x22, 0x33, 0x44, 0x55};
	uint8_t check_out_buf[5] = {0x00};

	W_SCK(0);
	W_SS(1);   
	W_CE(0);	

	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, check_in_buf, 5);

	NRF24L01_Read_Buf(nRF_READ_REG + TX_ADDR, check_out_buf, 5);

	if((check_out_buf[0] == 0x11) && (check_out_buf[1] == 0x22) && (check_out_buf[2] == 0x33) && (check_out_buf[3] == 0x44) && (check_out_buf[4] == 0x55))
	{
		return 0;
	}
	else 
	{
		return 1;
	}
}	

// 初始化收发配置
void NRF24L01_RT_Init(void) 
{	
	W_CE(0);		  
  	NRF24L01_Write_Reg(nRF_WRITE_REG + RX_PW_P0, RX_REG_WIDTH);		// 数据通道0，32字节
	NRF24L01_Write_Reg(CLEAR_RX_REG, NOP);							// 清除FIFO寄存器
  	NRF24L01_Write_Buf(nRF_WRITE_REG + TX_ADDR, (uint8_t*)TX_ADDRESS, TX_ADR_WIDTH);	// 写入发射地址，32字节
  	NRF24L01_Write_Buf(nRF_WRITE_REG + RX_ADDR_P0, (uint8_t*)RX_ADDRESS, RX_ADR_WIDTH); // 写入接收地址，32字节
  	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_AA, 0x01);     			// 数据通道0自动应答允许
  	NRF24L01_Write_Reg(nRF_WRITE_REG + EN_RXADDR, 0x01); 			// 接收数据通道0允许
  	NRF24L01_Write_Reg(nRF_WRITE_REG + SETUP_RETR, 0x1A);			// 自动重发，等待500+86us，自动重发10次
  	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_CH, 0);        			// 射频通道默认为0
  	NRF24L01_Write_Reg(nRF_WRITE_REG + RF_SETUP, 0x0F);  			// 设置发射的噪声放大器、功率、传输速率
  	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);    			// 设置为接收模式、上电、16位CRC校验、使能
	W_CE(1);									  
}

// 初始化NRF24L01模块
void NRF24L01_Init()
{
	NRF24L01_Pin_Init();		// 初始化针脚
	while(NRF24L01_Check());	// 检查设备是否存在
	NRF24L01_RT_Init();			// 默认设置为接收模式，如果需要发射的话，得更改一下
	
}

// 发送多个字节的数据，是对之前函数的再度封装
uint8_t NRF24L01_SendBuf(uint8_t *Buf)
{
	W_CE(0);									
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0E);   	// 更改接收模式为发射模式
	W_CE(1);
	Delay_us(15);											// 这个至少10ms，为了激发NRF24L01进行Enhanced ShockBurstTM发射
	uint8_t status = NRF24L01_SendTxBuf(Buf);								// 发送数据
	W_CE(0);
	NRF24L01_Write_Reg(nRF_WRITE_REG + CONFIG, 0x0F);		// 更改发射模式为接收模式
	W_CE(1);
	return status;
}

// 获取中断标志，一旦获取到数据，就返回0，获取到了就返回1
uint8_t NRF24L01_Get_Value_Flag()
{
	return R_IRQ();
}

// 发送数字,最大2**32
void NRF24L01_SendNum(uint32_t Num)
{
	uint8_t SendBuf[32] = {0};
	for (uint8_t i = 0; i < 32; i ++)							
	{
		SendBuf[i] = Num / (uint32_t)pow(10, 31 - i) % 10;
	}
	
	NRF24L01_SendBuf(SendBuf);
}

// 获取数字，最大2**32
uint32_t NRF24L01_GetNum(void)
{
	uint32_t Num = 0;
	uint8_t Buf[32] = {0};
	NRF24L01_GetRxBuf(Buf);
	for (uint16_t i = 0; i < 32; i ++)
	{
		Num += Buf[i] * pow(10, 31 - i);
	}
	
	return Num;
}

// 发送字符串，最多32位
void NRF24L01_SendString(uint8_t Data[32])
{
	uint8_t Psign[32] = {0};
	uint8_t SendBuf[33] = {0};
	uint8_t TSign = 0;
	for (uint8_t i = 0; i < 32; i ++)
	{
		if (Data[i] != 0)
		{
			Psign[TSign] = i;
			TSign ++;
		}
	}
	if (TSign > 0)
	{
		TSign --;
	}
	SendBuf[0] = Psign[TSign] + 1;				// 字符串总长度 + 中止标志位
	for (uint8_t i = 0; i < SendBuf[0]; i ++)
	{
		SendBuf[i + 1] = Data[i];
	}
	NRF24L01_SendBuf(SendBuf);
}

// 接收字符串，最多32位
void NRF24L01_GetString(uint8_t Data[32])
{
	uint8_t RxBuf[33];				// 临时接收数组
	NRF24L01_GetRxBuf(RxBuf);
	for (uint16_t i = 0; i < 32; i ++)
	{
		Data[i] = RxBuf[i + 1];
	}
}	


// 发送十六进制数据,必须为32个字节的数组
uint8_t NRF24L01_SendHex(uint8_t Data[32])
{
	uint8_t status = NRF24L01_SendBuf(&Data[0]);
	return status;
}

// 接收十六进制数据，必须为32个字节的数组，1--成功，0--失败
uint8_t NRF24L01_GetHex(uint8_t Data[32])
{
	uint8_t status = NRF24L01_GetRxBuf(&Data[0]);
	return status;
}
