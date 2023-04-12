//
// Created by stolij on 3/28/23.
//

#ifndef TI_R2D2_MOUSESENSORFIRMWARE_FCT3065_H
#define TI_R2D2_MOUSESENSORFIRMWARE_FCT3065_H


#include <stdint.h>

namespace FCT3065
{
    struct Vector2
    {
        Vector2(int8_t x, int8_t y)
        : X(x), Y(y)
        {}

        int8_t X;
        int8_t Y;
    };

    enum MotionFlags : uint8_t
    {
        RESERVED = 0b01100111,

        DELTA_X_Overflow =0b00001000,
        DELTA_Y_Overflow =0b00010000,
        MOTION_Detected =0b10000000,
    };

    enum OperatingModeFlags : uint8_t
    {
        STATIC_BITS = 0b00100000,

        WAKEUP = STATIC_BITS | 0b00000001,
        ENTER_Sleep1 = STATIC_BITS | 0b00000010,
        ENTER_Sleep2 = STATIC_BITS | 0b00000100,
        AUTO_ENTER_Sleep2 = STATIC_BITS | 0b00001000,
        AUTO_ENTER_Sleep1 = STATIC_BITS | 0b00010000,
        LED_SHUTTER_Enable = STATIC_BITS | 0b10000000
    };

    enum ConfigurationFlags : uint8_t
    {
        POWER_DOWN_Mode = 0b00001000,
        MOTSWK_Config = 0b01000000,
        RESET = 0b10000000
    };

    class Driver
    {
    public:
        Driver(uint8_t sclkPin, uint8_t sdioPin);

        MotionFlags GetMotion();

        int8_t GetDeltaX();
        int8_t GetDeltaY();

        inline Vector2 GetDelta()
        {
            return { GetDeltaX(), GetDeltaY() };
        }

        void SetMotionFlags(MotionFlags motionFlags);
        void SetOperatingModeFlags(OperatingModeFlags flags);
        void SetConfigurationFlags(ConfigurationFlags flags);

    private:
        uint8_t ReadRegister(uint8_t address);

        void WriteRegister(uint8_t address, uint8_t value);

        void WriteByte(uint8_t byte);

        uint8_t ReadByte();

        uint8_t _sclkPin;
        uint8_t _sdioPin;
    };
}


#endif //TI_R2D2_MOUSESENSORFIRMWARE_FCT3065_H
