#ifndef __CONTROL_H
#define __CONTROL_H

// 手柄控制模块
// 芯片      ---- STM32F103C8T6
// 控制杆    ---- PS2
// 通讯		 ---- NRF24L01，距离180-240（未实测）
// 电源      ---- 充电宝替代
// OLED      ---- 0.95英寸
// 电量检测  ---- 暂无
// 开关	     ---- 暂无
// 稳压      ---- 暂无

void Control_Init(void);

#endif
