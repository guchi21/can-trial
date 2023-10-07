#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
#endif

bool candrv_init() {

    if ( false == rp2040_init_spi(); ) {

        return CANDRV_FAILURE;
    }

    

    return CANDRV_SUCCESS;
}