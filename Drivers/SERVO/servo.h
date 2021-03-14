/*
 * servo.h
 *
 *  Created on: Nov 20, 2020
 *      Author: mybot
 */

#ifndef SERVO_SERVO_H_
#define SERVO_SERVO_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stm32f4xx_hal.h>
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

void servo_init();
//void read_servo(float* velocity,int wheel);
void write_servo(float velocity,float pre_velocity,int wheel);
                    /**
  * Initializes the Global MSP.
  */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */


#ifdef __cplusplus
extern "C" {
#endif
#endif /* SERVO_SERVO_H_ */
