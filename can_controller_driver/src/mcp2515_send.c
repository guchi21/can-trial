#include "can_driver.h"
#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "hardware_manager.h"
#include "hardware/timer.h"


/*! 
 * \defgroup AlarmId Send timeout alarm id
 */
/*! \{ */
#define INVALID_ALARM_ID                ( 0 )           /*!< Invalid alarm id */
#define MINOF_ALARM_ID                  ( 1 )           /*!< Minimim alarm id */
#define MAXOF_ALARM_ID                  ( INT32_MAX )   /*!< Maximum alarm id */
/*! \} */

/*! 
 * \defgroup TxSpiCmdIdx Index within SPI command table for CAN transmitter
 */
/*! \{ */
#define IDXOF_TX_SPICMD_WRITE_HDR       ( 0U )          /*!< Write CAN id and DLC to register   */
#define IDXOF_TX_SPICMD_WRITE_BODY      ( 1U )          /*!< Write CAN data                     */
#define IDXOF_TX_SPICMD_SEND            ( 2U )          /*!< Send CAN message                   */
#define NUMOF_TX_SPICMD_ITEMS           ( 3U )
/*! \} */

static const uint8_t REG_TXBCTRL_TBL[ CD_TX_NUMOF_ITEMS ]
    = { REG_TXB0CTRL, REG_TXB1CTRL, REG_TXB2CTRL };

static const uint8_t MASKOF_CANINT_TX_TBL[ CD_TX_NUMOF_ITEMS ]
    = { MASKOF_CANINT_TX0IF, MASKOF_CANINT_TX1IF, MASKOF_CANINT_TX2IF };

static const uint8_t TX_SPICMD_TBL[ CD_TX_NUMOF_ITEMS ][ NUMOF_TX_SPICMD_ITEMS ] = {
    { SPICMD_WRITE_TX0_ID, SPICMD_WRITE_TX0_CONTENT, SPICMD_REQ_TX0 },
    { SPICMD_WRITE_TX1_ID, SPICMD_WRITE_TX1_CONTENT, SPICMD_REQ_TX1 },
    { SPICMD_WRITE_TX2_ID, SPICMD_WRITE_TX2_CONTENT, SPICMD_REQ_TX2 }
};

static bool tx_availables[ CD_TX_NUMOF_ITEMS ] = { true, true, true };

static alarm_id_t tx_timeout_alarm_ids[ CD_TX_NUMOF_ITEMS ]
    = { INVALID_ALARM_ID, INVALID_ALARM_ID, INVALID_ALARM_ID };

static bool is_invalid_canid_kind( const enum CD_CANID_KIND kind ) {
    
    return ( ( CD_CANID_KIND_MINOF_IDX > kind ) || ( CD_CANID_KIND_MAXOF_IDX < kind ) )
        ? true : false;
}

static bool is_invalid_tx_priority( const enum CD_TX_PRIORITY priority ) {

    return ( ( CD_TX_PRIORITY_MINOF_IDX > priority ) || ( CD_TX_PRIORITY_MAXOF_IDX < priority ) )
        ? true : false;
}

static bool is_invalid_tx_idx( const enum CD_TX tx_idx ) {

    return ( ( CD_TX_MINOF_IDX > tx_idx ) || ( CD_TX_MAXOF_IDX < tx_idx ) )
        ? true : false;
}

static bool is_invalid_tx_timeout_alarm_id( const alarm_id_t alarm_id ) {

    return ( ( MINOF_ALARM_ID > alarm_id ) || ( MAXOF_ALARM_ID < alarm_id ) )
        ? true : false;
}

static cd_result_t set_can_id( const enum CD_CANID_KIND kind, const uint32_t can_id,
    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    /* Fails if illegal arguments. */
    if ( is_invalid_canid_kind( kind ) || ( NULL == hdr ) )
        return CD_FAILURE;

    if ( CD_CANID_KIND_STD == kind ) {
        /*--------------------------*/
        /* Case of standard format. */
        /*--------------------------*/

        /* Fails if too large. */
        if ( CD_MAXOF_STD_CANID < can_id )
            return CD_FAILURE;

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
        if ( CD_MAXOF_EXT_CANID < can_id )
            return CD_FAILURE;

        hdr[ MCP2515_CANHDR_EID0 ] = (uint8_t)can_id;
        hdr[ MCP2515_CANHDR_EID8 ] = (uint8_t)( can_id >> 8U );
        hdr[ MCP2515_CANHDR_SIDL ] = (uint8_t)( ( ( can_id >> 16U ) & 0x03U )
            | MASKOF_SIDL_IDE | ( ( can_id >> 13U ) & 0xE0U ) );
        hdr[ MCP2515_CANHDR_SIDH ] = (uint8_t)( can_id >> 21U );
    }

    return CD_SUCCESS;
}

