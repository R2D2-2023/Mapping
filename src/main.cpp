#include <Arduino.h>

#include <TinyWireS.h>
#include <FCT3065.h>

#include "RegisterHandlers.h"

#define I2C_Address 0x24

#define REGISTER_COUNT 1

#define AUTO_INCREMENT_REGISTER_MAX 1

#define MOUSE_SCLK_Pin PB4
#define MOUSE_SDIO_Pin PB3

struct RegisterDefinition
{
    void (*ReadHandlerFunc)();
    void (*WriteHandlerFunc)(uint8_t *);
    size_t WriteDataLength;
};

RegisterDefinition RegisterDefinitions[REGISTER_COUNT] = {
        [0] = {
                .ReadHandlerFunc = RegisterHandlers::HandleDistanceRegister,
                .WriteHandlerFunc = RegisterHandlers::HandleResetDistanceRegister,
                .WriteDataLength = 1
        },
};

FCT3065::Driver *MouseSensor;

int32_t DistanceTraversedX = 0;
int32_t DistanceTraversedY = 0;

uint8_t CurrentRegister = 0;

void OnReadHandler()
{
    if (CurrentRegister >= AUTO_INCREMENT_REGISTER_MAX)
        CurrentRegister = 0;

    RegisterDefinitions[CurrentRegister++].ReadHandlerFunc();
}

void OnWriteHandler(uint8_t reg)
{
    if (reg >= REGISTER_COUNT)
        return;

    CurrentRegister = reg;

    auto &registerDefinition = RegisterDefinitions[reg];
    if (TinyWireS::available() != registerDefinition.WriteDataLength)
        return;

    uint8_t data[registerDefinition.WriteDataLength];
    if (TinyWireS::read(data, registerDefinition.WriteDataLength) == registerDefinition.WriteDataLength)
        registerDefinition.WriteHandlerFunc(data);
}

void setup()
{
    TinyWireS::begin(I2C_Address);

    TinyWireS::setReadCallback(OnReadHandler);
    TinyWireS::setWriteCallback(OnWriteHandler);

    MouseSensor = new FCT3065::Driver(MOUSE_SCLK_Pin, MOUSE_SDIO_Pin);
}

void loop()
{
    if (MouseSensor->GetMotion())
    {
        auto delta = MouseSensor->GetDelta();
        DistanceTraversedX += delta.X;
        DistanceTraversedY += delta.Y;
    }

    delay(10);
}