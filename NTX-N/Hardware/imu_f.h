#ifndef __IMU_F_H
#define	__IMU_F_H

#ifndef KALMAN_NEW_H
#define KALMAN_NEW_H

#define squa( Sq )        (((float)Sq)*((float)Sq))

void Cal_f(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz, int16_t clock, int16_t* roll, int16_t* pitch, int16_t* yaw);

#endif
#endif 

