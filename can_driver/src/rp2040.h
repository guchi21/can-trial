#ifndef RP2040_H
#define RP2040_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Prototypes.                                                      */
/*==================================================================*/

/*! \brief Initialize SPI for MCP2515.
 * \note Baudrate is 10MHz. GP4: SPI MISO, GP5: SPI CS, GP6: SPI SCK, GP7: SPI MOSI.
 */
void rp2040_init_spi();


/*! \brief Begin sending SPI commands to MCP2515.
 */
void rp2040_begin_spi_commands();


/*! \brief End sending SPI commands to MCP2515.
 */
void rp2040_end_spi_commands();


/*! \brief Read a byte from SPI.
 * \return read value
 */
uint8_t rp2040_read_spi();

/*! \brief Read bytes from SPI.
 * \param array read values
 * \param length read bytes length
 */
void rp2040_read_array_spi( uint8_t *const array, const uint8_t length );

/*! \brief Write a byte to SPI.
 * \param val write value
 */
void rp2040_write_spi( const uint8_t val );

/*! \brief Write bytes to SPI.
 * \param array write values
 * \param length write bytes length
 */
void rp2040_write_array_spi( const uint8_t *const array, const uint8_t length );

/*! \brief Register callback function for CAN interruption.
 * \param callback callback function. unregister if NULL.
 */
void rp2040_register_can_int_callback( const gpio_irq_callback_t callback );

#endif  /* RP2040_H */