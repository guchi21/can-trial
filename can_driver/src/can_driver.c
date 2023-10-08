#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
#endif

candrv_result_t candrv_init() {

    rp2040_init_spi();

    if ( false == mcp2515_reset_blocking() ) {

        return CANDRV_FAILURE;
    }

    

    return CANDRV_SUCCESS;
}