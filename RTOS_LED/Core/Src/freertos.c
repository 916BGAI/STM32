/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "log.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for LEDTask */
osThreadId_t LEDTaskHandle;
const osThreadAttr_t LEDTask_attributes = {
    .name = "LEDTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for PrintTask */
osThreadId_t PrintTaskHandle;
const osThreadAttr_t PrintTask_attributes = {
    .name = "PrintTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for KeyTask */
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
    .name = "KeyTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal1,
};
/* Definitions for LEDStatus */
osMessageQueueId_t LEDStatusHandle;
const osMessageQueueAttr_t LEDStatus_attributes = {
    .name = "LEDStatus"};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task_LED(void *argument);
void Task_Print(void *argument);
void Task_Key(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* Create the queue(s) */
    /* creation of LEDStatus */
    LEDStatusHandle = osMessageQueueNew(16, sizeof(osThreadState_t), &LEDStatus_attributes);

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of LEDTask */
    LEDTaskHandle = osThreadNew(Task_LED, NULL, &LEDTask_attributes);

    /* creation of PrintTask */
    PrintTaskHandle = osThreadNew(Task_Print, NULL, &PrintTask_attributes);

    /* creation of KeyTask */
    KeyTaskHandle = osThreadNew(Task_Key, NULL, &KeyTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_Task_LED */
/**
 * @brief  Function implementing the LEDTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task_LED */
void Task_LED(void *argument)
{
    /* USER CODE BEGIN Task_LED */

    uint32_t PreviousWakeTime = osKernelGetTickCount();
    osThreadState_t LED_status = osThreadInactive;
    osStatus_t os_status;

    /* Infinite loop */
    for (;;)
    {
        LED_status = osThreadGetState(LEDTaskHandle);
        os_status = osMessageQueuePut(LEDStatusHandle, &LED_status, NULL, 0);
        if (os_status != osOK)
        {
            LOGE("osMessageQueuePut is not OK");
        }

        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        osDelayUntil(PreviousWakeTime += 1000);
    }
    /* USER CODE END Task_LED */
}

/* USER CODE BEGIN Header_Task_Print */
/**
 * @brief Function implementing the PrintTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task_Print */
void Task_Print(void *argument)
{
    /* USER CODE BEGIN Task_Print */

    uint32_t PreviousWakeTime = osKernelGetTickCount();
    osThreadState_t LED_status = osThreadInactive;
    osStatus_t os_status;

    /* Infinite loop */
    for (;;)
    {
        os_status = osMessageQueueGet(LEDStatusHandle, &LED_status, NULL, 0);
        if (os_status == osOK)
        {
            switch (LED_status)
            {
                case osThreadRunning:
                    LOGI("Task is Running");
                    break;
                case osThreadReady:
                    LOGI("Task is Ready");
                    break;
                case osThreadBlocked:
                    LOGI("Task is Blocked");
                    break;
                case osThreadTerminated:
                    LOGI("Task is Terminated");
                    break;
                case osThreadInactive:
                    LOGE("Task is Inactive");
                    break;
                case osThreadReserved:
                    LOGE("Task is Reserved");
                    break;
                case osThreadError:
                    LOGE("Task is Error");
                    break;
            }
        }
        else
        {
            LOGE("osMessageQueueGet is not OK");
        }
        osDelayUntil(PreviousWakeTime += 1000);
    }
    /* USER CODE END Task_Print */
}

/* USER CODE BEGIN Header_Task_Key */
/**
 * @brief Function implementing the KeyTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task_Key */
void Task_Key(void *argument)
{
    /* USER CODE BEGIN Task_Key */

    osThreadState_t LED_status = osThreadRunning;

    /* Infinite loop */
    for (;;)
    {
        if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
        {
            osDelay(20);
            if (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_RESET)
            {
                switch (LED_status)
                {
                    case osThreadRunning:
                        osThreadSuspend(LEDTaskHandle);
                        LED_status = osThreadInactive;
                        break;
                    case osThreadInactive:
                        osThreadResume(LEDTaskHandle);
                        LED_status = osThreadRunning;
                        break;
                    default:
                        break;
                }
            }
        }

        osDelay(10);
    }
    /* USER CODE END Task_Key */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
