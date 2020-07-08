#pragma once

#include <stm32f0xx_hal.h>
#include <cstdio>
#include "sys.hpp"

class Dbg {
public:
    static Dbg& inst() {
        static Dbg dbg;
        return dbg;
    }

    UART_HandleTypeDef* handle() {
        return &_uart;
    }

    template < typename... Args >
    static void print( const char *fmt, Args...args ) {
        inst()._print( fmt, args... );
    }

private:
    Dbg() {
        _setupUart();
        _setupGpio();
    }

    void _setupUart() {
        _uart.Instance = USART1;
        _uart.Init.BaudRate = 115200;
        _uart.Init.WordLength = UART_WORDLENGTH_8B;
        _uart.Init.StopBits = UART_STOPBITS_1;
        _uart.Init.Parity = UART_PARITY_NONE;
        _uart.Init.Mode = UART_MODE_TX_RX;
        _uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        _uart.Init.OverSampling = UART_OVERSAMPLING_16;
        _uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
        _uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
        if ( HAL_UART_Init( &_uart ) != HAL_OK )
            abort();

        HAL_NVIC_SetPriority( USART1_IRQn, 0, 0 );
        HAL_NVIC_EnableIRQ( USART1_IRQn );
    }

    void _setupGpio() {
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    }

    template < typename... Args >
    void _print( const char *fmt, Args...args ) {
        static constexpr int bufferSize = 512;
        static char buffer[ bufferSize ];

        int size = snprintf( buffer, bufferSize - 1, fmt, args... );
        buffer[ size ] = '\n';
        HAL_UART_Transmit( &_uart, reinterpret_cast< uint8_t *>( buffer ), size + 1, HAL_MAX_DELAY );
    }

    UART_HandleTypeDef _uart;
};