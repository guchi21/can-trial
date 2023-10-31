#include "wrapper.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

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
#define GPIO_MCP2515_INT            ( 21U )             /* RP2040 pin #27.  */


/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

static void cbk_gpio_irq( uint gpio, uint32_t event_mask ) {

    if ( ( GPIO_MCP2515_INT == gpio ) && ( (uint32_t)GPIO_IRQ_LEVEL_LOW == event_mask ) ) {

        picocan_wrap_ind_irq();
    }
}

void picocan_init() {

    spi_init( spi0, SPI_BAUDRATE );

    gpio_set_function( GPIO_MCP2515_MISO, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( GPIO_MCP2515_SCK,  GPIO_FUNC_SPI );

    gpio_init( GPIO_MCP2515_CS );
    gpio_set_dir( GPIO_MCP2515_CS, GPIO_OUT );
    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );

    gpio_set_irq_enabled_with_callback( GPIO_MCP2515_INT, (uint32_t)GPIO_IRQ_LEVEL_LOW, true, cbk_gpio_irq );
}

void picocan_begin_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_LOW );
}

void picocan_end_spi_commands() {

    gpio_put( GPIO_MCP2515_CS, PIN_VOLTAGE_HIGH );
}

void picocan_read_array_spi( const uint8_t n, uint8_t buf[n] ) {

    if ( 0U < n && NULL != buf ) {

        spi_read_blocking( spi0, 0U, buf, n );
    }
}

uint8_t picocan_read_spi() {

    uint8_t val = 0U;

    picocan_read_array_spi( 1U, &val );

    return val;
}

void picocan_write_array_spi( const uint8_t n, const uint8_t buf[n] ) {

    if ( 0U < n && NULL != buf ) {

        spi_write_blocking( spi0, buf, n );
    }
}

void picocan_write_spi( const uint8_t val ) {

    picocan_write_array_spi( 1U, &val );
}
