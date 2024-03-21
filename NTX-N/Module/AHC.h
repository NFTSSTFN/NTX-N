#ifndef AHC_H
#define AHC_H

void AHC_Send_AG_Data(int16_t AccX, int16_t AccY, int16_t AccZ,
					 int16_t GyroX, int16_t GyroY, int16_t GyroZ);
void AHC_Send_EA_Data(int16_t Roll, int16_t Pitch, int16_t Yaw, uint8_t Fusion_STA);
void AHC_Send_QA_Data(int16_t q0, int16_t q1, int16_t q2, int16_t q3, uint8_t Fusion_STA);
#endif

