/*
 * bot_hardware.h
 *
 *  Created on: Dec 18, 2020
 *      Author: mybot
 */

#ifndef INC_BOT_HARDWARE_H_
#define INC_BOT_HARDWARE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <imu.h>
#include <mpu6050.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

#define PI                  3.14159265359

/* Convert constant */
#define DEG2RAD(x)      (x * PI / 180.0f)     /*!< Convert from degree to radian (PI/180) */
#define RAD2DEG(x)      (x * 180.0f / PI)     /*!< convert from radian to degree (180/PI) */
#define TICK2RAD		2.0f * PI / 660.0f /2.0f
//#define TICK2M			2 *PI * 0.033 / 660.0f / 2.0f

/* Servo motor direction index */
#define MOTORLEFT_DIR_FORWARD       0
#define MOTORLEFT_DIR_BACKWARD      1
#define MOTORRIGHT_DIR_FORWARD      1
#define MOTORRIGHT_DIR_BACKWARD     0

/* Robot parameters */
#define WHEEL_RADIUS                0.033                                   /*!< Wheel radius in meter */
#define WHEEL_SEPARATION            0.165                                   /*!< Wheel separate distance in meter */
#define TURNING_RADIUS              0.08                                    /*!< Turning radius in degree */
#define ROBOT_RADIUS                0.1                                     /*!< Robot radius in meter    */
#define MAX_LINEAR_VELOCITY         (WHEEL_RADIUS * 2 * PI * 250 / 60)       /*!< Max linear velocity */
#define MAX_ANGULAR_VELOCITY        (MAX_LINEAR_VELOCITY / TURNING_RADIUS)  /*!< Max angular velocity */
#define MIN_LINEAR_VELOCITY         -MAX_LINEAR_VELOCITY                    /*!< Min linear velocity */
#define MIN_ANGULAR_VELOCITY        -MAX_ANGULAR_VELOCITY                   /*!< Min angular velocity */

#define MADGWICK_BETA                       0.1f
#define MADGWICK_SAMPLE_FREQ                10.0f

void robot_motor_init(void) ;
void robot_imu_init(void);
void robot_imu_get_accel(float accel[3]);
void robot_imu_get_gyro(float gyro[3]);
void robot_imu_get_quat(float quaternion[4]);
void robot_motor_left_start(void);
void robot_motor_left_stop(void);
void robot_motor_left_set_speed(float speed,float pre_speed);
void robot_motor_right_start(void);
void robot_motor_right_stop(void);
void robot_motor_right_set_speed(float speed,float pre_speed);

void robot_encoder_left_get_tick(int32_t *left_tick);
void robot_encoder_right_get_tick(int32_t *right_tick);
uint32_t millis(void);
float constrain(float x, float low_val, float high_val);



#ifdef __cplusplus
}
#endif
#endif /* INC_BOT_HARDWARE_H_ */
