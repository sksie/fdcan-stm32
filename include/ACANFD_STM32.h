//------------------------------------------------------------------------------

#pragma once

//------------------------------------------------------------------------------

#if defined(ARDUINO_NUCLEO_H743ZI2)
  #include "ACANFD_STM32_NUCLEO_H743ZI2-objects.h"
#elif defined(STM32H750VB_GENERIC)
  #include "ACANFD_STM32_H750VB-objects.h"
#else
  #error "Unhandled Nucleo Board"
#endif

//------------------------------------------------------------------------------