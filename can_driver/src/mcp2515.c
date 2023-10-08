#ifndef TYPES_H
#include "types.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
#endif

#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef MCP2515_REG_ADDR_H
#include "mcp2515_reg_addr.h"
#endif

#ifndef MCP2515_SPI_CMD_H
#include "mcp2515_spi_cmd.h"
#endif

#ifndef _HARDWARE_SPI_H
#include "hardware/spi.h"
#endif

#ifndef _HARDWARE_SYNC_H
#include "hardware/sync.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* MCP2515 timeout for write to the register. */
#define RESET_TIMEOUT   ( 1000ULL )

/* RP2040 pin voltage. */
#define PIN_VOLTAGE_LOW             ( 0U )
#define PIN_VOLTAGE_HIGH            ( 1U )


/* RP2040 SPI clock. */
#define SPI_BAUDRATE                ( 1000000U )        /* SPI baudrate in Hz. */


/* RP2040 GPIO numbers. */
#define GPIO_MCP2515_MISO           (  4U )             /* RP2040 pin #06.  */
#define GPIO_MCP2515_CS             (  5U )             /* RP2040 pin #07.  */
#define GPIO_MCP2515_SCK            (  6U )             /* RP2040 pin #09.  */
#define GPIO_MCP2515_MOSI           (  7U )             /* RP2040 pin #10.  */
#define GPIO_MCP2515_INT1           ( 21U )             /* RP2040 pin #27.  */


/* Related CAN id definitions */
#define STANDARD_CAN_ID_MAX         ( 0x7FFULL )        /* Maximum value of the standard CAN ID.    */
#define EXTENSION_CAN_ID_MAX        ( 0x1FFFFFFFULL )   /* Maximum value of the extension CAN ID.   */


/* Related CAN content length definitions */
#define CAN_CONTENT_LEN_MIN         ( 0U )              /* Minimum length of the CAN message data.  */
#define CAN_CONTENT_LEN_MAX         ( 8U )              /* Maximum length of the CAN message data.  */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef struct {
    uint8_t c1;
    uint8_t c2;
    uint8_t c3;
} can_baudrate_reg_t;


typedef struct{
    uint8_t sidh;
    uint8_t sidl;
    uint8_t eid8;
    uint8_t eid0;
} can_id_reg_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/
const can_baudrate_reg_t can_baudrates[ MCP2515_CAN_BAUDRATE_NUMOF_ITEMS ] = {
    { 0xA7U, 0XBFU, 0x07U },
    { 0x31U, 0XA4U, 0X04U },
    { 0x18U, 0XA4U, 0x04U },
    { 0x09U, 0XA4U, 0x04U },
    { 0x04U, 0x9EU, 0x03U },
    { 0x03U, 0x9EU, 0x03U },
    { 0x01U, 0x1EU, 0x03U },
    { 0x00U, 0x9EU, 0x03U },
    { 0x00U, 0x92U, 0x02U },
    { 0x00U, 0x82U, 0x02U }
};


/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

uint8_t mcp2515_read_register( const uint8_t addr ) {

    rp2040_begin_spi_commands();

    rp2040_write_spi( SPICMD_READ_REG );
    rp2040_write_spi( addr );

    const uint8_t val = rp2040_read_spi();

    rp2040_end_spi_commands();

    return val;
}

void mcp2515_write_register( const uint8_t addr, const uint8_t val ) {

    rp2040_begin_spi_commands();

    rp2040_write_spi( SPICMD_WRITE_REG );
    rp2040_write_spi( addr );
    rp2040_write_spi( val );

    rp2040_end_spi_commands();
}

void mcp2515_modbits_register( const uint8_t addr, const uint8_t maskof_write, const uint8_t val ) {

    rp2040_begin_spi_commands();

    rp2040_write_spi( SPICMD_MODBITS_REG );
    rp2040_write_spi( addr );
    rp2040_write_spi( maskof_write );
    rp2040_write_spi( val );

    rp2040_end_spi_commands();
}

