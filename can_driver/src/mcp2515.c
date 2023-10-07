#ifndef TYPES_H
#include "types.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
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
const can_baudrate_reg_t can_baudrate_config_table[ CAN_BAUDRATE_NUMOF_ITEMS ] = {
    /* CNF1 , CNF2 , CNF3 */
    {  0xA7U, 0XBFU, 0x07U }, /* CAN_BAUDRATE_5KBPS      */
    {  0x31U, 0XA4U, 0X04U }, /* CAN_BAUDRATE_10KBPS     */
    {  0x18U, 0XA4U, 0x04U }, /* CAN_BAUDRATE_20KBPS     */
    {  0x09U, 0XA4U, 0x04U }, /* CAN_BAUDRATE_50KBPS     */
    {  0x04U, 0x9EU, 0x03U }, /* CAN_BAUDRATE_100KBPS    */
    {  0x03U, 0x9EU, 0x03U }, /* CAN_BAUDRATE_125KBPS    */
    {  0x01U, 0x1EU, 0x03U }, /* CAN_BAUDRATE_250KBPS    */
    {  0x00U, 0x9EU, 0x03U }, /* CAN_BAUDRATE_500KBPS    */
    {  0x00U, 0x92U, 0x02U }, /* CAN_BAUDRATE_800KBPS    */
    {  0x00U, 0x82U, 0x02U }  /* CAN_BAUDRATE_1000KBPS   */
};

const uint8_t picocan_operation_mode_table[ PICOCAN_OPERATION_MODE_NUMOF_ITEMS ] = {
    OPERATION_MODE_NORMAL,      /* Normal mode.      */
    OPERATION_MODE_SLEEP,       /* Sleep mode.       */
    OPERATION_MODE_LOOPBACK,    /* Loop back mode.   */
    OPERATION_MODE_LISTENONLY,  /* Listen only mode. */
    OPERATION_MODE_CONFIG       /* Config mode.      */
};

/*! \brief Configure SPI for PICO CAN module
 * \note Baudrate is 1MHz. GP4: SPI MISO, GP5: SPI SS, GP6: SPI SCK, GP7: SPI MOSI.
 */
static void configure_spi_for_picocan() {

    spi_init( spi0, SPI_BAUDRATE );

    gpio_set_function( GPIO_MCP2515_MISO, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_SCK, GPIO_FUNC_SPI );

    gpio_init( GPIO_MCP2515_CS );
    gpio_set_dir( GPIO_MCP2515_CS, GPIO_OUT );
    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );
}

/*! \brief Begin sending SPI commands to PICO CAN module
 */
static void begin_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_LOW );
}

/*! \brief End sending SPI commands to PICO CAN module
 */
static void end_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );
}

/*! \brief Read a byte from SPI(PICO CAN module)
 */
static uint8_t read_byte_from_spi() {

    uint8_t val;

    spi_read_blocking( spi0, 0U, &val, 1U );

    return val;
}

/*! \brief Write a byte to SPI(PICO CAN module)
 * \param val write value
 */
static void write_byte_to_spi( const uint8_t val ) {

    spi_write_blocking( spi0, &val, 1U );
}

/*! \brief Read a byte from register in PICO CAN module
 * \param reg_addr register address
 * \return read value
 */
static uint8_t read_byte_from_register( const uint8_t reg_addr ) {

    begin_spi_commands();

    write_byte_to_spi( SPICMD_READ_REG );
    write_byte_to_spi( reg_addr );

    const uint8_t val = read_byte_from_spi();

    end_spi_commands();

    return val;
}

/*! \brief Verify a byte of refister
 * \param reg_addr register address
 * \param maskof_reg_val mask of a register value
 * \param exp_val expected value
 * \return true if successful, otherwise false
 */
static bool verify_byte_on_register( const uint8_t reg_addr, const uint8_t maskof_reg_val, const uint8_t exp_val ) {

    const uint64_t begun = time_us_64();

    while ( exp_val != (uint8_t) ( read_byte_from_register( reg_addr ) & maskof_reg_val ) ) {

        const uint64_t current = time_us_64();
        const uint64_t elapsed = ( current < begun ) ? ( UINT64_VALUE_MAX - begun + current + 1ULL ) : ( current - begun );

        if ( elapsed > PICOCAN_TIMEOUT_WRITE_REG ) {

            return false;
        }
    }

    return true;
}

/*! \brief Write a byte to register
 * \param reg_addr register address
 * \param write_val write value
 * \return true if successful, otherwise false
 */
static bool write_byte_to_register( const uint8_t reg_addr, const uint8_t val ) {

    begin_spi_commands();

    write_byte_to_spi( SPICMD_WRITE_REG );
    write_byte_to_spi( reg_addr );
    write_byte_to_spi( val );

    end_spi_commands();

    return verify_byte_on_register( reg_addr, 0xFFU, val );
}

