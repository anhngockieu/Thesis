/*
 * imu.h
 *
 *  Created on: Nov 18, 2020
 *      Author: anhngockieu
 */

#ifndef MPU6050_IMU_H_
#define MPU6050_IMU_H_
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "mpu6050.h"
#include "madgwick.h"


void Imu_init();
void Read_imu(float* roll, float* pitch, float* yaw);
void Get_acc(float* x, float* y, float* z);
void Get_gyro(float* x, float* y, float* z);
void Get_quat(float* x, float* y, float* z, float* t);

#ifdef __cplusplus
}
#endif

#endif /* MPU6050_IMU_H_ */

