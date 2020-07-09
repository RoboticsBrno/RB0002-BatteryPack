#include <stm32f0xx_hal.h>
#include "statusBar.hpp"
#include "adc.hpp"
#include "qc.hpp"

void ErrorHandler() {
    Dbg::print( "A fatal error occured" );
    while( true );
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.HSI14CalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        ErrorHandler();

    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
        ErrorHandler();

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        ErrorHandler();
}

int main() {
    HAL_Init();
    SystemClock_Config();

    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_ADC1_CLK_ENABLE();


    StatusBar bar;
    AnalogPhy analogPhy;
    Qc2Control qcControl( { GPIOA, GPIO_PIN_4 }, { GPIOA, GPIO_PIN_5 } );

    Percents percentAnim( bar, 0 );
    BusVoltage voltageAnim( bar, 0 );

    SlidingAverage< float, 4 > busSlidingAvg;

    const int MODE_PERIOD = 5000;

    while ( true ) {
        auto tick = HAL_GetTick();
        float busVoltage = analogPhy.busVoltage();
        busSlidingAvg.push( busVoltage );
        busVoltage = busSlidingAvg.avg();
        qcControl.run( busVoltage, tick );
        if ( tick % MODE_PERIOD < MODE_PERIOD / 2 || busVoltage < 3 ) {
            percentAnim.set( batteryPercent( analogPhy.batt1Voltage() ) );
            percentAnim( tick );
        }
        else {
            voltageAnim.set( busVoltage );
            voltageAnim( tick );
        }
    }
}