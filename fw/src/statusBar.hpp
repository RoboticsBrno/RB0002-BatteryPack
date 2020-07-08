#pragma once

#include <stm32f0xx_hal.h>
#include <array>

#include "gpio.hpp"
#include "dbg.hpp"
#include "common.hpp"

class StatusBar {
public:
    StatusBar() {
        for ( auto pin : _leds ) {
            setupPushPull( pin );
        }
    }

    void set( int idx, bool state ) {
        HAL_GPIO_WritePin( _leds[ idx ].first, _leds[ idx ].second,
            GPIO_PinState( state ) );
    }

    int size() {
        return _leds.size();
    }

    void clear() {
        for ( unsigned int i = 0; i != _leds.size(); i++ )
            set( i, false );
    }
private:
    std::array< Pin, 6 > _leds{{
        { GPIOF, GPIO_PIN_0 },
        { GPIOF, GPIO_PIN_1 },
        { GPIOB, GPIO_PIN_1 },
        { GPIOA, GPIO_PIN_7 },
        { GPIOA, GPIO_PIN_9 },
        { GPIOA, GPIO_PIN_10 }
    }};
};

class Mode {
public:
    Mode( StatusBar& bar, int blinkPeriod = 500 ) :
        _bar( &bar ),
        _blinkPeriod( blinkPeriod )
    {}

    virtual void operator()( uint32_t tick ) = 0;
protected:
    void _blink( int idx, uint32_t tick ) {
        if ( idx >= _bar->size() )
            return;
        bool state = ( tick % _blinkPeriod ) < _blinkPeriod  / 2;
        _bar->set( idx, state );
    }

    StatusBar* _bar;
    unsigned int _blinkPeriod;
};

class Percents: public Mode {
public:
    Percents( StatusBar& bar, int percent ):
        Mode( bar ),
        _percent( percent )
    {}

    Percents( const Percents& ) = default;
    Percents& operator=(const Percents& ) = default;

    void set( int percent ) {
        _percent = percent;
    }

    void operator()( uint32_t tick ) override {
        _bar->clear();
        int percent = _percent > 90 ? 100 : _percent;
        int fillLevel = percent * 6 / 100;
        int blinkLevel = percent % ( 100 / 6 );
        if ( blinkLevel > (100 / 6 / 2 ) || fillLevel == 0 ) {
            _blink( fillLevel, tick );
        }
        _fillUp( fillLevel );
    };

private:
    void _blink( int idx, uint32_t tick ) {
        if ( idx >= _bar->size() )
            return;
        bool state = ( tick % _blinkPeriod ) < _blinkPeriod  / 2;
        _bar->set( idx, state );
    }

    void _fillUp( int idx ) {
        for ( int i = 0; i != idx; i++ ) {
            _bar->set( i, true );
        }
    }

    int _percent;
};

class BusVoltage: public Mode {
public:
    BusVoltage( StatusBar& bar, float voltage ) :
        Mode( bar ),
        _voltage( voltage )
    {}

    void set( float voltage ) {
        _voltage = voltage;
    }

    void operator()( uint32_t tick ) override {
        _bar->clear();
        if ( _voltage == Approx{ 12.0f, 2.0f } ) {
            _bar->set( 5, true );
        }
        else if ( _voltage == Approx{ 9.0f, 2.0f } ) {
            _bar->set( 4, true );
        }
        else if ( _voltage == Approx{ 5.0f, 2.0f } ) {
            _bar->set( 3, true );
        } else {
            _blink( 5, tick );
            _blink( 4, tick );
            _blink( 3, tick );
        }
    };

private:
    float _voltage;
};