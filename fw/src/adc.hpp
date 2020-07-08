#pragma once

#include <stm32f0xx_hal.h>
#include "common.hpp"

template < typename T >
T clamp( T min, T val, T max ) {
    if ( val < min )
        return min;
    if ( val > max )
        return max;
    return val;
}

int batteryPercent( float voltage ) {
    const int max = 4.1;
    const int min = 3.7;

    int percent = 100 * (voltage - min) / (max - min);
    return clamp( 0, percent, 100 );
}

class AnalogPhy {
public:
    AnalogPhy() {
        _setupGpio();
        _setupAdc();
    }

    float busVoltage() {
        _readSamples();
        return dividerInversion( 62.f, 10.f ) * 3.3f * _busSample / 4095.f;
    }

    float batt1Voltage() {
        _readSamples();
        return dividerInversion( 10.f, 20.f ) * 3.3f * _batSamples[ 0 ] / 4095.f;
    }

    float batt2Voltage() {
        _readSamples();
        return dividerInversion( 10.f, 20.f ) * 3.3f * _batSamples[ 1 ] / 4095.f;
    }
private:
    void _setupGpio() {
        GPIO_InitTypeDef GPIO_InitStruct = { 0 };

        GPIO_InitStruct.Pin = GPIO_PIN_0  |GPIO_PIN_1 | GPIO_PIN_6;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );
    }

    void _setupAdc() {
        _adc.Instance = ADC1;
        _adc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
        _adc.Init.Resolution = ADC_RESOLUTION_12B;
        _adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        _adc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
        _adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        _adc.Init.LowPowerAutoWait = DISABLE;
        _adc.Init.LowPowerAutoPowerOff = DISABLE;
        _adc.Init.ContinuousConvMode = DISABLE;
        _adc.Init.DiscontinuousConvMode = ENABLE;
        _adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        _adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        _adc.Init.DMAContinuousRequests = DISABLE;
        _adc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
        HAL_CHECK( HAL_ADC_Init( &_adc ) );

        ADC_ChannelConfTypeDef sConfig = { 0 };

        sConfig.Channel = ADC_CHANNEL_0;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
        if ( HAL_ADC_ConfigChannel( &_adc, &sConfig) != HAL_OK )
            abort();

        sConfig.Channel = ADC_CHANNEL_1;
         if ( HAL_ADC_ConfigChannel( &_adc, &sConfig) != HAL_OK )
            abort();

        sConfig.Channel = ADC_CHANNEL_6;
        if ( HAL_ADC_ConfigChannel( &_adc, &sConfig) != HAL_OK )
            abort();
    }

    void _readSamples() {
        HAL_CHECK( HAL_ADC_Start( &_adc ) );
        HAL_CHECK( HAL_ADC_PollForConversion( &_adc, HAL_MAX_DELAY ) );
        _busSample = HAL_ADC_GetValue( &_adc );
        HAL_CHECK( HAL_ADC_Start( &_adc ) );
        HAL_CHECK( HAL_ADC_PollForConversion( &_adc, HAL_MAX_DELAY ) );
        _batSamples[ 0 ] = HAL_ADC_GetValue( &_adc );
        HAL_CHECK( HAL_ADC_Start( &_adc ) );
        HAL_CHECK( HAL_ADC_PollForConversion( &_adc, HAL_MAX_DELAY ) );
        _batSamples[ 1 ] = HAL_ADC_GetValue( &_adc );
    }

    static float dividerInversion( float top, float bottom ) {
        return ( top + bottom ) / bottom;
    }

    ADC_HandleTypeDef _adc = {0};
    uint32_t _busSample;
    std::array< uint32_t, 2 > _batSamples;
};