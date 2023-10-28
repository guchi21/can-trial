#include "can_driver.h"
#include "hardware/timer.h"
#include "mcp2515.h"
#include "pico_can.h"

/* temp */
#ifndef MCP2515_REGISTER_H
#include "mcp2515_register.h"
#endif

#include <stdio.h>

candrv_result_t candrv_init() {

    bool to_be_continued = true;

    picocan_init();

    to_be_continued = ( to_be_continued && mcp2515_reset() ) ? true : false;

    to_be_continued = ( to_be_continued && mcp2515_set_baudrate( MCP2515_CAN_BAUDRATE_125KBPS ) ) ? true : false;

    if ( to_be_continued ) {

        // Begin trial implements
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXBCTRL_RXM, MASKOF_RXBCTRL_RXM );
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXB0CTRL_BUKT, (uint8_t)( ~MASKOF_RXB0CTRL_BUKT ) );
        // End trial implements

        set_irq_enabled( CANDRV_IRQ_ERR, true );
        set_irq_enabled( CANDRV_IRQ_MSGERR, true );
        set_irq_enabled( CANDRV_IRQ_RX0_FULL, true );
    }

    to_be_continued = ( to_be_continued && mcp2515_set_opr_mode( OPR_MODE_NORMAL ) ) ? true : false;

    return to_be_continued ? CANDRV_SUCCESS : CANDRV_FAILURE;
}


