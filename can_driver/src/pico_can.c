#ifndef PICO_CAN_H
#include "pico_can.h"
#endif

#ifndef _HARDWARE_SPI_H
#include "hardware/spi.h"
#endif


/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/
#define UINT64MAX                   ( (uint64_t) 18446744073709551615ULL )

/* Pin voltage definitions. */
#define PIN_LOW                     ( (bool) 0U )           /* RP2040 pin low voltage.  */
#define PIN_HIGH                    ( (bool) 1U )           /* RP2040 pin high voltage. */

/* SPI clock. */
#define SPI_CLOCK                   ( (uint) 1000000U )     /* SPI baudrate definition in Hz. */

/* GPIO number definitions. */
#define GPIONUM_SPI_RX              ( (uint) 4U )           /* RP2040 pin #06.  */
#define GPIONUM_SPI_CS              ( (uint) 5U )           /* RP2040 pin #07.  */
#define GPIONUM_SPI_SCK             ( (uint) 6U )           /* RP2040 pin #09.  */
#define GPIONUM_SPI_TX              ( (uint) 7U )           /* RP2040 pin #10.  */
#define GPIONUM_CAN_INT             ( (uint) 21U )          /* RP2040 pin #27.  */

/* SPI commands. */
#define SPICMD_WRITE_REG            ( (uint8_t) 0x02U )     /* Write to register at MCP2515.                 */                      
#define SPICMD_READ_REG             ( (uint8_t) 0x03U )     /* Read from register at MCP2515.                */                       
#define SPICMD_MODBITS_REG          ( (uint8_t) 0x05U )     /* Modify register bits at MCP2515.              */                         
#define SPICMD_WRITE_TX0ID          ( (uint8_t) 0x40U )     /* Write CAN ID to TX0 on MCP2515.               */                        
#define SPICMD_WRITE_TX0DATA        ( (uint8_t) 0x41U )     /* Write data to TX0 on MCP2515.                 */                      
#define SPICMD_WRITE_TX1ID          ( (uint8_t) 0x42U )     /* Write CAN ID to TX1 on MCP2515.               */                        
#define SPICMD_WRITE_TX1DATA        ( (uint8_t) 0x43U )     /* Write data to TX1 on MCP2515.                 */                      
#define SPICMD_WRITE_TX2ID          ( (uint8_t) 0x44U )     /* Write CAN ID to TX2 on MCP2515.               */                        
#define SPICMD_WRITE_TX2DATA        ( (uint8_t) 0x45U )     /* Write data to TX2 on MCP2515.                 */                      
#define SPICMD_REQ_TX0              ( (uint8_t) 0x81U )     /* Request send CAN message from TX0 on MCP2515. */                                      
#define SPICMD_REQ_TX1              ( (uint8_t) 0x82U )     /* Request send CAN message from TX1 on MCP2515. */                                      
#define SPICMD_REQ_TX2              ( (uint8_t) 0x83U )     /* Request send CAN message from TX2 on MCP2515. */                                      
#define SPICMD_READ_RX0ID           ( (uint8_t) 0x90U )     /* Read CAN ID from RX0 on MCP2515.              */                         
#define SPICMD_READ_RX0DATA         ( (uint8_t) 0x92U )     /* Read data from RX0 on MCP2515.                */                       
#define SPICMD_READ_RX1ID           ( (uint8_t) 0x94U )     /* Read CAN ID from RX1 on MCP2515.              */                         
#define SPICMD_READ_RX1DATA         ( (uint8_t) 0x96U )     /* Read data from RX1 on MCP2515.                */                       
#define SPICMD_READ_STAT            ( (uint8_t) 0xA0U )     /* Read status at MCP2515.                       */                
#define SPICMD_READ_RXSTAT          ( (uint8_t) 0xB0U )     /* Read RX status at MCP2515.                    */                   
#define SPICMD_RESET                ( (uint8_t) 0xC0U )     /* Reset the MCP2515.                            */           

/* MCP2515 register write timeout. */
#define MCP2515_TIMEOUT_WRITE_REG   ( (uint64_t) 1000ULL )  /*  Timeout of write to register in microseconds on MCP2515. */

/* MCP2515 registers address. */
#define MCP2515_REG_CNF3            ( (uint8_t) 0x28U )     /* CAN bus interface bit timing #3. */
#define MCP2515_REG_CNF2            ( (uint8_t) 0x29U )     /* CAN bus interface bit timing #2. */
#define MCP2515_REG_CNF1            ( (uint8_t) 0x2AU )     /* CAN bus interface bit timing #1. */

#define MCP2515_REG_CANSTAT         ( (uint8_t) 0x0EU )     /* CAN status.                      */
#define MCP2515_REG_CANCTRL         ( (uint8_t) 0x0FU )     /* CAN control.                     */
#define MCP2515_REG_CANINTE         ( (uint8_t) 0x2BU )     /* CAN interrupt enables.           */
#define MCP2515_REG_CANINTF         ( (uint8_t) 0x2CU )     /* CAN interrupt flags.             */


