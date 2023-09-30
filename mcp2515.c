#include "mcp2515.h"
#include "comdef.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"
#include <stdio.h>

/* SPI baudrate definition in Hz */
#define SPI_CLOCK           ( (uint) 1000000U )

/* SPI pin number definitions */
#define SPI_PIN_RX          ( (uint) 4U )
#define SPI_PIN_CS          ( (uint) 5U )
#define SPI_PIN_SCK         ( (uint) 6U )
#define SPI_PIN_TX          ( (uint) 7U )
#define PIN_INT             ( (uint) 21U )


/* SPI commands */
#define CMD_WRITE_REG       ( (uint8_t) 0x02U )
#define CMD_READ_REG        ( (uint8_t) 0x03U )
#define CMD_MODBITS_REG     ( (uint8_t) 0x05U )
#define CMD_RESET           ( (uint8_t) 0xC0U )

/* MCP2515 timeout of write to register in microseconds */
#define TIMEOUT_WRITE_REG   ( (uint64_t) 1000ULL )

/* MCP2515 operation mode mask */
#define MASK_MODE           ( (uint8_t) 0xE0U )

/* MCP2515 operation modes */
#define MODE_NORMAL         ( (uint8_t) 0x00U )
#define MODE_SLEEP          ( (uint8_t) 0x20U )
#define MODE_LOOPBACK       ( (uint8_t) 0x40U )
#define MODE_LISTENONLY     ( (uint8_t) 0x60U )
#define MODE_CONFIG         ( (uint8_t) 0x80U )

/* MCP2515 registers address. CAN control. */
#define REG_CANSTAT         ( (uint8_t) 0x0EU )

/* MCP2515 registers address. CAN status. */
#define REG_CANCTRL         ( (uint8_t) 0x0FU )

/* MCP2515 registers address. CAN bus interface bit timing. */
#define REG_CNF3            ( (uint8_t) 0x28U )
#define REG_CNF2            ( (uint8_t) 0x29U )
#define REG_CNF1            ( (uint8_t) 0x2AU )

/* MCP2515 interrupt mask */
#define MASK_CANINT_RX0     ( (uint8_t) 0x01U )
#define MASK_CANINT_RX1     ( (uint8_t) 0x02U )
#define MASK_CANINT_TX0     ( (uint8_t) 0x04U )
#define MASK_CANINT_TX1     ( (uint8_t) 0x08U )
#define MASK_CANINT_TX2     ( (uint8_t) 0x10U )
#define MASK_CANINT_ERR     ( (uint8_t) 0x20U )
#define MASK_CANINT_WAK     ( (uint8_t) 0x40U )
#define MASK_CANINT_MERR    ( (uint8_t) 0x80U )

/* MCP2515 registers address. CAN interrupt enables. */
#define REG_CANINTE         ( (uint8_t) 0x2BU )

/* MCP2515 registers address. CAN interrupt flags. */
#define REG_CANINTF         ( (uint8_t) 0x2CU )

/* MCP2515 registers address. Tx buffer #1. */
#define REG_TXB0CTRL        ( (uint8_t) 0x30U )
#define REG_TXB0SIDH        ( (uint8_t) 0x31U )
#define REG_TXB0SIDL        ( (uint8_t) 0x32U )
#define REG_TXB0EID8        ( (uint8_t) 0x33U )
#define REG_TXB0EID0        ( (uint8_t) 0x34U )
#define REG_TXB0DLC         ( (uint8_t) 0x35U )
#define REG_TXB0D0          ( (uint8_t) 0x36U )
#define REG_TXB0D1          ( (uint8_t) 0x37U )
#define REG_TXB0D2          ( (uint8_t) 0x38U )
#define REG_TXB0D3          ( (uint8_t) 0x39U )
#define REG_TXB0D4          ( (uint8_t) 0x3AU )
#define REG_TXB0D5          ( (uint8_t) 0x3BU )
#define REG_TXB0D6          ( (uint8_t) 0x3CU )
#define REG_TXB0D7          ( (uint8_t) 0x3DU )

