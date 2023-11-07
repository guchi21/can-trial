/*
 * (c) 2023 guchi21.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef HARDWARE_MANAGER_H
#define HARDWARE_MANAGER_H

#include "pico/stdlib.h"

/*!
 * \file hardware_manager.h
 * \brief Hardware manager API.
 *
 * Control the hardware consisting of MCU and CAN controller.
 * MCU is raspberry pi pico. CAN controller is mcp2515.
 * The CAN controller is controlled from the MCU via SPI and the API abstracts it.
 * By registering a callback function, you can handle interrupts
 * from the CAN controller and perform arbitrary interrupt processing.
 *
 * \author guchi21
 * \date 2023-11-01
 */

/*! 
 * \defgroup Hw Hardware manager module
 */
/*! \{ */

/*! 
 * \enum HW_IRQ_CAUSE
 * \brief Represents a GPIO interrupt cause.
 */
enum HW_IRQ_CAUSE {
    HW_IRQ_CAUSE_CAN_CONTROLLER = 0,       /*!< CAN controller. */
    HW_IRQ_CAUSE_HALL_SENSOR = 1,          /*!< Hall sensor.    */
    HW_IRQ_CAUSE_NUMOF_ITEMS = 2,
    HW_IRQ_CAUSE_MINOF_IDX = HW_IRQ_CAUSE_CAN_CONTROLLER,
    HW_IRQ_CAUSE_MAXOF_IDX = HW_IRQ_CAUSE_HALL_SENSOR
};

/*! 
 * \defgroup HwResult Execution result of API
 */
/*! \{ */

/*!
 * \brief Execution result of API.
 */
typedef uint8_t hw_result_t;

#define HW_SUCCESS ( 0U ) /*!< Success */
#define HW_FAILURE ( 1U ) /*!< Failure */
/*! \} */

/*!
 * \brief Callback function for the GPIO interruption.
 * \param event_mask Which events caused this interrupt. See \ref gpio_irq_level for details.
 */
typedef void (*hw_irq_callback_t)( uint32_t event_mask );

/*!
 * \brief Initialize the hardware manager module.
 *
 * It only initializes the GPIO configuration and does nothing else.
 * Please execute only once.
 * The results of the second and subsequent executions are not guaranteed.
 */
void hw_init( void );

/*!
 * \brief Begin SPI transaction.
 *
 * From the time this function is executed until the end of the transaction,
 * SPI commands can be sent and results can be received.
 * 
 * Only one CAN controller is connected to SPI.
 */
void hw_begin_spi_commands( void );

/*!
 * \brief End SPI transaction.
 */
void hw_end_spi_commands( void );

/*!
 * \brief Read a byte from SPI.
 */
uint8_t hw_read_spi( void );

/*!
 * \brief Read multi bytes from SPI.
 *
 * @param[in] n Size of read data
 * @param[out] buf Read data
 * @exception illegal_arguments Do nothing if \a buf is \a NULL or \a n is \a 0.
 */
void hw_read_array_spi( const size_t n, uint8_t buf[ n ] );

/*!
 * \brief Write a byte to SPI.
 *
 * @param[in] val Write data
 */
void hw_write_spi( const uint8_t val );

/*!
 * \brief Write multi bytes to SPI.
 *
 * @param[in] n Size of write data
 * @param[out] buf Write data
 * @exception illegal_arguments Do nothing if \a buf is \a NULL or \a n is \a 0.
 */
void hw_write_array_spi( const size_t n, const uint8_t buf[ n ] );

/*!
 * \brief Set callback function for GPIO interruption.
 *
 * @param[in] cause interrupt cause
 * @param[in] callback callback function
 * @return \a HW_FAILURE if cause is out of range, otherwise \a HW_SUCCESS.
 * @exception null Un-set the callback if \a callback is \a NULL.
 */
hw_result_t hw_set_irq_callback( enum HW_IRQ_CAUSE cause, hw_irq_callback_t callback );

/*! \} */

#endif  /* HARDWARE_MANAGER_H */
