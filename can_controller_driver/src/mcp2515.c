#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "hardware_manager.h"
#include "hardware/timer.h"

/* MCP2515 operation modes. */


/*! 
 * \defgroup Mcp2515OpMode MCP2515 operation mode
 */
/*! \{ */
#define OPMODE_NORMAL       ( 0x00U )   /*!< Normal mode          */
#define OPMODE_SLEEP        ( 0x20U )   /*!< Sleep mode           */
#define OPMODE_LOOPBACK     ( 0x40U )   /*!< Loop back mode       */
#define OPMODE_LISTENONLY   ( 0x60U )   /*!< Listen only mode     */
#define OPMODE_CONFIG       ( 0x80U )   /*!< Config mode.         */
/*! \} */

/*!
 * @brief Time out of change operation mode.
 */
#define TIMEOUTOF_OPMODE_CHANGE   ( 1000000UL )       /*!< 1,000,000 us */

/*!
 * @brief Register item number of the CAN network budrate.
 */
#define BAUDRATE_NUMOF_ITEMS    ( 3 )   /* 3 items that CNF1, CNF2 and CNF3 */

static const uint8_t OPMODE_TBL[ MCP2515_OPMODE_NUMOF_ITEMS ]
    = { OPMODE_NORMAL, OPMODE_SLEEP, OPMODE_LOOPBACK, OPMODE_LISTENONLY, OPMODE_CONFIG };

static const uint8_t BAUDRATE_TBL[ MCP2515_BAUDRATE_NUMOF_ITEMS ][ BAUDRATE_NUMOF_ITEMS ] = {
    { 0xA7U, 0XBFU, 0x07U }, { 0x31U, 0XA4U, 0X04U }, { 0x18U, 0XA4U, 0x04U }, { 0x09U, 0XA4U, 0x04U }, { 0x04U, 0x9EU, 0x03U },
    { 0x03U, 0x9EU, 0x03U }, { 0x01U, 0x1EU, 0x03U }, { 0x00U, 0x9EU, 0x03U }, { 0x00U, 0x92U, 0x02U }, { 0x00U, 0x82U, 0x02U }
};

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

static uint8_t mcp2515_get_opmode( void ) {

    return (uint8_t)( mcp2515_read_register( REG_CANSTAT ) & MASKOF_OPMOD );
}

static cd_result_t wait_until_change_opmode( const uint8_t exp_mode ) {

    const uint32_t begun = time_us_32();
    uint8_t opr_mode = (uint8_t)( ~exp_mode );

    do {
        opr_mode = mcp2515_get_opmode();

        const uint32_t current = time_us_32();
        const uint32_t elapsed = ( current < begun ) ? ( UINT32_MAX - begun + current + 1UL ) : ( current - begun );

        if ( elapsed > TIMEOUTOF_OPMODE_CHANGE )
            return CD_FAILURE;

    } while ( exp_mode != opr_mode );

    return CD_SUCCESS;
}

cd_result_t mcp2515_reset( void ) {

    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_RESET );
    picocan_end_spi_commands();

    return wait_until_change_opmode( OPMODE_CONFIG );
}

static cd_result_t wakeup( void ) {

    const bool allow_wake = ( 0U != ( mcp2515_read_register( REG_CANINTE ) & MASKOF_CANINT_WAKIF ) ) ? true : false;

    /* To be enabled interrupt of wake up if disabled. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Request wake up. */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0xFFU );

    /* Temporarily switch to listen-only mode. */
    if ( CD_FAILURE == mcp2515_set_opmode( OPMODE_LISTENONLY ) )
        return CD_FAILURE;

    /* Restore interrupt of wake up. */
    if ( false == allow_wake )
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_WAKIF, 0U );

    /* Clear wake up request */
    mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );
}

