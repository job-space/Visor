#include "flash_storage.h"
#include <string.h>

#define FLASH_EMPTY_VALUE  0xFFFFFFFF

bool Flash_SaveFloat(float value)
{
    uint32_t data;
    uint32_t page_error = 0;

    memcpy(&data, &value, sizeof(float));

    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase   = FLASH_TYPEERASE_PAGES;
    erase.PageAddress = FLASH_USER_START_ADDR;
    erase.NbPages     = 1;

    if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }

    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,
                          FLASH_USER_START_ADDR,
                          data) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }

    HAL_FLASH_Lock();
    return true;
}

bool Flash_LoadFloat(float *value)
{
    uint32_t data = *(uint32_t *)FLASH_USER_START_ADDR;

    if (data == FLASH_EMPTY_VALUE)
    {
        return false;   // Flash empty
    }

    memcpy(value, &data, sizeof(float));
    return true;
}
