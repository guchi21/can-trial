#ifndef MCP2515_H
#include "mcp2515.h"
#endif

#ifndef MCP2515_REG_ADDR_H
#include "mcp2515_reg_addr.h"
#endif

#ifndef MCP2515_SPI_CMD_H
#include "mcp2515_spi_cmd.h"
#endif

#include "pico_can.h"

#ifndef _HARDWARE_TIMER_H
#include "hardware/timer.h"
#endif

#ifndef _HARDWARE_SYNC_H
#include "hardware/sync.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* Related CAN id definitions */
#define MAXOF_STD_CAN_ID                ( 0x000007FFULL )                       /* Maximum value of the standard CAN ID.    */
#define MAXOF_EXT_CAN_ID                ( 0x1FFFFFFFULL )                       /* Maximum value of the extension CAN ID.   */

/* Related CAN content length definitions */
#define MINOF_CAN_LEN                   ( 0U )                                  /* Minimum length of the content of CAN message.  */
#define MAXOF_CAN_LEN                   ( 8U )                                  /* Maximum length of the content of CAN message.  */

/* Index of the buffer corresponding to the CAN ID on the register. */
#define IDBUF_SIDH_IDX                  ( 0U )
#define IDBUF_SIDL_IDX                  ( 1U )
#define IDBUF_EID8_IDX                  ( 2U )
#define IDBUF_EID0_IDX                  ( 3U )
#define IDBUF_DLC_IDX                   ( 4U )
#define IDBUF_NUMOF_ITEMS               ( 5U )

/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef struct{ uint8_t sidh, sidl, eid8, eid0; } can_id_reg_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/


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

        if ( elapsed > TIMEOUTOF_OPR_MODE_CHANGE ) {

            return CANDRV_FAILURE;
        }

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

    const bool allow_wake = ( 0U < ( mcp2515_read_register( REG_CANINTE_FLGS ) & MASKOF_CANINTE_WAKIF ) ) ? true : false;

    /* To be enabled interrupt of wake up if disabled. */
    if ( false == allow_wake ) {

        mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_WAKIF, 0xFFU );
    }

    /* Request wake up. */
    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, 0xFFU );

    /* Temporarily switch to listen-only mode. */
    if ( CANDRV_FAILURE == mcp2515_change_opr_mode( OPR_MODE_LISTENONLY ) ) {

        return CANDRV_FAILURE;
    }

    /* Restore interrupt of wake up. */
    if ( false == allow_wake ) {

        mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_WAKIF, 0U );
    }

    /* Clear wake up request */
    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, 0U );
}

candrv_result_t mcp2515_change_opr_mode( const uint8_t mode ) {

    uint8_t current;

    if ( ( OPR_MODE_NORMAL     == mode ) || ( OPR_MODE_SLEEP  == mode ) || ( OPR_MODE_LOOPBACK == mode )
      || ( OPR_MODE_LISTENONLY == mode ) || ( OPR_MODE_CONFIG == mode ) ) {

        current = mcp2515_get_opr_mode();

        /* Do nothing if the operation mode does not change. */
        if ( current == mode ) {

            return CANDRV_SUCCESS;
        }

        /* Wake up if current mode is sleep. */
        if ( ( OPR_MODE_SLEEP == current ) && ( CANDRV_FAILURE == wakeup() ) ) {

            return CANDRV_FAILURE;
        }

        /* Clear waked up request if to be sleep. */
        if ( OPR_MODE_SLEEP == mode ) {

            mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, 0U );
        }

        /* Apply request mode */
        mcp2515_modbits_register( REG_CANCTRL_FLGS, MASKOF_CANCTRL_OPMOD, mode );

        /* Wait until applied. */
        return verify_opr_mode( mode );
    }
    else {

        return CANDRV_FAILURE;
    }
}

