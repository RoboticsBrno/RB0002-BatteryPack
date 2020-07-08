#pragma once

#include <stm32f0xx_hal.h>

#include <utility>

using Pin = std::pair< GPIO_TypeDef*, uint16_t>;

void setupPushPull( Pin pin ) {
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    HAL_GPIO_WritePin( pin.first, pin.second, GPIO_PIN_RESET );
    GPIO_InitStruct.Pin = pin.second;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( pin.first, &GPIO_InitStruct );
}
