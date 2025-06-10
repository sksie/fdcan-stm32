#include "ACANFD_STM32_Generic.h"
#include "stm32h7xx_hal.h"

// Static instance
ACANFD_STM32_Generic FDCAN1;

static FDCAN_HandleTypeDef hfdcan1;

ACANFD_STM32_Generic::ACANFD_STM32_Generic() {}

bool ACANFD_STM32_Generic::begin(uint32_t baudrate) {
  __HAL_RCC_FDCAN_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  // Configure PD0 (RX) and PD1 (TX) for FDCAN1
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 1;
  hfdcan1.Init.DataTimeSeg2 = 1;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;

  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK) {
    return false;
  }

  // Configure a filter to accept all messages
  FDCAN_FilterTypeDef sFilterConfig;
  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  sFilterConfig.FilterType = FDCAN_FILTER_MASK;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0;
  sFilterConfig.FilterID2 = 0;
  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
    return false;
  }

  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
    return false;
  }

  return true;
}

bool ACANFD_STM32_Generic::write(const CANFDMessage& msg) {
  FDCAN_TxHeaderTypeDef txHeader;
  txHeader.Identifier = msg.id;
  txHeader.IdType = msg.extended ? FDCAN_EXTENDED_ID : FDCAN_STANDARD_ID;
  txHeader.TxFrameType = msg.rtr ? FDCAN_REMOTE_FRAME : FDCAN_DATA_FRAME;
  txHeader.DataLength = (msg.len << 16); // DLC encoding
  txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
  txHeader.BitRateSwitch = FDCAN_BRS_OFF;
  txHeader.FDFormat = FDCAN_CLASSIC_CAN;
  txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  txHeader.MessageMarker = 0;

  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, (uint8_t*)msg.data) != HAL_OK) {
    return false;
  }
  return true;
}

bool ACANFD_STM32_Generic::read(CANFDMessage& msg) {
  FDCAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[64];
  if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK) {
    return false;
  }
  msg.id = rxHeader.Identifier;
  msg.extended = (rxHeader.IdType == FDCAN_EXTENDED_ID);
  msg.rtr = (rxHeader.RxFrameType == FDCAN_REMOTE_FRAME);
  msg.len = rxHeader.DataLength >> 16; // DLC decoding
  memcpy(msg.data, rxData, msg.len);
  return true;
}