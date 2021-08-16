#include "hw.h"

/*
 * Commands
 */
#define ESC_READ 0x03
#define ESC_WRITE 0x04

/*
 * Chip Select/Deselect
 */
#define SELECT_ESC() HAL_GPIO_WritePin(ESC_CS_GPIO_Port, ESC_CS_Pin, GPIO_PIN_RESET)
#define DESELECT_ESC() HAL_GPIO_WritePin(ESC_CS_GPIO_Port, ESC_CS_Pin, GPIO_PIN_SET)

/*
 * Generic
 */
UINT16 HW_Init(void) {
  UINT32 intMask;

  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_TIM7_Init();

  do {
    intMask = 0x93;
    HW_EscWriteDWordIsr(intMask, ESC_AL_EVENTMASK_OFFSET);
    intMask = 0;
    HW_EscReadDWordIsr(intMask, ESC_AL_EVENTMASK_OFFSET);
  } while (intMask != 0x93);

  intMask = 0;
  HW_EscWriteDWordIsr(intMask, ESC_AL_EVENTMASK_OFFSET);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 2, 0);
  HAL_NVIC_SetPriority(TIM7_IRQn, 1, 0);

  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  HAL_NVIC_EnableIRQ(TIM7_IRQn);

  HAL_TIM_Base_Start_IT(&htim7);

  return 0;
}

UINT16 HW_GetALEventRegister(void) {
  UINT16 event;

  DISABLE_ESC_INT();
  event = HW_GetALEventRegister_Isr();
  ENABLE_ESC_INT();
  return event;
}

UINT16 HW_GetALEventRegister_Isr(void) {
  UINT16 event;
  UINT8 cmd[2];

  cmd[1] = ESC_READ;
  SELECT_ESC();
  HAL_SPI_TransmitReceive(&hspi2, cmd, (UINT8 *) &event, sizeof(cmd), 2000);
  DESELECT_ESC();
  return event;
}

/*
 * Read Access
 */
void HW_EscRead(MEM_ADDR *pData, UINT16 Address, UINT16 Len) {
  UINT8 *pTmpData = (UINT8 *) pData;
  UINT8 cmd[3];

  cmd[2] = 0xFF;
  for (int i = 0; i < Len; i++) {
    cmd[0] = Address >> 5;
    cmd[1] = Address << 3 | ESC_READ;

    DISABLE_ESC_INT();
    SELECT_ESC();
    HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd), 2000);
    HAL_SPI_TransmitReceive(&hspi2, &cmd[2], pTmpData, 1, 2000);
    DESELECT_ESC();
    ENABLE_ESC_INT();

    pTmpData++;
    Address++;
  }
}

void HW_EscReadIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len) {
  UINT8 *pTmpData = (UINT8 *) pData;
  UINT8 cmd[3];

  cmd[0] = Address >> 5;
  cmd[1] = Address << 3 | ESC_READ;
  cmd[2] = 0xFF;
  HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd), 2000);
  for (int i = 0; i < Len - 1; i++) {
    HAL_SPI_Receive(&hspi2, pTmpData, 1, 2000);
    pTmpData++;
    Address++;
  }
  HAL_SPI_TransmitReceive(&hspi2, &cmd[2], pTmpData, 1, 2000);
}

/*
 * Write Access
 */
void HW_EscWrite(MEM_ADDR *pData, UINT16 Address, UINT16 Len) {
  UINT8 *pTmpData = (UINT8 *) pData;
  UINT8 cmd[2];

  for (int i = 0; i < Len; i++) {
    cmd[0] = Address >> 5;
    cmd[1] = Address << 3 | ESC_WRITE;

    DISABLE_ESC_INT();
    SELECT_ESC();
    HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd), 2000);
    HAL_SPI_Transmit(&hspi2, pTmpData, 1, 2000);
    DESELECT_ESC();
    ENABLE_ESC_INT();

    pTmpData++;
    Address++;
  }
}

void HW_EscWriteIsr(MEM_ADDR *pData, UINT16 Address, UINT16 Len) {
  UINT8 cmd[2];

  cmd[0] = Address >> 5;
  cmd[1] = Address << 3 | ESC_WRITE;
  SELECT_ESC();
  HAL_SPI_Transmit(&hspi2, cmd, sizeof(cmd), 2000);
  HAL_SPI_Transmit(&hspi2, (UINT8 *) pData, Len, 2000);
  DESELECT_ESC();
}

/*
 * Interrupt Request Handler
 */
void EXTI9_5_IRQHandler(void) {
  PDI_Isr();
  HAL_GPIO_EXTI_IRQHandler(PDI_IRQ_Pin);
}

void EXTI0_IRQHandler(void) {
  Sync0_Isr();
  HAL_GPIO_EXTI_IRQHandler(SYNC0_Pin);
}

void EXTI1_IRQHandler(void) {
  Sync1_Isr();
  HAL_GPIO_EXTI_IRQHandler(SYNC1_Pin);
}

void TIM7_IRQHandler(void) {
  ECAT_CheckTimer();
  HAL_TIM_IRQHandler(&htim7);
}
