#include "FreeRTOS.h"
#include "semphr.h"
#include "cmsis_os.h"

#include "gpio.h"

#include "dw.h"
#include "nrf.h"
#include "dump.h"

extern osMutexId myMutexDWHandle;

void wait_while_busy(void)
{
  uint8_t chr;

  do
  {
    DW_CS_LOW();
    dw_send_read(ReadStatusReg1);
    chr=dw_send_read(0);
    DW_CS_HIGH();
  } while(chr&0x01);
}

uint32_t read_ts(uint16_t adr)
{
  uint32_t ts=0;
  uint8_t chr;

  wait_while_busy();
  DW_CS_LOW();
  dw_send_read(ReadData);
  dw_send_read((uint8_t) (adr>>8));    // A23-A16
  dw_send_read((uint8_t) adr);         // A15-A8
  dw_send_read(0);                     // A7-A0
  chr=dw_send_read(0);
  ts = chr;
  chr=dw_send_read(0);
  ts = ts<<8 | chr;
  chr=dw_send_read(0);
  ts = ts<<8 | chr;
  chr=dw_send_read(0);
  ts = ts<<8 | chr;
  DW_CS_HIGH();
  return ts;
}

void write_page(uint16_t adr, uint8_t volatile * dat)
{
  uint8_t i;
  
  wait_while_busy();
  DW_CS_LOW();
  dw_send_read(WriteEnable);
  DW_CS_HIGH();

  DW_CS_LOW();
  dw_send_read(PageProgram);
  dw_send_read((uint8_t) (adr>>8));    // A23-A16
  dw_send_read((uint8_t) adr);         // A15-A8
  dw_send_read(0);                     // A7-A0
  for(i=0; i<253; i++) dw_send_read(dat[i]);
  DW_CS_HIGH();
}

void erase_block(uint16_t adr)
{
  wait_while_busy();
  DW_CS_LOW();
  dw_send_read(WriteEnable);
  DW_CS_HIGH();

  DW_CS_LOW();
  dw_send_read(BlockErase4);
  dw_send_read((uint8_t) (adr>>8));    // A23-A16
  dw_send_read((uint8_t) adr);         // A15-A8
  dw_send_read(0x00);                  // A7-A0
  DW_CS_HIGH();
}

void eePush(uint8_t volatile * dat)
{
  static uint16_t nPage=0;
  uint32_t ts, prev;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
    ts = read_ts(nPage++);
    if(ts==0xFFFFFFFF) ts=0;
    for(;;nPage++)
    {
      prev = ts;
      ts = read_ts(nPage);
      if(ts == 0xFFFFFFFF)
      {
        write_page(nPage, dat);
        break;
      } else {
        if(ts < prev)
        {
          erase_block(nPage);
          write_page(nPage, dat);
          break;
        }
      }
    }
    xSemaphoreGive(myMutexDWHandle);
  }
}
