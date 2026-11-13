void initNRF(void);
uint8_t nrf_send_read(uint8_t);
uint8_t dw_send_read(uint8_t);

void G_GetRTC(uint8_t * dat);
void S_SetRTC(uint8_t * dat);
void F_FetchData(uint8_t * dat);
void r_ReadPage(uint8_t * dat);
void R_FastReadPage(uint8_t * dat);
void T_Test(uint8_t * dat);
void t_TestForCodes(uint8_t * dat);
void c_CheckForSR1(uint8_t * dat);
void e_Erase(uint8_t * dat);
void E_EraseChip(uint8_t * dat);
void w_Write_0(uint8_t * dat);
void W_Write_1(uint8_t * dat);

#define CSN_LOW() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);	// NRF_SCN	bcf	_CSN
#define CSN_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET);	// NRF_SCN	bsf	_CSN
#define CE_LOW() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);	// NRF_CE	bcf	_CE
#define CE_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);	// NRF_CE	bsf	_CE

#define DW_CS_LOW() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);	// DW_CS
#define DW_CS_HIGH() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);	// DW_CS

#define	cmdREAD_REG	0x00	// Read command and status registers. AAAAA = 5 bit Register Map Address
#define	cmdWRITE_REG	0x20	// Write command and status registers. AAAAA = 5 bit Register Map Address. Executable in power down or standby modes only
#define	cmdRD_RX_PLOAD	0x61	// Read RX-payload: 1 – 32 bytes. A read operation always starts at byte 0. Payload is deleted from FIFO after it is read. Used in RX mode
#define	cmdWR_TX_PLOAD	0xA0	// Write TX-payload: 1 – 32 bytes. A write operation always starts at byte 0 used in TX payload
#define	cmdFLUSH_TX	0xE1	// Flush TX FIFO, used in TX mode
#define	cmdFLUSH_RX	0xE2	// Flush RX FIFO, used in RX mode. Should not be executed during transmission of acknowledge, that is, acknowledge package will not be completed
#define	cmdREUSE_TX_PL	0xE3	// Used for a PTX device. Reuse last transmitted payload. Packets are repeatedly retransmitted as long as CE is high. TX payload reuse is active until W_TX_PAYLOAD or FLUSH TX is executed. TX payload reuse must not be activated or deactivated during package transmission
#define	cmdACTIVATE	0x50	// This write command followed by data 0x73 activates the following features:
				// • R_RX_PL_WID
				// • W_ACK_PAYLOAD
				// • W_TX_PAYLOAD_NOACK
				// A new ACTIVATE command with the same data deactivates them again. This is executable in power down or stand by modes only.
				// The R_RX_PL_WID, W_ACK_PAYLOAD, and W_TX_PAYLOAD_NOACK features registers are initially in a deactivated state// a write has no
				// effect, a read only results in zeros on MISO. To activate these registers, use the ACTIVATE command followed by data 0x73. Then they can be
				// accessed as any other register in nRF24L01. Use the same command and data to deactivate the registers again.
#define	cmdR_RX_PL_WID	0x60	// Read RX-payload width for the top R_RX_PAYLOAD in the RX FIFO
#define	cmdW_ACK_PAYLOAD	0xA8	// Used in RX mode.
				// Write Payload to be transmitted together with ACK packet on PIPE PPP. (PPP valid in the range from 000 to 101)
				// Maximum three ACK packet payloads can be pending. Payloads with same PPP are handled using first in - first out principle
				// Write payload: 1– 32 bytes. A write operation always starts at byte 0.
#define	cmdW_TX_PAYLOAD_NOACK	0xB0	// Used in TX mode. Disables AUTOACK on this specific packet.
#define	cmdNOP		0xFF	// No Operation. Might be used to read the STATUS register

#define	regCONFIG	0x00	// Configuration Register
#define	MASK_RX_DR	6	// Mask interrupt caused by RX_DR 1: Interrupt not reflected on the IRQ pin 0: Reflect RX_DR as active low interrupt on the IRQ pin
#define	MASK_TX_DS	5	// Mask interrupt caused by TX_DS 1: Interrupt not reflected on the IRQ pin 0: Reflect TX_DS as active low interrupt on the IRQ pin
#define	MASK_MAX_RT	4	// Mask interrupt caused by MAX_RT 1: Interrupt not reflected on the IRQ pin 0: Reflect MAX_RT as active low interrupt on the IRQ pin
#define	EN_CRC		3	// Enable CRC. Forced high if one of the bits in the EN_AA is high
#define	CRCO		2	// CRC encoding scheme '0' - 1 byte '1' – 2 bytes
#define	PWR_UP		1	// 1: POWER UP, 0:POWER DOWN
#define	PRIM_RX		0	// RX/TX control 1: PRX, 0: PTX

#define	regEN_AA	0x01	// Enable ‘Auto Acknowledgment’ function. Disable this functionality to be compatible with nRF2401
#define	ENAA_P5		5	// Enable auto acknowledgement data pipe 5
#define	ENAA_P4		4	// Enable auto acknowledgement data pipe 4
#define	ENAA_P3		3	// Enable auto acknowledgement data pipe 3
#define	ENAA_P2		2	// Enable auto acknowledgement data pipe 2
#define	ENAA_P1		1	// Enable auto acknowledgement data pipe 1
#define	ENAA_P0		0	// Enable auto acknowledgement data pipe 0

