#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "pico_can.h"
#include "hardware/timer.h"

/*==================================================================*/
/* Define.                                                          */
/*==================================================================*/

#define MAXOF_STD_CAN_ID                ( 0x000007FFULL )                       /* Maximum value of the standard  CAN ID.   */
#define MAXOF_EXT_CAN_ID                ( 0x1FFFFFFFULL )                       /* Maximum value of the extension CAN ID.   */

#define INVALID_ALARM_ID                ( 0 )
#define MINOF_ALARM_ID                  ( 1 )
#define MAXOF_ALARM_ID                  ( INT32_MAX )

#define TIMEOUTOF_SEND_MS               ( 100U )

#define IDXOF_TX_SPICMD_WRITE_HDR       ( 0U )
#define IDXOF_TX_SPICMD_WRITE_BODY      ( 1U )
#define IDXOF_TX_SPICMD_SEND            ( 2U )
#define NUMOF_TX_SPICMD_ITEMS           ( 3U )


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef struct { uint8_t sidh, sidl, eid8, eid0; } can_id_reg_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

static const uint8_t REG_TXBCTRL_TBL[ CANDRV_TX_NUMOF_ITEMS ]
    = { REG_TXB0CTRL, REG_TXB1CTRL, REG_TXB2CTRL };

static const uint8_t MASKOF_CANINT_TX_TBL[ CANDRV_TX_NUMOF_ITEMS ]
    = { MASKOF_CANINT_TX0IF, MASKOF_CANINT_TX1IF, MASKOF_CANINT_TX2IF };

static const uint8_t TX_SPICMD_TBL[ CANDRV_TX_NUMOF_ITEMS ][ NUMOF_TX_SPICMD_ITEMS ] = {
    { SPICMD_WRITE_TX0_ID, SPICMD_WRITE_TX0_CONTENT, SPICMD_REQ_TX0 },
    { SPICMD_WRITE_TX1_ID, SPICMD_WRITE_TX1_CONTENT, SPICMD_REQ_TX1 },
    { SPICMD_WRITE_TX2_ID, SPICMD_WRITE_TX2_CONTENT, SPICMD_REQ_TX2 }
};


/*==================================================================*/
/* Variables.                                                       */
/*==================================================================*/

static bool is_tx_available[ CANDRV_TX_NUMOF_ITEMS ] = { true, true, true };

static alarm_id_t tx_timeout_alarm_id[ CANDRV_TX_NUMOF_ITEMS ]
    = { INVALID_ALARM_ID, INVALID_ALARM_ID, INVALID_ALARM_ID };


/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

static bool is_invalid_canid_kind( const canid_kind_t kind ) {
    
    return ( ( CANID_KIND_MINOF_IDX > kind ) || ( CANID_KIND_MAXOF_IDX < kind ) )
        ? true : false;
}

static bool is_invalid_tx_priority( const enum CANDRV_TX_PRIORITY priority ) {

    return ( ( CANDRV_TX_PRIORITY_MINOF_IDX > priority ) || ( CANDRV_TX_PRIORITY_MAXOF_IDX < priority ) )
        ? true : false;
}

static bool is_invalid_tx_idx( const enum CANDRV_TX tx_idx ) {

    return ( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx ) )
        ? true : false;
}

static bool is_invalid_tx_timeout_alarm_id( const alarm_id_t alarm_id ) {

    return ( ( MINOF_ALARM_ID > alarm_id ) || ( MAXOF_ALARM_ID < alarm_id ) )
        ? true : false;
}

static candrv_result_t set_can_id( const canid_kind_t kind,
    const uint32_t can_id, uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    /* Fails if illegal arguments. */
    if ( is_invalid_canid_kind( kind ) || ( NULL == hdr ) )
        return CANDRV_FAILURE;

    if ( CANID_KIND_STD == kind ) {
        /*--------------------------*/
        /* Case of standard format. */
        /*--------------------------*/

        /* Fails if too large. */
        if ( MAXOF_STD_CAN_ID < can_id )
            return CANDRV_FAILURE;

        hdr[ MCP2515_CANHDR_EID0 ] = 0U;
        hdr[ MCP2515_CANHDR_EID8 ] = 0U;
        hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( can_id << 5U ) & 0xE0U );
        hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 3U );
    } 
    else {
        /*--------------------------*/
        /* Case of extended format. */
        /*--------------------------*/

        /* Fails if too large. */
        if ( MAXOF_EXT_CAN_ID < can_id )
            return CANDRV_FAILURE;

        hdr[ MCP2515_CANHDR_EID0 ] = (uint8_t)can_id;
        hdr[ MCP2515_CANHDR_EID8 ] = (uint8_t)( can_id >> 8U );
        hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)(    ( ( can_id >> 16U ) & 0x03U )
                                                 | MASKOF_SIDL_IDE
                                                 | ( ( can_id >> 13U ) & 0xE0U ) );
        hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 21U );
    }

    return CANDRV_SUCCESS;
}

