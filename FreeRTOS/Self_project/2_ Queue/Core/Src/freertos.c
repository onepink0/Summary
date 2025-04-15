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
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1; //UART1
uint8_t data[20];

QueueHandle_t xQueue;//定义一个队列


/* USER CODE END Variables */
osThreadId Send1Handle;
osThreadId Send2Handle;
osThreadId Rece1Handle;
osMessageQId Queue01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task1(void const * argument);
void Task2(void const * argument);
void Task3(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
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

  /* Create the queue(s) */
  /* definition and creation of Queue01 */
  osMessageQDef(Queue01, 2, uint32_t);
  Queue01Handle = osMessageCreate(osMessageQ(Queue01), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  xQueue = xQueueCreate(1, sizeof(uint32_t));//创建一个队列
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of Send1 */
  osThreadDef(Send1, Task1, osPriorityNormal, 0, 128);
  Send1Handle = osThreadCreate(osThread(Send1), NULL);

  /* definition and creation of Send2 */
  osThreadDef(Send2, Task2, osPriorityNormal, 0, 128);
  Send2Handle = osThreadCreate(osThread(Send2), NULL);

  /* definition and creation of Rece1 */
  osThreadDef(Rece1, Task3, osPriorityNormal, 0, 128);
  Rece1Handle = osThreadCreate(osThread(Rece1), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task1 */
/**
  * @brief  Function implementing the Send1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task1 */
void Task1(void const * argument)
{
  /* USER CODE BEGIN Task1 */
  BaseType_t xStatus;
  uint32_t xData = 10086;
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
      {
        xStatus = xQueueSendToBack(xQueue, &xData, 0);//不等待
        if(xStatus != pdPASS)
        {
          HAL_UART_Transmit(&huart1, (uint8_t*)"full [10086]\r\n", 14, 1000);
        }else{
          HAL_UART_Transmit(&huart1, (uint8_t*)"not full [10086]\r\n", 18, 1000);
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET);
    }
  }
  /* USER CODE END Task1 */
}

/* USER CODE BEGIN Header_Task2 */
/**
* @brief Function implementing the Send2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task2 */
void Task2(void const * argument)
{
  /* USER CODE BEGIN Task2 */
  BaseType_t xStatus;
  uint32_t xData = 66666;
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
    {
      osDelay(20);
      // if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)  实验二
      // {
      //   xStatus = xQueueSendToBack(xQueue, &xData, portMAX_DELAY);  //最大等待时间
      //   if(xStatus != pdPASS)
      //   {
      //     HAL_UART_Transmit(&huart1, (uint8_t*)"full [66666]\r\n", 15, 1000);
      //   }else{
      //     HAL_UART_Transmit(&huart1, (uint8_t*)"not full [66666]\r\n", 18, 1000);
      //   }
      // }
      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)//实验三
      {
        xStatus = xQueueOverwrite(xQueue, &xData);  //覆盖
        if(xStatus != pdPASS)
        {
          HAL_UART_Transmit(&huart1, (uint8_t*)"task2 no\r\n", 11, 1000);
        }else{
          HAL_UART_Transmit(&huart1, (uint8_t*)"task2 yes\r\n", 11, 1000);
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET);
    }
  }
  /* USER CODE END Task2 */
}

/* USER CODE BEGIN Header_Task3 */
/**
* @brief Function implementing the Rece1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task3 */
void Task3(void const * argument)
{
  /* USER CODE BEGIN Task3 */
  BaseType_t xStatus;
  uint32_t xData1;
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)
    {
      osDelay(20);
      // if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)
      // {
      //   sprintf(data, "read data:%ld\r\n", xData1);
      //   HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data), 1000);
      //   xStatus = xQueueReceive(xQueue, &xData1, portMAX_DELAY);  //接收数据,最大等待时间
      //   if(xStatus != pdPASS)
      //   {
      //     HAL_UART_Transmit(&huart1, (uint8_t*)"read no\r\n", 9, 1000);
      //   }else{
      //     HAL_UART_Transmit(&huart1, (uint8_t*)"read ok\r\n", 9, 1000);
      //   }
      // }
      if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET)
      {
        sprintf(data, "read data:%ld\r\n", xData1);
        HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data), 1000);
        xStatus = xQueuePeek(xQueue, &xData1, 0);  //接收数据,最大等待时间
        if(xStatus != pdPASS)
        {
          HAL_UART_Transmit(&huart1, (uint8_t*)"watch no\r\n", 10, 1000);
        }else{
          HAL_UART_Transmit(&huart1, (uint8_t*)"watch ok\r\n", 10, 1000);
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) == GPIO_PIN_SET);
    }
  }
  /* USER CODE END Task3 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

