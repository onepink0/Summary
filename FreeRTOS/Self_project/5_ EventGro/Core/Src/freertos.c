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

/* USER CODE END Variables */
osThreadId KEYHandle;
osThreadId builHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
EventGroupHandle_t myEventGroup=NULL;
/* USER CODE END FunctionPrototypes */

void Task01(void const * argument);
void Task02(void const * argument);

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of KEY */
  osThreadDef(KEY, Task01, osPriorityNormal, 0, 128);
  KEYHandle = osThreadCreate(osThread(KEY), NULL);

  /* definition and creation of buil */
  osThreadDef(buil, Task02, osPriorityNormal, 0, 128);
  builHandle = osThreadCreate(osThread(buil), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  myEventGroup = xEventGroupCreate();
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Task01 */
/**
  * @brief  Function implementing the KEY thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Task01 */
void Task01(void const * argument)
{
  /* USER CODE BEGIN Task01 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
      {
        printf("KEY1 pressed\r\n");
        xEventGroupSetBits(myEventGroup,0x0001);
      }
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET);
    }

    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
      {
        printf("KEY2 pressed\r\n");
        xEventGroupSetBits(myEventGroup,0x0010);
      }
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET);
    }

    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET)
    {
      osDelay(20);
      if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET)
      {
        printf("KEY3 pressed\r\n");
        xEventGroupSetBits(myEventGroup,0x0100);
      }
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET);
    }
  }
  /* USER CODE END Task01 */
}

/* USER CODE BEGIN Header_Task02 */
/**
* @brief Function implementing the buil thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Task02 */
void Task02(void const * argument)
{
  /* USER CODE BEGIN Task02 */
  uint32_t bit;
  /* Infinite loop */
  for(;;)
  {
    // bit = xEventGroupWaitBits(myEventGroup,0x0111,pdTRUE,pdTRUE,portMAX_DELAY);
    // if(bit == 0x0111)
    // {
    //   printf("KEY1,KEY2,KEY3 pressed\r\n");
    // }
    //  三个按键需要全部按下
    bit = xEventGroupWaitBits(myEventGroup,0x0111,pdTRUE,pdFALSE,portMAX_DELAY);
    if(bit == 0x0001 || bit == 0x0010 || bit == 0x0100)
    {
      printf("KEY1,KEY2,KEY3其一pressed\r\n");
    }
  }
  /* USER CODE END Task02 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

