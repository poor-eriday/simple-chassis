/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "NRF24L01.h"
#include "stm32f103xb.h"
#include "Key.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_adc_ex.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint8_t KeyNum;
uint8_t SendFlag;								//发送标志位
uint8_t SendSuccessCount, SendFailedCount;		//发送成功计次，发送失败计次

uint8_t ReceiveFlag;							//接收标志位
uint8_t ReceiveSuccessCount, ReceiveFailedCount;//接收成功计次，接收失败计次
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	OLED_Init();
	//NRF24L01初始化
	NRF24L01_Init();

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	
	/*OLED显示静态字符串*/

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc1);
	OLED_ShowString(1, 1, "T:000-000-0");		//格式为：T:发送成功计次-发送失败计次-发送标志位


  uint32_t ADC_value[2];
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)ADC_value, 2);//开启dma转运
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    NRF24L01_TxPacket[1] = ADC_value[0]/100;//拆成两个字节发送
    NRF24L01_TxPacket[2] = ADC_value[0]%100;
    NRF24L01_TxPacket[3] = ADC_value[1]/100;
    NRF24L01_TxPacket[4] = ADC_value[1]%100;


    
    KeyNum = Key_GetNum();
    if (KeyNum == 0) 
    {
      NRF24L01_TxPacket[5] =0;
      NRF24L01_TxPacket[6] =0;
    }
    if (KeyNum == 1)				//左键按下
		{
			NRF24L01_TxPacket[5] =1;
    }
    if (KeyNum == 2)				//右键按下
    {
      NRF24L01_TxPacket[6] =1;
    }
    NRF24L01_TxPacket[0] = 0;  //消息id（默认为0）


			/*调用NRF24L01_Send函数，发送数据，同时返回发送标志位，方便用户了解发送状态*/
			/*发送标志位与发送状态的对应关系，可以转到此函数定义上方查看*/
			SendFlag = NRF24L01_Send();
			
			if (SendFlag == 1)			//发送标志位为1，表示发送成功
			{
				SendSuccessCount ++;
        HAL_Delay(100);	//发送成功计次变量自增
			}
			else						//发送标志位不为1，即2/3/4，表示发送不成功
			{
				SendFailedCount ++;		//发送失败计次变量自增
			}
			
			OLED_ShowNum(1, 3, SendSuccessCount, 3);	//显示发送成功次数
			OLED_ShowNum(1, 7, SendFailedCount, 3);		//显示发送失败次数
			OLED_ShowNum(1, 11, SendFlag, 1);			//显示最近一次的发送标志位
			
			/*显示发送数据*/
			OLED_ShowNum(2, 1, NRF24L01_TxPacket[1], 2);
			OLED_ShowNum(2, 3, NRF24L01_TxPacket[2], 2);
      OLED_ShowNum(2, 6, NRF24L01_TxPacket[3], 2);
			OLED_ShowNum(2, 8, NRF24L01_TxPacket[4], 2);
			OLED_ShowHexNum(3, 1, NRF24L01_TxPacket[5], 2);
			OLED_ShowHexNum(3, 4, NRF24L01_TxPacket[6], 2);
			
    //下方为江科大的接收示例，如有反馈需求可取消注释
		
		/*主循环内循环执行NRF24L01_Receive函数，接收数据，同时返回接收标志位，方便用户了解接收状态*/
		/*接收标志位与接收状态的对应关系，可以转到此函数定义上方查看*/
		// ReceiveFlag = NRF24L01_Receive();
		
		// if (ReceiveFlag)				//接收标志位不为0，表示收到了一个数据包
		// {
		// 	if (ReceiveFlag == 1)		//接收标志位为1，表示接收成功
		// 	{
		// 		ReceiveSuccessCount ++;	//接收成功计次变量自增
		// 	}
		// 	else	//接收标志位不为0也不为1，即2/3，表示此次接收产生了错误，错误接收的数据不应该使用
		// 	{
		// 		ReceiveFailedCount ++;	//接收失败计次变量自增
		// 	}
			
		// 	OLED_ShowNum(3, 3, ReceiveSuccessCount, 3);	//显示接收成功次数
		// 	OLED_ShowNum(3, 7, ReceiveFailedCount, 3);	//显示接收失败次数
		// 	OLED_ShowNum(3, 11, ReceiveFlag, 1);		//显示最近一次的接收标志位
			
		// 	/*显示接收数据*/
		// 	OLED_ShowHexNum(4, 1, NRF24L01_RxPacket[0], 2);
		// 	OLED_ShowHexNum(4, 4, NRF24L01_RxPacket[1], 2);
		// 	OLED_ShowHexNum(4, 7, NRF24L01_RxPacket[2], 2);
		// 	OLED_ShowHexNum(4, 10, NRF24L01_RxPacket[3], 2);
			
		// 	/*RX字符串闪烁一次，表明接收到了一次数据*/
		// 	OLED_ShowString(3, 15, "RX");
		// 	HAL_Delay(100);
		// 	OLED_ShowString(3, 15, "  ");
		// }
	}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
