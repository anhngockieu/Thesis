/*
 * servo.c
 *
 *  Created on: Nov 20, 2020
 *      Author: mybot
 */
#include "servo.h"
#include <math.h>


#define KP   20.0f
#define KI   50.0f
#define KD   2.0f
#define PI                  3.14159265359f
#define WHEEL_RADIUS                0.033f
#define SAMPLE_TIME 				100.0f
//#define scale_x					(660.0f * 2.0f) * SAMPLE_TIME/(2.0f * PI *WHEEL_RADIUS * 1000)
//#define scale_pre_x				(660.0f * 2.0f) / (2.0f * PI)

#define MIN_PWM					0.0f
#define RADS2RPM				60.0f / 2.0f / PI
#define MS2RPM 					60.0f / 2.0f / PI /0.033
#define RADS2MS					WHEEL_RADIUS
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim5;


//khai bao bien
int i,j=0;
volatile short encoder_cnt_l =0, encoder_cnt_pre_l = 0 , encoder_cnt_r =0, encoder_cnt_pre_r = 0;
float temp_l = 0, temp_r = 0 ,rate_l = 0,rate_r = 0, deg_l =0; deg_r =0 ;
float out_left = 0,out_right = 0;
float value,Kp,Ki,Kd;

float static error_l, pre_error_l, pre2_error_l, out_l , pre_out_l   , Kp_part_l, Ki_part_l, Kd_part_l, Ts=SAMPLE_TIME/1000.0;
float static error_r, pre_error_r, pre2_error_r, out_r, pre_out_r , Kp_part_r, Ki_part_r, Kd_part_r;

float PID_l( float setpoint_l,float current_l,float Kp,float Ki, float Kd)
		{

			pre2_error_l = pre_error_l;
			pre_error_l = error_l;
			error_l = setpoint_l - current_l;
			pre_out_l = out_l;

			Kp_part_l = Kp * ( error_l - pre_error_l );
			Ki_part_l = 0.5 * Ki * Ts *( error_l + pre_error_l );
			Kd_part_l = Kd / Ts * ( error_l - 2*pre_error_l + pre2_error_l);
			out_l = pre_out_l + Kp_part_l + Ki_part_l + Kd_part_l ;
			if ( out_l >16799)
				{
					out_l =16799;
				}
			else if ( out_l <-16799)
			{
					out_l = -16799;
				}
//			else if(abs(out_l)<MIN_PWM)
//						{
//							if (setpoint_l >0)
//							out_l = MIN_PWM;
//							else
//								out_l =-MIN_PWM;
//
//						}
			return out_l;
		}
float PID_r( float setpoint_r,float current_r,float Kp,float Ki, float Kd)
		{

			pre2_error_r = pre_error_r;
			pre_error_r = error_r;
			error_r = setpoint_r - current_r;
			pre_out_r = out_r;

			Kp_part_r = Kp * ( error_r - pre_error_r );
			Ki_part_r = 0.5 * Ki * Ts *( error_r + pre_error_r );
			Kd_part_r = Kd / Ts * ( error_r - 2*pre_error_r + pre2_error_r);
			out_r = pre_out_r + Kp_part_r + Ki_part_r + Kd_part_r ;
			if ( out_r >16799)
				{
					out_r =16799;
				}
			else if ( out_r <-16799)
				{
					out_r = -16799;
				}
//			else if(abs(out_r)<MIN_PWM)
//									{
//										if (setpoint_r >0)
//										out_r = MIN_PWM;
//										else
//											out_r =-MIN_PWM;
//
//									}
			return out_r;
		}

// Cap xung PWM cho dong co
void run_l(float  out)
	{
		out = out*1;
		if (out > 0)
			{
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,out);

				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);


				}
		else if (out <0)
			{
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,-out);

				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET);

				}
		return;
  }
void run_r(float  out)
	{
		out = out*1;
		if (out > 0)
			{
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,out);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_RESET);


				}
		else if (out <0)
			{
			__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,-out);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);
				}
		return;
  }


