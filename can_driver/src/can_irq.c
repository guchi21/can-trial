#include "can_driver_irq.h"
#include "mcp2515.h"
#include "mcp2515_register.h"

#define IRQ_REASON_MSGERR               ( 0U )
#define IRQ_REASON_ERRSTAT              ( 1U )
#define IRQ_REASON_WAKEUP               ( 2U )
#define IRQ_REASON_TX0                  ( 3U )
#define IRQ_REASON_TX1                  ( 4U )
#define IRQ_REASON_TX2                  ( 5U )
#define IRQ_REASON_RX0                  ( 6U )
#define IRQ_REASON_RX1                  ( 7U )

static uint32_t countof_msgerr_irq = 0U;
static uint32_t countof_errstat_irq = 0U;
static candrv_cbk_recv_t cbk_recv = NULL;
static candrv_cbk_sent_t cbk_sent = NULL;
static candrv_cbk_rbuf_ovf_t cbk_rbuf_ovf = NULL;
static candrv_cbk_busoff_t cbk_busoff = NULL;

void candrv_set_cbk_recv( candrv_cbk_recv_t cbk ) {

    cbk_recv = cbk;
}

void candrv_set_cbk_sent( candrv_cbk_sent_t cbk ) {

    cbk_sent = cbk;
}

void candrv_set_cbk_rbuf_ovf( candrv_cbk_rbuf_ovf_t cbk ) {

    cbk_rbuf_ovf = cbk;
}

void candrv_set_cbk_busoff( candrv_cbk_busoff_t cbk ) {

    cbk_busoff = cbk;
}

void candrv_ind_irq( void ) {
    
    const uint8_t reason = (uint8_t)( (uint8_t)(mcp2515_read_register( REG_CANSTAT ) & MASKOF_CANSTAT_ICOD) >> 1 );

    switch ( reason ) {

    case IRQ_REASON_MSGERR:
    
        /* Clear causes. */
        mcp2515_clr_all_send_req_if_err();
        
        /* Clear interruption by message error. */
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_MERRF, 0U );

        // ToDo: Register errors.
        countof_msgerr_irq++;

        break;

    case IRQ_REASON_ERRSTAT:

        /* Clear causes. */
        mcp2515_clr_all_send_req_if_err();

        if ( NULL != cbk_rbuf_ovf ) {

            uint8_t eflg = mcp2515_read_register( REG_EFLG );


            if ( NULL != cbk_busoff ) {

                cbk_busoff( 0U != ( eflg & MASKOF_EFLG_TXBO ) );
            }

            if ( 0U != ( eflg & MASKOF_EFLG_RX0OVR ) ) {

                cbk_rbuf_ovf( CANDRV_RX_0 );

                if ( NULL != cbk_recv ) {

                    cbk_recv( CANDRV_RX_0 );
                }
                else {

                    /* Clear interruption by received to RX0. */
                    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );
                }

                /* Clear RX overflow. */
                mcp2515_modbits_register( REG_EFLG, MASKOF_EFLG_RX0OVR, 0U );
            }

            if ( 0U != ( eflg & MASKOF_EFLG_RX1OVR ) ) {

                cbk_rbuf_ovf( CANDRV_RX_1 );

                if ( NULL != cbk_recv ) {

                    cbk_recv( CANDRV_RX_1 );
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

        // ToDo: Register errors.
        countof_errstat_irq++;

        break;

    case IRQ_REASON_TX0:

        if ( NULL != cbk_sent ) {

            cbk_sent( CANDRV_TX_0 );
        }

        /* Clear interruption by sent from TX0. */
        mcp2515_clr_send_req( CANDRV_TX_0 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX0IF, 0U );

        break;

    case IRQ_REASON_TX1:

        if ( NULL != cbk_sent ) {

            cbk_sent( CANDRV_TX_1 );
        }

        /* Clear interruption by sent from TX1. */
        mcp2515_clr_send_req( CANDRV_TX_1 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX1IF, 0U );

        break;

    case IRQ_REASON_TX2:

        if ( NULL != cbk_sent ) {

            cbk_sent( CANDRV_TX_2 );
        }

        /* Clear interruption by sent from TX2. */
        mcp2515_clr_send_req( CANDRV_TX_2 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX2IF, 0U );

        break;

    case IRQ_REASON_RX0:

        if ( NULL != cbk_recv ) {

            cbk_recv( CANDRV_RX_0 );
        }
        else {

            /* Clear interruption by received to RX0. */
            mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );
        }

        break;

    case IRQ_REASON_RX1:

        if ( NULL != cbk_recv ) {

            cbk_recv( CANDRV_RX_1 );
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