static candrv_result_t set_can_dlc( const uint8_t dlc, const bool is_remote,
    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    uint8_t reg_val = 0U;

    /* Fails if illegal arguments. */
    if ( ( CAN_MAXOF_LEN < dlc ) || ( NULL == hdr ) )
        return CANDRV_FAILURE;
    
    reg_val = (uint8_t)( reg_val | (uint8_t)( dlc & 0xF ) );

    if( true == is_remote ) {

        reg_val = (uint8_t)( reg_val | MASKOF_RTR );
    }

    hdr[ MCP2515_CANHDR_DLC ] = reg_val;

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_set_tx_msg( const enum CANDRV_TX tx_idx,
    const can_msg_t *const msg, const enum CANDRV_TX_PRIORITY priority ) {

    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] = { 0U };

    /* Fails if illegal arguments. */
    if ( is_invalid_tx_idx( tx_idx ) || ( NULL == msg ) || ( NULL == msg->data )
      || is_invalid_tx_priority( priority ) )
        return CANDRV_FAILURE;

    /* Fails if not available. */
    if ( true != is_tx_available[ tx_idx ] )
        return CANDRV_FAILURE;

    /* Set CAN id in register format. */
    if ( CANDRV_FAILURE == set_can_id( msg->id_kind, msg->id, hdr ) )
        return CANDRV_FAILURE;
    
    /* Set CAN DLC in regester format. */
    if ( CANDRV_FAILURE == set_can_dlc( msg->dlc, msg->is_remote, hdr ) )
        return CANDRV_FAILURE;

    /* Write to the register. */
    mcp2515_modbits_register(
        REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXP, (uint8_t)priority );

    if ( ( false == msg->is_remote ) && ( CAN_EMPTY < msg->dlc ) ) {

        picocan_begin_spi_commands();
        picocan_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_WRITE_BODY ]);
        picocan_write_array_spi( msg->dlc, msg->data );
        picocan_end_spi_commands();
    }

    picocan_begin_spi_commands();
    picocan_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_WRITE_HDR ] );
    picocan_write_array_spi( MCP2515_CANHDR_NUMOF_ITEMS, hdr );
    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_clr_send_req( const enum CANDRV_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CANDRV_FAILURE;

    /* Clear send request. */
    mcp2515_modbits_register( REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXREQ, 0U );

    /* Disabled sent interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );

    /* Cancel timeout alarm. */
    if ( !( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_id[ tx_idx ] ) ) ) {

        (void)cancel_alarm( tx_timeout_alarm_id[ tx_idx ] );

        tx_timeout_alarm_id[ tx_idx ] = INVALID_ALARM_ID;
    }

    /* TXn to be available. */
    is_tx_available[ tx_idx ] = true;

    return CANDRV_SUCCESS;
}

static int64_t cbk_send_timeout( alarm_id_t id, void *user_data ) {

    if ( !is_invalid_tx_timeout_alarm_id( id ) ) {

        for ( enum CANDRV_TX i = CANDRV_TX_MINOF_IDX; i <= CANDRV_TX_MAXOF_IDX; i++ ) {

            if ( tx_timeout_alarm_id[ i ] == id ) {

                (void)mcp2515_clr_send_req( i );
            }
        }
    }

    /* Not reschedule the alarm. */
    return 0LL;
}

candrv_result_t mcp2515_set_send_req( const enum CANDRV_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CANDRV_FAILURE;

    /* Fails if not available. */
    if ( true != is_tx_available[ tx_idx ] )
        return CANDRV_FAILURE;

    /* Set timeout alarm. */
    tx_timeout_alarm_id[ tx_idx ] = add_alarm_in_ms( TIMEOUTOF_SEND_MS, cbk_send_timeout, NULL, false );

    /* Fails if alarm set fails. */
    if ( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_id[ tx_idx ] ) )
        return CANDRV_FAILURE;

    /* Requested send CAN message. */
    is_tx_available[ tx_idx ] = false;

    /* Enabled TX interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0xFFU );

    /* Request to send. */
    picocan_begin_spi_commands();
    picocan_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_SEND ] );
    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}

 // todo:trial
candrv_result_t mcp2515_is_tx_available( const enum CANDRV_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CANDRV_FAILURE;

    /* Fails if not available. */
    if ( true != is_tx_available[ tx_idx ] )
        return CANDRV_FAILURE;

    return CANDRV_SUCCESS;
}