uint8_t mcp2515_get_opr_mode( void ) {

    return (uint8_t) ( mcp2515_read_register( REG_CANSTAT ) & MASKOF_CANSTAT_OPMOD );
}

static candrv_result_t verify_opr_mode_blocking( const uint8_t exp_mode ) {

    const uint32_t begun = time_us_32();
    uint8_t opr_mode = OPR_MODE_INVALID;

    do {
        opr_mode = mcp2515_get_opr_mode();

        const uint32_t current = time_us_32();
        const uint32_t elapsed = ( current < begun ) ? ( UINT32_VALUE_MAX - begun + current + 1UL ) : ( current - begun );

        if ( elapsed > TIMEOUTOF_OPR_MODE_CHANGE ) {

            return CANDRV_FAILURE;
        }

    } while ( exp_mode != opr_mode );

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_reset_blocking( void ) {

    rp2040_begin_spi_commands();

    rp2040_write_spi( SPICMD_RESET );

    rp2040_end_spi_commands();

    return verify_opr_mode_blocking( OPR_MODE_CONFIG );
}

static candrv_result_t wakeup( void ) {

    const bool allow_wakeup_int = ( 0U < ( mcp2515_read_register( REG_CANINTE_FLGS ) & MASKOF_CANINTE_WAKIF ) ) ? true : false;

    /* To be enabled interrupt of wake up if disabled. */
    if ( false == allow_wakeup_int ) {

        mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_WAKIF, MASKOF_CANINTE_WAKIF );
    }

    /* Request wake up. */
    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, MASKOF_CANINTF_WAKIF );

    /* Temporarily switch to listen-only mode. */
    if ( CANDRV_FAILURE == mcp2515_change_opr_mode_blocking( OPR_MODE_LISTENONLY ) ) {

        return CANDRV_FAILURE;
    }

    /* Restore interrupt of wake up. */
    if ( false == allow_wakeup_int ) {

        mcp2515_modbits_register( REG_CANINTE_FLGS, MASKOF_CANINTE_WAKIF, (uint8_t) ( ~MASKOF_CANINTE_WAKIF ) );
    }

    /* Clear wake up request */
    mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, (uint8_t) ( ~MASKOF_CANINTF_WAKIF ) );
}

candrv_result_t mcp2515_change_opr_mode_blocking( const uint8_t mode ) {

    uint8_t current;

    /* Verify request mode. */
    switch ( mode ) {
    
    case OPR_MODE_NORMAL:
    case OPR_MODE_SLEEP:
    case OPR_MODE_LOOPBACK:
    case OPR_MODE_LISTENONLY:
    case OPR_MODE_CONFIG:

        break;

    default:

        return CANDRV_FAILURE;
    }

    current = mcp2515_get_opr_mode();

    /* Do nothing if the operation mode does not change. */
    if ( current == mode ) {

        return CANDRV_SUCCESS;
    }

    /* Wake up if current mode is sleep. */
    if ( OPR_MODE_SLEEP == current ) {

        if ( CANDRV_FAILURE == wakeup() ) {

            return CANDRV_FAILURE;
        }
    }

    /* Clear waked up request if to be sleep. */
    if ( OPR_MODE_SLEEP == mode ) {

        mcp2515_modbits_register( REG_CANINTF_FLGS, MASKOF_CANINTF_WAKIF, (uint8_t) ( ~MASKOF_CANINTF_WAKIF ) );
    }

    /* Apply request mode */
    mcp2515_modbits_register( REG_CANCTRL_FLGS, MASKOF_CANCTRL_OPMOD, mode );

    /* Wait until applied. */
    return verify_opr_mode_blocking( mode );
}

