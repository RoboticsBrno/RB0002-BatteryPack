#pragma once

#include "gpio.hpp"
#include "common.hpp"

class Qc2Control {
    enum State { IDLE, HANDSHAKE, VOLTAGE9, VOLTAGE12 };
public:
    Qc2Control( Pin dPlusPin, Pin dMinusPin ):
        _dPlus( dPlusPin ), _dMinus( dMinusPin )
    {}

    void run( float inputVoltage, uint32_t tick ) {
        switch ( _state ) {
        case IDLE:
            onIdle( inputVoltage, tick );
            break;
        case HANDSHAKE:
            onHandshake( inputVoltage, tick );
            break;
        case VOLTAGE9:
            onVoltage9( inputVoltage, tick );
            break;
        case VOLTAGE12:
            onVoltage12( inputVoltage, tick );
            break;
        }
    }
private:
    void onIdle( float inputVoltage, uint32_t tick ) {
        setupInput( _dPlus );
        setupInput( _dMinus );
        if ( inputVoltage == Approx{ 5.0f, 1.0f } ) {
            _ticks = tick;
            _state = State::HANDSHAKE;
        }
    }

    void onHandshake( float inputVoltage, uint32_t tick ) {
        if ( inputVoltage < 3 ) {
            _state = State::IDLE;
            return;
        }
        if ( tick - _ticks > 1500 ) {
            setupPushPull( _dMinus );
            HAL_GPIO_WritePin( _dMinus.first, _dMinus.second, GPIO_PinState( false ) );
            HAL_Delay( 100 );
            _state = State::VOLTAGE12;
            _ticks = tick;
            return;
        }
    }

    void onVoltage9( float inputVoltage, uint32_t tick ) {
        if ( inputVoltage < 3 ) {
            _state = State::IDLE;
            return;
        }
        if ( tick - _ticks > 2000 && inputVoltage < 7 ) {
            _state = State::IDLE;
            return;
        }
        setupInput( _dMinus );
        setupPushPull( _dPlus );
        HAL_GPIO_WritePin( _dPlus.first, _dPlus.second, GPIO_PinState( true ) );
    }

    void onVoltage12( float inputVoltage, uint32_t tick ) {
        if ( inputVoltage < 3 ) {
            _state = State::IDLE;
            return;
        }
        if ( tick - _ticks > 2000 && inputVoltage < 10 ) {
            _state = State::VOLTAGE9;
            return;
        }
        setupInput( _dMinus );
        setupInput( _dPlus );
    }

    Pin _dPlus, _dMinus;
    uint32_t _ticks;
    State _state = IDLE;
};