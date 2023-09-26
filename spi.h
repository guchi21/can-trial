/**
 * Copyright (c) 2023 Project-K.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef SPI_H
#define SPI_H

#ifndef COMDEF_H
#include "comdef.h"
#endif

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

#ifndef _HARDWARE_SPI_H
#include "hardware/spi.h"
#endif

/* Pin number definitions for SPI */
#define PIN_SPI_RX 4
#define PIN_SPI_CSN 5
#define PIN_SPI_SCK 6
#define PIN_SPI_TX 7

/*! \brief Write a byte to SPI
 *  \ingroup driver
 *
 * \param value a byte that to be written
 * \note Write to MCP2515
 */
void spi_write(uint8_t value);

/*! \brief Read a byte from SPI
 *  \ingroup driver
 *
 * \return a byte that readed
 * \note Read from MCP2515
 */
uint8_t spi_read(void);

/*! \brief Configure to use SPI
 *  \ingroup driver
 *
 * \note For use with MCP2515
 */
void spi_configure(void);

#endif