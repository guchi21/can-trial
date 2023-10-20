#include "can_driver.h"
#include "hardware/timer.h"
#include "mcp2515.h"
#include "pico_can.h"

/* temp */
#ifndef MCP2515_REG_ADDR_H
#include "mcp2515_reg_addr.h"
#endif

#include <stdio.h>

static void set_irq_enabled( enum CANDRV_IRQ irq, bool enabled ) {

    const enum CANDRV_IRQ irq_table[ CANDRV_IRQ_NUMOF_ITEMS ] = {
        MASKOF_CANINT_RX0IF, MASKOF_CANINT_RX1IF, MASKOF_CANINT_TX0IF, MASKOF_CANINT_TX1IF,
        MASKOF_CANINT_TX2IF, MASKOF_CANINT_ERRIF, MASKOF_CANINT_WAKIF, MASKOF_CANINT_MERRF
    };

    uint8_t mask;
    uint8_t val;

    if ( CANDRV_IRQ_MINOF_IDX <= irq && CANDRV_IRQ_MAXOF_IDX >= irq ) {

        mask = irq_table[ irq ];

        val = ( true == enabled ) ? mask : (uint8_t)( ~mask );

        mcp2515_modbits_register( REG_CANINTE, mask, val );
    }
}

candrv_result_t candrv_init() {

    bool to_be_continued = true;

    picocan_init();

    to_be_continued = ( to_be_continued && mcp2515_reset() ) ? true : false;

    to_be_continued = ( to_be_continued && mcp2515_set_baudrate( MCP2515_CAN_BAUDRATE_125KBPS ) ) ? true : false;

    if ( to_be_continued ) {

        // Begin trial implements
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXB0CTRL_RXM, MASKOF_RXB0CTRL_RXM );
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXB0CTRL_BUKT, (uint8_t)( ~MASKOF_RXB0CTRL_BUKT ) );
        // End trial implements

        set_irq_enabled( CANDRV_IRQ_ERR, true );
        set_irq_enabled( CANDRV_IRQ_MSGERR, true );
        set_irq_enabled( CANDRV_IRQ_RX0_FULL, true );
    }

    to_be_continued = ( to_be_continued && mcp2515_set_opr_mode( OPR_MODE_NORMAL ) ) ? true : false;

    return to_be_continued ? CANDRV_SUCCESS : CANDRV_FAILURE;
}

void candrv_tmp_clr_rx0( void ) {

    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );
}


void candrv_tmp_clr_tx0( void ) {

    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX0IF, 0U );
}

candrv_result_t candrv_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg ) {

    return mcp2515_get_rx_msg( rx_idx, msg );
}

candrv_result_t candrv_req_send_msg( const enum CANDRV_TX tx_idx ) {

    return mcp2515_set_send_req( tx_idx );
}