candrv_result_t mcp2515_change_can_baudrate( const enum MCP2515_CAN_BAUDRATE baudrate ) {

    if ( MCP2515_CAN_BAUDRATE_INDEX_MIN > baudrate || MCP2515_CAN_BAUDRATE_INDEX_MAX < baudrate ) {

        return CANDRV_FAILURE;
    }

    if ( OPR_MODE_CONFIG != mcp2515_get_opr_mode() ) {

        return CANDRV_FAILURE;
    }

    can_baudrate_reg_t conf = can_baudrates[ baudrate ];

    mcp2515_write_register( REG_CNF1_FLGS, conf.c1 );
    mcp2515_write_register( REG_CNF2_FLGS, conf.c2 );
    mcp2515_write_register( REG_CNF3_FLGS, conf.c3 );

    return true;
}

candrv_result_t mcp2515_change_tx_priority( const enum MCP2515_TX tx_idx, const uint8_t priority ) {

    /* Verify priprity. */
    if ( TX_PRIORITY_HIGH < priority ) {

        return CANDRV_FAILURE;
    }

    /* Verify currunt operation mode. */
    if ( OPR_MODE_CONFIG != mcp2515_get_opr_mode() ) {

        return CANDRV_FAILURE;
    }

    switch ( tx_idx ) {

    case MCP2515_TX_0:

        mcp2515_modbits_register( REG_TXB0CTRL_FLGS, MASKOF_TXB0CTRL_TXP, priority );
        break;

    case MCP2515_TX_1:

        mcp2515_modbits_register( REG_TXB1CTRL_FLGS, MASKOF_TXB1CTRL_TXP, priority );
        break;

    case MCP2515_TX_2:

        mcp2515_modbits_register( REG_TXB2CTRL_FLGS, MASKOF_TXB2CTRL_TXP, priority );
        break;

    default:

        return CANDRV_FAILURE;
    }

    return CANDRV_SUCCESS;
}

static bool is_valid_can_id( const can_message_t *const msg ) {

    return (   ( NULL != msg )
            && (   ( CAN_FORMAT_KIND_STANDARD  == ( msg->kind ) &&  STANDARD_CAN_ID_MAX >= ( msg->id ) )
                || ( CAN_FORMAT_KIND_EXTENSION == ( msg->kind ) && EXTENSION_CAN_ID_MAX >= ( msg->id ) ) ) )
        ? true : false;
}

static bool is_valid_can_kind( const can_message_t *const msg ) {

    return ( ( NULL != msg ) && ( CAN_FORMAT_KIND_INDEX_MIN <= ( msg->kind ) ) && ( CAN_FORMAT_KIND_INDEX_MAX >= ( msg->kind ) ) )
        ? true : false;
}

static bool is_valid_can_message( const can_message_t *const msg ) {

    return (   ( true == is_valid_can_id( msg ) ) && ( CAN_FORMAT_KIND_INDEX_MIN <= ( msg->kind ) ) && ( CAN_FORMAT_KIND_INDEX_MAX >= ( msg->kind ) )
            && ( CAN_CONTENT_LEN_MAX >= ( msg->length ) ) && ( ( CAN_CONTENT_LEN_MIN == ( msg->length ) ) || ( NULL != ( msg->content ) ) ) )
        ? true : false;
}

