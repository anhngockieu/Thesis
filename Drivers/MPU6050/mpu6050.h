/*
 * mpu6050.h
 *
 *  Created on: Nov 17, 2020
 *      Author: mybot
 */

#ifndef MPU6050_MPU6050_H_
#define MPU6050_MPU6050_H_
#ifdef __cplusplus
 extern "C" {
#endif
/*
 * mpu6050.h
 *
 *  Created on: Nov 17, 2020
 *      Author: mybot
 */


#include <stdint.h>
#include <stm32f4xx_hal.h>

// MPU6050 structure
typedef struct {

    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;

    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;

    float Temperature;

    double KalmanAngleX;
    double KalmanAngleY;
} MPU6050_t;
typedef struct {
    double ACCX;
    double ACCY;
    double ACCZ;
    double GYROX;
    double GYROY;
    double GYROZ	;
} OFFSET_t;

// Kalman structure
typedef struct {
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;


uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx);

void MPU6050_Read_Accel(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

void MPU6050_Read_Gyro(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

void MPU6050_Read_Temp(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

void Calibration(I2C_HandleTypeDef *I2Cx);

void MPU6050_Read_All(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

double Kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);

void MPU6050_Read(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);

#ifdef __cplusplus
}
#endif
#endif /* MPU6050_MPU6050_H_ */
