/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
osThreadId bmi088TaskHandle;
osThreadId usbTaskHandle;
osThreadId PIDTask03Handle;
osThreadId can_ConTask04Handle;
osMessageQId RollQueue02Handle;
osMessageQId yawQueue03Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void Bmi088Task(void const * argument);
void usbTask01(void const * argument);
void PIDcalcTask03(void const * argument);
void can_motorTask04(void const * argument);

extern void MX_USB_DEVICE_Init(void);
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
  /* definition and creation of RollQueue02 */
  osMessageQDef(RollQueue02, 8, float);
  RollQueue02Handle = osMessageCreate(osMessageQ(RollQueue02), NULL);

  /* definition and creation of yawQueue03 */
  osMessageQDef(yawQueue03, 8, float);
  yawQueue03Handle = osMessageCreate(osMessageQ(yawQueue03), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of bmi088Task */
  osThreadDef(bmi088Task, Bmi088Task, osPriorityHigh, 0, 512);
  bmi088TaskHandle = osThreadCreate(osThread(bmi088Task), NULL);

  /* definition and creation of usbTask */
  osThreadDef(usbTask, usbTask01, osPriorityRealtime, 0, 3072);
  usbTaskHandle = osThreadCreate(osThread(usbTask), NULL);

  /* definition and creation of PIDTask03 */
  osThreadDef(PIDTask03, PIDcalcTask03, osPriorityHigh, 0, 512);
  PIDTask03Handle = osThreadCreate(osThread(PIDTask03), NULL);

  /* definition and creation of can_ConTask04 */
  osThreadDef(can_ConTask04, can_motorTask04, osPriorityRealtime, 0, 1024);
  can_ConTask04Handle = osThreadCreate(osThread(can_ConTask04), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_Bmi088Task */
/**
  * @brief  Function implementing the bmi088Task thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_Bmi088Task */
__weak void Bmi088Task(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN Bmi088Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Bmi088Task */
}

/* USER CODE BEGIN Header_usbTask01 */
/**
* @brief Function implementing the usbTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_usbTask01 */
__weak void usbTask01(void const * argument)
{
  /* USER CODE BEGIN usbTask01 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END usbTask01 */
}

/* USER CODE BEGIN Header_PIDcalcTask03 */
/**
* @brief Function implementing the PIDTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_PIDcalcTask03 */
__weak void PIDcalcTask03(void const * argument)
{
  /* USER CODE BEGIN PIDcalcTask03 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END PIDcalcTask03 */
}

/* USER CODE BEGIN Header_can_motorTask04 */
/**
* @brief Function implementing the can_ConTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_can_motorTask04 */
__weak void can_motorTask04(void const * argument)
{
  /* USER CODE BEGIN can_motorTask04 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END can_motorTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
