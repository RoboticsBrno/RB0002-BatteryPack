#include <stm32f0xx_hal.h>
#include "dbg.hpp"

extern "C" void SysTick_Handler(void) {
    HAL_IncTick();
}

extern "C" void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler( Dbg::inst().handle() );
}