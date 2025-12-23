#include "ds18b20.h"

extern TIM_HandleTypeDef htim3;
#define TIMER TIM3
#define PORT  GPIOB

// Reset pulse
uint8_t ds_reset_pulse(uint16_t PinMask) {
    uint16_t result;

    if((PORT->IDR & PinMask)==0) return 2;
    PORT->ODR &= ~PinMask;
    TIMER->CNT=0;
    while(TIMER->CNT<480);
    PORT->ODR |= PinMask;
    while(TIMER->CNT<550);
    result = PORT->IDR & PinMask;
    while(TIMER->CNT<960);
    return (result) ? 1 : 0;
}

void ds_write_bit(uint8_t bit, uint16_t PinMask) {
    TIMER->CNT=0;
    PORT->ODR &= ~PinMask;
    while(TIMER->CNT<2);
    if(bit) PORT->ODR |= PinMask;
    while(TIMER->CNT<60);
    PORT->ODR |= PinMask;
}

uint16_t ds_read_bit(uint16_t PinMask) {
    uint16_t result;
    TIMER->CNT=0;
    PORT->ODR &= ~PinMask;
    while(TIMER->CNT<2);
    PORT->ODR |= PinMask;
    while(TIMER->CNT<15);
    result = PORT->IDR & PinMask;
    while(TIMER->CNT<60);
    return result;
}

void ds_write_byte(uint8_t byte, uint16_t PinMask) {
    for(uint8_t i=0;i<8;i++) ds_write_bit(byte&(1<<i), PinMask);
}

uint8_t ds_read_byte(uint16_t PinMask) {
    uint8_t result = 0;
    for(uint8_t i=0;i<8;i++) {
        if(ds_read_bit(PinMask)) result |= 1<<i;
    }
    return result;
}

// Obtaining temperature
float ds18b20_get_temp(uint16_t PinMask) {
    uint8_t ds_buff[9];

    ds_reset_pulse(PinMask);
    ds_write_byte(SKIP_ROM_ADR, PinMask);
    ds_write_byte(CONVERT_TEMP, PinMask);
    HAL_Delay(750);

    // Reading memory
    ds_reset_pulse(PinMask);
    ds_write_byte(SKIP_ROM_ADR, PinMask);
    ds_write_byte(READ_DATA_COMAND, PinMask);
    for(int i=0; i<9; i++) ds_buff[i] = ds_read_byte(PinMask);

    // Formation of the result
    int16_t raw = (int16_t)((ds_buff[1] << 8) | ds_buff[0]);
    return raw / 16.0f;
}