#define CANINT_TX0_ENABLE   ( (uint8_t) 0x04U )
#define CANINT_RX0_ENABLE   ( (uint8_t) 0x01U )
#define CANINT_RX1_ENABLE   ( (uint8_t) 0x02U )
#define CANINT_TX0_ENABLE   ( (uint8_t) 0x04U )
#define CANINT_TX1_ENABLE   ( (uint8_t) 0x08U )
#define CANINT_TX2_ENABLE   ( (uint8_t) 0x10U )
#define CANINT_ERR_ENABLE   ( (uint8_t) 0x20U )
#define CANINT_WAK_ENABLE   ( (uint8_t) 0x40U )
#define CANINT_MERR_ENABLE  ( (uint8_t) 0x80U )

#define DLC8  ( (uint8_t) 8U )

typedef enum MCP2515_CANRATE
{
    CANRATE_5KBPS = 0,
    CANRATE_10KBPS,
    CANRATE_20KBPS,
    CANRATE_50KBPS,
    CANRATE_100KBPS,
    CANRATE_125KBPS,
    CANRATE_250KBPS,
    CANRATE_500KBPS,
    CANRATE_800KBPS,
    CANRATE_1000KBPS,
    CANRATE_SIZE
} en_mcp2515_canrate;

static void spi_write( const uint8_t value )
{
    spi_write_blocking( spi0, &value, 1 );
}

static uint8_t spi_read()
{
    uint8_t buf[1];

    spi_read_blocking( spi0, 0, buf, 1 );

    return buf[0];
}

static void spi_op_begin() 
{
    gpio_put( SPI_PIN_CS, LOW );
}

static void spi_op_end()
{
    gpio_put( SPI_PIN_CS, HIGH );
}

static void spi_configure()
{
    /*-----------------------------------------------------------*/
    /* Baudrate: 10MHz, RX: 4pin, CSn: 5pin, SCK: 6pin, TX: 7pin */
    /*-----------------------------------------------------------*/

    spi_init( spi0, SPI_CLOCK );

    gpio_set_function( SPI_PIN_RX, GPIO_FUNC_SPI );
    gpio_set_function( SPI_PIN_TX, GPIO_FUNC_SPI );
    gpio_set_function( SPI_PIN_SCK, GPIO_FUNC_SPI );

    gpio_init( SPI_PIN_CS );
    gpio_set_dir( SPI_PIN_CS, GPIO_OUT );
    spi_op_end();
}

static uint8_t mcp2515_read_reg( const uint8_t addr )
{
    spi_op_begin();

    spi_write(CMD_READ_REG);
    spi_write(addr);

    const uint8_t val = spi_read();

    spi_op_end();

    return val;
}

static bool mcp2515_verify_written( const uint8_t addr, const uint8_t exp_mask, const uint8_t exp_val )
{
    const uint64_t begin = time_us_64();

    do
    {
        const uint64_t now = time_us_64();

        const uint64_t diff = !( now < begin ) 
                                ? (uint64_t) ( now - begin )
                                : (uint64_t) ( UINT64MAX - begin + now + 1 );

        if ( diff > TIMEOUT_WRITE_REG )
            return false;

    } while ( exp_val != (uint8_t) ( mcp2515_read_reg( addr ) & exp_mask ) );

    return true;
}

static bool mcp2515_write_reg( const uint8_t addr, const uint8_t data )
{
    spi_op_begin();

    spi_write( CMD_WRITE_REG );
    spi_write( addr );
    spi_write( data );

    spi_op_end();

    return mcp2515_verify_written( addr, 0xFFU, data );
}

static bool mcp2515_modbits_reg( const uint8_t addr, const uint8_t mask, const uint8_t data )
{
    const uint8_t val = (uint8_t) ( data & mask );

    spi_op_begin();

    spi_write( CMD_MODBITS_REG );
    spi_write( addr );
    spi_write( mask );
    spi_write( val );

    spi_op_end();

    return mcp2515_verify_written( addr, mask, val );
}

static bool mcp2515_request_newmode( const uint8_t mode )
{
    const uint8_t request = (uint8_t) ( mode & MASK_MODE );
    const uint8_t val = (uint8_t) ( request | (uint8_t) ( mcp2515_read_reg( REG_CANCTRL ) & (uint8_t) ( ~MASK_MODE ) ) );

    return mcp2515_write_reg( REG_CANCTRL, val );
}