candrv_result_t mcp2515_change_can_baudrate( const enum MCP2515_CAN_BAUDRATE baudrate ) {
    
    /* Begin validate arguments. */
    if ( ( MCP2515_CAN_BAUDRATE_MINOF_IDX > baudrate ) || ( MCP2515_CAN_BAUDRATE_MAXOF_IDX < baudrate ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */


    struct CAN_BAUDRATE_CTX { uint8_t c1, c2, c3; };

    const struct CAN_BAUDRATE_CTX baudrate_ctx_table[ MCP2515_CAN_BAUDRATE_NUMOF_ITEMS ] = {
        { 0xA7U, 0XBFU, 0x07U }, { 0x31U, 0XA4U, 0X04U }, { 0x18U, 0XA4U, 0x04U }, { 0x09U, 0XA4U, 0x04U }, { 0x04U, 0x9EU, 0x03U },
        { 0x03U, 0x9EU, 0x03U }, { 0x01U, 0x1EU, 0x03U }, { 0x00U, 0x9EU, 0x03U }, { 0x00U, 0x92U, 0x02U }, { 0x00U, 0x82U, 0x02U }
    };

    /* Confirm operation mode. */
    if ( OPR_MODE_CONFIG != mcp2515_get_opr_mode() ) {

        return CANDRV_FAILURE;
    }

    struct CAN_BAUDRATE_CTX conf = baudrate_ctx_table[ baudrate ];

    mcp2515_write_register( REG_CNF1_FLGS, conf.c1 );
    mcp2515_write_register( REG_CNF2_FLGS, conf.c2 );
    mcp2515_write_register( REG_CNF3_FLGS, conf.c3 );

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_change_tx_priority( const enum CANDRV_TX tx_idx, const uint8_t priority ) {

    /* Begin validate arguments. */
    if( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx )
     || ( TX_PRIORITY_HIGH < priority ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */


    const uint8_t reg_addr_table[ CANDRV_TX_NUMOF_ITEMS ] = { REG_TXB0CTRL_FLGS  , REG_TXB1CTRL_FLGS  , REG_TXB2CTRL_FLGS   };
    const uint8_t reg_mask_table[ CANDRV_TX_NUMOF_ITEMS ] = { MASKOF_TXB0CTRL_TXP, MASKOF_TXB1CTRL_TXP, MASKOF_TXB2CTRL_TXP };


    /* Verify currunt operation mode. */
    if ( OPR_MODE_CONFIG != mcp2515_get_opr_mode() ) {

        return CANDRV_FAILURE;
    }

    mcp2515_modbits_register( reg_addr_table[ tx_idx ], reg_mask_table[ tx_idx ], priority );

    return CANDRV_SUCCESS;
}

static candrv_result_t build_can_id_reg( const uint32_t can_id, const can_format_kind_t can_kind,
                                         can_id_reg_t *const id_reg ) {

    if ( ( CAN_KIND_MINOF_IDX > can_kind ) || ( CAN_KIND_MAXOF_IDX < can_kind )
      || ( NULL == id_reg )
      || ( CAN_KIND_STD == can_kind && can_id > MAXOF_STD_CAN_ID )
      || ( CAN_KIND_EXT == can_kind && can_id > MAXOF_EXT_CAN_ID ) ) {

        return CANDRV_FAILURE;
    }

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

static bool is_available_tx( const enum CANDRV_TX tx_idx ) {

    /* Begin validate arguments. */
    if( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */

    struct reg_ctx_t { uint8_t send_req, maskof_send_req, maskof_irq_occered; };

    const struct reg_ctx_t reg_ctx_table[ CANDRV_TX_NUMOF_ITEMS ] = {
        { REG_TXB0CTRL_FLGS, MASKOF_TXB0CTRL_TXP, MASKOF_CANINTF_TX0IF },
        { REG_TXB1CTRL_FLGS, MASKOF_TXB1CTRL_TXP, MASKOF_CANINTF_TX1IF },
        { REG_TXB2CTRL_FLGS, MASKOF_TXB2CTRL_TXP, MASKOF_CANINTF_TX2IF } };

    const struct reg_ctx_t reg_ctx = reg_ctx_table[ tx_idx ];

    if (   ( 0U < ( mcp2515_read_register( reg_ctx.send_req ) & reg_ctx.maskof_send_req    ) )
        || ( 0U < ( mcp2515_read_register( REG_CANINTF_FLGS ) & reg_ctx.maskof_irq_occered ) ) ) {

        /* If already requested. */
        /* If there is an outstanding transmission completion interrupt. */
        return false;
    }

    return true;
}

candrv_result_t mcp2515_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg ) {

    /* Begin validate arguments. */
    if( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx ) || ( NULL == msg )
     || ( CAN_KIND_MINOF_IDX > ( msg->kind ) ) || ( CAN_KIND_MAXOF_IDX < ( msg->kind ) ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */


    const uint8_t spicmd_write_id_table[ CANDRV_TX_NUMOF_ITEMS ] = {
        SPICMD_WRITE_TX0_ID, SPICMD_WRITE_TX1_ID, SPICMD_WRITE_TX2_ID };

    const uint8_t spicmd_write_content_table[ CANDRV_TX_NUMOF_ITEMS ] = {
        SPICMD_WRITE_TX0_CONTENT, SPICMD_WRITE_TX1_CONTENT, SPICMD_WRITE_TX2_CONTENT };

    can_id_reg_t id_reg;    /* CAN id in the register format.       */


    /* Confirm available of TX. */
    if ( false == is_available_tx( tx_idx ) ) {

        return CANDRV_FAILURE;
    }

    /* Write to the register that CAN content. */
    if ( ( NULL != ( msg->content ) )
      && ( MINOF_CAN_LEN < ( msg->length ) ) && ( MAXOF_CAN_LEN >= ( msg->length ) ) ) {

        picocan_begin_spi_commands();
        picocan_write_spi( spicmd_write_content_table[ tx_idx ] );
        picocan_write_array_spi( msg->length, msg->content );
        picocan_end_spi_commands();
    }
    else if ( ( MINOF_CAN_LEN == ( msg->length ) ) ) {

        /* No processing. */
    }
    else {

        return CANDRV_FAILURE;
    }


    /* Build CAN id in the register format. */
    if ( CANDRV_FAILURE == build_can_id_reg( msg->id, msg->kind, &id_reg ) ) {

        return CANDRV_FAILURE;
    }

    const uint8_t id_buf[ IDBUF_NUMOF_ITEMS ] 
        = { id_reg.sidh, id_reg.sidl, id_reg.eid8, id_reg.eid0, msg->length };


    /* Write to the register that CAN id and DLC. */
    picocan_begin_spi_commands();
    picocan_write_spi( spicmd_write_id_table[ tx_idx ] );
    picocan_write_array_spi( IDBUF_NUMOF_ITEMS, id_buf );
    picocan_write_spi( msg->length );
    picocan_end_spi_commands();


    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_req_send_msg( const enum CANDRV_TX tx_idx ) {

    /* Begin validate arguments. */
    if( ( CANDRV_TX_MINOF_IDX > tx_idx ) || ( CANDRV_TX_MAXOF_IDX < tx_idx ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */


    const uint8_t spicmd_table[ CANDRV_TX_NUMOF_ITEMS ] = { SPICMD_REQ_TX0, SPICMD_REQ_TX1, SPICMD_REQ_TX2 };
    const uint8_t maskof_caninte_table[ CANDRV_TX_NUMOF_ITEMS ] 
        = { MASKOF_CANINTE_TX0IF, MASKOF_CANINTE_TX1IF, MASKOF_CANINTE_TX2IF };


    if ( false == is_available_tx( tx_idx ) ) {

        return CANDRV_FAILURE;
    }

    /* Enabled TX interrupt. */
    mcp2515_modbits_register( REG_CANINTE_FLGS, maskof_caninte_table[ tx_idx ], 0xFFU );

    /* Requested send CAN message. */
    picocan_begin_spi_commands();
    picocan_write_spi( spicmd_table[ tx_idx ] );
    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}

static uint32_t build_ext_can_id( const uint8_t eid0, const uint8_t eid8, const uint8_t sidl, const uint8_t sidh ) {

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

candrv_result_t mcp2515_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg ) {

    /* Begin validate arguments. */
    if( ( CANDRV_RX_MINOF_IDX > rx_idx ) || ( CANDRV_RX_MAXOF_IDX < rx_idx ) || ( NULL == msg ) ) {

        return CANDRV_FAILURE;
    }
    /* End validate arguments. */

    struct RX_SPICMD { uint8_t read_id, read_content; };

    const struct RX_SPICMD rx_spicmd_table[ CANDRV_RX_NUMOF_ITEMS ] = {
        { SPICMD_READ_RX0_ID, SPICMD_READ_RX0_CONTENT },
        { SPICMD_READ_RX1_ID, SPICMD_READ_RX1_CONTENT }
    };

    const struct RX_SPICMD spicmd = rx_spicmd_table[ rx_idx ];
    uint8_t id_buf[ IDBUF_NUMOF_ITEMS ] = { 0U };
    uint8_t *const content = msg->content;
    uint8_t length;

    picocan_begin_spi_commands();
    picocan_write_spi( spicmd.read_id );
    picocan_read_array_spi( IDBUF_NUMOF_ITEMS, id_buf );
    picocan_end_spi_commands();

    /* Content length. */
    length = id_buf[ IDBUF_DLC_IDX ];
    msg->length = length;

    /* CAN content. */
    if ( ( NULL != content ) && ( MINOF_CAN_LEN < length ) && ( MAXOF_CAN_LEN >= length ) ) {

        picocan_begin_spi_commands();
        picocan_write_spi( spicmd.read_content );
        picocan_read_array_spi( length, content );
        picocan_end_spi_commands();
    }
    else if ( MINOF_CAN_LEN == length ) {

        /* No processing. */
    }
    else {

        return CANDRV_FAILURE;
    }

    /* CAN kind. */
    msg->kind = ( 0U < ( id_buf[ IDBUF_SIDL_IDX ] & MASKOF_SIDL_IDE ) ) ? CAN_KIND_EXT : CAN_KIND_STD;

    /* CAN id. */
    msg->id = ( CAN_KIND_STD == ( msg->kind ) )
        ? build_std_can_id( id_buf[ IDBUF_SIDL_IDX ], id_buf[ IDBUF_SIDH_IDX ] )
        : build_ext_can_id( id_buf[ IDBUF_EID0_IDX ], id_buf[ IDBUF_EID8_IDX ],
                            id_buf[ IDBUF_SIDL_IDX ], id_buf[ IDBUF_SIDH_IDX ] );

    return CANDRV_SUCCESS;
}

