/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "u8g2.h"
#include "u8x8.h"
#include "stdio.h"
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
    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART3_UART_Init();
    /* USER CODE BEGIN 2 */
    u8g2_t u8g2;
    u8g2Init(&u8g2);

    const unsigned char logo[] U8X8_PROGMEM = {
        0xff, 0xff, 0xff, 0xdf, 0xfe, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x20, 0x81, 0xff, 0xff, 0x7f, 0xff, 0xff, 0x1f, 0xff, 0x7f, 0xfc, 0xff, 0x7f, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xe3, 0xff, 0x7f, 0xff, 0xff, 0xfc, 0xff, 0xbf, 0xcf, 0xff, 0x7f, 0xff, 0x7f, 0xff, 0xf3, 0x4f, 0x3f, 0xff, 0x7f, 0xff, 0x9f, 0xff, 0xf9, 0x97, 0xff, 0xfc, 0x7f, 0xff, 0xef, 0xff, 0xfb, 0xdf, 0xff, 0xf9, 0x7f, 0xff, 0x67, 0xfd, 0xfc, 0xcf, 0xff, 0xf7, 0x7f, 0xff, 0x7b, 0xfe, 0xff, 0xb7, 0xff, 0xe6, 0x7f, 0xff, 0x7d, 0xf9, 0xff, 0xff, 0x5f, 0xdf, 0x7f, 0xff, 0x7c, 0xf6, 0x97, 0xf4, 0x2f, 0x9f, 0x7f, 0x7f, 0xff, 0xfd, 0xb4, 0x55, 0xbf, 0x7f, 0x7f, 0x3f, 0xff, 0xbe, 0xaa, 0xaa, 0xbe, 0x7f, 0x7e, 0xbf, 0xfd, 0xaf, 0xad, 0xd6, 0xfa, 0xff, 0x7e, 0xdf, 0xff, 0x57, 0x75, 0xad, 0xf6, 0xff, 0x7d, 0xcf, 0xfd, 0x6b, 0xcb, 0xb5, 0xed, 0xb7, 0x7d, 0x6f, 0xf1, 0x55, 0x61, 0x6b, 0xed, 0xdf, 0x7b, 0x77, 0xeb, 0x5a, 0xe0, 0x5e, 0x55, 0x17, 0x72, 0xf7, 0x7d, 0x17, 0x6a, 0x55, 0xdb, 0xa7, 0x77, 0xf7, 0x7b, 0x4d, 0xe0, 0x01, 0xb7, 0xfb, 0x77, 0xfb, 0xbf, 0x15, 0x62, 0x49, 0xeb, 0xfe, 0x6f, 0xfb, 0x5f, 0x9b, 0xe0, 0x11, 0x7f, 0xff, 0x6f, 0xfb, 0xff, 0x2e, 0x24, 0x1c, 0xdb, 0xfa, 0x6f, 0xfd, 0xaf, 0x15, 0x81, 0x48, 0xbf, 0xfd, 0x4f, 0xfd, 0x5f, 0x1f, 0xa8, 0x82, 0x6b, 0xfb, 0x5f, 0xfd, 0xef, 0x2a, 0xc1, 0x70, 0xdf, 0xfe, 0x5f, 0xfd, 0x5f, 0x1f, 0x84, 0x54, 0x6b, 0xfb, 0x5f, 0xfd, 0xb7, 0x95, 0x28, 0x0e, 0xbf, 0xfd, 0x5f, 0xfc, 0xcf, 0x1f, 0xce, 0x21, 0xeb, 0xf6, 0x5f, 0x1d, 0x4f, 0x95, 0x28, 0x04, 0xbf, 0xfb, 0x5e, 0xbe, 0x8f, 0x1f, 0x95, 0x50, 0xeb, 0x7e, 0x5d, 0xfd, 0x57, 0x60, 0x01, 0x02, 0xbf, 0xfb, 0x5f, 0x1c, 0xbf, 0x1a, 0x49, 0x08, 0xdb, 0xf6, 0x5d, 0xfd, 0xef, 0x8f, 0xc1, 0x27, 0xff, 0x7d, 0x5f, 0x7d, 0xbf, 0x5a, 0xf4, 0x0f, 0x57, 0xff, 0x5e, 0x5d, 0xef, 0x1f, 0x70, 0x5f, 0xff, 0xfd, 0x5e, 0xbd, 0xbf, 0x9d, 0xfa, 0x0b, 0xdb, 0xbe, 0x4f, 0xfd, 0x7f, 0xd7, 0xbc, 0x04, 0x7f, 0x7b, 0x5f, 0x7b, 0xdf, 0x1f, 0xfe, 0xa9, 0xd7, 0xff, 0x6e, 0x7b, 0xff, 0x96, 0xee, 0xc3, 0xff, 0xbe, 0x6f, 0xfb, 0xbd, 0x1f, 0xff, 0xff, 0xb6, 0xbf, 0x67, 0x77, 0xff, 0x5d, 0x7f, 0xff, 0xff, 0x7f, 0x77, 0xf7, 0xfe, 0x97, 0xf7, 0x6f, 0xdb, 0xcf, 0x77, 0xe7, 0xff, 0xde, 0xff, 0xff, 0xff, 0xff, 0x7b, 0xef, 0xf7, 0xdf, 0xdd, 0xdd, 0xb7, 0xc3, 0x7b, 0xdf, 0xfb, 0xdb, 0xad, 0xac, 0xfd, 0xff, 0x79, 0xdf, 0xdf, 0xff, 0x9b, 0xcb, 0xff, 0xfb, 0x7e, 0xbf, 0xef, 0xff, 0xfb, 0xdd, 0xff, 0xf4, 0x7e, 0x3f, 0x5f, 0x7f, 0xff, 0xff, 0xff, 0x7b, 0x7e, 0x7f, 0xdf, 0xff, 0xff, 0xff, 0x3f, 0xbf, 0x7f, 0xff, 0xbc, 0xfe, 0xff, 0xff, 0xef, 0xbe, 0x7f, 0xff, 0xfd, 0xf6, 0xff, 0xff, 0xdb, 0xcf, 0x7f, 0xff, 0xf3, 0xab, 0xff, 0xff, 0xb7, 0xef, 0x7f, 0xff, 0xf7, 0xdb, 0xfe, 0xbf, 0xef, 0xf3, 0x7f, 0xff, 0xcf, 0xdf, 0xaa, 0x56, 0xff, 0xfd, 0x7f, 0xff, 0x9f, 0xbf, 0x7b, 0x57, 0x7f, 0xfe, 0x7f, 0xff, 0x7f, 0xfe, 0x6b, 0xff, 0xbf, 0xff, 0x7f, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xcf, 0xff, 0x7f, 0xff, 0xff, 0xe3, 0xff, 0xff, 0xf1, 0xff, 0x7f, 0xff, 0xff, 0x1f, 0xfe, 0x5f, 0xfe, 0xff, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x40, 0xff, 0xff, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        u8g2_FirstPage(&u8g2);
        do
        {
            u8g2_DrawXBMP(&u8g2, 32, 0, 64, 64, logo);

        } while (u8g2_NextPage(&u8g2));
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
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
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
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

#ifdef USE_FULL_ASSERT
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
