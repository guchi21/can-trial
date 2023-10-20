#include "mcp2515.h"
#include "mcp2515_reg_addr.h"
#include "mcp2515_spi_cmd.h"
#include "pico_can.h"
#include "hardware/timer.h"
#include "hardware/sync.h"

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* Related CAN id definitions */
#define MAXOF_STD_CAN_ID                ( 0x000007FFULL )                       /* Maximum value of the standard  CAN ID.   */
#define MAXOF_EXT_CAN_ID                ( 0x1FFFFFFFULL )                       /* Maximum value of the extension CAN ID.   */

/* Related CAN content length definitions */
#define MINOF_CAN_LEN                   ( 0U )                                  /* Minimum length of the content of CAN message.  */
#define MAXOF_CAN_LEN                   ( 8U )                                  /* Maximum length of the content of CAN message.  */


#define MINOF_ALARM_ID                  ( 1 )
#define MAXOF_ALARM_ID                  ( INT32_MAX )


#define TIMEOUTOF_SEND                  ( 1099U )


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

/* Index of the buffer corresponding to the CAN ID on the register. */
 enum IDBUF {
    IDBUF_MINOF_IDX = 0,
    IDBUF_SIDH = IDBUF_MINOF_IDX,
    IDBUF_SIDL,
    IDBUF_EID8,
    IDBUF_EID0,
    IDBUF_DLC,
    IDBUF_MAXOF_IDX = IDBUF_DLC,
    IDBUF_NUMOF_ITEMS
};


/*==================================================================*/
/* Variables.                                                       */
/*==================================================================*/

static bool is_tx_available[ CANDRV_TX_NUMOF_ITEMS ] = { true, true, true };
static alarm_id_t tx_timeout_alarm_id[ CANDRV_TX_NUMOF_ITEMS ] = { NULL, NULL, NULL };


/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

uint8_t mcp2515_read_register( const uint8_t addr ) {

    picocan_begin_spi_commands();

    picocan_write_spi( SPICMD_READ_REG );
    picocan_write_spi( addr );

    const uint8_t val = picocan_read_spi();

    picocan_end_spi_commands();

    return val;
}

void mcp2515_write_register( const uint8_t addr, const uint8_t val ) {

    picocan_begin_spi_commands();

    picocan_write_spi( SPICMD_WRITE_REG );
    picocan_write_spi( addr );
    picocan_write_spi( val );

    picocan_end_spi_commands();
}

void mcp2515_modbits_register( const uint8_t addr, const uint8_t maskof_write, const uint8_t val ) {

    picocan_begin_spi_commands();

    picocan_write_spi( SPICMD_MODBITS_REG );
    picocan_write_spi( addr );
    picocan_write_spi( maskof_write );
    picocan_write_spi( val );

    picocan_end_spi_commands();
}

uint8_t mcp2515_get_opr_mode( void ) {

    return (uint8_t)( mcp2515_read_register( REG_CANSTAT ) & MASKOF_CANSTAT_OPMOD );
}

static candrv_result_t verify_opr_mode( const uint8_t exp_mode ) {

    const uint32_t begun = time_us_32();
    uint8_t opr_mode = OPR_MODE_INVALID;

    do {
        opr_mode = mcp2515_get_opr_mode();

        const uint32_t current = time_us_32();
        const uint32_t elapsed = ( current < begun ) ? ( UINT32_MAX - begun + current + 1UL ) : ( current - begun );

        if ( elapsed > TIMEOUTOF_OPR_MODE_CHANGE )
            return CANDRV_FAILURE;

    } while ( exp_mode != opr_mode );

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_reset( void ) {

    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_RESET );
    picocan_end_spi_commands();

    return verify_opr_mode( OPR_MODE_CONFIG );
}

static candrv_result_t wakeup( void ) {

    const bool allow_wake = ( 0U != ( mcp2515_read_register( REG_CANINTE ) & MASKOF_CANINT_WAKIF ) ) ? true : false;

    /* To be enabled interrupt of wake up if disabled. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Request wake up. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Temporarily switch to listen-only mode. */
    if ( CANDRV_FAILURE == mcp2515_set_opr_mode( OPR_MODE_LISTENONLY ) )
        return CANDRV_FAILURE;

    /* Restore interrupt of wake up. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0U );

    /* Clear wake up request */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );
}

// ------------------------------------------------------------------------------------------------

static bool is_invalid_msg_kind( const can_format_kind_t kind ) {
    
    return ( ( CAN_KIND_MINOF_IDX > kind ) || ( CAN_KIND_MAXOF_IDX < kind ) ) ? true : false;
}

