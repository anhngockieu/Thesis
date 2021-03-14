/*
 * imu.c
 *
 *  Created on: Nov 18, 2020
 *      Author: anhngockieu
 */

#include <math.h>
#include "imu.h"

#define MADGWICK_BETA           0.1f
#define MADGWICK_SAMPLE_RATE    100.0f
#define DEG2RAD 				3.14f / 180.0f



madgwick_handle_t madgwick_handle;
I2C_HandleTypeDef hi2c1;
MPU6050_t MPU6050;
madgwick_quat_data_t quat_data;
float AX, AY, AZ, GX, GY, GZ;


void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE();
    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    //__HAL_RCC_I2C1_CLK_ENABLE();
  /* USER CODE BEGIN I2C1_MspInit 1 */

  /* USER CODE END I2C1_MspInit 1 */
  }

}

/**
* @brief I2C MSP De-Initialization
* This function freeze the hardware resources used in this example
* @param hi2c: I2C handle pointer
* @retval None
*/
void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }

}

void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}
static void User_I2C1_GeneralPurposeOutput_Init(I2C_HandleTypeDef* i2cHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(i2cHandle->Instance==I2C1)
    {
        /*   PB10     ------> I2C2_SCL; PB11     ------> I2C2_SDA */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
static void User_I2C1_AlternateFunction_Init(I2C_HandleTypeDef* i2cHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if(i2cHandle->Instance==I2C1)
    {
        /*   PB6     ------> I2C1_SCL; PB7     ------> I2C1_SDA */
        GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
}
HAL_StatusTypeDef I2CResetBus(void)
{
    hi2c1.ErrorCode = HAL_I2C_ERROR_AF;
    /* 1. Disable the I2C peripheral by clearing the PE bit in I2Cx_CR1 register */
    __HAL_I2C_DISABLE(&hi2c1);
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

    /* 2. Configure the SCL and SDA I/Os as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR) */
    User_I2C1_GeneralPurposeOutput_Init(&hi2c1);
    HAL_Delay(1);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);

    /* 3. Check SCL and SDA High level in GPIOx_IDR */
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET))
    {
#ifdef I2C_TEST
        printf("3.PB10=%d, PB11=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /* 4. Configure the SDA I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR).
     * 5. Check SDA Low level in GPIOx_IDR.
     * 6. Configure the SCL I/O as General Purpose Output Open-Drain, Low level (Write 0 to GPIOx_ODR)
     * 7. Check SCL Low level in GPIOx_IDR.
     * */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_Delay(1);
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_RESET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_RESET))
    {
#ifdef I2C_TEST
        printf("4-7.PB10=%d, PB11=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /*
     * 8. Configure the SCL I/O as General Purpose Output Open-Drain, High level (Write 1 to GPIOx_ODR).
     * 9. Check SCL High level in GPIOx_IDR.
     * 10. Configure the SDA I/O as General Purpose Output Open-Drain , High level (Write 1 to GPIOx_ODR).
     * 11. Check SDA High level in GPIOx_IDR.
     */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
    HAL_Delay(1);
    if ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) != GPIO_PIN_SET)||(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) != GPIO_PIN_SET))
    {
#ifdef I2C_TEST
        printf("8-11.PB10=%d, PB11=%d\r\n", HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6), HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7));
#endif
        return HAL_ERROR;
    }

    /* 12. Configure the SCL and SDA I/Os as Alternate function Open-Drain. */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
    User_I2C1_AlternateFunction_Init(&hi2c1);

    /* 13. Set SWRST bit in I2Cx_CR1 register. */
    hi2c1.Instance->CR1 |=  I2C_CR1_SWRST;
    HAL_Delay(2);
    /* 14. Clear SWRST bit in I2Cx_CR1 register. */
    hi2c1.Instance->CR1 &=  ~I2C_CR1_SWRST;
    HAL_Delay(2);
    /* 15. Enable the I2C peripheral by setting the PE bit in I2Cx_CR1 register */
    MX_I2C1_Init();
    __HAL_I2C_ENABLE(&hi2c1);
    HAL_Delay(2);
