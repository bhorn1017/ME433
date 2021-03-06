#ifndef IMU_H__
#define IMU_H__

#include <xc.h>

#define IMU_WHOAMI 0x0F
#define IMU_ADDR 0b1101010
#define IMU_CTRL1_XL 0x10
#define IMU_CTRL2_G 0x11
#define IMU_CTRL3_C 0x12
#define IMU_OUT_TEMP_L 0x20

void imu_setup();

void xbar(signed short);
void ybar(signed short);

#endif
