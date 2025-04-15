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
/*
  实验一：模拟计数信号量

*/
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
uint32_t Num; // 全局变量
/* USER CODE END Variables */
osThreadId KEY1Handle;
osThreadId KEY2Handle;
osThreadId KEY3Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Task01(void const *argument);
void Task02(void const *argument);
void Task03(void const *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of KEY1 */
  osThreadDef(KEY1, Task01, osPriorityNormal, 0, 128);
  KEY1Handle = osThreadCreate(osThread(KEY1), NULL);

  /* definition and creation of KEY2 */
  osThreadDef(KEY2, Task02, osPriorityNormal, 0, 128);
  KEY2Handle = osThreadCreate(osThread(KEY2), NULL);

  /* definition and creation of KEY3 */
  osThreadDef(KEY3, Task03, osPriorityNormal, 0, 128);
  KEY3Handle = osThreadCreate(osThread(KEY3), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */
}

/* USER CODE BEGIN Header_Task01 */
/**
 * @brief  Function implementing the KEY1 thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task01 */
void Task01(void const *argument)
{
  /* USER CODE BEGIN Task01 */
  /* Infinite loop */
  for (;;)
  {
    // 实验一
    // if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
    // {
    //   osDelay(20);
    //   if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
    //   {
    //     if(Num<20)
    //     {
    //       xTaskNotifyGive(KEY2Handle);
    //       Num++;
    //       printf("Car Num:%d\r\n",Num);
    //     }else{
    //       printf("Car full\r\n");
    //     }
    //   }
    //   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET);
    // }

    // 实验二
    // if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
    // {
    //   osDelay(20);
    //   if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET)
    //   {
    //     xTaskNotify(KEY3Handle,0x0001,eSetBits);
    //     printf("KEY1 press!\r\n");
    //   }
    //   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==GPIO_PIN_SET);
    // }

    // 实验三
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
    {
      osDelay(20);
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
      {
        if (xTaskNotify(KEY2Handle, (uint32_t) "逆天!!!", eSetValueWithoutOverwrite) == pdTRUE)
        { // eSetValueWithOverwrite 覆盖
          printf("KEY1 press!\r\n");
        }
        else
        {
          printf("KEY1 press fail!\r\n");
        }
      }
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET)
        ;
    }
  }
  /* USER CODE END Task01 */
}

/* USER CODE BEGIN Header_Task02 */
/**
 * @brief Function implementing the KEY2 thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_Task02 */
void Task02(void const *argument)
{
  /* USER CODE BEGIN Task02 */
  /* Infinite loop */
  for (;;)
  {
    // 实验一
    // if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
    // {
    //   osDelay(20);
    //   if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
    //   {
    //     if(ulTaskNotifyTake(pdFALSE,0)>0)
    //     {
    //       Num--;
    //       printf("Car Num:%d\r\n",Num);
    //     }else{
    //       printf("Car vacant\r\n");
    //     }
    //   }
    //   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET);
    // }

    // 实验二
    // if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
    // {
    //   osDelay(20);
    //   if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET)
    //   {
    //     xTaskNotify(KEY3Handle,0x0010,eSetBits);
    //     printf("KEY2 press!\r\n");
    //   }
    //   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==GPIO_PIN_SET);
    // }

    // 实验三
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
    {
      osDelay(20);
      if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
      {
        if (xTaskNotifyWait(0x00000000, 0xffffffff, &Num, 0) == pdPASS)
        {
          printf("Data:%s \r\n", (uint8_t *)Num);
        }
      }
      while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) == GPIO_PIN_SET)
        ;
    }
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
void Task03(void const *argument)
{
  /* USER CODE BEGIN Task03 */
  /* Infinite loop */
  for (;;)
  {
    // 实验二
    // if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET)
    // {
    //   osDelay(20);
    //   if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET)
    //   {
    //     xTaskNotifyWait(0x00000000, 0xffffffff, &Num, 0);//进入之前不清除，进入之后清除
    //     if(Num == 0x0011)
    //     {
    //       printf("KEY1 and KEY2 press!\r\n");
    //     }else{
    //       printf("Not press!\r\n");
    //     }
    //   }
    //   while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)==GPIO_PIN_SET);
    // }
  }
  /* USER CODE END Task03 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
