/*==============================================================================================================*
 
    @file     TinyWireS.cpp
    @license  MIT (c) 2016 Nadav Matalon

    ATtiny I2C Slave Library

    Ver. 1.0.0 - First release (1.12.16)
 
 *===============================================================================================================*
    LICENSE
 *===============================================================================================================*

    The MIT License (MIT)
    Copyright (c) 2016 Nadav Matalon

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
    documentation files (the "Software"), to deal in the Software without restriction, including without
    limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial
    portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
    LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
 *==============================================================================================================*/

extern "C"
{
    #include <inttypes.h>
    #include <avr/interrupt.h>
    #include "utility/USI_TWI_Slave.h"
}

#include "Arduino.h"
#include "TinyWireS.h"

// initialize I2C lib
void TinyWireS::begin(uint8_t slaveAddr)
{
    usiTwiSlaveInit(slaveAddr);
}

// send it back to master
void TinyWireS::write(uint8_t data)
{
    usiTwiTransmitByte(data);
}

// the bytes available that haven't been read yet
uint8_t TinyWireS::available()
{
    return usiTwiAmountDataInReceiveBuffer();
}

// returns the bytes received one at a time
uint8_t TinyWireS::read()
{
    return usiTwiReceiveByte();
}

size_t TinyWireS::read(uint8_t *output, size_t count)
{
    size_t readCount = 0;
    for (; usiTwiAmountDataInReceiveBuffer() > 0 && count > 0; count--, readCount++)
        (*output++) = usiTwiReceiveByte();

    return readCount;
}

// sets function called on slave write
void TinyWireS::setWriteCallback(void (*function)(uint8_t))
{
    usi_onReceiverPtr = function;
}

// sets function called on slave read
void TinyWireS::setReadCallback(void (*function)(void))
{
    usi_onRequestPtr = function;
}

void TinyWireS_stop_check()
{
    if (!usi_onReceiverPtr) return;                         // no onReceive callback, nothing to do...
    if (!(USISR & (1 << USIPF))) return;                    // Stop not detected
    uint8_t amount = usiTwiAmountDataInReceiveBuffer();
    if (amount == 0) return;                                // no data in buffer
    usi_onReceiverPtr(amount);
}

void tws_delay(unsigned long ms)
{
    auto start = (uint16_t)micros();
    while (ms > 0)
    {
        TinyWireS_stop_check();
        if (((uint16_t)micros() - start) >= 1000)
        {
            ms--;
            start += 1000;
        }
    }
}
