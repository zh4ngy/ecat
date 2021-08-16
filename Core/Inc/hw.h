#ifndef __HW_H_
#define __HW_H_

#include "ecat_def.h"
#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"

#define DISABLE_ESC_INT() __set_BASEPRI(0x0010)
#define ENABLE_ESC_INT() __set_BASEPRI(0)

#define HW_EscReadDWordIsr(DWordValue, Address) HW_EscReadIsr((MEM_ADDR *)&DWordValue, Address, 4)
#define HW_EscReadWordIsr(WordValue, Address) HW_EscReadIsr((MEM_ADDR *) &WordValue, Address, 2)
#define HW_EscReadByteIsr(ByteValue, Address) HW_EscReadIsr(((MEM_ADDR *) &ByteValue), Address, 1)
#define HW_EscReadDWord(DWordValue, Address) HW_EscRead((MEM_ADDR *)&DWordValue, Address, 4)
#define HW_EscReadWord(WordValue, Address) HW_EscRead((MEM_ADDR *) &WordValue, Address, 2)
#define HW_EscReadByte(ByteValue, Address) HW_EscRead(((MEM_ADDR *) &ByteValue), Address, 1)
#define HW_EscReadMbxMem(pData, Address, Len) HW_EscRead(pData, Address, Len)

#define HW_EscWriteDWordIsr(DWordValue, Address) HW_EscWriteIsr((MEM_ADDR *) &DWordValue, Address, 4)
#define HW_EscWriteDWord(DWordValue, Address) HW_EscWrite((MEM_ADDR *) &DWordValue, Address, 4)
#define HW_EscWriteWordIsr(WordValue, Address) HW_EscWriteIsr((MEM_ADDR *) &WordValue, Address, 2)
#define HW_EscWriteWord(WordValue, Address) HW_EscWrite((MEM_ADDR *) &WordValue, Address, 2)
#define HW_EscWriteByteIsr(ByteValue, Address) HW_EscWriteIsr((MEM_ADDR *) &ByteValue, Address, 1)
#define HW_EscWriteByte(ByteValue, Address) HW_EscWrite((MEM_ADDR *) &ByteValue, Address, 1)
#define HW_EscWriteMbxMem(pData, Address, Len) HW_EscWrite(pData, Address, Len)

/*
 * Generic
 */
UINT16 HW_Init(void);

UINT16 HW_GetALEventRegister(void);

UINT16 HW_GetALEventRegister_Isr(void);

/*
 * Read Access
 */
void HW_EscRead(MEM_ADDR *pData, UINT16 Address, UINT16 Len);

void HW_EscReadIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len);

/*
 * Write Access
 */
void HW_EscWrite(MEM_ADDR *pData, UINT16 Address, UINT16 Len);

void HW_EscWriteIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len);

#endif