#ifdef I2C_TEST
    printf("I2CResetBus\r\n");
#endif
    hi2c1.ErrorCode = HAL_I2C_STATE_ERROR ;
    hi2c1.State = HAL_I2C_STATE_READY;
    hi2c1.PreviousState = HAL_I2C_STATE_RESET;
    hi2c1.Mode = HAL_I2C_MODE_NONE;
    return HAL_OK;
}
// Initial IMU :
void Imu_init()
{
	MX_I2C1_Init();
	//HAL_I2C_MspInit(&hi2c1);
	//HAL_I2C_MspDeInit(&hi2c1);


	MPU6050_Init(&hi2c1);

	madgwick_cfg_t madgwick_cfg;
	madgwick_cfg.beta = MADGWICK_BETA;
	madgwick_cfg.sample_freq = MADGWICK_SAMPLE_RATE;
	madgwick_handle = madgwick_init(&madgwick_cfg);

	if (MPU6050_Init(&hi2c1) == 1){
		I2CResetBus();
	//	HAL_Delay(10);
	}


	Calibration(&hi2c1);
}

// Read IMU + Calib + Madgwick filter
void Read_imu(float* roll , float* pitch, float* yaw)
{
	MPU6050_Read_All(&hi2c1,&MPU6050);
	AX = (float)MPU6050.Ax;
	AY = (float)MPU6050.Ay;
	AZ = (float)MPU6050.Az;
	GX = (float)MPU6050.Gx;
	GY = (float)MPU6050.Gy;
	GZ = (float)MPU6050.Gz;
	madgwick_update_6dof(madgwick_handle, GX,GY,GZ,AX,AY,AZ);
	madgwick_get_quaternion(madgwick_handle, &quat_data);
	*roll = 180.0 / 3.14 * atan2(2 * (quat_data.q0 * quat_data.q1 + quat_data.q2 * quat_data.q3), 1 - 2 * (quat_data.q1 * quat_data.q1 + quat_data.q2 * quat_data.q2));
	*pitch = 180.0 / 3.14 * asin(2 * (quat_data.q0 * quat_data.q2 - quat_data.q3 * quat_data.q1));
	*yaw = 180.0 / 3.14 * atan2f(quat_data.q0 * quat_data.q3 + quat_data.q1 * quat_data.q2, 0.5f - quat_data.q2 * quat_data.q2 - quat_data.q3 * quat_data.q3);
	//HAL_Delay (10);
	//HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
}
void Get_acc(float* x, float* y, float* z)
{
	MPU6050_Read_All(&hi2c1,&MPU6050);
	*x = (float)MPU6050.Ax;
	*y = (float)MPU6050.Ay;
	*z = (float)MPU6050.Az;
}
void Get_gyro(float* x, float* y, float* z)
{
	MPU6050_Read_All(&hi2c1,&MPU6050);
	*x = (float)MPU6050.Gx;
	*y = (float)MPU6050.Gy;
	*z = (float)MPU6050.Gz;
}
void Get_quat(float* x, float* y, float* z, float* t)
{
	MPU6050_Read_All(&hi2c1,&MPU6050);
	AX = (float)MPU6050.Ax;
	AY = (float)MPU6050.Ay;
	AZ = (float)MPU6050.Az;
	GX = (float)MPU6050.Gx;
	GY = (float)MPU6050.Gy;
	GZ = (float)MPU6050.Gz;
	madgwick_update_6dof(madgwick_handle, GX,GY,GZ,AX,AY,AZ);
	madgwick_get_quaternion(madgwick_handle, &quat_data);
	*x = quat_data.q0;
	*y = quat_data.q1;
	*z = quat_data.q2;
	*t = quat_data.q3;

}





