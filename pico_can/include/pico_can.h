#ifndef PICO_CAN_H
#define PICO_CAN_H

#include "pico/stdlib.h"

void picocan_init( void );
void picocan_begin_spi_commands();
void picocan_end_spi_commands();
uint8_t picocan_read_spi();
void picocan_read_array_spi( const uint8_t n, uint8_t buf[n] );
void picocan_write_spi( const uint8_t val );
void picocan_write_array_spi( const uint8_t n, const uint8_t buf[n] );


#endif  /* PICO_GAN_H */
