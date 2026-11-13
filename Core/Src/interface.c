#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "cmsis_os.h"

#include "gpio.h"
#include "usart.h"
#include "nrf.h"
#include "dump.h"
#include "dw.h"
#include "i2c.h"
#include "rtc.h"

extern osMessageQId serialQueueHandle;
extern osMutexId myMutexDWHandle;

void G_GetRTC(uint8_t * dat)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  emit(0x4E);
  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
  emit(sDate.Year);
  emit(sDate.Month);
  emit(sDate.Date);
  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
  emit(sTime.Hours);
  emit(sTime.Minutes);
  emit(sTime.Seconds);
}

void S_SetRTC(uint8_t * dat)
{
  RTC_DateTypeDef sDate;
  RTC_TimeTypeDef sTime;

  sDate.Year = dat[1];
  sDate.Month = dat[2];
  sDate.Date = dat[3];
  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

  sTime.Hours = dat[4];
  sTime.Minutes = dat[5];
  sTime.Seconds = dat[6];
  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
}

void F_FetchData(uint8_t * dat)
{
  uint8_t i, chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(WriteEnable);
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(BlockErase4);
    dw_send_read(0);       // A23-A16
    dw_send_read(0);       // A15-A8
    dw_send_read(0);       // A7-A0
    DW_CS_HIGH();

    for(i=0; i<200; i++)
    {
      osDelay(50);

      DW_CS_LOW();
      dw_send_read(ReadStatusReg1);
      chr=dw_send_read(0);
      DW_CS_HIGH();
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    }
    xSemaphoreGive(myMutexDWHandle);
  }
}

void r_ReadPage(uint8_t * dat)
{
  uint16_t i;
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(ReadStatusReg1);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(ReadData);
    dw_send_read(0);       // A23-A16
    dw_send_read(0);       // A15-A8
    dw_send_read(0);       // A7-A0
    for(i=0;i<512;i++)
    {
      chr=dw_send_read(0);
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    }
    DW_CS_HIGH();
    xSemaphoreGive(myMutexDWHandle);
  }
}

void R_FastReadPage(uint8_t * dat)
{
  uint8_t i, chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(FastRead);
    dw_send_read(dat[1]);  // A23-A16
    dw_send_read(dat[2]);  // A15-A8
    dw_send_read(0x00);    // A7-A0
    dw_send_read(0);       // dummy
    for(i=0;i<252;i++)
    {
      chr=dw_send_read(0);
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    }
    DW_CS_HIGH();
    xSemaphoreGive(myMutexDWHandle);
  }
}

void T_Test(uint8_t * dat)
{
  for(uint8_t i=0; i<8; i++) xQueueSend(serialQueueHandle, &dat[i], portMAX_DELAY);
}

void t_TestForCodes(uint8_t * dat)
{
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(ReadStatusReg1);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(ReadStatusReg2);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(Release_DevID);        // ReadDeviceID
    dw_send_read(0);
    dw_send_read(0);
    dw_send_read(0);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(Manu_DevID);        // Read Manufacturer / Device ID
    dw_send_read(0);
    dw_send_read(0);
    dw_send_read(0);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(ReadUniqueID);        // Read Unique ID Number
    dw_send_read(0);
    dw_send_read(0);
    dw_send_read(0);
    dw_send_read(0);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

    DW_CS_LOW();
    dw_send_read(JEDEC_ID);        // JEDEC ID
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    xSemaphoreGive(myMutexDWHandle);
  }
}

void c_CheckForSR1(uint8_t * dat)
{
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(ReadStatusReg1);
    chr=dw_send_read(0);
    DW_CS_HIGH();
    xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    xSemaphoreGive(myMutexDWHandle);
  }
}

void e_Erase(uint8_t * dat)
{
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(WriteEnable);
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(BlockErase4);
    dw_send_read(0);       // A23-A16
    dw_send_read(0);       // A15-A8
    dw_send_read(0);       // A7-A0
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(ReadStatusReg1);
    chr=dw_send_read(0);
    DW_CS_HIGH();

    for(; chr; )
    {
      osDelay(50);

      DW_CS_LOW();
      dw_send_read(ReadStatusReg1);
      chr=dw_send_read(0);
      DW_CS_HIGH();
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
    }
    xSemaphoreGive(myMutexDWHandle);
  }
}

void E_EraseChip(uint8_t * dat)
{
  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(WriteEnable);
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(ChipErase);
    DW_CS_HIGH();
    xSemaphoreGive(myMutexDWHandle);
  }
}

void w_Write_0(uint8_t * dat)
{
  uint16_t i;
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(WriteEnable);
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(PageProgram);
    dw_send_read(0);   // A23-A16
    dw_send_read(0);   // A15-A8
    dw_send_read(0);   // A7-A0
    for(i=0; i<256; i++) dw_send_read((uint8_t)i);
    DW_CS_HIGH();

    for(i=0; i<10; i++)
    {
      DW_CS_LOW();
      dw_send_read(ReadStatusReg1);
      chr=dw_send_read(0);
      DW_CS_HIGH();
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

      osDelay(1);
    }
    xSemaphoreGive(myMutexDWHandle);
  }
}

void W_Write_1(uint8_t * dat)
{
  uint16_t i;
  uint8_t chr;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    DW_CS_LOW();
    dw_send_read(WriteEnable);
    DW_CS_HIGH();

    DW_CS_LOW();
    dw_send_read(PageProgram);
    dw_send_read(0);   // A23-A16
    dw_send_read(01);   // A15-A8
    dw_send_read(0x2F);       // A7-A0
    for(i=0; i<209; i++) dw_send_read(255 - (uint8_t)i);
    DW_CS_HIGH();

    for(i=0; i<10; i++)
    {
      DW_CS_LOW();
      dw_send_read(ReadStatusReg1);
      chr=dw_send_read(0);
      DW_CS_HIGH();
      xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);

      osDelay(1);
    }
    xSemaphoreGive(myMutexDWHandle);
  }
}
