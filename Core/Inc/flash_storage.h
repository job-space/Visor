#ifndef FLASH_STORAGE_H
#define FLASH_STORAGE_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>

/* Address of the last Flash page (STM32F103C8T6, 64 KB) */
#define FLASH_USER_START_ADDR  0x0800FC00

/* API */
bool Flash_SaveFloat(float value);
bool Flash_LoadFloat(float *value);

#endif /* FLASH_STORAGE_H */