/* MCP2515 operation modes. */
#define MCP2515_OPMODE_NORMAL         ( (uint8_t) 0x00U )     /* Normal mode.                     */
#define MCP2515_OPMODE_SLEEP          ( (uint8_t) 0x20U )     /* Sleep mode.                      */
#define MCP2515_OPMODE_LOOPBACK       ( (uint8_t) 0x40U )     /* Loop back mode.                  */
#define MCP2515_OPMODE_LISTENONLY     ( (uint8_t) 0x60U )     /* Listen only mode.                */
#define MCP2515_OPMODE_CONFIG         ( (uint8_t) 0x80U )     /* Config mode.                     */

/* Mask of MCP2515 operation modes. */
#define MASKOF_OPMODE               ( (uint8_t) 0xE0U )

/* Mask of CAN interruptions. */
#define MASKOF_RX0INT               ( (uint8_t) 0x01U )     /* Mask of RX buffer #0 interrupt.  */
#define MASKOF_RX1INT               ( (uint8_t) 0x02U )     /* Mask of RX buffer #1 interrupt.  */
#define MASKOF_TX0INT               ( (uint8_t) 0x04U )     /* Mask of TX buffer #0 interrupt.  */
#define MASKOF_TX1INT               ( (uint8_t) 0x08U )     /* Mask of TX buffer #1 interrupt.  */
#define MASKOF_TX2INT               ( (uint8_t) 0x10U )     /* Mask of TX buffer #2 interrupt.  */
#define MASKOF_ERRINT               ( (uint8_t) 0x20U )     /* Mask of error interrupt.         */
#define MASKOF_WAKINT               ( (uint8_t) 0x40U )     /* Mask of wake up interrupt.       */
#define MASKOF_MSGERRINT            ( (uint8_t) 0x80U )     /* Mask of message error interrupt. */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/
typedef struct {
    uint8_t c1;
    uint8_t c2;
    uint8_t c3;
} canrate_config;

/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/
const canrate_config canrate_config_table[] = {
    /* CNF1 , CNF2 , CNF3 */
    {  0xA7U, 0XBFU, 0x07U }, /* CANRATE_5KBPS      */
    {  0x31U, 0XA4U, 0X04U }, /* CANRATE_10KBPS     */
    {  0x18U, 0XA4U, 0x04U }, /* CANRATE_20KBPS     */
    {  0x09U, 0XA4U, 0x04U }, /* CANRATE_50KBPS     */
    {  0x04U, 0x9EU, 0x03U }, /* CANRATE_100KBPS    */
    {  0x03U, 0x9EU, 0x03U }, /* CANRATE_125KBPS    */
    {  0x01U, 0x1EU, 0x03U }, /* CANRATE_250KBPS    */
    {  0x00U, 0x9EU, 0x03U }, /* CANRATE_500KBPS    */
    {  0x00U, 0x92U, 0x02U }, /* CANRATE_800KBPS    */
    {  0x00U, 0x82U, 0x02U }  /* CANRATE_1000KBPS   */
};

const uint8_t mcp2515_opmode[] = {
    MCP2515_OPMODE_NORMAL,      /* Normal mode.      */
    MCP2515_OPMODE_SLEEP,       /* Sleep mode.       */
    MCP2515_OPMODE_LOOPBACK,    /* Loop back mode.   */
    MCP2515_OPMODE_LISTENONLY,  /* Listen only mode. */
    MCP2515_OPMODE_CONFIG       /* Config mode.      */
};


static void init_spi() {

    spi_init( spi0, SPI_CLOCK );

    gpio_set_function( GPIONUM_SPI_RX, GPIO_FUNC_SPI );
    gpio_set_function( GPIONUM_SPI_TX, GPIO_FUNC_SPI );
    gpio_set_function( GPIONUM_SPI_SCK, GPIO_FUNC_SPI );

    gpio_init( GPIONUM_SPI_CS );
    gpio_set_dir( GPIONUM_SPI_CS, GPIO_OUT );
    gpio_put( GPIONUM_SPI_CS, PIN_HIGH );
}

static void begin_spicmd() {

    gpio_put( GPIONUM_SPI_CS, PIN_LOW );
}

static void end_spicmd() {

    gpio_put( GPIONUM_SPI_CS, PIN_HIGH );
}

static uint8_t read_spi() {

    uint8_t val;

    spi_read_blocking( spi0, (uint8_t) 0U, &val, (size_t) 1U );

    return val;
}

static void write_spi( const uint8_t val ) {

    spi_write_blocking( spi0, &val, (size_t) 1U );
}

static uint8_t read_reg( const uint8_t addr ) {

    begin_spicmd();

    spi_write(SPICMD_READ_REG);
    spi_write(addr);

    const uint8_t val = spi_read();

    end_spicmd();

    return val;
}

