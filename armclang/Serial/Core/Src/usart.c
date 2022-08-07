/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart3;

/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/*
int stdout_putchar (int ch)
{
	while ((USART3->SR & UART_FLAG_TC) == RESET);
	USART3->DR = (uint8_t)ch;
	return ch;
}

int stdin_getchar (void)
{
	while ((USART3->SR & UART_IT_RXNE) == RESET);
	return USART3->DR;
}
*/

//#if 1
//#include <stdio.h>

///* 告知连接器不从C库链接使用半主机的函�? */
//#ifdef __CC_ARM
//#pragma import(__use_no_semihosting)
//#elif defined(__GNUC__) || defined(__clang__)
//__ASM(".global __use_no_semihosting");
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch) //不使用microlib，请注释
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
////标准库需要的支持函数
//struct __FILE
//{
//    int handle;
//    /* Whatever you require here. If the only file you are using is */
//    /* standard output using printf() for debugging, no file handling */
//    /* is required. */
//};
//#endif

///* FILE is typedef�? d in stdio.h. */
//FILE __stdout;

////解决HAL库使用时,某些情况可能报错的bug
//int _ttywrch(int ch)
//{
//    ch = ch;
//    return ch;
//}

////定义_sys_exit()以避免使用半主机模式
//void _sys_exit(int x)
//{
//    x = x;
//}

//int fputc(int ch, FILE *stream)
//{
//    /* 堵塞判断串口是否发�?�完�? */
//    while ((USART3->SR & UART_FLAG_TC) == RESET);
//    /* 串口发�?�完成，将该字符发�?? */
//    USART3->DR = (uint8_t)ch;

//    return ch;
//}

//int fgetc(FILE *stream)
//{
//    while ((USART3->SR & UART_IT_RXNE) == RESET);
//	
//    return USART3->DR;
//}

//#endif

/* USER CODE END 1 */
