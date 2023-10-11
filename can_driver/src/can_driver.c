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

#include <stdio.h>

candrv_result_t candrv_init() {

    bool to_be_continued = true;

    rp2040_init_spi();

    to_be_continued = ( to_be_continued && mcp2515_reset_blocking() ) ? true : false;

    to_be_continued = ( to_be_continued && mcp2515_change_can_baudrate( MCP2515_CAN_BAUDRATE_125KBPS ) ) ? true : false;

    // Begin trial implements
    mcp2515_modbits_register( REG_RXB0CTRL_FLGS, MASKOF_RXB0CTRL_RXM, MASKOF_RXB0CTRL_RXM );
    mcp2515_modbits_register( REG_RXB0CTRL_FLGS, MASKOF_RXB0CTRL_BUKT, (uint8_t)( ~MASKOF_RXB0CTRL_BUKT ) );
    // End trial implements

    if ( to_be_continued ) {

        candrv_set_irq_enabled( CANDRV_IRQ_RX0_FULL, true );
    }

    to_be_continued = ( to_be_continued && mcp2515_change_opr_mode_blocking( OPR_MODE_NORMAL ) ) ? true : false;

    return to_be_continued ? CANDRV_SUCCESS : CANDRV_FAILURE;
}

void candrv_tmp_clr_rx0( void ) {

    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_RX0IF, 0 );
}

void candrv_tmp1(can_message_t *msg) {

    candrv_get_rx_msg( CANDRV_RX_0, msg );

    // clear
    mcp2515_modbits_register( REG_CANINTF_FLGS, 0xFF, 0x0 );
}

bool candrv_tmp2() {

    uint8_t v = mcp2515_read_register( REG_CANINTF_FLGS );
    
    // is read
    return (v & 0x1);
}

void candrv_tmp3() {

    mcp2515_write_register( REG_EFLG_FLGS, 0x00 );
    mcp2515_modbits_register( REG_CANINTF_FLGS, 0xFF, 0x0 );

    // printf("CANSTAT=%#x.", mcp2515_read_register( REG_CANSTAT ) );
    // printf("CANCTRL=%#x.", mcp2515_read_register( REG_CANCTRL_FLGS ) );
    // printf("CANINTE=%#x.", mcp2515_read_register( REG_CANINTE_FLGS ) );
    // printf("CANINTF=%#x.", mcp2515_read_register( REG_CANINTF_FLGS ) );
}

void candrv_tmp4() {

    uint8_t rec = mcp2515_read_register( REG_REC );
    uint8_t tec = mcp2515_read_register( REG_TEC );

    printf( "\n----------\n");
    printf( "■エラー数\n");
    printf( "RX Error count is %d.  ", rec );
    printf( "TX Error count is %d.\n", tec );

    uint8_t stat = mcp2515_read_register( REG_CANSTAT );
    uint8_t int_reason = (uint8_t)(stat >> 1) & 0x07;

    printf("CANSTAT(raw)=%#x", stat);
    printf("■割り込み要因=");

    switch ( int_reason ) {
    case 0:
        printf("なし\n");
        break;
    case 1:
        printf("エラー\n");
        break;
    case 2:
        printf("ウェイクアップ\n");
        break;
    case 3:
        printf("TX#1\n");
        break;
    case 4:
        printf("TX#2\n");
        break;
    case 5:
        printf("TX#3\n");
        break;
    case 6:
        printf("RX#1\n");
        break;
    case 7:
        printf("RX#2\n");
        break;
    }

    printf( "■割り込みフラグ\n");
    uint8_t int_flg = mcp2515_read_register( REG_CANINTF_FLGS );

    printf( "MSG ERR is %d.  ", (int_flg & 0x80) ? 1 : 0 );
    printf( "Wake up is %d.  ", (int_flg & 0x40) ? 1 : 0 );
    printf( "Errors  is %d.\n", (int_flg & 0x20) ? 1 : 0 );
    printf( "TX#3 is %d.  ", (int_flg & 0x10) ? 1 : 0 );
    printf( "TX#2 is %d.  ", (int_flg & 0x08) ? 1 : 0 );
    printf( "TX#1 is %d.\n", (int_flg & 0x04) ? 1 : 0 );
    printf( "RX#2 is %d.  ", (int_flg & 0x02) ? 1 : 0 );
    printf( "RX#1 is %d.\n", (int_flg & 0x01) ? 1 : 0 );

    uint8_t err_flg = mcp2515_read_register( REG_EFLG_FLGS );

    printf( "■エラーフラグ\n");

    printf( "RX#2 overflow is %d.  ", (err_flg & 0x80) ? 1 : 0 );
    printf( "RX#1 overflow is %d.\n", (err_flg & 0x40) ? 1 : 0 );
    printf( "Bus off error is %d.\n", (err_flg & 0x20) ? 1 : 0 );
    printf( "TX error pasv is %d.  ", (err_flg & 0x10) ? 1 : 0 );
    printf( "RX error pasv is %d.\n", (err_flg & 0x08) ? 1 : 0 );
    printf( "TX warn is %d.  ", (err_flg & 0x04) ? 1 : 0 );
    printf( "RX warn is %d.  ", (err_flg & 0x02) ? 1 : 0 );
    printf( "ERR warn is %d.\n", (err_flg & 0x01) ? 1 : 0 );

    uint8_t tx0ctrl = mcp2515_read_register( REG_TXB0CTRL_FLGS );

    printf( "■TX#1\n");

    printf( "ABTF is %d.  ", (tx0ctrl & 0x40) ? 1 : 0 );
    printf( "MLOA is %d.  ", (tx0ctrl & 0x20) ? 1 : 0 );
    printf( "TX error is %d.  ", (tx0ctrl & 0x10) ? 1 : 0 );
    printf( "TX REQ is %d.\n", (tx0ctrl & 0x04) ? 1 : 0 );

}

