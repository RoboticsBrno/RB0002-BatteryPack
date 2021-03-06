#pragma once

#include <stm32f0xx_hal.h>
#include "dbg.hpp"
#include "sys.hpp"

/* #define HAL_CHECK(EXPR)                                               \
    do {                                                              \
        auto ret = EXPR;                                              \
        if ( ret != HAL_OK ) {                                        \
            Dbg::print("Failed '%s' with code: %d", #EXPR, ret );     \
            Dbg::print("Error: %x", HAL_ADC_GetError(& _adc ) );      \
            abort();                                                  \
        }                                                             \
    } while( false ); */

#define HAL_CHECK(EXPR) EXPR

template < typename T >
struct Approx {
    T val, eps;
};
template < typename T > Approx(T, T) -> Approx< T>;

template < typename T >
bool operator==( const T& val, Approx< T > a ) {
    return val >= a.val - a.eps && val <= a.val + a.eps;
}

template < typename T >
bool operator==( Approx< T > a, const T& val ) {
    return val >= a.val - a.eps && val <= a.val + a.eps;
}

template < typename T, int N >
class SlidingAverage {
public:
    SlidingAverage() {
        std::fill( _data.begin(), _data.end(), 0 );
    }

    void push( T sample ) {
        for ( unsigned int i = 1; i < _data.size(); i++ )
            _data[ i - 1 ] = _data[ i ];
        _data.back() = sample;
    }

    T avg() const {
        T sum = 0;
        for ( unsigned int i = 0; i != _data.size(); i++ )
            sum += _data[ i ];
        return sum / _data.size();
    }

private:
    std::array< T, N > _data;
};