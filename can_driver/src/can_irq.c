#include "can_driver_irq.h"
#include "mcp2515.h"
#include "MCP2515_REGISTER.h"

#define IRQ_REASON_MSGERR               ( 0U )
#define IRQ_REASON_ERRSTAT              ( 1U )
#define IRQ_REASON_WAKEUP               ( 2U )
#define IRQ_REASON_TX0                  ( 3U )
#define IRQ_REASON_TX1                  ( 4U )
#define IRQ_REASON_TX2                  ( 5U )
#define IRQ_REASON_RX0                  ( 6U )
#define IRQ_REASON_RX1                  ( 7U )

static uint8_t recv_content0[8] = { 0U };
static can_message_t recv_msg0 = { CAN_KIND_STD, 0U, 0U, recv_content0 };

static uint8_t recv_content1[8] = { 0U };
static can_message_t recv_msg1 = { CAN_KIND_STD, 0U, 0U, recv_content1 };

can_message_t* candrv_get_recv_msg0( void ) {

    return &recv_msg0;
}

can_message_t* candrv_get_recv_msg1( void ) {

    return &recv_msg1;
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
        break;

    case IRQ_REASON_ERRSTAT:

        /* Clear causes. */
        mcp2515_clr_all_send_req_if_err();
        
        /* Clear interruption by message error. */
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_ERRIF, 0U );


        // ToDo: Register errors.
        break;

    case IRQ_REASON_TX0:

        /* Clear interruption by sent from TX0. */
        mcp2515_clr_send_req( CANDRV_TX_0 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX0IF, 0U );

        break;

    case IRQ_REASON_TX1:

        /* Clear interruption by sent from TX1. */
        mcp2515_clr_send_req( CANDRV_TX_1 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX1IF, 0U );

        break;

    case IRQ_REASON_TX2:

        /* Clear interruption by sent from TX2. */
        mcp2515_clr_send_req( CANDRV_TX_2 );
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX2IF, 0U );

        break;

    case IRQ_REASON_RX0:

        mcp2515_get_rx_msg( CANDRV_RX_0, &recv_msg0 );

        /* Clear interruption by received to RX0. */
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX0IF, 0U );

        break;

    case IRQ_REASON_RX1:

        mcp2515_get_rx_msg( CANDRV_RX_1, &recv_msg1 );

        /* Clear interruption by received to RX1. */
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_RX1IF, 0U );

        break;

    default:
        /* No processing. */
        break;
    }
}