candrv_result_t mcp2515_set_opr_mode( const uint8_t mode ) {

    const uint8_t current = mcp2515_get_opr_mode();

    /* Validate arguments. */
    if ( !( ( OPR_MODE_NORMAL     == mode ) || ( OPR_MODE_SLEEP  == mode ) || ( OPR_MODE_LOOPBACK == mode )
         || ( OPR_MODE_LISTENONLY == mode ) || ( OPR_MODE_CONFIG == mode ) ) )
        return CANDRV_FAILURE;

    /* Do nothing if the operation mode does not change. */
    if ( current == mode )
        return CANDRV_SUCCESS;

    /* Wake up if current mode is sleep. */
    if ( ( OPR_MODE_SLEEP == current ) && ( CANDRV_FAILURE == wakeup() ) )
        return CANDRV_FAILURE;

    /* Clear waked up request if to be sleep. */
    if ( OPR_MODE_SLEEP == mode )
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );

    /* Apply request mode */
    mcp2515_modbits_register( REG_CANCTRL, MASKOF_CANCTRL_OPMOD, mode );

    /* Wait until applied. */
    return verify_opr_mode( mode );
}

candrv_result_t mcp2515_set_baudrate( const enum MCP2515_CAN_BAUDRATE baudrate ) {

    static const uint8_t BAUDRATE_TBL[ MCP2515_CAN_BAUDRATE_NUMOF_ITEMS ][ 3 ] = {
        { 0xA7U, 0XBFU, 0x07U }, { 0x31U, 0XA4U, 0X04U }, { 0x18U, 0XA4U, 0x04U }, { 0x09U, 0XA4U, 0x04U }, { 0x04U, 0x9EU, 0x03U },
        { 0x03U, 0x9EU, 0x03U }, { 0x01U, 0x1EU, 0x03U }, { 0x00U, 0x9EU, 0x03U }, { 0x00U, 0x92U, 0x02U }, { 0x00U, 0x82U, 0x02U } };

    /* Validate arguments. */
    if ( ( MCP2515_CAN_BAUDRATE_MINOF_IDX > baudrate ) || ( MCP2515_CAN_BAUDRATE_MAXOF_IDX < baudrate ) )
        return CANDRV_FAILURE;

    /* Fails if operation mode is not config mode. */
    if ( OPR_MODE_CONFIG != mcp2515_get_opr_mode() )
        return CANDRV_FAILURE;

    /* Apply. */
    picocan_begin_spi_commands();

    picocan_write_spi( SPICMD_WRITE_REG );
    picocan_write_spi( REG_CNF3 );
    picocan_write_array_spi( 3U, BAUDRATE_TBL[ baudrate ] );

    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}

static candrv_result_t build_can_id_reg(
    const uint32_t can_id, const can_format_kind_t can_kind, can_id_reg_t *const id_reg ) {

    if ( is_invalid_msg_kind( can_kind ) || ( NULL == id_reg )
      || ( CAN_KIND_STD == can_kind && can_id > MAXOF_STD_CAN_ID )
      || ( CAN_KIND_EXT == can_kind && can_id > MAXOF_EXT_CAN_ID ) )
        return CANDRV_FAILURE;

    if ( CAN_KIND_STD == can_kind ) {
        /*--------------------------*/
        /* Case of standard format. */
        /*--------------------------*/
        id_reg->eid0 = 0U;
        id_reg->eid8 = 0U;
        id_reg->sidl = (uint8_t)( (uint8_t)( can_id << 5U ) & 0xE0U );
        id_reg->sidh = (uint8_t)( can_id >> 3U );
    } 
    else {
        /*--------------------------*/
        /* Case of extended format. */
        /*--------------------------*/
        id_reg->eid0 = (uint8_t)can_id;
        id_reg->eid8 = (uint8_t)( can_id >> 8U );
        id_reg->sidl = (uint8_t)( (uint8_t)( (uint8_t)( can_id >> 16U ) & 0x03U ) | MASKOF_SIDL_IDE
                                | (uint8_t)( (uint8_t)( can_id >> 13U ) & 0xE0U ) );
        id_reg->sidh = (uint8_t)( can_id >> 21U );
    }

    return CANDRV_SUCCESS;
}

static bool is_invalid_tx_priority( const enum CANDRV_TX_PRIORITY priority ) {

    return ( ( CANDRV_TX_PRIORITY_MINOF_IDX > priority )
          || ( CANDRV_TX_PRIORITY_MAXOF_IDX < priority ) ) ? true : false;
}

static bool is_invalid_tx_idx( const enum CANDRV_TX tx_idx ) {

    return ( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx ) ) ? true : false;
}

