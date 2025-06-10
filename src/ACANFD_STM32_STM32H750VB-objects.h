#pragma once

#include "ACANFD_STM32_Generic.h"

// FDCAN1 on PD0 (RX) and PD1 (TX)
#define FDCAN1_RX_PIN   GPIOD, 0
#define FDCAN1_TX_PIN   GPIOD, 1

extern ACANFD_STM32_Generic FDCAN1;