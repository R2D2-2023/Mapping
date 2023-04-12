//
// Created by stolij on 3/28/23.
//

#include <Arduino.h>

#include "FCT3065.h"

#define PRODUCT_ID1 0x00
#define PRODUCT_ID2 0x01
#define MOTION_STATUS 0X02
#define DELTA_X 0x03
#define DELTA_Y 0x04
#define OPERATING_MODE 0x05
#define CONFIGURATION 0x06
#define IMAGE_QUALITY 0x07
#define OPERATING_STATE 0x08
#define WRITE_PROTECT 0x09
#define SLEEP1_SET 0x0A
#define ENTER_TIME 0x0B
#define SLEEP2_SET 0x0C
#define IMAGE_THRESHOLD 0x0D
#define IMAGE_RECOGNITION 0x0E
#define NO_SLEEP 0xA0

#define WRITE_BIT 0x80

namespace FCT3065
{
    Driver::Driver(uint8_t sclkPin, uint8_t sdioPin) : _sclkPin(sclkPin), _sdioPin(sdioPin)
    {
        pinMode(_sclkPin, OUTPUT);

        digitalWrite(_sclkPin, LOW);
        digitalWrite(_sclkPin, HIGH);
        delayMicroseconds(1);

        digitalWrite(_sclkPin, LOW);
        delay(320);
        digitalWrite(_sclkPin, HIGH);

        WriteRegister(OPERATING_MODE, NO_SLEEP);
    }

    uint8_t Driver::ReadRegister(uint8_t address)
    {
        pinMode(_sdioPin, OUTPUT);

        WriteByte(address);

        pinMode(_sdioPin, INPUT_PULLUP);

        return ReadByte();
    }

    void Driver::WriteRegister(uint8_t address, uint8_t value)
    {
        address |= WRITE_BIT;
        pinMode(_sdioPin, OUTPUT);

        WriteByte(address);
        WriteByte(value);
    }

    void Driver::WriteByte(uint8_t byte)
    {
        for (uint8_t i = 0x80; i > 0; i >>= 1)
        {
            digitalWrite(_sclkPin, LOW);
            digitalWrite(_sdioPin, (byte & i) != 0 ? HIGH : LOW);
            digitalWrite(_sclkPin, HIGH);
        }
    }

    uint8_t Driver::ReadByte()
    {
        uint8_t result = 0;
        for (byte i = 0x80; i > 0; i >>= 1)
        {
            digitalWrite(_sclkPin, LOW);
            digitalWrite(_sclkPin, HIGH);

            if (digitalRead(_sdioPin) == HIGH)
                result |= i;
        }

        return result;
    }

    MotionFlags Driver::GetMotion()
    {
        return static_cast<MotionFlags>(ReadRegister(MOTION_STATUS));
    }

    int8_t Driver::GetDeltaX()
    {
        return (int8_t)ReadRegister(DELTA_X);
    }

    int8_t Driver::GetDeltaY()
    {
        return (int8_t)ReadRegister(DELTA_Y);
    }

    void Driver::SetMotionFlags(MotionFlags flags)
    {
        WriteRegister(MOTION_STATUS, flags);
    }

    void Driver::SetOperatingModeFlags(OperatingModeFlags flags)
    {
        WriteRegister(OPERATING_MODE, flags);
    }

    void Driver::SetConfigurationFlags(ConfigurationFlags flags)
    {
        WriteRegister(CONFIGURATION, flags);
    }
}
