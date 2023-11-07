#include "can_driver.h"
#include "mcp2515.h"
#include "mcp2515_register.h"
#include "pico/stdlib.h"

/*! 
 * \defgroup Mcp2515IrqReason MCP2515 interrupt reason
 */
/*! \{ */
#define IRQ_REASON_MSGERR   ( 0U )  /*!< Cause send message error   */
#define IRQ_REASON_ERRSTAT  ( 1U )  /*!< Changed error status       */
#define IRQ_REASON_WAKEUP   ( 2U )  /*!< Wake up MCP2515            */
#define IRQ_REASON_TX0      ( 3U )  /*!< Sent message from transmitter#1 */
#define IRQ_REASON_TX1      ( 4U )  /*!< Sent message from transmitter#2 */
#define IRQ_REASON_TX2      ( 5U )  /*!< Sent message from transmitter#3 */
#define IRQ_REASON_RX0      ( 6U )  /*!< Received message in receiver#1  */
#define IRQ_REASON_RX1      ( 7U )  /*!< Received message in receiver#2  */
/*! \} */

static cd_received_callback_t cbk_recv = NULL;
static cd_sent_callback_t cbk_sent = NULL;
static cd_overflow_callback_t cbk_overflow = NULL;

void cd_set_received_callback( cd_received_callback_t cbk ) {

    cbk_recv = cbk;
}

void cd_set_overflow_callback( cd_overflow_callback_t cbk ) {

    cbk_overflow = cbk;
}

void cd_set_sent_callback( cd_sent_callback_t cbk ) {

    cbk_sent = cbk;
}

// 送信エラー割込を解除
void static cbk_send_error( void ) {

    /* Clear causes. */
    mcp2515_clear_all_send_req_if_error();
    
    /* Clear interrupt occurred flag by message error. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_MERRF, 0U );
}

void static cbk_receive_message( void ) {

    if ( NULL != cbk_recv )
        cbk_recv( CD_RX_0 );

    /* Clear interruption by received to RX0. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );
}

void static cbk_change_error_status( void ) {

    /* Clear causes. */
    mcp2515_clear_all_send_req_if_error();

    if ( NULL != cbk_overflow ) {

        const uint8_t eflg = mcp2515_read_register( REG_EFLG );

        if ( 0U != ( eflg & MASKOF_EFLG_RX0OVR ) ) {

            cbk_overflow( CD_RX_0 );

            if ( NULL != cbk_recv ) {
                cbk_recv( CD_RX_0 );

            /* Clear interruption by received to RX0. */
            mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );

            /* Clear RX overflow. */
            mcp2515_modbits_register( REG_EFLG, MASKOF_EFLG_RX0OVR, 0U );
        }

        if ( 0U != ( eflg & MASKOF_EFLG_RX1OVR ) ) {

            cbk_rbuf_ovf( CD_RX_1 );

            if ( NULL != cbk_recv ) {

                cbk_recv( CD_RX_1 );
            }
            else {

                /* Clear interruption by received to RX1. */
                mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX1IF, 0U );
            }

            /* Clear RX overflow. */
            mcp2515_modbits_register( REG_EFLG, MASKOF_EFLG_RX1OVR, 0U );
        }
    }

    /* Clear interruption by message error. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_ERRIF, 0U );
}

void mcp2515_cbk_can_controller( uint32_t event_mask ) {
    
    const uint8_t canstat = mcp2515_read_register( REG_CANSTAT );
    const uint8_t reason = (uint8_t)( (uint8_t)( canstat & MASKOF_CANSTAT_ICOD) >> 1 );

    switch ( reason ) {

    case IRQ_REASON_MSGERR:
        cbk_send_error();
        break;

    case IRQ_REASON_ERRSTAT:
        cbk_change_error_status();
        break;

    case IRQ_REASON_TX0:

        if ( NULL != cbk_sent ) {

            cbk_sent( CD_TX_0 );
        }

        /* Clear interruption by sent from TX0. */
        mcp2515_clr_send_req( CD_TX_0 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX0IF, 0U );

        break;

    case IRQ_REASON_TX1:

        if ( NULL != cbk_sent ) {

            cbk_sent( CD_TX_1 );
        }

        /* Clear interruption by sent from TX1. */
        mcp2515_clr_send_req( CD_TX_1 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX1IF, 0U );

        break;

    case IRQ_REASON_TX2:

        if ( NULL != cbk_sent ) {

            cbk_sent( CD_TX_2 );
        }

        /* Clear interruption by sent from TX2. */
        mcp2515_clr_send_req( CD_TX_2 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX2IF, 0U );

        break;

    case IRQ_REASON_RX0:

        cbk_receive_message();
        if ( NULL != cbk_recv ) {

            cbk_recv( CD_RX_0 );
        }
        else {

            /* Clear interruption by received to RX0. */
            mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );
        }

        break;

    case IRQ_REASON_RX1:

        if ( NULL != cbk_recv ) {

            cbk_recv( CD_RX_1 );
        }
        else {

            /* Clear interruption by received to RX1. */
            mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX1IF, 0U );
        }

        break;

    default:
        /* No processing. */
        break;
    }
}