static bool is_invalid_tx_timeout_alarm_id( const alarm_id_t alarm_id ) {

    return ( ( MINOF_ALARM_ID > alarm_id ) || ( MAXOF_ALARM_ID < alarm_id ) ) ? true : false;
}

candrv_result_t mcp2515_set_tx_msg( const enum CANDRV_TX tx_idx,
    const can_message_t *const msg, const enum CANDRV_TX_PRIORITY priority ) {

    static const uint8_t IDXOF_WRITE_ID      = 0U;
    static const uint8_t IDXOF_WRITE_CONTENT = 1U;

    static const uint8_t SPICMD_TBL[][ CANDRV_TX_NUMOF_ITEMS ] = {
        { SPICMD_WRITE_TX0_ID     , SPICMD_WRITE_TX1_ID     , SPICMD_WRITE_TX2_ID      },
        { SPICMD_WRITE_TX0_CONTENT, SPICMD_WRITE_TX1_CONTENT, SPICMD_WRITE_TX2_CONTENT } };

    can_id_reg_t id_reg; /* CAN id in the register format. */

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) || ( NULL == msg )
      || is_invalid_msg_kind( msg->kind ) || is_invalid_tx_priority( priority ) )
        return CANDRV_FAILURE;

    /* Fails if not available. */
    if ( true != is_tx_available[ tx_idx ] )
        return CANDRV_FAILURE;

    /* Fails if illegal CAN length. */
    if ( MAXOF_CAN_LEN < msg->length )
        return CANDRV_FAILURE;

    /* Fails if illegal CAN content. */
    if ( ( NULL == msg->content ) && ( MINOF_CAN_LEN < msg->length ) )
        return CANDRV_FAILURE;

    /* Build CAN id in register format. */
    /* Fails if build CAN id fails.     */
    if ( CANDRV_FAILURE == build_can_id_reg( msg->id, msg->kind, &id_reg ) )
        return CANDRV_FAILURE;

    const uint8_t id_buf[ IDBUF_NUMOF_ITEMS ] 
        = { id_reg.sidh, id_reg.sidl, id_reg.eid8, id_reg.eid0 };

    /* Write to the register. */
    if ( MINOF_CAN_LEN < msg->length ) {

        picocan_begin_spi_commands();
        picocan_write_spi( SPICMD_TBL[ IDXOF_WRITE_CONTENT ][ tx_idx ] );
        picocan_write_array_spi( msg->length, msg->content );
        picocan_end_spi_commands();
    }

    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_TBL[ IDXOF_WRITE_ID ][ tx_idx ] );
    picocan_write_array_spi( IDBUF_NUMOF_ITEMS, id_buf );
    picocan_write_spi( msg->length );
    picocan_end_spi_commands();

    mcp2515_modbits_register( REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXP, (uint8_t)priority );

    return CANDRV_SUCCESS;
}

void mcp2515_clr_send_req( const enum CANDRV_TX tx_idx ) {

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return;

    /* Clear send request. */
    mcp2515_modbits_register( REG_TXBCTRL_TBL[ tx_idx ], MASKOF_TXBCTRL_TXREQ, 0U );

    /* Disabled sent interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_TX_TBL[ tx_idx ], 0U );

    /* Cancel timeout alarm. */
    if ( !( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_id[ tx_idx ] ) ) ) {

        (void)cancel_alarm( tx_timeout_alarm_id[ tx_idx ] );

        tx_timeout_alarm_id[ tx_idx ] = (alarm_id_t)NULL;
    }

    /* TXn to be available. */
    is_tx_available[ tx_idx ] = true;
}

static int64_t cbk_send_timeout( alarm_id_t id, void *user_data ) {

    if ( !( is_invalid_tx_timeout_alarm_id( id ) ) ) {

        for ( enum CANDRV_TX i = CANDRV_TX_MINOF_IDX; i <= CANDRV_TX_MAXOF_IDX; i++ ) {

            if ( tx_timeout_alarm_id[ i ] == id ) {

                mcp2515_clr_send_req( i );
            }
        }
    }

    /* Not reschedule the alarm. */
    return 0;
}

