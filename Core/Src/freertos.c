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
#include "gpio.h"
#include "usart.h"
#include "nrf.h"
#include "dump.h"
#include "dw.h"
#include "i2c.h"
#include "rtc.h"
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
uint8_t for_display[16];
/* USER CODE END Variables */
/* Definitions for SerialRXTask */
osThreadId_t SerialRXTaskHandle;
const osThreadAttr_t SerialRXTask_attributes = {
  .name = "SerialRXTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for nrfTask */
osThreadId_t nrfTaskHandle;
const osThreadAttr_t nrfTask_attributes = {
  .name = "nrfTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for serialTask */
osThreadId_t serialTaskHandle;
const osThreadAttr_t serialTask_attributes = {
  .name = "serialTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for i2cTask */
osThreadId_t i2cTaskHandle;
const osThreadAttr_t i2cTask_attributes = {
  .name = "i2cTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for parserTask */
osThreadId_t parserTaskHandle;
const osThreadAttr_t parserTask_attributes = {
  .name = "parserTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for filterTask */
osThreadId_t filterTaskHandle;
const osThreadAttr_t filterTask_attributes = {
  .name = "filterTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for serialQueue */
osMessageQueueId_t serialQueueHandle;
const osMessageQueueAttr_t serialQueue_attributes = {
  .name = "serialQueue"
};
/* Definitions for parserQueue */
osMessageQueueId_t parserQueueHandle;
const osMessageQueueAttr_t parserQueue_attributes = {
  .name = "parserQueue"
};
/* Definitions for filterQueue */
osMessageQueueId_t filterQueueHandle;
const osMessageQueueAttr_t filterQueue_attributes = {
  .name = "filterQueue"
};
/* Definitions for myMutexDW */
osMutexId_t myMutexDWHandle;
const osMutexAttr_t myMutexDW_attributes = {
  .name = "myMutexDW"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTaskRXSerial(void *argument);
void StartTaskNRF(void *argument);
void StartTaskSerial(void *argument);
void StartTaskI2C(void *argument);
void StartTaskParser(void *argument);
void StartTaskFilter(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of myMutexDW */
  myMutexDWHandle = osMutexNew(&myMutexDW_attributes);

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
  /* creation of serialQueue */
  serialQueueHandle = osMessageQueueNew (64, sizeof(uint8_t), &serialQueue_attributes);

  /* creation of parserQueue */
  parserQueueHandle = osMessageQueueNew (4, sizeof(uint8_t[8]), &parserQueue_attributes);

  /* creation of filterQueue */
  filterQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &filterQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SerialRXTask */
  SerialRXTaskHandle = osThreadNew(StartTaskRXSerial, NULL, &SerialRXTask_attributes);

  /* creation of nrfTask */
  nrfTaskHandle = osThreadNew(StartTaskNRF, NULL, &nrfTask_attributes);

  /* creation of serialTask */
  serialTaskHandle = osThreadNew(StartTaskSerial, NULL, &serialTask_attributes);

  /* creation of i2cTask */
  i2cTaskHandle = osThreadNew(StartTaskI2C, NULL, &i2cTask_attributes);

  /* creation of parserTask */
  parserTaskHandle = osThreadNew(StartTaskParser, NULL, &parserTask_attributes);

  /* creation of filterTask */
  filterTaskHandle = osThreadNew(StartTaskFilter, NULL, &filterTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskRXSerial */
/**
  * @brief  Function implementing the SerialRXTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskRXSerial */
void StartTaskRXSerial(void *argument)
{
  /* USER CODE BEGIN StartTaskRXSerial */
  uint8_t dat[8];
  /* Infinite loop */
  for(;;)
  {
    while(HAL_UART_Receive_IT(&huart1, dat, 8) == HAL_OK)
    {
      xQueueSend(parserQueueHandle, &dat, portMAX_DELAY);
    }
    osDelay(1);
  }
  /* USER CODE END StartTaskRXSerial */
}

/* USER CODE BEGIN Header_StartTaskNRF */
/**
* @brief Function implementing the nrfTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskNRF */
void StartTaskNRF(void *argument)
{
  /* USER CODE BEGIN StartTaskNRF */
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  uint8_t chr, i;
  for(;;)
  {
    CSN_LOW();
    chr = nrf_send_read(cmdNOP);
    CSN_HIGH();
    if(chr&0x40)
    {
      CSN_LOW();
      nrf_send_read(cmdWRITE_REG + regSTATUS);
      nrf_send_read(1<<RX_DR | 1<<TX_DS | 1<<MAX_RT | 1<<TX_FULL);
      CSN_HIGH();
    }
    if((chr&0x0E) == 0x0A)      // Pipe 5 - display
    {
      HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
      HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
      for_display[0] = sTime.Hours;
      for_display[1] = sTime.Minutes;
      for_display[2] = sTime.Seconds;
      for_display[3] = 0x20;      // Century
      for_display[4] = sDate.Year;
      for_display[5] = sDate.Month;
      for_display[6] = sDate.Date;

      CSN_LOW();
      nrf_send_read(cmdFLUSH_TX);
      CSN_HIGH();
      CSN_LOW();
      nrf_send_read(cmdFLUSH_RX);
      CSN_HIGH();

      CSN_LOW();
      nrf_send_read(cmdW_ACK_PAYLOAD | 5);      // Payload for pipe 5 - display
      for(i=0; i<10; i++) nrf_send_read(for_display[i]);
      CSN_HIGH();
    } else if((chr&0x0E) != 0x0E)
    {
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      for_display[7] = chr;
      CSN_LOW();
      nrf_send_read(cmdRD_RX_PLOAD);
      chr = nrf_send_read(cmdNOP);
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      if(for_display[7]=='@') for_display[8] = chr;     // Only outdoor sensor
      chr = nrf_send_read(cmdNOP);
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      if(for_display[7]=='@') for_display[9] = chr;     // Only outdoor sensor
      chr = nrf_send_read(cmdNOP);
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = nrf_send_read(cmdNOP);
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      CSN_HIGH();
    }
    osDelay(1);
  }
  /* USER CODE END StartTaskNRF */
}

/* USER CODE BEGIN Header_StartTaskSerial */
/**
* @brief Function implementing the serialTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskSerial */
void StartTaskSerial(void *argument)
{
  /* USER CODE BEGIN StartTaskSerial */
  uint8_t chr;
  for(;;)
  {
    xQueueReceive(serialQueueHandle, &chr, portMAX_DELAY);
    HAL_UART_Transmit(&huart1, &chr, 1, 100);  //PC9
    osDelay(1);
  }
  /* USER CODE END StartTaskSerial */
}

/* USER CODE BEGIN Header_StartTaskI2C */
/**
* @brief Function implementing the i2cTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskI2C */
void StartTaskI2C(void *argument)
{
  /* USER CODE BEGIN StartTaskI2C */
  typedef struct
  {
    int16_t AC1;
    int16_t AC2;
    int16_t AC3;
    uint16_t AC4;
    uint16_t AC5;
    uint16_t AC6;
    int16_t B1;
    int16_t B2;
    int16_t MB;
    int16_t MC;
    int16_t MD;
  } type_cal_dat;
  union u64 
  {
    uint8_t dt[22];
    type_cal_dat dat; 
  } cal;

  uint8_t cmd[2], chr;
  int32_t ut, x1, x2, b5, t, up, x3, b6, b3, p;
  uint32_t b4, b7;

  for(;;)
  {
    if(HAL_I2C_IsDeviceReady(&hi2c1,0xEE,1,portMAX_DELAY)==HAL_OK)
    {
      cmd[0] = 0xAA;      // Calibration data
      HAL_I2C_Master_Transmit(&hi2c1,0xEE,cmd,1,portMAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1,0xEE,cal.dt,22,portMAX_DELAY);
      for(uint8_t i=0; i<11; i++)
      {
        chr = cal.dt[i*2];
        cal.dt[i*2] = cal.dt[i*2+1];
        cal.dt[i*2+1] = chr;
      }
      cmd[0] = 0xF4;      // Control register
      cmd[1] = 0x2E;      // cmd Convert Temperature
      HAL_I2C_Master_Transmit(&hi2c1,0xEE,cmd,2,portMAX_DELAY);
      osDelay(15);       // Conversion time 4.5 ms
      cmd[0] = 0xF6;      // Result MSB
      HAL_I2C_Master_Transmit(&hi2c1,0xEE,cmd,1,portMAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1,0xEE,cmd,2,portMAX_DELAY);
      
      ut = ((int32_t)cmd[0] << 8) + cmd[1];
      x1 = ((ut - cal.dat.AC6)*cal.dat.AC5)>>15;
      x2 = ((int32_t)cal.dat.MC << 11) / (x1 + cal.dat.MD);
      b5 = x1 + x2;
      t = (b5 + 8)/10;
      cmd[1] = (uint8_t)t;
      cmd[0] = (uint8_t)(t>>8);
      chr = 'T';
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[0];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[1];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[0];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[1];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);

      cmd[0] = 0xF4;      // Control register
      cmd[1] = 0x34;      // cmd Convert Pressure
      HAL_I2C_Master_Transmit(&hi2c1,0xEE,cmd,2,portMAX_DELAY);
      osDelay(15);       // Conversion time 4.5 ms
      cmd[0] = 0xF6;      // Result MSB
      HAL_I2C_Master_Transmit(&hi2c1,0xEE,cmd,1,portMAX_DELAY);
      HAL_I2C_Master_Receive(&hi2c1,0xEE,cmd,2,portMAX_DELAY);
      
      up = ((int32_t)cmd[0] << 8) + cmd[1];
      b6 = b5 - 4000;
      x1 = (b6*b6) >> 12;	 	 
      x1 *= cal.dat.B2;
      x1 >>=11;

      x2 = (cal.dat.AC2*b6);
      x2 >>=11;

      x3 = x1 +x2;

      b3 = (((((long)cal.dat.AC1 )*4 + x3) <<0) + 2) >> 2;

      //*****calculate B4************
      x1 = (cal.dat.AC3* b6) >> 13;
      x2 = (cal.dat.B1 * ((b6*b6) >> 12) ) >> 16;
      x3 = ((x1 + x2) + 2) >> 2;
      b4 = (cal.dat.AC4 * (unsigned long) (x3 + 32768)) >> 15;
        
      b7 = ((unsigned long)(up - b3) * (50000>>0));   
      if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
      else
        p = (b7 / b4) << 1;
      
      x1 = p >> 8;
      x1 *= x1;
      x1 = (x1 * 3038) >> 16;
      x2 = (p * (-7357)) >> 16;
      p += (x1 + x2 + 3791) >> 4;	// pressure in Pa
      p = p * 760 / 101325;             // pressure in mm h

      cmd[1] = (uint8_t)p;
      cmd[0] = (uint8_t)(p>>8);
      chr = 'P';
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[0];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[1];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[0];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
      chr = cmd[1];
      xQueueSend(filterQueueHandle, &chr, portMAX_DELAY);
    }
//    xQueueSend(serialQueueHandle, &cmd[0], portMAX_DELAY);
    osDelay(30000);
  }
  /* USER CODE END StartTaskI2C */
}

/* USER CODE BEGIN Header_StartTaskParser */
/**
* @brief Function implementing the parserTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskParser */
void StartTaskParser(void *argument)
{
  /* USER CODE BEGIN StartTaskParser */
  uint8_t dat[8];
  /* Infinite loop */
  for(;;)
  {
    xQueueReceive(parserQueueHandle, &dat, portMAX_DELAY);
//    for(uint8_t i=0; i<8; i++) for_display[i] = dat[i];
    switch(dat[0])
    {
      case 'T': T_Test(dat);  break;
      case 'G': G_GetRTC(dat); break;
      case 'S': S_SetRTC(dat); break;
      case 'F': F_FetchData(dat); break;
      case 'r': r_ReadPage(dat); break;
      case 'R': R_FastReadPage(dat); break;
      case 't': t_TestForCodes(dat); break;
      case 'c': c_CheckForSR1(dat); break;
      case 'e': e_Erase(dat); break;
      case 'E': E_EraseChip(dat); break;
      case 'w': w_Write_0(dat); break;
      case 'W': W_Write_1(dat); break;
      default: break;
    }
    osDelay(1);
  }
  /* USER CODE END StartTaskParser */
}

/* USER CODE BEGIN Header_StartTaskFilter */
/**
* @brief Function implementing the filterTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTaskFilter */
void StartTaskFilter(void *argument)
{
  /* USER CODE BEGIN StartTaskFilter */
  uint8_t chr;
  static volatile uint16_t i=0;
  static volatile uint8_t dat[256];
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;
  /* Infinite loop */
  for(;;)
  {
    xQueueReceive(filterQueueHandle, &chr, portMAX_DELAY);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
    if(sDate.Year==0)
      //xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
      ;
    else
    {
      HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
      dat[i++]=sDate.Year;
      dat[i++]=sDate.Month;
      dat[i++]=sDate.Date;
      dat[i++]=sTime.Hours;
      dat[i++]=sTime.Minutes;
      dat[i++]=sTime.Seconds;
      dat[i++]=chr;     // sensor
      xQueueReceive(filterQueueHandle, &chr, portMAX_DELAY);
      dat[i++]=chr;
      xQueueReceive(filterQueueHandle, &chr, portMAX_DELAY);
      dat[i++]=chr;
      xQueueReceive(filterQueueHandle, &chr, portMAX_DELAY);
      dat[i++]=chr;
      xQueueReceive(filterQueueHandle, &chr, portMAX_DELAY);
      dat[i++]=chr;
      if(i>252)
      {
        eePush(dat);
//        for(i=0; i<253; i++)
//        {
//          chr=dat[i];
//          xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
//        }
        i=0;
      }
    }
    osDelay(1);
  }
  /* USER CODE END StartTaskFilter */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

