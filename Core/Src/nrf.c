#include "gpio.h"
#include "spi.h"
#include "nrf.h"
#include "dw.h"

uint8_t nrf_send_read(uint8_t snt)
{
  HAL_SPI_TransmitReceive(&hspi1, &snt, &snt, 1, 100);
  return snt;
}

uint8_t dw_send_read(uint8_t snt)
{
  HAL_SPI_TransmitReceive(&hspi2, &snt, &snt, 1, 100);
  return snt;
}

void initNRF(void)
{
  DW_CS_LOW();
  dw_send_read(ModeBitReset);
  dw_send_read(ModeBitReset);
  DW_CS_HIGH();

  DW_CS_LOW();
  dw_send_read(ModeBitReset);
  dw_send_read(ModeBitReset);
  DW_CS_HIGH();

  CSN_HIGH();
  CE_LOW();

  CSN_LOW();
  nrf_send_read(cmdNOP);
  CSN_HIGH();
  
  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regCONFIG);
  nrf_send_read(0<<MASK_RX_DR | 0<<MASK_TX_DS | 0<<MASK_MAX_RT | 1<<EN_CRC | 0<<CRCO | 1<<PWR_UP | 1<<PRIM_RX); // PRX, CRC enabled
  CSN_HIGH();

//  osDelay(2);   //	1,5 ms after PWR_UP = 1

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regSETUP_AW);
  nrf_send_read(0x03); // address width = 5 bytes
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regEN_AA);
  nrf_send_read(1<<ENAA_P5 | 0<<ENAA_P4 | 0<<ENAA_P3 | 0<<ENAA_P2 | 0<<ENAA_P1 | 0<<ENAA_P0); // disable auto-ack for all channels
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRF_SETUP);
  nrf_send_read(0<<CONT_WAVE | 1<<RF_DR_LOW | 0<<PLL_LOCK | 0<<RF_DR_HIGH | 1<<RF_PWR2 | 1<<RF_PWR1); // [RF_DR_LOW, RF_DR_HIGH]: ‘00’ – 1Mbps; ‘01’ – 2Mbps; ‘10’ – 250kbps; ‘11’ – Reserved
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRF_CH);
  nrf_send_read(110);
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regEN_RXADDR);
  nrf_send_read(1<<ERX_P5 | 1<<ERX_P4 | 1<<ERX_P3 | 1<<ERX_P2 | 1<<ERX_P1 | 1<<ERX_P0);
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_PW_P0);
  nrf_send_read(0x04);  // 4 byte payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P0);
  nrf_send_read(0xE7);
  nrf_send_read(0xC2);
  nrf_send_read(0xC2);
  nrf_send_read(0xC2);
  nrf_send_read(0xE7);
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_PW_P1);
  nrf_send_read(0x04);  // 4 byte payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P1);
  nrf_send_read(0xC2);
  nrf_send_read(0xE7);
  nrf_send_read(0xE7);
  nrf_send_read(0xE7);
  nrf_send_read(0xC2);
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_PW_P2);
  nrf_send_read(0x04);  // 4 byte payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P2);
  nrf_send_read(0x2C);  // Only LSB. MSBytes are equal to RX_ADDR_P1
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_PW_P3);
  nrf_send_read(0x04);  // 4 byte payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P3);
  nrf_send_read(0x7C);  // Only LSB. MSBytes are equal to RX_ADDR_P1
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_PW_P4);
  nrf_send_read(0x04);  // 4 byte payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P4);
  nrf_send_read(0x5C);  // Only LSB. MSBytes are equal to RX_ADDR_P1
  CSN_HIGH();

//  CSN_LOW();
//  nrf_send_read(cmdWRITE_REG + regRX_PW_P5);
//  nrf_send_read(0x04);  // 4 byte payload
//  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regRX_ADDR_P5);
  nrf_send_read(0x6C);  // Only LSB. MSBytes are equal to RX_ADDR_P1
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regSETUP_RETR);
  nrf_send_read(0xFF); // auto retransmit
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + cmdACTIVATE);
  nrf_send_read(0x73); // enable R_RX_PL_WID W_ACK_PAYLOAD W_TX_PAYLOAD_NOACK
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regFEATURE);
  nrf_send_read(1<<EN_DPL | 1<<EN_ACK_PAY | 0<<EN_DYN_ACK); // enable ack payload
  CSN_HIGH();

  CSN_LOW();
  nrf_send_read(cmdWRITE_REG + regDYNPD);
  nrf_send_read(1<<DPL_P5 | 0<<DPL_P4 | 0<<DPL_P3 | 0<<DPL_P2 | 0<<DPL_P1 | 0<<DPL_P0); // enable dyn pl
  CSN_HIGH();

  CE_HIGH();
}