cd_result_t mcp2515_set_opmode( const enum MCP2515_OPMODE mode ) {

    /* Fails if illegal arguments. */
    if( ( MCP2515_OPMODE_MINOF_IDX > mode ) || ( MCP2515_OPMODE_MAXOF_IDX < mode ) )
        return CD_FAILURE;

    const uint8_t next = OPMODE_TBL[ mode ];
    const uint8_t current = mcp2515_get_opmode();

    /* Do nothing if the operation mode does not change. */
    if ( current == next )
        return CD_SUCCESS;

    /* Wake up if current mode is sleep. */
    if ( ( OPMODE_SLEEP == current ) && ( CD_FAILURE == wakeup() ) )
        return CD_FAILURE;

    /* Clear waked up interruption if to be sleep. */
    if ( OPMODE_SLEEP == next )
        mcp2515_modbits_register( REG_CANINTF, MASKOF_CANINT_WAKIF, 0U );

    /* Apply next mode */
    mcp2515_modbits_register( REG_CANCTRL, MASKOF_OPMOD, next );

    /* Wait until applied. */
    return wait_until_change_opmode( next );
}

cd_result_t mcp2515_set_baudrate( const enum MCP2515_BAUDRATE baudrate ) {

    /* Validate arguments. */
    if ( ( MCP2515_BAUDRATE_MINOF_IDX > baudrate ) || ( MCP2515_BAUDRATE_MAXOF_IDX < baudrate ) )
        return CD_FAILURE;

    /* Fails if operation mode is not config mode. */
    if ( OPMODE_CONFIG != mcp2515_get_opmode() )
        return CD_FAILURE;

    /* Apply. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_WRITE_REG );
    picocan_write_spi( REG_CNF3 );
    picocan_write_array_spi( BAUDRATE_NUMOF_ITEMS, BAUDRATE_TBL[ baudrate ] );
    picocan_end_spi_commands();

    return CD_SUCCESS;
}

uint8_t mcp2515_get_numof_tx_err( void ) {

    return mcp2515_read_register( REG_TEC );
}
uint8_t mcp2515_get_numof_rx_err( void ) {

    return mcp2515_read_register( REG_REC );
}

void mcp2515_clear_all_send_req_if_error( void ) {

    /* Get status of send request. */
    picocan_begin_spi_commands();
    picocan_write_spi( SPICMD_READ_STAT );
    const uint8_t stat = picocan_read_spi();
    picocan_end_spi_commands();

    /* Clear send request of TX0 if occurrs an error. */
    if ( ( 0U != ( stat & 0x04U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB0CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CD_TX_0 );

    /* Clear send request of TX1 if occurrs an error. */
    if ( ( 0U != ( stat & 0x10U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB1CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CD_TX_1 );

    /* Clear send request of TX2 if occurrs an error. */
    if ( ( 0U != ( stat & 0x40U ) )
      && ( 0U != ( mcp2515_read_register( REG_TXB2CTRL ) & MASKOF_TXBCTRL_TXERR ) ) )
        mcp2515_clr_send_req( CD_TX_2 );
}

cd_result_t cd_init( void ) {

    bool go_on = true;

    hw_init();

    go_on = (bool)( go_on && ( HW_SUCCESS == hw_set_irq_callback( mcp2515_dispatch_irq ) ) );

    go_on = (bool)( go_on && ( CD_SUCCESS == mcp2515_reset() ) );

    go_on = (bool)( go_on && ( CD_SUCCESS == mcp2515_set_baudrate( MCP2515_BAUDRATE_1000KBPS ) ) );

    if ( go_on ) {

        /* Receive filter is none */
        mcp2515_modbits_register( REG_RXB0CTRL, MASKOF_RXBCTRL_RXM, 0U );

        /* Allow interruptions */
        mcp2515_modbits_register( REG_CANINTE, MASKOF_CANINT_MERRF | MASKOF_CANINT_ERRIF | 
            MASKOF_CANINT_RX0IF | MASKOF_CANINT_RX1IF, 0xFFU );
    }

    go_on = (bool)( go_on && ( CD_SUCCESS == mcp2515_set_opmode( MCP2515_OPMODE_NORMAL ) ) );

    return go_on ? CD_SUCCESS : CD_FAILURE;
}