static cd_result_t set_can_dlc( const uint8_t dlc, const bool is_remote,
    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    uint8_t reg_val = 0U;

    /* Fails if illegal arguments. */
    if ( ( CD_CANDLC_MAXOF_LEN < dlc ) || ( NULL == hdr ) )
        return CD_FAILURE;
    
    reg_val = (uint8_t)( reg_val | (uint8_t)( dlc & 0xF ) );

    if( true == is_remote ) {

        reg_val = (uint8_t)( reg_val | MASKOF_RTR );
    }

    hdr[ MCP2515_CANHDR_DLC ] = reg_val;

    return CD_SUCCESS;
}

cd_result_t cd_set_message_with_priority( const enum CD_TX tx_idx,
    const enum CD_TX_PRIORITY priority, const struct cd_can_message *const msg ) {

    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ];

    /* Fails if illegal arguments. */
    if ( is_invalid_tx_idx( tx_idx ) || ( NULL == msg ) || ( NULL == msg->data )
        || is_invalid_tx_priority( priority ) )
        return CD_FAILURE;

    /* Fails if not available. */
    if ( true != tx_availables[ tx_idx ] )
        return CD_FAILURE;

    /* Set CAN id in register format. */
    if ( CD_FAILURE == set_can_id( msg->id_kind, msg->id, hdr ) )
        return CD_FAILURE;
    
    /* Set CAN DLC in regester format. */
    if ( CD_FAILURE == set_can_dlc( msg->dlc, msg->is_remote, hdr ) )
        return CD_FAILURE;

    /* Write to the register. */
    mcp2515_modbits_register( REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXP, (uint8_t)priority );

    if ( ( false == msg->is_remote ) && ( CD_CANDLC_EMPTY < msg->dlc ) ) {

        hw_begin_spi_commands();
        hw_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_WRITE_BODY ]);
        hw_write_array_spi( msg->dlc, msg->data );
        hw_end_spi_commands();
    }

    hw_begin_spi_commands();
    hw_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_WRITE_HDR ] );
    hw_write_array_spi( MCP2515_CANHDR_NUMOF_ITEMS, hdr );
    hw_end_spi_commands();

    return CD_SUCCESS;
}

cd_result_t cd_set_message( const enum CD_TX tx_idx, const struct cd_can_message *const msg ) {

    return cd_set_message_with_priority( tx_idx, CD_TX_PRIORITY_MIDLOW, msg );
}

cd_result_t cd_clear_send_request( const enum CD_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CD_FAILURE;

    /* Clear send request. */
    mcp2515_modbits_register( REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXREQ, 0U );

    /* Disabled sent interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );

    /* Cancel timeout alarm. */
    if ( !( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_ids[ tx_idx ] ) ) ) {

        (void)cancel_alarm( tx_timeout_alarm_ids[ tx_idx ] );

        tx_timeout_alarm_ids[ tx_idx ] = INVALID_ALARM_ID;
    }

    /* TXn to be available. */
    tx_availables[ tx_idx ] = true;

    return CD_SUCCESS;
}

static int64_t cbk_send_timeout( alarm_id_t id, void *user_data ) {

    if ( !is_invalid_tx_timeout_alarm_id( id ) ) {

        for ( enum CD_TX i = CD_TX_MINOF_IDX; i <= CD_TX_MAXOF_IDX; i++ ) {

            if ( tx_timeout_alarm_ids[ i ] == id ) {

                (void)cd_clear_send_request( i );
            }
        }
    }

    /* Not reschedule the alarm. */
    return 0LL;
}

cd_result_t cd_set_send_request( const enum CD_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CD_FAILURE;

    /* Fails if not available. */
    if ( true != tx_availables[ tx_idx ] )
        return CD_FAILURE;

    /* Set timeout alarm. */
    tx_timeout_alarm_ids[ tx_idx ] 
        = add_alarm_in_ms( CD_TIMEOUTOF_SEND_MS, cbk_send_timeout, NULL, false );

    /* Fails if alarm set fails. */
    if ( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_ids[ tx_idx ] ) )
        return CD_FAILURE;

    /* Requested send CAN message. */
    tx_availables[ tx_idx ] = false;

    /* Enabled TX interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0xFFU );

    /* Request to send. */
    hw_begin_spi_commands();
    hw_write_spi( TX_SPICMD_TBL[ tx_idx ][ IDXOF_TX_SPICMD_SEND ] );
    hw_end_spi_commands();

    return CD_SUCCESS;
}

enum CD_TX cd_get_available_tx( void ) {

    for ( enum CD_TX i = CD_TX_MINOF_IDX; CD_TX_NUMOF_ITEMS > i; i++ ) {

        if ( true == tx_availables[ i ] )
            return i;
    }

    return CD_TX_INVALID;
}
