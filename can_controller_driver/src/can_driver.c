#include "can_driver.h"
#include "hardware/timer.h"
#include "mcp2515.h"
#include "hardware_manager.h"

/* temp */
#ifndef MCP2515_REGISTER_H
#include "mcp2515_register.h"
#endif

#include <stdio.h>

candrv_result_t candrv_init() {

    bool to_be_continued = true;

    picocan_init();

    to_be_continued = ( to_be_continued && mcp2515_reset() ) ? true : false;

    to_be_continued = ( to_be_continued && mcp2515_set_baudrate( MCP2515_BAUDRATE_1000KBPS ) ) ? true : false;

    if ( to_be_continued ) {

        // Begin trial implements
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXBCTRL_RXM, MASKOF_RXBCTRL_RXM );

        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_MERRF | MASKOF_CANINT_ERRIF | MASKOF_CANINT_RX0IF | MASKOF_CANINT_RX1IF, 0xFFU );
        // End trial implements

    }

    to_be_continued = ( to_be_continued && hw_set_irq_callback( HW_IRQ_CAUSE_CAN_CONTROLLER, mcp2515_cbk_can_controller ) );

    to_be_continued = ( to_be_continued && mcp2515_set_opmode( MCP2515_OPMODE_NORMAL ) ) ? true : false;

    return to_be_continued ? CD_SUCCESS : CD_FAILURE;
}

candrv_result_t candrv_get_rx_msg( const enum CD_RX rx_idx, can_msg_t *const msg ) {

    return mcp2515_get_rx_msg( rx_idx, msg );
}

candrv_result_t candrv_set_tx_msg( const enum CD_TX tx_idx,
    const can_msg_t *const msg, const enum CD_TX_PRIORITY priority ) {

    return mcp2515_set_tx_msg( tx_idx, msg, priority );
}

candrv_result_t candrv_set_send_req( const enum CD_TX tx_idx ) {

    return mcp2515_set_send_req( tx_idx );
}
candrv_result_t candrv_clr_send_req( const enum CD_TX tx_idx ) {

    return mcp2515_clr_send_req( tx_idx );
}

uint8_t candrv_get_numof_tx_err( void ) {

    return mcp2515_get_numof_tx_err();
}
uint8_t candrv_get_numof_rx_err( void ) {

    return mcp2515_get_numof_rx_err();
}

enum CD_TX candrv_get_available_tx( void ) {

    for ( enum CD_TX i = CD_TX_MINOF_IDX; CD_TX_NUMOF_ITEMS > i; i++ ) {

        if ( true ==  mcp2515_is_tx_available( i ) )
            return i;
    }

    return CD_TX_INVALID;
}