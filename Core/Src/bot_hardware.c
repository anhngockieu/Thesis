/*
 * bot_hardware.c
 *
 *  Created on: Dec 20, 2020
 *      Author: mybot
 */

#include "bot_hardware.h"
#include "servo.h"
#include "imu.h"
#include "FreeRTOS.h"
#include "task.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;
extern I2C_HandleTypeDef hi2c1;
extern MPU6050_t MPU6050;
void robot_motor_init(void)
{
	servo_init();
	//write_servo(0.5,0,0);
	//write_servo(0.5,0,1);

	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
}


void robot_imu_init(void)
{
	Imu_init();
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
}


void robot_imu_get_accel(float accel[3])
{

	Get_acc(&accel[0],&accel[1],&accel[2]);


}


void robot_imu_get_gyro(float gyro[3])
{
	Get_gyro(&gyro[0],&gyro[1],&gyro[2]);
}


void robot_imu_get_quat(float quaternion[4])
{
	Get_quat(&quaternion[0],&quaternion[1],&quaternion[2],&quaternion[3]);
}


void robot_motor_left_start(void)
{
	write_servo(0.8,0,0);
}


void robot_motor_left_stop(void)
{
	write_servo(0,0,0);
}


void robot_motor_left_set_speed(float speed,float pre_speed)
{
	write_servo(speed,pre_speed,0);
}


void robot_motor_right_start(void)
{
	write_servo(0.8,0,1);
}


void robot_motor_right_stop(void)
{
	write_servo(0,0,1);
}


void robot_motor_right_set_speed(float speed,float pre_speed)
{
	write_servo(speed,pre_speed,1);
}


void robot_encoder_left_get_tick(int32_t *left_tick)
{
	*left_tick = (int32_t)(htim5.Instance->CNT);
	//__HAL_TIM_GET_COUNTER(&htim2) = 0;
	//htim2.Instance->CNT &= 0x00000000;
	//*left_tick = 0;
}


void robot_encoder_right_get_tick(int32_t *right_tick)
{

	*right_tick = (int32_t)(htim2.Instance->CNT);
	//*right_tick = (int32_t)(htim5.Instance->CNT) ;
	//htim5.Instance->CNT &= 0x00000000;
	//__HAL_TIM_GET_COUNTER(&htim5) = 0;

}


uint32_t millis(void)
{
    return xTaskGetTickCount();
	//return HAL_GetTick();
}

float constrain(float x, float low_val, float high_val)
{
    float value;
    if (x > high_val)
    {
        value = high_val;
    }
    else if (x < low_val)
    {
        value = low_val;
    }
    else
    {
        value = x;
    }
    return value;
}