void candrv_tmp5() {

    // clear
    mcp2515_modbits_register( REG_CANINTF_FLGS, 0xFF, 0x0 );
}


void candrv_tmp6() {

    // int enabled
    mcp2515_modbits_register( REG_CANINTF_FLGS, 0xA4 , 0x00 );
    mcp2515_modbits_register( REG_CANINTE_FLGS, 0xA4 , 0xFF );
}

void candrv_tmp_clr_exerr() {

// printf("めっせーじえらーをくりあします");
        mcp2515_modbits_register( REG_CANINTE_FLGS, 0xFF, 0x00 );
        mcp2515_modbits_register( REG_CANINTF_FLGS, 0x80 , 0x00 ); // Clear MSG ERR
        mcp2515_write_register( REG_TEC, 0x00 );
        mcp2515_modbits_register( REG_EFLG_FLGS, 0xFF, 0x00 );
        mcp2515_modbits_register( REG_CANINTF_FLGS, 0xFF, 0x00 );
        mcp2515_modbits_register( REG_CANINTE_FLGS, 0xFF, 0xFF );
}

void candrv_tmp_cancel_tx_if_msgerr( void ) {

    // mcp2515_modbits_register( REG_CANINTE_FLGS, 0x04, 0 );  // Disable TX0 INT.
    // mcp2515_modbits_register( REG_CANINTF_FLGS, 0x04, 0 );  // Clear TX0 INT.
    // mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_MERRF | MASKOF_CANINTE_ERRIF, 0 );  // Disable MERR int.
    mcp2515_modbits_register( REG_TXB0CTRL_FLGS, MASKOF_TXB0CTRL_TXREQ | MASKOF_TXB0CTRL_TXERR | MASKOF_TXB0CTRL_ABTF | MASKOF_TXB0CTRL_MLOA, 0);  // Clear TXCTRL.
    // mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_MERRF | MASKOF_CANINTF_ERRIF, 0 );  // Clear MERR.

    // busy_wait_us_32(3 * 1000 * 10);  //まってみる->いみなし

    // mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_MERRF | MASKOF_CANINTE_ERRIF, 0xFF );  // Enable MERR int.
}

bool candrv_tmp_is_merr( void ) {

    return 0 < ( mcp2515_read_register( REG_CANINTF_FLGS ) & 0x80 );
}

bool candrv_tmp_is_err( void ) {

    return 0 < ( mcp2515_read_register( REG_CANINTF_FLGS ) & 0x20 );
}

void candrv_register_callback( const can_int_callback_t callback ) {

    rp2040_register_can_int_callback( (gpio_irq_callback_t)callback );
}

void candrv_set_irq_enabled( enum CANDRV_IRQ irq, bool enabled ) {

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