candrv_result_t candrv_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg ) {

    return mcp2515_set_tx_msg( tx_idx, msg, CANDRV_TX_PRIORITY_MIDLOW );
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

void test( void ) {
    // printf("REG_TXRTSCTRL       %#X\n", mcp2515_read_register(REG_TXRTSCTRL       ) );
    // printf("REG_CANSTAT         %#X\n", mcp2515_read_register(REG_CANSTAT         ) );
    // printf("REG_CANCTRL         %#X\n", mcp2515_read_register(REG_CANCTRL         ) );
    // printf("REG_RXF3SIDH        %#X\n", mcp2515_read_register(REG_RXF3SIDH        ) );
    // printf("REG_RXF3SIDL        %#X\n", mcp2515_read_register(REG_RXF3SIDL        ) );
    // printf("REG_RXF3EID8        %#X\n", mcp2515_read_register(REG_RXF3EID8        ) );
    // printf("REG_RXF3EID0        %#X\n", mcp2515_read_register(REG_RXF3EID0        ) );
    // printf("REG_RXF4SIDH        %#X\n", mcp2515_read_register(REG_RXF4SIDH        ) );
    // printf("REG_RXF4SIDL        %#X\n", mcp2515_read_register(REG_RXF4SIDL        ) );
    // printf("REG_RXF4EID8        %#X\n", mcp2515_read_register(REG_RXF4EID8        ) );
    // printf("REG_RXF4EID0        %#X\n", mcp2515_read_register(REG_RXF4EID0        ) );
    // printf("REG_RXF5SIDH        %#X\n", mcp2515_read_register(REG_RXF5SIDH        ) );
    // printf("REG_RXF5SIDL        %#X\n", mcp2515_read_register(REG_RXF5SIDL        ) );
    // printf("REG_RXF5EID8        %#X\n", mcp2515_read_register(REG_RXF5EID8        ) );
    // printf("REG_RXF5EID0        %#X\n", mcp2515_read_register(REG_RXF5EID0        ) );
    // printf("REG_TEC             %#X\n", mcp2515_read_register(REG_TEC             ) );
    // printf("REG_REC             %#X\n", mcp2515_read_register(REG_REC             ) );
    // printf("REG_RXM0SIDH        %#X\n", mcp2515_read_register(REG_RXM0SIDH        ) );
    // printf("REG_RXM0SIDL        %#X\n", mcp2515_read_register(REG_RXM0SIDL        ) );
    // printf("REG_RXM0EID8        %#X\n", mcp2515_read_register(REG_RXM0EID8        ) );
    // printf("REG_RXM0EID0        %#X\n", mcp2515_read_register(REG_RXM0EID0        ) );
    // printf("REG_RXM1SIDH        %#X\n", mcp2515_read_register(REG_RXM1SIDH        ) );
    // printf("REG_RXM1SIDL        %#X\n", mcp2515_read_register(REG_RXM1SIDL        ) );
    // printf("REG_RXM1EID8        %#X\n", mcp2515_read_register(REG_RXM1EID8        ) );
    // printf("REG_RXM1EID0        %#X\n", mcp2515_read_register(REG_RXM1EID0        ) );
    // printf("REG_CNF3            %#X\n", mcp2515_read_register(REG_CNF3            ) );
    // printf("REG_CNF2            %#X\n", mcp2515_read_register(REG_CNF2            ) );
    // printf("REG_CNF1            %#X\n", mcp2515_read_register(REG_CNF1            ) );
    printf("REG_CANINTE         %#X\n", mcp2515_read_register(REG_CANINTE         ) );
    printf("REG_CANINTF         %#X\n", mcp2515_read_register(REG_CANINTF         ) );
    printf("REG_EFLG            %#X\n", mcp2515_read_register(REG_EFLG            ) );
    printf("REG_TXB0CTRL        %#X\n", mcp2515_read_register(REG_TXB0CTRL        ) );
    // printf("REG_TXB0SIDH        %#X\n", mcp2515_read_register(REG_TXB0SIDH        ) );
    // printf("REG_TXB0SIDL        %#X\n", mcp2515_read_register(REG_TXB0SIDL        ) );
    // printf("REG_TXB0EID8        %#X\n", mcp2515_read_register(REG_TXB0EID8        ) );
    // printf("REG_TXB0EID0        %#X\n", mcp2515_read_register(REG_TXB0EID0        ) );
    // printf("REG_TXB0DLC         %#X\n", mcp2515_read_register(REG_TXB0DLC         ) );
    // printf("REG_TXB0D0          %#X\n", mcp2515_read_register(REG_TXB0D0          ) );
    // printf("REG_TXB0D1          %#X\n", mcp2515_read_register(REG_TXB0D1          ) );
    // printf("REG_TXB0D2          %#X\n", mcp2515_read_register(REG_TXB0D2          ) );
    // printf("REG_TXB0D3          %#X\n", mcp2515_read_register(REG_TXB0D3          ) );
    // printf("REG_TXB0D4          %#X\n", mcp2515_read_register(REG_TXB0D4          ) );
    // printf("REG_TXB0D5          %#X\n", mcp2515_read_register(REG_TXB0D5          ) );
    // printf("REG_TXB0D6          %#X\n", mcp2515_read_register(REG_TXB0D6          ) );
    // printf("REG_TXB0D7          %#X\n", mcp2515_read_register(REG_TXB0D7          ) );
    // printf("REG_TXB1CTRL        %#X\n", mcp2515_read_register(REG_TXB1CTRL        ) );
    // printf("REG_TXB1SIDH        %#X\n", mcp2515_read_register(REG_TXB1SIDH        ) );
    // printf("REG_TXB1SIDL        %#X\n", mcp2515_read_register(REG_TXB1SIDL        ) );
    // printf("REG_TXB1EID8        %#X\n", mcp2515_read_register(REG_TXB1EID8        ) );
    // printf("REG_TXB1EID0        %#X\n", mcp2515_read_register(REG_TXB1EID0        ) );
    // printf("REG_TXB1DLC         %#X\n", mcp2515_read_register(REG_TXB1DLC         ) );
    // printf("REG_TXB1D0          %#X\n", mcp2515_read_register(REG_TXB1D0          ) );
    // printf("REG_TXB1D1          %#X\n", mcp2515_read_register(REG_TXB1D1          ) );
    // printf("REG_TXB1D2          %#X\n", mcp2515_read_register(REG_TXB1D2          ) );
    // printf("REG_TXB1D3          %#X\n", mcp2515_read_register(REG_TXB1D3          ) );
    // printf("REG_TXB1D4          %#X\n", mcp2515_read_register(REG_TXB1D4          ) );
    // printf("REG_TXB1D5          %#X\n", mcp2515_read_register(REG_TXB1D5          ) );
    // printf("REG_TXB1D6          %#X\n", mcp2515_read_register(REG_TXB1D6          ) );
    // printf("REG_TXB1D7          %#X\n", mcp2515_read_register(REG_TXB1D7          ) );
    // printf("REG_TXB2CTRL        %#X\n", mcp2515_read_register(REG_TXB2CTRL        ) );
    // printf("REG_TXB2SIDH        %#X\n", mcp2515_read_register(REG_TXB2SIDH        ) );
    // printf("REG_TXB2SIDL        %#X\n", mcp2515_read_register(REG_TXB2SIDL        ) );
    // printf("REG_TXB2EID8        %#X\n", mcp2515_read_register(REG_TXB2EID8        ) );
    // printf("REG_TXB2EID0        %#X\n", mcp2515_read_register(REG_TXB2EID0        ) );
    // printf("REG_TXB2DLC         %#X\n", mcp2515_read_register(REG_TXB2DLC         ) );
    // printf("REG_TXB2D0          %#X\n", mcp2515_read_register(REG_TXB2D0          ) );
    // printf("REG_TXB2D1          %#X\n", mcp2515_read_register(REG_TXB2D1          ) );
    // printf("REG_TXB2D2          %#X\n", mcp2515_read_register(REG_TXB2D2          ) );
    // printf("REG_TXB2D3          %#X\n", mcp2515_read_register(REG_TXB2D3          ) );
    // printf("REG_TXB2D4          %#X\n", mcp2515_read_register(REG_TXB2D4          ) );
    // printf("REG_TXB2D5          %#X\n", mcp2515_read_register(REG_TXB2D5          ) );
    // printf("REG_TXB2D6          %#X\n", mcp2515_read_register(REG_TXB2D6          ) );
    // printf("REG_TXB2D7          %#X\n", mcp2515_read_register(REG_TXB2D7          ) );
    // printf("REG_RXB0CTRL        %#X\n", mcp2515_read_register(REG_RXB0CTRL        ) );
    // printf("REG_RXB0SIDH        %#X\n", mcp2515_read_register(REG_RXB0SIDH        ) );
    // printf("REG_RXB0SIDL        %#X\n", mcp2515_read_register(REG_RXB0SIDL        ) );
    // printf("REG_RXB0EID8        %#X\n", mcp2515_read_register(REG_RXB0EID8        ) );
    // printf("REG_RXB0EID0        %#X\n", mcp2515_read_register(REG_RXB0EID0        ) );
    // printf("REG_RXB0DLC         %#X\n", mcp2515_read_register(REG_RXB0DLC         ) );
    printf("REG_RXB0D0          %#X\n", mcp2515_read_register(REG_RXB0D0          ) );
    printf("REG_RXB0D1          %#X\n", mcp2515_read_register(REG_RXB0D1          ) );
    printf("REG_RXB0D2          %#X\n", mcp2515_read_register(REG_RXB0D2          ) );
    printf("REG_RXB0D3          %#X\n", mcp2515_read_register(REG_RXB0D3          ) );
    printf("REG_RXB0D4          %#X\n", mcp2515_read_register(REG_RXB0D4          ) );
    printf("REG_RXB0D5          %#X\n", mcp2515_read_register(REG_RXB0D5          ) );
    printf("REG_RXB0D6          %#X\n", mcp2515_read_register(REG_RXB0D6          ) );
    printf("REG_RXB0D7          %#X\n", mcp2515_read_register(REG_RXB0D7          ) );
    // printf("REG_RXB1CTRL        %#X\n", mcp2515_read_register(REG_RXB1CTRL        ) );
    // printf("REG_RXB1SIDH        %#X\n", mcp2515_read_register(REG_RXB1SIDH        ) );
    // printf("REG_RXB1SIDL        %#X\n", mcp2515_read_register(REG_RXB1SIDL        ) );
    // printf("REG_RXB1EID8        %#X\n", mcp2515_read_register(REG_RXB1EID8        ) );
    // printf("REG_RXB1EID0        %#X\n", mcp2515_read_register(REG_RXB1EID0        ) );
    // printf("REG_RXB1DLC         %#X\n", mcp2515_read_register(REG_RXB1DLC         ) );
    // printf("REG_RXB1D0          %#X\n", mcp2515_read_register(REG_RXB1D0          ) );
    // printf("REG_RXB1D1          %#X\n", mcp2515_read_register(REG_RXB1D1          ) );
    // printf("REG_RXB1D2          %#X\n", mcp2515_read_register(REG_RXB1D2          ) );
    // printf("REG_RXB1D3          %#X\n", mcp2515_read_register(REG_RXB1D3          ) );
    // printf("REG_RXB1D4          %#X\n", mcp2515_read_register(REG_RXB1D4          ) );
    // printf("REG_RXB1D5          %#X\n", mcp2515_read_register(REG_RXB1D5          ) );
    // printf("REG_RXB1D6          %#X\n", mcp2515_read_register(REG_RXB1D6          ) );
    // printf("REG_RXB1D7          %#X\n", mcp2515_read_register(REG_RXB1D7          ) );
}