/*! \brief Modify a register bits
 * \param reg_addr register address
 * \param maskof_write mask of modify
 * \param val write value
 * \return true if successful, otherwise false
 */
static bool modify_bits_on_register( const uint8_t reg_addr, const uint8_t maskof_write, const uint8_t val ) {

    const uint8_t exp_val = (uint8_t) ( val & maskof_write );

    begin_spi_commands();

    write_byte_to_spi( SPICMD_MODBITS_REG );
    write_byte_to_spi( reg_addr );
    write_byte_to_spi( maskof_write );
    write_byte_to_spi( val );

    end_spi_commands();

    return verify_byte_on_register( reg_addr, maskof_write, exp_val );
}

/*! \brief Get operation mode of the PICO CAN module
 * \return operation mode of the PICO CAN
 */
static uint8_t get_picocan_operation_mode() {

    return (uint8_t) ( read_byte_from_register( REG_CANSTAT ) & MASKOF_REG_OPERATION_MODE );
}

static bool wakeup_picocan() {

    const bool allow_wakeup_interrupt = (bool) ( 0U < ( read_byte_from_register( REG_CANINTE ) & MASKOF_REG_WAKINT ) );

    /* Enabled interrupt by wake up. */
    if ( false == allow_wakeup_interrupt ) {

        if( false == modify_bits_on_register( REG_CANINTE, MASKOF_REG_WAKINT, MASKOF_REG_WAKINT ) ) {

            return false;
        }
    }

    /* Request wakeup. */
    if ( false == modify_bits_on_register( REG_CANINTF, MASKOF_REG_WAKINT, MASKOF_REG_WAKINT ) ) {

        return false;
    }

    /* Temporarily switch to listen-only mode. */
    if ( false == modify_bits_on_register( REG_CANCTRL, MASKOF_REG_OPERATION_MODE, OPERATION_MODE_LISTENONLY ) ) {

        return false;
    }

    /* Restore interrupt permission */
    if ( false == allow_wakeup_interrupt ) {

        if ( false == modify_bits_on_register( REG_CANINTE, MASKOF_REG_WAKINT, (uint8_t) ( ~MASKOF_REG_WAKINT ) ) ) {

            return false;
        }
    }

    /* Clear wakeup request */
    return modify_bits_on_register( REG_CANINTF, MASKOF_REG_WAKINT, (uint8_t) ( ~MASKOF_REG_WAKINT ) );
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

    if (   ( true == is_valid_can_id( msg ) )
        && ( CAN_FORMAT_KIND_INDEX_MIN <= ( msg->kind ) )
        && ( CAN_FORMAT_KIND_INDEX_MAX >= ( msg->kind ) )
        && ( CAN_CONTENT_LEN_MAX >= ( msg->length ) )
        && ( ( CAN_CONTENT_LEN_MIN == ( msg->length ) ) || ( NULL != ( msg->content ) ) )
    ) {

        return true;
    }
    else {

        return false;
    }

}

