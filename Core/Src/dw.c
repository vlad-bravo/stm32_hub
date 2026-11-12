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

uint32_t read(uint16_t adr)
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

void write(uint16_t adr, uint8_t volatile * dat)
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

void erase(uint16_t adr)
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
  uint16_t nPage;	// (uint8_t) nPage>>8	// (uint8_t) nPage
  uint32_t ts, prev;

  if(xSemaphoreTake(myMutexDWHandle, portMAX_DELAY) == pdTRUE)
  {
//msg(mStart);
    ts = read(0xFFFF);
//msg(m_ts); dump32(ts); crlf();
    if(ts==0xFFFFFFFF) ts=0;
//msg(m_ts); dump32(ts); crlf();
    for(nPage = 0; nPage<=0xFFFF; nPage++)
    {
//msg(m_nPage); dump16(nPage); crlf();
      prev = ts;
      ts = read(nPage);
//msg(m_ts); dump32(ts); crlf();
      if(ts == 0xFFFFFFFF)
      {
//msg(m_writePage); dump16(nPage); crlf();
        write(nPage, dat);
        break;
      } else {
        if(ts < prev)
        {
//msg(m_writePage); dump16(nPage); crlf();
//msg(m_prev); dump32(prev); crlf();
//msg(m_ts); dump32(ts); crlf();
//msg(m_erasePage); dump16(nPage); crlf();
          erase(nPage);
//msg(m_writePage); dump16(nPage); crlf();
          write(nPage, dat);
          break;
        }
      }
    }
//msg(mEnd); crlf();
    xSemaphoreGive(myMutexDWHandle);
  }
}
