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

    if ( CANDRV_FAILURE == mcp2515_reset_blocking() ) {

        return CANDRV_FAILURE;
    }

    if ( CANDRV_FAILURE == mcp2515_change_can_baudrate( MCP2515_CAN_BAUDRATE_1000KBPS ) ) {

        return CANDRV_FAILURE;
    }

    if ( CANDRV_FAILURE == mcp2515_change_tx_priority( CANDRV_TX_0, TX_PRIORITY_LOW ) ) {

        return CANDRV_FAILURE;
    }

    if ( CANDRV_FAILURE == mcp2515_change_tx_priority( CANDRV_TX_1, TX_PRIORITY_MIDDLE_LOW ) ) {

        return CANDRV_FAILURE;
    }

    if ( CANDRV_FAILURE == mcp2515_change_tx_priority( CANDRV_TX_2, TX_PRIORITY_MIDDLE_HIGH ) ) {

        return CANDRV_FAILURE;
    }

    if ( CANDRV_FAILURE == mcp2515_change_opr_mode_blocking( OPR_MODE_NORMAL ) ) {

        return CANDRV_FAILURE;
    }
    
    return CANDRV_SUCCESS;
}