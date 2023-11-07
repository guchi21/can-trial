/*
 * (c) 2023 guchi21.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "hardware_manager.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

/*!
 * \file hardware_manager.c
 * \brief Hardware manager.
 *
 * \author guchi21
 * \date 2023-11-03
 */

/*! 
 * \defgroup GpioLogic GPIO logic
 */
/*! \{ */
#define HW_GPIO_VOLTAGE_LOW             ( 0U ) /*!< Negative logic */
#define HW_GPIO_VOLTAGE_HIGH            ( 1U ) /*!< Positive logic */
/*! \} */

/*!
 * \brief SPI clock.
 */
#define HW_SPI_BAUDRATE                 ( 10000000U ) /*!< 10MHz */

/*! 
 * \defgroup GpioNumber GPIO number
 */
/*! \{ */
#define HW_GPIO_MCP2515_MISO            (  4U )     /*!< Connected to MISO on mcp2515   */
#define HW_GPIO_MCP2515_CS              (  5U )     /*!< Connected to CS on mcp2515     */
#define HW_GPIO_MCP2515_SCK             (  6U )     /*!< Connected to SCK on mcp2515    */
#define HW_GPIO_MCP2515_MOSI            (  7U )     /*!< Connected to MOSI on mcp2515   */
#define HW_GPIO_MCP2515_INT             ( 21U )     /*!< Connected to INT on mcp2515    */
#define HW_GPIO_HALL_SENSOR             ( 22U )     /*!< Connected to hall sensor       */
/*! \} */

/*!
 * \brief GPIO interrupt configuration.
 *
 * It consists of a GPIO number, an interrupt cause, and a callback function when an interrupt occurs.
 * For interrupt cause, see \ref gpio_irq_level.
 */
typedef struct {
    uint gpio;                      /*!< Target GPIO number         */
    uint32_t events;                /*!< Target interrupt causes    */
    hw_irq_callback_t callback;     /*!< callback function          */
} hw_irq_config_t;

static hw_irq_config_t irq_configs[ HW_IRQ_CAUSE_NUMOF_ITEMS ] = {
    { HW_GPIO_MCP2515_INT, GPIO_IRQ_LEVEL_LOW, NULL },
    { HW_GPIO_HALL_SENSOR, GPIO_IRQ_EDGE_RISE, NULL }
};

static void cbk_gpio_irq( uint gpio, uint32_t event_mask ) {

    hw_irq_config_t *cfg;

    for( enum HW_IRQ_CAUSE i = HW_IRQ_CAUSE_MINOF_IDX; HW_IRQ_CAUSE_NUMOF_ITEMS > i; i++ ) {

        cfg = &irq_configs[ i ];

        if ( ( NULL != cfg->callback ) && ( cfg->gpio == gpio )
                && ( 0U != (uint32_t)( cfg->events & event_mask ) ) ) {

            cfg->callback( event_mask );

            break;
        }
    }
}

hw_result_t hw_set_irq_callback( enum HW_IRQ_CAUSE cause, hw_irq_callback_t callback ) {

    hw_irq_config_t *cfg;
    bool irq_enabled;

    /* Validate arguments. */
    if ( ( HW_IRQ_CAUSE_MINOF_IDX > cause ) || ( HW_IRQ_CAUSE_MAXOF_IDX < cause ) )
        return HW_FAILURE;
    
    irq_enabled = ( NULL != callback ) ? true : false;
    
    cfg = &irq_configs[ cause ];

    cfg->callback = callback;

    gpio_set_irq_enabled( cfg->gpio, cfg->events, irq_enabled );

    return HW_SUCCESS;
}

void hw_init() {

    (void)spi_init( spi0, HW_SPI_BAUDRATE );

    gpio_set_function( HW_GPIO_MCP2515_MISO, GPIO_FUNC_SPI );
    gpio_set_function( HW_GPIO_MCP2515_MOSI, GPIO_FUNC_SPI );
    gpio_set_function( HW_GPIO_MCP2515_SCK,  GPIO_FUNC_SPI );

    gpio_init( HW_GPIO_MCP2515_CS );
    gpio_set_dir( HW_GPIO_MCP2515_CS, GPIO_OUT );
    gpio_put( HW_GPIO_MCP2515_CS, HW_GPIO_VOLTAGE_HIGH );

    gpio_set_irq_callback( cbk_gpio_irq );
}

void hw_begin_spi_commands() {

    gpio_put( HW_GPIO_MCP2515_CS, HW_GPIO_VOLTAGE_LOW );
}

void hw_end_spi_commands() {

    gpio_put( HW_GPIO_MCP2515_CS, HW_GPIO_VOLTAGE_HIGH );
}

void hw_read_array_spi( const size_t n, uint8_t buf[ n ] ) {

    if ( ( 0U < n ) && ( NULL != buf ) ) {

        spi_read_blocking( spi0, 0U, buf, n );
    }
}

uint8_t hw_read_spi() {

    uint8_t val;

    hw_read_array_spi( 1U, &val );

    return val;
}

void hw_write_array_spi( const size_t n, const uint8_t buf[ n ] ) {

    if ( ( 0U < n ) && ( NULL != buf ) ) {

        spi_write_blocking( spi0, buf, n );
    }
}

void hw_write_spi( const uint8_t val ) {

    hw_write_array_spi( 1U, &val );
}