static bool can_message_to_can_id_reg( const can_message_t *const msg, can_id_reg_t *const id_reg ) {

    uint32_t id;

    /* Begin verify arguments. */
    if ( false == is_valid_can_message( msg ) || NULL == id_reg ) {

        return false;
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

    return true;
}

static bool set_tx_buffer( const can_message_t *const msg, const uint8_t cmd_w_tx_id, const uint8_t cmd_w_tx_content ) {

    can_id_reg_t id_reg;
    uint8_t id_buf[5];

    /* Begin verify arguments. */
    if ( false == can_message_to_can_id_reg( msg, &id_reg ) ) {

        return false;
    }
    /* End verify arguments. */

    id_buf[0] = id_reg.sidh;
    id_buf[1] = id_reg.sidl;
    id_buf[2] = id_reg.eid8;
    id_buf[3] = id_reg.eid0;
    id_buf[4] = msg->length;

    begin_spi_commands();

    write_byte_to_spi( SPICMD_WRITE_TX0ID );
    spi_write_blocking( spi0, id_buf, sizeof( id_buf ) );

    end_spi_commands();
    
    if ( 0U < msg->length ) {

        begin_spi_commands();

        write_byte_to_spi( SPICMD_WRITE_TX0DATA );
        spi_write_blocking( spi0, msg->content, msg->length );

        end_spi_commands();
    }

    return true;
}

static void request_send_tx( const uint8_t cmd_req_tx ) {

    begin_spi_commands();

    write_byte_to_spi( cmd_req_tx );

    end_spi_commands();
}

static bool set_priority_tx( uint8_t reg_addr, enum PICOCAN_TX_PRIORITY priority ) {

    bool is_success;

    /* Begin verify arguments. */
    if ( PICOCAN_TX_PRIORITY_INDEX_MIN > priority || PICOCAN_TX_PRIORITY_INDEX_MAX < priority ) {

        return false;
    }

    switch ( reg_addr ) {

    case REG_TXB0CTRL:
    case REG_TXB1CTRL:
    case REG_TXB2CTRL:
        /* No processing */
        break;
    
    default:
        return false;
    }
    /* End verify arguments. */

    switch ( priority ) {
    
    case PICOCAN_TX_PRIORITY_LOW:

        is_success = modify_bits_on_register( reg_addr, MASKOF_REG_TX_PRIORITY, TX_PRIORITY_LOW );
        break;
    
    case PICOCAN_TX_PRIORITY_MIDDLE_LOW:

        is_success = modify_bits_on_register( reg_addr, MASKOF_REG_TX_PRIORITY, TX_PRIORITY_MIDDLE_LOW );
        break;
    
    case PICOCAN_TX_PRIORITY_MIDDLE_HIGH:

        is_success = modify_bits_on_register( reg_addr, MASKOF_REG_TX_PRIORITY, TX_PRIORITY_MIDDLE_HIGH );
        break;
    
    case PICOCAN_TX_PRIORITY_HIGH:

        is_success = modify_bits_on_register( reg_addr, MASKOF_REG_TX_PRIORITY, TX_PRIORITY_HIGH );
        break;

    default:
        /* No processing */
        return false;
    }

    return is_success;
}

bool picocan_change_can_baudrate( const enum CAN_BAUDRATE baudrate ) {

    if ( CAN_BAUDRATE_INDEX_MIN > baudrate || CAN_BAUDRATE_INDEX_MAX < baudrate ) {

        return false;
    }

    can_baudrate_reg_t conf = can_baudrate_config_table[ baudrate ];

    if ( false == write_byte_to_register( REG_CNF1, conf.c1 ) ) {

        return false;
    }

    if ( false == write_byte_to_register( REG_CNF2, conf.c2 ) ) {

        return false;
    }

    if ( false == write_byte_to_register( REG_CNF3, conf.c3 ) ) {

        return false;
    }

    return true;
}

bool picocan_reset() {

    configure_spi_for_picocan();

    begin_spi_commands();

    write_byte_to_spi( SPICMD_RESET );

    end_spi_commands();

    return verify_byte_on_register( REG_CANCTRL, MASKOF_REG_OPERATION_MODE, OPERATION_MODE_CONFIG );
}

bool picocan_change_operation_mode( const enum PICOCAN_OPERATION_MODE mode ) {

    uint8_t request;
    uint8_t current;

    if ( PICOCAN_OPERATION_MODE_INDEX_MIN > mode || PICOCAN_OPERATION_MODE_INDEX_MAX < mode ) {

        return false;
    }

    request = picocan_operation_mode_table[ mode ];
    current = get_picocan_operation_mode();

    /* Do nothing if the mode does not change. */
    if ( current == request ) {

        return true;
    }

    /* Wake up if current mode is sleep. */
    if ( OPERATION_MODE_SLEEP == current ) {

        if( false == wakeup_picocan() ) {

            return false;
        }
    }

    /* Clear waked up request if to be sleep. */
    if ( OPERATION_MODE_SLEEP == request ) {

        if ( false == modify_bits_on_register( REG_CANINTF, MASKOF_REG_WAKINT, (uint8_t) ( ~MASKOF_REG_WAKINT ) ) ) {

            return false;
        }
    }

    /* Write new mode */
    return modify_bits_on_register( REG_CANCTRL, MASKOF_REG_OPERATION_MODE, request );
}

bool picocan_set_tx0_buffer( const can_message_t *const msg ) {

    return set_tx_buffer( msg, SPICMD_WRITE_TX0ID, SPICMD_WRITE_TX0DATA );
}

bool picocan_set_tx1_buffer( const can_message_t *const msg ) {

    return set_tx_buffer( msg, SPICMD_WRITE_TX1ID, SPICMD_WRITE_TX1DATA );
}

bool picocan_set_tx2_buffer( const can_message_t *const msg ) {

    return set_tx_buffer( msg, SPICMD_WRITE_TX2ID, SPICMD_WRITE_TX2DATA );
}

bool picocan_request_send_tx0() {

    request_send_tx( SPICMD_REQ_TX0 );
}

bool picocan_request_send_tx1() {

    request_send_tx( SPICMD_REQ_TX1 );
}

bool picocan_request_send_tx2() {

    request_send_tx( SPICMD_REQ_TX2 );
}

bool picocan_set_tx0_priority( enum PICOCAN_TX_PRIORITY priority ) {

    return set_priority_tx( REG_TXB0CTRL, priority );
}

bool picocan_set_tx1_priority( enum PICOCAN_TX_PRIORITY priority ) {
    
    return set_priority_tx( REG_TXB1CTRL, priority );
}

bool picocan_set_tx2_priority( enum PICOCAN_TX_PRIORITY priority ) {
    
    return set_priority_tx( REG_TXB2CTRL, priority );
}