#define	regEN_RXADDR	0x02	// Enabled RX addresses
#define	ERX_P5		5	// Enable data pipe 5
#define	ERX_P4		4	// Enable data pipe 4
#define	ERX_P3		3	// Enable data pipe 3
#define	ERX_P2		2	// Enable data pipe 2
#define	ERX_P1		1	// Enable data pipe 1
#define	ERX_P0		0	// Enable data pipe 0

#define	regSETUP_AW	0x03	// Setup address width RX/TX Address field width '00' - Illegal '01' - 3 bytes '10' - 4 bytes '11' – 5 bytes LSByte is used if address width is below 5 bytes
#define	regSETUP_RETR	0x04	// Setup of Automatic Retransmission
#define	regRF_CH	0x05	// RF channel
#define	regRF_SETUP	0x06	// RF setup
#define	CONT_WAVE	7	// Enables continuous carrier transmit when high
#define	RF_DR_LOW	5	// Air Data Rate [RF_DR_LOW, RF_DR_HIGH]: ‘00’ – 1Mbps// ‘01’ – 2Mbps// ‘10’ – 250kbps// ‘11’ – Reserved
#define	PLL_LOCK	4	// Force PLL lock signal. Only used in test
#define	RF_DR_HIGH	3	// Air Data Rate [RF_DR_LOW, RF_DR_HIGH]: ‘00’ – 1Mbps// ‘01’ – 2Mbps// ‘10’ – 250kbps// ‘11’ – Reserved
#define	RF_PWR2		2	// Set RF output power in TX mode
#define	RF_PWR1		1	// '00' – -18dBm '01' – -12dBm '10' – -6dBm '11' – 0dBm

#define	regSTATUS	0x07	// Status Register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
#define	RX_DR		6	// Data Ready RX FIFO interrupt. Asserted when new data arrives RX FIFO. Write 1 to clear bit
#define	TX_DS		5	// Data Sent TX FIFO interrupt. Asserted when packet transmitted on TX. If AUTO_ACK is activated, this bit is set high only when ACK is received. Write 1 to clear bit
#define	MAX_RT		4	// Maximum number of TX retransmits interrupt Write 1 to clear bit. If MAX_RT is asserted it must be cleared to enable further communication 
#define	RX_P_NO3	3	// Data pipe number for the payload available for reading from RX_FIFO
#define	RX_P_NO2	2	// 000-101: Data Pipe Number
#define	RX_P_NO1	1	// 111: RX FIFO Empty
#define	TX_FULL		0	// TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO.

#define	regOBSERVE_TX	0x08	// Observe TX
#define	regCD		0x09	// Carrier Detect
#define	RPD		0	// Received Power Detector

#define	regRX_ADDR_P0	0x0A	// RX address pipe0
#define	regRX_ADDR_P1	0x0B	// RX address pipe1
#define	regRX_ADDR_P2	0x0C	// RX address pipe2
#define	regRX_ADDR_P3	0x0D	// RX address pipe3
#define	regRX_ADDR_P4	0x0E	// RX address pipe4
#define	regRX_ADDR_P5	0x0F	// RX address pipe5
#define	regTX_ADDR	0x10	// TX address
#define	regRX_PW_P0	0x11	// RX payload width, pipe0
#define	regRX_PW_P1	0x12	// RX payload width, pipe1
#define	regRX_PW_P2	0x13	// RX payload width, pipe2
#define	regRX_PW_P3	0x14	// RX payload width, pipe3
#define	regRX_PW_P4	0x15	// RX payload width, pipe4
#define	regRX_PW_P5	0x16	// RX payload width, pipe5
#define	regFIFO_STATUS	0x17	// FIFO Status Register
#define	TX_REUSE	6	// Reuse last transmitted data packet if set high. The packet is repeatedly retransmitted as long as CE is high.
				// TX_REUSE is set by the SPI command REUSE_TX_PL, and is reset by the SPI commands W_TX_PAYLOAD or FLUSH TX
//#define	TX_FULL		5	// TX FIFO full flag. 1: TX FIFO full. 0: Available locations in TX FIFO
#define	TX_EMPTY	4	// TX FIFO empty flag. 1: TX FIFO empty. 0: Data in TX FIFO
#define	RX_FULL		1	// RX FIFO full flag. 1: RX FIFO full. 0: Available locations in RX FIFO
#define	RX_EMPTY	0	// RX FIFO empty flag. 1: RX FIFO empty. 0: Data in RX FIFO

#define	regDYNPD	0x1C	// Enable dynamic payload length
#define	DPL_P5		5	// Enable dyn. payload length data pipe 5. (Requires EN_DPL and ENAA_P5)
#define	DPL_P4		4	// Enable dyn. payload length data pipe 4. (Requires EN_DPL and ENAA_P4)
#define	DPL_P3		3	// Enable dyn. payload length data pipe 3. (Requires EN_DPL and ENAA_P3)
#define	DPL_P2		2	// Enable dyn. payload length data pipe 2. (Requires EN_DPL and ENAA_P2)
#define	DPL_P1		1	// Enable dyn. payload length data pipe 1. (Requires EN_DPL and ENAA_P1)
#define	DPL_P0		0	// Enable dyn. payload length data pipe 0. (Requires EN_DPL and ENAA_P0)

#define	regFEATURE	0x1D	// Feature Register
#define	EN_DPL		2	// Enables Dynamic Payload Length
#define	EN_ACK_PAY	1	// Enables Payload with ACK
#define	EN_DYN_ACK	0	// Enables the W_TX_PAYLOAD_NOACK command
