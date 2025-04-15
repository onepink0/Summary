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
osThreadId KEYHandle;
osThreadId UARTHandle;
osSemaphoreId myCountingSem01Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task1(void const * argument);
void Task2(void const * argument);

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

  /* Create the semaphores(s) */
  /* definition and creation of myCountingSem01 */
  // osSemaphoreDef(myCountingSem01);
  // myCountingSem01Handle = osSemaphoreCreate(osSemaphore(myCountingSem01), 10);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  //上面CUBEMA初始化为最大值，这里初始化为0
  myCountingSem01Handle = xSemaphoreCreateCounting(10,0);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of KEY */
  osThreadDef(KEY, Task1, osPriorityNormal, 0, 128);
  KEYHandle = osThreadCreate(osThread(KEY), NULL);

  /* definition and creation of UART */
  osThreadDef(UART, Task2, osPriorityNormal, 0, 128);
  UARTHandle = osThreadCreate(osThread(UART), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task1 */
/**
  * @brief  Function implementing the KEY1 thread.
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
    osDelay(10);
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_SET)
      {
        if(xSemaphoreGive(myCountingSem01Handle) == pdTRUE)
        {
          printf("Count+1  True##\r\n");
        }else{
          printf("Count+1  False##\r\n");
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1) == GPIO_PIN_SET);
    }
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_SET)
      {
        if(xSemaphoreTake(myCountingSem01Handle,0) == pdTRUE)
        {
          printf("Count-1  True##\r\n");
        }else{
          printf("Count-1  False##\r\n");
        }
      }
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4) == GPIO_PIN_SET);
    }
  }
  /* USER CODE END Task1 */
}

/* USER CODE BEGIN Header_Task2 */
/**
* @brief Function implementing the KEY2 thread.
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
    osDelay(1000);
    printf("Count:%d  ##\r\n",(uint8_t)uxSemaphoreGetCount(myCountingSem01Handle));
  }
  /* USER CODE END Task2 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