//void read_servo(float* velocity_current, int wheel)
//	{
//		switch (wheel){
//		case 0 :
//			encoder_cnt_l = __HAL_TIM_GET_COUNTER(&htim2);
//			deg_l = encoder_cnt_l * 360 /660;
//			temp_l = (encoder_cnt_l - encoder_cnt_pre_l);
//			*velocity_current = temp_l *scale_v ;
//			encoder_cnt_pre_l = encoder_cnt_l;
//			break;
//		case 1:
//			encoder_cnt_r = __HAL_TIM_GET_COUNTER(&htim5);
//			deg_r = encoder_cnt_r * 360 /660;
//			temp_r = (encoder_cnt_r - encoder_cnt_pre_r);
//			*velocity_current = temp_r *scale_v  ;
//			encoder_cnt_pre_r = encoder_cnt_r;
//			break;
//		default:
//			break;
//		}
//	}

void servo_init()
{

	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2 );
	HAL_TIM_Encoder_Start(&htim2,TIM_CHANNEL_1 | TIM_CHANNEL_2);
	HAL_TIM_Encoder_Start(&htim5,TIM_CHANNEL_1 | TIM_CHANNEL_2);
}

void write_servo(float velocity,float pre_velocity, int wheel)
{
	switch (wheel){
	case 1:

			if (velocity == 0)
			{
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_1 );
				HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11 | GPIO_PIN_13,GPIO_PIN_RESET);
				out_l = 0;
			}
//			else if( velocity >0)
//			{
//				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
//				run_l(11000);
//			}
//			else
//			{
//							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
//							run_l(-11000);
//			}
			else if(abs(velocity )<0.03)
			{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
				out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,300,10);
				run_l(out_left);
			}
			else if(abs(velocity  )<0.055)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
							out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,5);
							run_l(out_left);
						}
			else if(abs(velocity  )<0.085)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
							out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,1000,5);
							run_l(out_left);
						}
			else if(abs(velocity  )<0.125)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
							out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,150,930,3);
							run_l(out_left);
						}
			else if(abs(velocity  )<0.175)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
							out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,700,3);
							run_l(out_left);
						}
			else if(abs(velocity  )<0.225)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
							out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,3);
							run_l(out_left);
						}
			else if(abs(velocity  )<0.275)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,500,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.425)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,500,0);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.475)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120, 700,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.575)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.625)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,120, 500,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.675)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,100,500,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.725)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,80,450,1);
										run_l(out_left);
									}
			else if(abs(velocity  )<0.775)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
										out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,30,100,1);
										run_l(out_left);
									}
			else{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1 );
				out_left = PID_l(velocity*MS2RPM,pre_velocity*RADS2RPM,20,100,1);
				run_l(out_left);
			}
		break;
	case 0:
			if (velocity == 0)
			{
				__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
				HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_2 );
				HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13 | GPIO_PIN_15,GPIO_PIN_RESET);
				out_r = 0;

			}
//			else if (velocity >0)
//			{
//				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
//				run_r(11000);
//			}
//			else
//			{
//				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
//				run_r(-11000);
//			}
			else if(abs(velocity)<0.03)
			{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
				out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,300,10);
				run_r(out_right);
			}
			else if(abs(velocity  )<0.055)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
							out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,5);
							run_r(out_right);
						}
			else if(abs(velocity  )<0.085)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
							out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,1000,5);
							run_r(out_right);
						}
			else if(abs(velocity  )<0.125)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
							out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,150,930,3);
							run_r(out_right);
						}
			else if(abs(velocity  )<0.175)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
							out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,700,3);
							run_r(out_right);
						}
			else if(abs(velocity  )<0.225)
						{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
							out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,3);
							run_r(out_right);
						}
			else if(abs(velocity  )<0.275)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,500,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.425)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,500,0);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.475)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120, 700,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.575)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120,600,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.625)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,120, 500,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.675)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,100,500,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.725)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,80,450,1);
										run_r(out_right);
									}
			else if(abs(velocity  )<0.775)
									{
							HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
										out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,30,100,1);
										run_r(out_right);
									}
			else{
				HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
				out_right = PID_r(velocity*MS2RPM,pre_velocity*RADS2RPM,20,100,1);
				run_r(out_right);
			}
		break;
	default:
		break;
	}

}

