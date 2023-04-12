//
// Created by lukkens on 3/28/23.
//

#ifndef TI_R2D2_MOUSESENSORFIRMWARE_GLOBALS_H
#define TI_R2D2_MOUSESENSORFIRMWARE_GLOBALS_H

#include <stdint.h>

namespace FCT3065
{
    class Driver;
}

extern FCT3065::Driver *MouseSensor;

extern int32_t DistanceTraversedX;
extern int32_t DistanceTraversedY;

#endif //TI_R2D2_MOUSESENSORFIRMWARE_GLOBALS_H
