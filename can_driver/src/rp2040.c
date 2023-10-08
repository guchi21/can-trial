#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef _HARDWARE_SPI_H
#include "hardware/spi.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* RP2040 pin voltage. */
#define PIN_VOLTAGE_LOW             ( 0U )
#define PIN_VOLTAGE_HIGH            ( 1U )


/* RP2040 SPI clock. */
#define SPI_BAUDRATE                ( 10000000U )        /* SPI baudrate in Hz. */


/* RP2040 GPIO numbers. */
#define GPIO_MCP2515_MISO           (  4U )             /* RP2040 pin #06.  */
#define GPIO_MCP2515_CS             (  5U )             /* RP2040 pin #07.  */
#define GPIO_MCP2515_SCK            (  6U )             /* RP2040 pin #09.  */
#define GPIO_MCP2515_MOSI           (  7U )             /* RP2040 pin #10.  */
#define GPIO_MCP2515_INT1           ( 21U )             /* RP2040 pin #27.  */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

void rp2040_init_spi() {

    spi_init( spi0, SPI_BAUDRATE );

    gpio_set_function( GPIO_MCP2515_MISO, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_SCK,  GPIO_FUNC_SPI );

    gpio_init( GPIO_MCP2515_CS );
    gpio_set_dir( GPIO_MCP2515_CS, GPIO_OUT );
    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );
}

void rp2040_begin_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_LOW );
}

void rp2040_end_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );
}

uint8_t rp2040_read_spi() {

    uint8_t val;

    spi_read_blocking( spi0, 0U, &val, 1U );

    return val;
}

void rp2040_write_spi( const uint8_t val ) {

    spi_write_blocking( spi0, &val, 1U );
}

void rp2040_write_array_spi( const uint8_t *const array, const uint8_t length ) {

    if ( NULL != array && length > 0 ) {

        spi_write_blocking( spi0, array, length );
    }
}
