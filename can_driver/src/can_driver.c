#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
#endif

/* temp */
#ifndef MCP2515_REG_ADDR_H
#include "mcp2515_reg_addr.h"
#endif
#ifndef _HARDWARE_TIMER_H
#include "hardware/timer.h"
#endif
#ifndef MCP2515_IRQ_H
#include "mcp2515_irq.h"
#endif

#include <stdio.h>

static void set_irq_enabled( enum CANDRV_IRQ irq, bool enabled ) {

    const enum CANDRV_IRQ irq_table[ CANDRV_IRQ_NUMOF_ITEMS ] = {
        MASKOF_CANINTE_RX0IF, MASKOF_CANINTE_RX1IF, MASKOF_CANINTE_TX0IF, MASKOF_CANINTE_TX1IF,
        MASKOF_CANINTE_TX2IF, MASKOF_CANINTE_ERRIF, MASKOF_CANINTE_WAKIF, MASKOF_CANINTE_MERRF
    };

    uint8_t mask;
    uint8_t val;

    if ( CANDRV_IRQ_MINOF_IDX <= irq && CANDRV_IRQ_MAXOF_IDX >= irq ) {

        mask = irq_table[ irq ];

        val = ( true == enabled ) ? mask : (uint8_t)( ~mask );

        mcp2515_modbits_register( REG_CANINTE_FLGS, mask, val );
    }
}

candrv_result_t candrv_init() {

    bool to_be_continued = true;

    rp2040_init_spi();

    to_be_continued = ( to_be_continued && mcp2515_reset() ) ? true : false;

    to_be_continued = ( to_be_continued && mcp2515_change_can_baudrate( MCP2515_CAN_BAUDRATE_125KBPS ) ) ? true : false;

    if ( to_be_continued ) {

        // Begin trial implements
        mcp2515_modbits_register( REG_RXB0CTRL_FLGS, MASKOF_RXB0CTRL_RXM, MASKOF_RXB0CTRL_RXM );
        mcp2515_modbits_register( REG_RXB0CTRL_FLGS, MASKOF_RXB0CTRL_BUKT, (uint8_t)( ~MASKOF_RXB0CTRL_BUKT ) );
        // End trial implements

        rp2040_init_irq();

        set_irq_enabled( CANDRV_IRQ_RX0_FULL, true );
        set_irq_enabled( CANDRV_IRQ_TX0_EMPTY, true );
    }

    to_be_continued = ( to_be_continued && mcp2515_change_opr_mode( OPR_MODE_NORMAL ) ) ? true : false;

    return to_be_continued ? CANDRV_SUCCESS : CANDRV_FAILURE;
}

void candrv_tmp_clr_rx0( void ) {

    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_RX0IF, 0U );
}


void candrv_tmp_clr_tx0( void ) {

    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_TX0IF, 0U );
}

candrv_result_t candrv_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg ) {

    return mcp2515_get_rx_msg( rx_idx, msg );
}

candrv_result_t candrv_req_send_msg( const enum CANDRV_TX tx_idx ) {

    return mcp2515_req_send_msg( tx_idx );
}

candrv_result_t candrv_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg ) {

    return mcp2515_set_tx_msg( tx_idx, msg );
}


can_irq_callback_t irq_callback = NULL;

void candrv_set_irq_callback( const can_irq_callback_t callback ) {

    irq_callback = callback;
}

can_irq_callback_t candrv_get_irq_callback( void ) {

    return irq_callback;
}


uint8_t candrv_tmp_get_irq_reason( void ) {

    return (uint8_t)( mcp2515_read_register( REG_CANSTAT ) & MASKOF_CANSTAT_ICOD );
}

void mcp2515_irq_callback( void ) {
    
    if ( NULL != irq_callback ) {

        irq_callback();
    }
}
