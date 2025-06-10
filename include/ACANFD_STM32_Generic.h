#pragma once

#include <stdint.h>
#include <stdbool.h>

struct CANFDMessage {
  uint32_t id;
  uint8_t  data[64];
  uint8_t  len;
  bool     extended;
  bool     rtr;
};

class ACANFD_STM32_Generic {
public:
  ACANFD_STM32_Generic();
  bool begin(uint32_t baudrate = 500000); // Default 500k
  bool write(const CANFDMessage& msg);
  bool read(CANFDMessage& msg);
};