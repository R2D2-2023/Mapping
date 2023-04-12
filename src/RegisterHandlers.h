//
// Created by lukkens on 3/28/23.
//

#ifndef TI_R2D2_MOUSESENSORFIRMWARE_REGISTERHANDLERS_H
#define TI_R2D2_MOUSESENSORFIRMWARE_REGISTERHANDLERS_H

#include <stdint.h>

namespace RegisterHandlers
{
    void NullHandler();
    void HandleDistanceRegister();
    void HandleResetDistanceRegister(uint8_t *data);
}


#endif //TI_R2D2_MOUSESENSORFIRMWARE_REGISTERHANDLERS_H
