#ifndef DS18B20_H
#define DS18B20_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

// DS18B20 command constants
#define SKIP_ROM_ADR       0xCC
#define CONVERT_TEMP       0x44
#define READ_DATA_COMAND   0xBE

// API function declarations
uint8_t ds_reset_pulse(uint16_t PinMask);
void ds_write_bit(uint8_t bit, uint16_t PinMask);
uint16_t ds_read_bit(uint16_t PinMask);
void ds_write_byte(uint8_t byte, uint16_t PinMask);
uint8_t ds_read_byte(uint16_t PinMask);
float ds18b20_get_temp(uint16_t PinMask);

#endif