static candrv_result_t build_can_id_reg( const can_message_t *const msg, can_id_reg_t *const id_reg ) {

    uint32_t id;

    /* Begin verify arguments. */
    if ( false == is_valid_can_message( msg ) || NULL == id_reg ) {

        return CANDRV_FAILURE;
    }
    /* End verify arguments. */

    id = msg->id;

    if ( CAN_FORMAT_KIND_EXTENSION == ( msg->kind ) ) {
        
        id_reg->eid0 = (uint8_t) ( id & 0xFFU );
        id_reg->eid8 = (uint8_t) ( (uint8_t) ( id >> 8 ) & 0xFFU );

        /* Begin SIDL */
        id_reg->sidl = (uint8_t) ( (uint8_t) ( id >> 16U ) & 0x03U );                                 /* Bit0 to Bit1 */
        id_reg->sidl = (uint8_t) ( id_reg->sidl + 0x08U );                                            /* Bit2 to Bit4 */
        id_reg->sidl = (uint8_t) ( id_reg->sidl + (uint8_t) ( (uint8_t) ( id >> 13U ) & 0xE0U ) );    /* Bit5 to Bit7 */
        /* End SIDL */

        id_reg->sidh = (uint8_t) ( (uint8_t) ( id >> 21 ) & 0xFFU );
    } 
    else {

        id_reg->eid0 = 0U;
        id_reg->eid8 = 0U;
        id_reg->sidl = (uint8_t) ( (uint8_t) ( id << 5 ) & 0xE0U );
        id_reg->sidh = (uint8_t) ( (uint8_t) ( id >> 3 ) & 0xFFU );
    }

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_set_tx_buffer( const enum MCP2515_TX tx_idx, const can_message_t *const msg ) {

    can_id_reg_t id_reg;
    uint8_t id_buf[5];
    uint8_t cmd_write_id;
    uint8_t cmd_write_buf;

    if ( CANDRV_FAILURE == build_can_id_reg( msg, &id_reg ) ) {

        return CANDRV_FAILURE;
    }

    switch ( tx_idx ) {

    case MCP2515_TX_0:

        cmd_write_id = SPICMD_WRITE_TX0ID;
        cmd_write_buf = SPICMD_WRITE_TX0DATA;
        break;

    case MCP2515_TX_1:

        cmd_write_id = SPICMD_WRITE_TX1ID;
        cmd_write_buf = SPICMD_WRITE_TX1DATA;
        break;

    case MCP2515_TX_2:

        cmd_write_id = SPICMD_WRITE_TX2ID;
        cmd_write_buf = SPICMD_WRITE_TX2DATA;
        break;

    default:

        return CANDRV_FAILURE;
    }

    id_buf[0] = id_reg.sidh;
    id_buf[1] = id_reg.sidl;
    id_buf[2] = id_reg.eid8;
    id_buf[3] = id_reg.eid0;
    id_buf[4] = msg->length;

    rp2040_begin_spi_commands();

    rp2040_write_spi( SPICMD_WRITE_TX0ID );
    rp2040_write_array_spi( id_buf, (uint8_t) ( sizeof( id_buf ) / sizeof( id_buf[ 0 ] ) ) );

    rp2040_end_spi_commands();
    
    if ( 0U < msg->length ) {

        rp2040_begin_spi_commands();

        rp2040_write_spi( SPICMD_WRITE_TX0DATA );
        rp2040_write_array_spi( msg->content, msg->length );

        rp2040_end_spi_commands();
    }

    return true;
}

candrv_result_t mcp2515_req_send_msg( const enum MCP2515_TX tx_idx ) {

    uint8_t cmd;
    uint8_t ctrl_reg;
    uint8_t maskof_req;

    switch ( tx_idx ) {

    case MCP2515_TX_0:

        cmd = SPICMD_REQ_TX0;
        ctrl_reg = REG_TXB0CTRL_FLGS;
        maskof_req = MASKOF_TXB0CTRL_TXP;
        break;

    case MCP2515_TX_1:

        cmd = SPICMD_REQ_TX1;
        ctrl_reg = REG_TXB1CTRL_FLGS;
        maskof_req = MASKOF_TXB1CTRL_TXP;
        break;

    case MCP2515_TX_2:

        cmd = SPICMD_REQ_TX2;
        ctrl_reg = REG_TXB1CTRL_FLGS;
        maskof_req = MASKOF_TXB2CTRL_TXP;
        break;

    default:

        return CANDRV_FAILURE;
    }

    if ( 0U < (uint8_t) ( mcp2515_read_register( ctrl_reg ) & maskof_req ) ) {

        return CANDRV_FAILURE;
    }

    rp2040_begin_spi_commands();

    rp2040_write_spi( cmd );

    rp2040_end_spi_commands();
}

