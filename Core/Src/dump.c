#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "dump.h"

extern osMessageQId serialQueueHandle;

void emit(uint8_t chr)
{
  xQueueSend(serialQueueHandle, &chr, portMAX_DELAY);
}

void crlf(void)
{
  emit(0x0D);
  emit(0x0A);
}

uint8_t dig(uint8_t chr)
{
  chr = chr & 0x0F;
  if(chr<10)
    return '0'+chr;
  else
    return '7'+chr;
}

void dump8(uint8_t val)
{
  emit(dig(val>>4));
  emit(dig(val));
}

void dump16(uint16_t val)
{
  dump8(val>>8);
  dump8(val);
}

void dump32(uint32_t val)
{
  dump16(val>>16);
  dump16(val);
}

void msg(uint8_t msgID)
{
  switch(msgID)
  {
    case mStart:
      emit('S'); emit('t'); emit('a'); emit('r'); emit('t'); crlf(); break;
    case mEnd:
      emit('E'); emit('n'); emit('d'); crlf(); break;
    case m_ts:
      emit('t'); emit('s'); emit(':'); emit(' '); break;
    case m_prev:
      emit('p'); emit('r'); emit('e'); emit('v'); emit(':'); emit(' '); break;
    case m_nPage:
      emit('n'); emit('P'); emit('a'); emit('g'); emit('e'); emit(':'); emit(' '); break;
    case m_writePage:
      emit('w'); emit('r'); emit('i'); emit('t'); emit('e'); emit(':'); emit(' '); break;
    case m_erasePage:
      emit('e'); emit('r'); emit('a'); emit('s'); emit('e'); emit(':'); emit(' '); break;
    default: break;
  }
}