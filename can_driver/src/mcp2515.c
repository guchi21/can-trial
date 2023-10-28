#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "pico_can.h"
#include "hardware/timer.h"

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

#define OPMODE_NORMAL                   ( 0x00U )               /* Normal mode.         */
#define OPMODE_SLEEP                    ( 0x20U )               /* Sleep mode.          */
#define OPMODE_LOOPBACK                 ( 0x40U )               /* Loop back mode.      */
#define OPMODE_LISTENONLY               ( 0x60U )               /* Listen only mode.    */
#define OPMODE_CONFIG                   ( 0x80U )               /* Config mode.         */

#define BAUDRATE_NUMOF_ITEMS            ( 3 )                   /* Number of CAN baudrate configuration items. */

/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

static const uint8_t OPMODE_TBL[ MCP2515_OPMODE_NUMOF_ITEMS ]
    = { OPMODE_NORMAL, OPMODE_SLEEP, OPMODE_LOOPBACK, OPMODE_LISTENONLY, OPMODE_CONFIG };


static const uint8_t BAUDRATE_TBL[ MCP2515_BAUDRATE_NUMOF_ITEMS ][ BAUDRATE_NUMOF_ITEMS ] = {
    { 0xA7U, 0XBFU, 0x07U }, { 0x31U, 0XA4U, 0X04U }, { 0x18U, 0XA4U, 0x04U }, { 0x09U, 0XA4U, 0x04U }, { 0x04U, 0x9EU, 0x03U },
    { 0x03U, 0x9EU, 0x03U }, { 0x01U, 0x1EU, 0x03U }, { 0x00U, 0x9EU, 0x03U }, { 0x00U, 0x92U, 0x02U }, { 0x00U, 0x82U, 0x02U }
};


/*==================================================================*/
/* Variables.                                                       */
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

static uint8_t mcp2515_get_opr_mode( void ) {

    return (uint8_t)( mcp2515_read_register( REG_CANSTAT ) & MASKOF_OPMOD );
}

static candrv_result_t wait_until_change_opmode( const uint8_t exp_mode ) {

    const uint32_t begun = time_us_32();
    uint8_t opr_mode = OPMODE_INVALID;

    do {
        opr_mode = mcp2515_get_opr_mode();

        const uint32_t current = time_us_32();
        const uint32_t elapsed = ( current < begun ) ? ( UINT32_MAX - begun + current + 1UL ) : ( current - begun );

        if ( elapsed > TIMEOUTOF_OPMODE_CHANGE )
            return CANDRV_FAILURE;

    } while ( exp_mode != opr_mode );

    return CANDRV_SUCCESS;
}

candrv_result_t mcp2515_reset( void ) {

    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_RESET );
    picocan_end_spi_commands();

    return wait_until_change_opmode( OPMODE_CONFIG );
}

static candrv_result_t wakeup( void ) {

    const bool allow_wake = ( 0U != ( mcp2515_read_register( REG_CANINTE ) & MASKOF_CANINT_WAKIF ) ) ? true : false;

    /* To be enabled interrupt of wake up if disabled. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Request wake up. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Temporarily switch to listen-only mode. */
    if ( CANDRV_FAILURE == mcp2515_set_opmode( OPMODE_LISTENONLY ) )
        return CANDRV_FAILURE;

    /* Restore interrupt of wake up. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0U );

    /* Clear wake up request */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );
}

candrv_result_t mcp2515_set_opmode( const enum MCP2515_OPMODE mode ) {

    /* Fails if illegal arguments. */
    if( ( MCP2515_OPMODE_MINOF_IDX > mode ) || ( MCP2515_OPMODE_MAXOF_IDX < mode ) )
        return CANDRV_FAILURE;

    const uint8_t next = OPMODE_TBL[ mode ];
    const uint8_t current = mcp2515_get_opr_mode();

    /* Do nothing if the operation mode does not change. */
    if ( current == next )
        return CANDRV_SUCCESS;

    /* Wake up if current mode is sleep. */
    if ( ( OPMODE_SLEEP == current ) && ( CANDRV_FAILURE == wakeup() ) )
        return CANDRV_FAILURE;

    /* Clear waked up interruption if to be sleep. */
    if ( OPMODE_SLEEP == next )
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );

    /* Apply next mode */
    mcp2515_modbits_register( REG_CANCTRL, MASKOF_OPMOD, next );

    /* Wait until applied. */
    return wait_until_change_opmode( next );
}

candrv_result_t mcp2515_set_baudrate( const enum MCP2515_BAUDRATE baudrate ) {

    /* Validate arguments. */
    if ( ( MCP2515_BAUDRATE_MINOF_IDX > baudrate ) || ( MCP2515_BAUDRATE_MAXOF_IDX < baudrate ) )
        return CANDRV_FAILURE;

    /* Fails if operation mode is not config mode. */
    if ( OPMODE_CONFIG != mcp2515_get_opr_mode() )
        return CANDRV_FAILURE;

    /* Apply. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_WRITE_REG );
    picocan_write_spi( REG_CNF3 );
    picocan_write_array_spi( BAUDRATE_NUMOF_ITEMS, BAUDRATE_TBL[ baudrate ] );
    picocan_end_spi_commands();

    return CANDRV_SUCCESS;
}
