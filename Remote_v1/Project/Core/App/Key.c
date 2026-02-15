#include "main.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"


/*
 * @brief: 获取按键编号
 * @param: 无
 * @retval: 按键编号（0：无按键按下，1：左键按下，2：右键按下）
*/
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == 0)
	{
		HAL_Delay(20);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == 0)
		{
			HAL_Delay(20);
			KeyNum = 1;
		}
	}
	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
	{
		HAL_Delay(20);
		if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == 0)
		{
			HAL_Delay(20);
			KeyNum = 2;
		}
	}
	
	return KeyNum;
}
