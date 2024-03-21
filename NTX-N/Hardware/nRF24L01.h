#ifndef __NRF24L01_H
#define __NRF24L01_H

void NRF24L01_Init(void);

void NRF24L01_SendString(uint8_t Data[32]);
void NRF24L01_GetString(uint8_t Data[32]);

void NRF24L01_SendNum(uint32_t Num);
uint32_t NRF24L01_GetNum(void);

uint8_t NRF24L01_SendHex(uint8_t Data[32]);
uint8_t NRF24L01_GetHex(uint8_t Data[32]);

#endif