candrv_result_t mcp2515_set_send_req( const enum CANDRV_TX tx_idx ) {

    static const uint8_t SPICMD_SEND_REQ_TBL[ CANDRV_TX_NUMOF_ITEMS ]
        = { SPICMD_REQ_TX0, SPICMD_REQ_TX1, SPICMD_REQ_TX2 };

    /* Validate arguments. */
    if ( is_invalid_tx_idx( tx_idx ) )
        return CANDRV_FAILURE;

    /* Fails if not available. */
    if ( true != is_tx_available[ tx_idx ] )
        return CANDRV_FAILURE;

    /* Set timeout alarm. */
    tx_timeout_alarm_id[ tx_idx ] = add_alarm_in_ms( TIMEOUTOF_SEND, cbk_send_timeout, NULL, false );

    /* Fails if alarm set fails. */
    if ( is_invalid_tx_timeout_alarm_id( tx_timeout_alarm_id[ tx_idx ] ) )
        return CANDRV_FAILURE;

    /* Requested send CAN message. */
    is_tx_available[ tx_idx ] = false;

    /* Enabled TX interruption. */
    mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_TX_TBL[ tx_idx ], 0xFFU );

    /* Request to send. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_SEND_REQ_TBL[ tx_idx ] );
    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}

static uint32_t build_ext_can_id( const uint8_t eid0, const uint8_t eid8,
    const uint8_t sidl, const uint8_t sidh ) {

    return (uint32_t)(
        (uint32_t)( (uint32_t)( (uint32_t)sidh << 21U ) & 0x1FE00000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)sidl << 13U ) & 0x001C0000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)sidl << 16U ) & 0x00030000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)eid8 <<  8U ) & 0x0000FF00UL )
      | (uint32_t)(             (uint32_t)eid0          & 0x000000FFUL )
    );
}

static uint32_t build_std_can_id( const uint8_t sidl, const uint8_t sidh ) {

    return (uint32_t)(
        (uint32_t)( (uint32_t)( (uint32_t)sidh << 3U ) & 0x000007F8UL )
      | (uint32_t)( (uint32_t)( (uint32_t)sidl >> 5U ) & 0x00000007UL )
    );
}

static bool is_invalid_rx_idx( const enum CANDRV_RX rx_idx ) {

    return ( ( CANDRV_RX_MINOF_IDX > rx_idx ) || ( CANDRV_RX_MAXOF_IDX < rx_idx ) ) ? true : false;
}

candrv_result_t mcp2515_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg ) {

    static const uint8_t IDXOF_READ_ID      = 0U;
    static const uint8_t IDXOF_READ_CONTENT = 1U;

    static const uint8_t SPICMD_TBL[][ CANDRV_RX_NUMOF_ITEMS ] = {
        { SPICMD_READ_RX0_ID     , SPICMD_READ_RX1_ID      },
        { SPICMD_READ_RX0_CONTENT, SPICMD_READ_RX1_CONTENT } };

    uint8_t id_buf[ IDBUF_NUMOF_ITEMS ] = { 0U };

    /* Validate arguments. */
    if ( is_invalid_rx_idx( rx_idx ) || ( NULL == msg ) || ( ( NULL == msg->content ) && ( 0U < msg->length ) ) )
        return CANDRV_FAILURE;

    /* CAN id. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_TBL[ IDXOF_READ_ID ][ rx_idx ] );
    picocan_read_array_spi( IDBUF_NUMOF_ITEMS, id_buf );
    picocan_end_spi_commands();

    /* CAN content length. */
    msg->length = id_buf[ IDBUF_DLC ];

    /* CAN content. */
    if ( 0U < msg->length ) {

        picocan_begin_spi_commands();
        picocan_write_spi( SPICMD_TBL[ IDXOF_READ_CONTENT ][ rx_idx ] );
        picocan_read_array_spi( msg->length, msg->content );
        picocan_end_spi_commands();
    }

    /* CAN kind. */
    msg->kind = ( 0U != ( id_buf[ IDBUF_SIDL ] & MASKOF_SIDL_IDE) ) ? CAN_KIND_EXT : CAN_KIND_STD;

    /* CAN id. */
    msg->id = ( CAN_KIND_STD == msg->kind )
                ? build_std_can_id( id_buf[ IDBUF_SIDL ], id_buf[ IDBUF_SIDH ] )
                : build_ext_can_id( id_buf[ IDBUF_EID0 ], id_buf[ IDBUF_EID8 ], id_buf[ IDBUF_SIDL ], id_buf[ IDBUF_SIDH ] );

    return CANDRV_SUCCESS;
}

void mcp2515_clr_all_send_req_if_err( void ) {

    /* Get status of send request. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_READ_STAT );
    const uint8_t stat = picocan_read_spi();
    picocan_end_spi_commands();

    /* Clear send request of TX0 if occurrs an error. */
    if ( ( 0U != ( stat & 0x04U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB0CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CANDRV_TX_0 );

    /* Clear send request of TX1 if occurrs an error. */
    if ( ( 0U != ( stat & 0x10U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB1CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CANDRV_TX_1 );

    /* Clear send request of TX2 if occurrs an error. */
    if ( ( 0U != ( stat & 0x40U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB2CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CANDRV_TX_2 );
}