static bool mcp2515_wakeup()
{
    const bool allow_wakeupint = (bool) ( 0 < (uint8_t) ( mcp2515_read_reg( REG_CANINTE ) & MASK_CANINT_WAK ) );

    // Enabled interrupt permission
    if ( ( false == allow_wakeupint )
      && ( false == mcp2515_modbits_reg( REG_CANINTE, MASK_CANINT_WAK, MASK_CANINT_WAK ) ) )
        return false;

    /* Request wakeup */
    if ( mcp2515_modbits_reg( REG_CANINTF, MASK_CANINT_WAK, MASK_CANINT_WAK ) )
        return false;

    /* Verify written */
    if ( ( MASK_CANINT_WAK != (uint8_t) ( mcp2515_read_reg(REG_CANINTE) & MASK_CANINT_WAK ) )
      || ( MASK_CANINT_WAK != (uint8_t) ( mcp2515_read_reg(REG_CANINTF) & MASK_CANINT_WAK ) ) )
        return false;

    /* Temporarily switch to listen-only mode */
    if ( false == mcp2515_request_newmode( MODE_LISTENONLY ) )
        return false;

    /* Restore interrupt permission */
    if ( ( false == allow_wakeupint )
      && ( false == mcp2515_modbits_reg( REG_CANINTE, MASK_CANINT_WAK, (uint8_t) ( ~MASK_CANINT_WAK ) ) ) )
        return false;

    /* Clear wakeup request */
    return mcp2515_modbits_reg( REG_CANINTF, MASK_CANINT_WAK, (uint8_t) ( ~MASK_CANINT_WAK ) );
}

static uint8_t mcp2515_get_mode()
{
    return (uint8_t) ( mcp2515_read_reg( REG_CANSTAT ) & MASK_MODE );
}

static bool mcp2515_change_mode( const uint8_t mode )
{
    const uint8_t request = (uint8_t) ( mode & MASK_MODE );
    const uint8_t current = mcp2515_get_mode();

    /* Do nothing if the mode does not change */
    if ( current == request )
        return true;

    /* Wake up if sleep mode now */
    if ( ( MODE_SLEEP == current ) && ( false == mcp2515_wakeup() ) )
        return false; 

    /* Enable wakeup if sleep and clear wakeup request */
    if ( MODE_SLEEP == request )
        if ( mcp2515_modbits_reg( REG_CANINTF, MASK_CANINT_WAK, (uint8_t) ( ~MASK_CANINT_WAK ) ) )
            return false;

    /* Write new mode */
    return mcp2515_request_newmode( request );
}

static bool mcp2515_set_canrate( const en_mcp2515_canrate rate )
{
    /* +------+------+------+----------+ */
    /* | CNF1 | CNF2 | CNF3 | CAN_RATE | */
    /* +------+------+------+----------+ */
    /* | 0xA7 | 0XBF | 0x07 |    5Kbps | */
    /* | 0x31 | 0XA4 | 0X04 |   10Kbps | */
    /* | 0x18 | 0XA4 | 0x04 |   20Kbps | */
    /* | 0x09 | 0XA4 | 0x04 |   50Kbps | */
    /* | 0x04 | 0x9E | 0x03 |  100Kbps | */
    /* | 0x03 | 0x9E | 0x03 |  125Kbps | */
    /* | 0x01 | 0x1E | 0x03 |  250Kbps | */
    /* | 0x00 | 0x9E | 0x03 |  500Kbps | */
    /* | 0x00 | 0x92 | 0x02 |  800Kbps | */
    /* | 0x00 | 0x82 | 0x02 | 1000Kbps | */
    /* +------+------+------+----------+ */
    const uint8_t CAN_RATE[CANRATE_SIZE][3] = {
        {0xA7, 0XBF, 0x07},
        {0x31, 0XA4, 0X04},
        {0x18, 0XA4, 0x04},
        {0x09, 0XA4, 0x04},
        {0x04, 0x9E, 0x03},
        {0x03, 0x9E, 0x03},
        {0x01, 0x1E, 0x03},
        {0x00, 0x9E, 0x03},
        {0x00, 0x92, 0x02},
        {0x00, 0x82, 0x02}
    };
    
    mcp2515_write_reg( REG_CNF1, CAN_RATE[rate][0] );
    mcp2515_write_reg( REG_CNF2, CAN_RATE[rate][1] );
    mcp2515_write_reg( REG_CNF3, CAN_RATE[rate][2] );
}