static bool verify_written( const uint8_t addr, const uint8_t exp_mask, const uint8_t exp_val ) {

    const uint64_t begun = time_us_64();

    while ( exp_val != (uint8_t) ( read_reg( addr ) & exp_mask ) ) {

        const uint64_t current = time_us_64();
        const uint64_t elapsed = ( current < begun ) ? ( UINT64MAX - begun + current + 1U ) : ( current - begun );

        if ( elapsed > MCP2515_TIMEOUT_WRITE_REG ) {

            return false;
        }
    }

    return true;
}

static bool write_reg( const uint8_t addr, const uint8_t val ) {

    begin_spicmd();

    spi_write( SPICMD_WRITE_REG );
    spi_write( addr );
    spi_write( val );

    end_spicmd();

    return verify_written( addr, 0xFFU, val );
}

static bool modbits_reg( const uint8_t addr, const uint8_t mask, const uint8_t val ) {

    const uint8_t exp_val = (uint8_t) ( val & mask );

    begin_spicmd();

    spi_write( SPICMD_MODBITS_REG );
    spi_write( addr );
    spi_write( mask );
    spi_write( val );

    end_spicmd();

    return verify_written( addr, mask, exp_val );
}

static uint8_t get_mode() {

    return (uint8_t) ( read_reg( MCP2515_REG_CANSTAT ) & MASKOF_OPMODE );
}

static bool wakeup() {

    const bool allow_wakeupint = (bool) ( ( (uint8_t) 0U ) < ( (uint8_t) ( mcp2515_read_reg( MCP2515_REG_CANINTE ) & MASKOF_WAKINT ) ) );

    /* Enabled interrupt by wake up. */
    if ( false == allow_wakeupint ) {

        if( false == mcp2515_modbits_reg( MCP2515_REG_CANINTE, MASKOF_WAKINT, MASKOF_WAKINT ) ) {

            return false;
        }
    }

    /* Request wakeup. */
    if ( false == mcp2515_modbits_reg( MCP2515_REG_CANINTF, MASKOF_WAKINT, MASKOF_WAKINT ) ) {

        return false;
    }

    /* Temporarily switch to listen-only mode. */
    if ( false == modbits_reg( MCP2515_REG_CANCTRL, MASKOF_OPMODE, MCP2515_OPMODE_LISTENONLY ) ) {

        return false;
    }

    /* Restore interrupt permission */
    if ( false == allow_wakeupint ) {

        if ( false == mcp2515_modbits_reg( MCP2515_REG_CANINTE, MASKOF_WAKINT, (uint8_t) ( ~MASKOF_WAKINT ) ) ) {

            return false;
        }
    }

    /* Clear wakeup request */
    return mcp2515_modbits_reg( MCP2515_REG_CANINTF, MASKOF_WAKINT, (uint8_t) ( ~MASKOF_WAKINT ) );
}

// todo:
// static bool clear tx0
// static bool clear tx1
// static bool clear tx2
// static bool clear rx0
// static bool clear rx1

bool picocan_set_canrate( const picocan_canrate rate ) {

    if ( CANRATE_MIN > rate || CANRATE_MAX < rate ) {

        return false;
    }

    canrate_config conf = canrate_config_table[ rate ];

    if ( false == write_reg( MCP2515_REG_CNF1, conf.c1 ) ) {

        return false;
    }

    if ( false == write_reg( MCP2515_REG_CNF2, conf.c2 ) ) {

        return false;
    }

    if ( false == write_reg( MCP2515_REG_CNF3, conf.c3 ) ) {

        return false;
    }

    return true;
}

bool picocan_reset() {

    init_spi();

    begin_spicmd();

    spi_write( SPICMD_RESET );

    end_spicmd();

    return verify_written( MCP2515_REG_CANCTRL, MASKOF_OPMODE, MCP2515_OPMODE_CONFIG );
}

bool picocan_set_mode( const picocan_opmode mode ) {

    if ( OPMODE_MIN > mode || OPMODE_MAX < mode ) {

        return false;
    }

    const uint8_t request = mcp2515_opmode[ mode ];
    const uint8_t current = get_mode();

    /* Do nothing if the mode does not change. */
    if ( current == request ) {

        return true;
    }

    /* Wake up if current mode is sleep. */
    if ( MCP2515_OPMODE_SLEEP == current ) {

        if( false == wakeup() ) {

            return false;
        }
    }

    /* Clear waked up request if to be sleep. */
    if ( MCP2515_OPMODE_SLEEP == request ) {

        if ( false == modbits_reg( MCP2515_REG_CANINTF, MASKOF_WAKINT, (uint8_t) ( ~MASKOF_WAKINT ) ) ) {

            return false;
        }
    }

    /* Write new mode */
    return modbits_reg( MCP2515_REG_CANCTRL, MASKOF_OPMODE, request );
}