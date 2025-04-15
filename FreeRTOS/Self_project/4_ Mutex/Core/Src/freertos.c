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
osThreadId H_TaskHandle;
osThreadId M_TaskHandle;
osThreadId L_TaskHandle;
osMutexId myMutex01Handle;

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
  /* Create the mutex(es) */
  /* definition and creation of myMutex01 */
  osMutexDef(myMutex01);
  myMutex01Handle = osMutexCreate(osMutex(myMutex01));

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
  /* definition and creation of H_Task */
  osThreadDef(H_Task, Task1, osPriorityAboveNormal, 0, 128);
  H_TaskHandle = osThreadCreate(osThread(H_Task), NULL);

  /* definition and creation of M_Task */
  osThreadDef(M_Task, Task2, osPriorityNormal, 0, 128);
  M_TaskHandle = osThreadCreate(osThread(M_Task), NULL);

  /* definition and creation of L_Task */
  osThreadDef(L_Task, Task3, osPriorityBelowNormal, 0, 128);
  L_TaskHandle = osThreadCreate(osThread(L_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task1 */
/**
  * @brief  Function implementing the H_Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task1 */
void Task1(void const * argument)
{
  /* USER CODE BEGIN Task1 */
  /* Infinite loop */
  for(;;)
  {   
  /* USER CODE BEGIN Task1 */
  /* Infinite loop */
    osDelay(10); // 模拟高优先级任务的周期性延时
    if(xSemaphoreTake(myMutex01Handle, portMAX_DELAY) == pdTRUE)  // 等待获取互斥量
    {
      printf("H  有纸,上厕所\r\n");
      // 执行占用互斥量的任务
    }
    printf("H  离开厕所\r\n");
    xSemaphoreGive(myMutex01Handle);  // 释放互斥量
    
  /* USER CODE END Task1 */
  }
}

/* USER CODE BEGIN Header_Task2 */
/**
* @brief Function implementing the M_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task2 */
void Task2(void const * argument)
{
  /* USER CODE BEGIN Task2 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(5);
    printf("M  厕所使用\r\n");
  }
  /* USER CODE END Task2 */
}

/* USER CODE BEGIN Header_Task3 */
/**
* @brief Function implementing the L_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task3 */
void Task3(void const * argument)
{
  /* USER CODE BEGIN Task3 */
  /* Infinite loop */
  for(;;)
  {
    if(xSemaphoreTake(myMutex01Handle, portMAX_DELAY) == pdTRUE)  // 等待获取互斥量
    {
      printf("L  有纸,上厕所\r\n");
      // 执行占用互斥量的任务
    }
    printf("L  离开厕所\r\n");
    xSemaphoreGive(myMutex01Handle);  // 释放互斥量
  }
  /* USER CODE END Task3 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

