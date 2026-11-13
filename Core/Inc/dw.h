void eePush(uint8_t volatile *);
void wait_while_busy(void);

#define WriteEnable	0x06
#define WriteDisable	0x04
#define ReadStatusReg1	0x05
#define ReadStatusReg2	0x35
#define WriteStatusReg	0x01
#define PageProgram	0x02
#define QuadPageProgram	0x32
#define BlockErase64	0xD8
#define BlockErase32	0x52
#define BlockErase4	0x20
#define ChipErase	0xC7
#define ChipErase1	0x60
#define EraseSuspend	0x75
#define EraseResume	0x7A
#define PowerDown	0xB9
#define HighPerfMode	0xA3
#define ModeBitReset	0xFF
#define Release_DevID	0xAB
#define Manu_DevID	0x90
#define ReadUniqueID	0x4B
#define JEDEC_ID	0x9F

#define ReadData	0x03
#define FastRead	0x0B
#define FastReadDualOut	0x3B
#define FastReadDualIO	0xBB
#define FastReadQuadOut	0x6B
#define FastReadQuadIO	0xEB
