//
// Created by stolij on 3/28/23.
//

#include "RegisterHandlers.h"
#include "Globals.h"

#include <TinyWireS.h>
#include <stddef.h>

namespace RegisterHandlers
{
    template <typename T>
    void WriteValue(const T &value)
    {
        for (size_t i = 0; i < sizeof(T); i++)
            TinyWireS::write(((uint8_t*)&value)[i]);
    }

    void HandleDistanceRegister()
    {
        WriteValue(DistanceTraversedX);
        WriteValue(DistanceTraversedY);
    }

    void HandleResetDistanceRegister(uint8_t *data)
    {
        DistanceTraversedX = 0;
        DistanceTraversedY = 0;
    }

    void NullHandler()
    { }
}