static void mcp2515_reset()
{ 
    spi_op_begin();
    spi_write( CMD_RESET );
    spi_op_end();
}

static void test() {
    printf("hello\n");
}

static void mcp2515_init_txbuf1() {

    mcp2515_clear_txbuf1();

    mcp2515_int_tx0_enable();

    gpio_set_irq_enabled_with_callback( PIN_INT, (uint32_t) GPIO_IRQ_EDGE_FALL, true, test );

    gpio_set_irq_enabled( PIN_INT, (uint32_t) GPIO_IRQ_EDGE_FALL, true );
}

void mcp2515_init() {

    spi_configure();

    mcp2515_reset();

    mcp2515_change_mode( MODE_CONFIG );

    mcp2515_set_canrate( CANRATE_125KBPS );

    mcp2515_clear_txbuf1();

    mcp2515_init_txbuf1();

    mcp2515_to_normalmode();
}

void mcp2515_clear_txbuf1()
{
    mcp2515_write_reg(REG_TXB0CTRL, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0SIDH, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0SIDL, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0EID8, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0EID0, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0DLC, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D0, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D1, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D2, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D3, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D4, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D5, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D6, (uint8_t) 0x00U );
    mcp2515_write_reg(REG_TXB0D7, (uint8_t) 0x00U );
}

bool mcp2515_to_normalmode()
{
    return mcp2515_change_mode( MODE_NORMAL );
}

bool mcp2515_to_sleepmode()
{
    return mcp2515_change_mode( MODE_SLEEP );
}

void mcp2515_int_tx0_enable() {
    mcp2515_modbits_reg( REG_CANINTE, MASK_CANINT_TX0, CANINT_TX0_ENABLE );
}

void mcp2515_int_tx0_disable() {
    mcp2515_modbits_reg( REG_CANINTE, MASK_CANINT_TX0, (uint8_t) ( ~CANINT_TX0_ENABLE ) );
}

void mcp2515_int_tx0_clear() {

    // mcp2515_modbits_reg( REG_CANINTF, MASK_CANINT_TX0, ~CANINT_TX0_ENABLE );
    mcp2515_modbits_reg( REG_CANINTF, MASK_CANINT_TX0, ~CANINT_TX0_ENABLE );
    mcp2515_write_reg(REG_TXB0CTRL, 0x00);
}

bool mcp2515_is_ready_tx0() {

    uint8_t ctrl = mcp2515_read_reg(REG_TXB0CTRL);
    return (bool) ((ctrl & 0x80) == 0);
}

static void mcp2515_req_send( uint8_t tx_n ) {

    uint8_t a;

    switch( tx_n ) {
        case 1:
            a = 1;
            break;
        case 2:
            a = 2;
            break;
        case 3:
            a = 4;
            break;
        default:
            return;
    }

    spi_op_begin();

    spi_write( 0x80 + a );

    spi_op_end();
}

void mcp2515_req_tx0_send() {

    mcp2515_req_send(1);
}

void mcp2515_set_tx0( const uint16_t id, const uint8_t data[8] ) {

    uint8_t idh = (uint8_t) ( ( ( uint16_t ) ( id >> 8 ) ) & ( ( uint16_t ) 0x00FFU ) );
    uint8_t idl = (uint8_t) ( id & ( uint16_t ) 0x00FFU );

    mcp2515_write_reg( REG_TXB0SIDH, idh );
    mcp2515_write_reg( REG_TXB0SIDL, idl );

    mcp2515_write_reg( REG_TXB0DLC, DLC8 );

    mcp2515_write_reg( REG_TXB0D0, data[0] );
    mcp2515_write_reg( REG_TXB0D1, data[1] );
    mcp2515_write_reg( REG_TXB0D2, data[2] );
    mcp2515_write_reg( REG_TXB0D3, data[3] );
    mcp2515_write_reg( REG_TXB0D4, data[4] );
    mcp2515_write_reg( REG_TXB0D5, data[5] );
    mcp2515_write_reg( REG_TXB0D6, data[6] );
    mcp2515_write_reg( REG_TXB0D7, data[7] );
}