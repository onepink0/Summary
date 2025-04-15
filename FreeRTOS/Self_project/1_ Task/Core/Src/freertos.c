/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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
extern UART_HandleTypeDef huart1;

/* USER CODE END Variables */
osThreadId LED1Handle;
osThreadId LED2Handle;
osThreadId KEY3Handle;
osThreadId KEY6Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task01(void const * argument);
void Task02(void const * argument);
void Task03(void const * argument);
void Task04(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of LED1 */
  osThreadDef(LED1, Task01, osPriorityNormal, 0, 128);
  LED1Handle = osThreadCreate(osThread(LED1), NULL);

  /* definition and creation of LED2 */
  osThreadDef(LED2, Task02, osPriorityNormal, 0, 128);
  LED2Handle = osThreadCreate(osThread(LED2), NULL);

  /* definition and creation of KEY3 */
  osThreadDef(KEY3, Task03, osPriorityNormal, 0, 128);
  KEY3Handle = osThreadCreate(osThread(KEY3), NULL);

  /* definition and creation of KEY6 */
  osThreadDef(KEY6, Task04, osPriorityIdle, 0, 128);
  KEY6Handle = osThreadCreate(osThread(KEY6), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task01 */
/**
 * @brief  Function implementing the LED1 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task01 */
void Task01(void const * argument)
{
  /* USER CODE BEGIN Task01 */
  /* Infinite loop */
  for (;;)
  {
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
    osDelay(1000);
  }
  /* USER CODE END Task01 */
}

/* USER CODE BEGIN Header_Task02 */
/**
 * @brief Function implementing the LED2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task02 */
void Task02(void const * argument)
{
  /* USER CODE BEGIN Task02 */
  /* Infinite loop */
  for (;;)
  {
    // HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
    // osDelay(500);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    osDelay(500);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    osDelay(200);
    
  }
  /* USER CODE END Task02 */
}

/* USER CODE BEGIN Header_Task03 */
/**
 * @brief Function implementing the KEY3 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task03 */
void Task03(void const * argument)
{
  /* USER CODE BEGIN Task03 */
  //按键1控制LED1线程的删除和创建,按键:PA3,    LED1:PB3
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 1)
    {
      osDelay(20);
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 1)
      {
        if (LED1Handle == NULL)
        {//任务创建
          osThreadDef(LED1, Task01, osPriorityNormal, 0, 128);
          LED1Handle = osThreadCreate(osThread(LED1), NULL);
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED1 is OK\r\n", 12, 1000);
        }
        else
        {//任务删除
          vTaskDelete(LED1Handle);
          LED1Handle = NULL;
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED1 is NO\r\n", 12, 1000);
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) == 1);
    }
  }
  /* USER CODE END Task03 */
}

/* USER CODE BEGIN Header_Task04 */
/**
* @brief Function implementing the KEY6 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task04 */
void Task04(void const * argument)
{
  /* USER CODE BEGIN Task04 */
  static uint8_t flag = 1;
  //按键控制LED2线程的挂起和恢复,按键:PA6,    LED2:PB4
  /* Infinite loop */
  for(;;)
  {
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 1)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 1)
      {
        if(flag == 0)
        {
          flag = 1;//开始
          vTaskResume(LED2Handle);
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED2 is OK\r\n", 13, 1000);
        }
        else
        {
          flag = 0;//停止
          vTaskSuspend(LED2Handle);
          HAL_UART_Transmit(&huart1, (uint8_t *)"LED2 is NO\r\n", 13, 1000);
        }
        while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) == 1);
      }
    }
  }
  /* USER CODE END Task04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

