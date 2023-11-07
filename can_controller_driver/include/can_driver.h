/*
 * (c) 2023 guchi21.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "pico/stdlib.h"

/*!
 * \file can_driver.h
 * \brief CAN controller driver API.
 *
 * Manages CAN controllers. Its main function is to receive and send CAN messages.
 *
 * \author guchi21
 * \date 2023-11-01
 */

/*! 
 * \defgroup Cd CAN controller driver
 */
/*! \{ */

/*! 
 * \defgroup CdResult Execution result of API
 */
/*! \{ */

/*!
 * \brief Execution result of API.
 */
typedef uint8_t cd_result_t;

#define CD_SUCCESS ( 0U ) /*!< Success */
#define CD_FAILURE ( 1U ) /*!< Failure */
/*! \} */

/*! 
 * \defgroup CdDlc CAN data length code
 */
/*! \{ */

#define CD_CANDLC_EMPTY        ( 0U )               /*!< Empty of CAN message data          */
#define CD_CANDLC_MINOF_LEN    ( CD_CANDLC_EMPTY )  /*!< Minimum length of CAN message data */
#define CD_CANDLC_MAXOF_LEN    ( 8U )               /*!< Maximum length of CAN message data */
/*! \} */

/*!
 * @brief Timeout of send CAN message.
 */
#define CD_TIMEOUTOF_SEND_MS    ( 100U )    /*!< 100ms */

/*! 
 * \defgroup CanId Definitions related CAN ID
 */
/*! \{ */
#define CD_MAXOF_STD_CANID  ( 0x000007FFULL )   /*!< Maximum value of the standard CAN ID   */
#define CD_MAXOF_EXT_CANID  ( 0x1FFFFFFFULL )   /*!< Maximum value of the extension CAN ID  */

/*! 
 * \enum CD_CANID_KIND
 * \brief Represents a kind of CAN id.
 */
enum CD_CANID_KIND {
    CD_CANID_KIND_STD = 0,  /* Standard CAN id */
    CD_CANID_KIND_EXT = 1,  /* Extended CAN id */
    CD_CANID_KIND_NUMOF_ITEMS = 2,
    CD_CANID_KIND_MINOF_IDX = CD_CANID_KIND_STD,
    CD_CANID_KIND_MAXOF_IDX = CD_CANID_KIND_EXT
};
/*! \} */


/*! 
 * \struct cd_can_message
 * \brief CAN message.
 */
struct cd_can_message {
    enum CD_CANID_KIND id_kind;             /*!< Kind of CAN id         */
    uint32_t id;                            /*!< CAN id                 */
    bool is_remote;                         /*!< true if remote frame   */
    uint8_t dlc;                            /*!< Data lengh code        */
    uint8_t data[ CD_CANDLC_MAXOF_LEN ];    /*!< CAN data               */
};

/*! 
 * \enum CD_TX
 * \brief CAN transmitter.
 */
enum CD_TX {
    CD_TX_0 = 0,                /*!< CAN transmitter #1 */
    CD_TX_1 = 1,                /*!< CAN transmitter #2 */
    CD_TX_2 = 2,                /*!< CAN transmitter #3 */
    CD_TX_NUMOF_ITEMS = 3,
    CD_TX_MINOF_IDX = CD_TX_0,
    CD_TX_MAXOF_IDX = CD_TX_2,
    CD_TX_INVALID = 9
};

/*! 
 * \enum CD_TX_PRIORITY
 * \brief CAN transmitter priority.
 */
enum CD_TX_PRIORITY {
    CD_TX_PRIORITY_LOW = 0,     /*!< Low priority           */
    CD_TX_PRIORITY_MIDLOW = 1,  /*!< Middle low priority    */
    CD_TX_PRIORITY_MIDHIGH = 2, /*!< Middle high priority   */
    CD_TX_PRIORITY_HIGH = 3,    /*!< High priority          */
    CD_TX_PRIORITY_NUMOF_ITEMS = 4,
    CD_TX_PRIORITY_MINOF_IDX = CD_TX_PRIORITY_LOW,
    CD_TX_PRIORITY_MAXOF_IDX = CD_TX_PRIORITY_HIGH
};

/*! 
 * \enum CD_TX
 * \brief CAN receiver.
 */
enum CD_RX {
    CD_RX_0 = 0,            /*!< CAN receiver #1 */
    CD_RX_1 = 1,            /*!< CAN receiver #2 */
    CD_RX_NUMOF_ITEMS = 2,
    CD_RX_MINOF_IDX = CD_RX_0,
    CD_RX_MAXOF_IDX = CD_RX_1
};

/*! 
 * \enum CD_IRQ_CAUSE
 * \brief Represents a CAN interrupt cause.
 */
enum CD_IRQ_CAUSE {
    CD_IRQ_CAUSE_RX0_FULL = 0,  /*!< Received CAN message by receiver #1    */
    CD_IRQ_CAUSE_RX1_FULL = 1,  /*!< Received CAN message by receiver #2    */
    CD_IRQ_CAUSE_TX0_EMPTY = 2, /*!< Sent CAN message by transmitter #1     */
    CD_IRQ_CAUSE_TX1_EMPTY = 3, /*!< Sent CAN message by transmitter #2     */
    CD_IRQ_CAUSE_TX2_EMPTY = 4, /*!< Sent CAN message by transmitter #3     */
    CD_IRQ_CAUSE_ERR = 5,       /*!< Change error state in CAN network      */
    CD_IRQ_CAUSE_WAKEUP = 6,    /*!< CAN controller wake-up requested       */
    CD_IRQ_CAUSE_MSGERR = 7,    /*!< Cause send CAN error                   */
    CD_IRQ_CAUSE_NUMOF_ITEMS = 8,
    CD_IRQ_CAUSE_MINOF_IDX = CD_IRQ_CAUSE_RX0_FULL,
    CD_IRQ_CAUSE_MAXOF_IDX = CD_IRQ_CAUSE_MSGERR
};

/*!
 * \brief Initialize the CAN controller.
 *
 * Confiugure the CAN baudrate.
 */
cd_result_t cd_init( void );

/*!
 * \brief Get received CAN message.
 *
 * When processing is successful, the CAN message received interrupt is cleared.
 *
 * @param[in] rx_idx CAN receiver
 * @param[out] msg Received CAN message
 * @return CD_SUCCESS if successful, otherwise CD_FAILURE.
 * @exception illegal_arguments Do nothing if \a msg is \a NULL or \a rx_idx is out of range.
 * @exception illegal_state Do nothing if not received CAN message.
 */
cd_result_t cd_get_message( const enum CD_RX rx_idx, struct cd_can_message *const msg );

/*!
 * \brief Set send CAN message.
 *
 * Transmission priority will be \ref CD_TX_PRIORITY_MIDLOW .
 *
 * @param[in] tx_idx CAN transmitter
 * @param[out] msg Send CAN message
 * @return CD_SUCCESS if successful, otherwise CD_FAILURE.
 * @exception illegal_arguments Do nothing if \a msg is \a NULL or \a tx_idx is out of range.
 * @exception illegal_state Do nothing if not available CAN transmitter.
 */
cd_result_t cd_set_message( const enum CD_TX tx_idx, const struct cd_can_message *const msg );

/*!
 * \brief Set send CAN message with send priprity.
 *
 * @param[in] tx_idx CAN transmitter
 * @param[in] priority CAN transmitter priority
 * @param[out] msg Send CAN message
 * @return CD_SUCCESS if successful, otherwise CD_FAILURE.
 * @exception illegal_arguments Do nothing if \a msg is \a NULL or \a tx_idx is out of range or \a priority is out of range.
 * @exception illegal_state Do nothing if not available CAN transmitter.
 */
cd_result_t cd_set_message_with_priority( const enum CD_TX tx_idx, const enum CD_TX_PRIORITY priority, 
    const struct cd_can_message *const msg );

/*!
 * \brief Set request of send CAN message.
 *
 * When processing is successful, the CAN message sent interrupt is clear and enabled.
 * If the submission is not completed by the timeout, the request will be cleared.
 * Timeout is \ref CD_TIMEOUTOF_SEND_MS .
 *
 * @param[in] tx_idx CAN transmitter
 * @return CD_SUCCESS if successful, otherwise CD_FAILURE.
 * @exception illegal_arguments Do nothing if \a tx_idx is out of range.
 * @exception illegal_state Do nothing if not available CAN transmitter.
 */
cd_result_t cd_set_send_request( const enum CD_TX tx_idx );

/*!
 * \brief Clear request of send CAN message.
 *
 * When processing is successful, the CAN message sent interrupt is clear and disabled.
 *
 * @param[in] tx_idx CAN transmitter
 * @return CD_SUCCESS if successful, otherwise CD_FAILURE.
 * @exception illegal_arguments Do nothing if \a tx_idx is out of range.
 */
cd_result_t cd_clear_send_request( const enum CD_TX tx_idx );

/*!
 * \brief Get available CAN transmitter.
 *
 * @return Returns one of the available CAN transceivers. Return CD_TX_INVALID if all CAN transmitter is not available.
 */
enum CD_TX cd_get_available_tx( void );

/*!
 * \brief Callback for received CAN message.
 */
typedef void (*cd_received_callback_t)( enum CD_RX rx_idx );

/*!
 * \brief Callback for overflow CAN message.
 */
typedef void (*cd_overflow_callback_t)( enum CD_RX rx_idx );

/*!
 * \brief Callback for sent CAN message.
 */
typedef void (*cd_sent_callback_t)( enum CD_TX tx_idx );

/*!
 * \brief Set callback function when received CAN message.
 * \param cbk callback function when received CAN message
 */
void cd_set_received_callback( cd_received_callback_t cbk );

/*!
 * \brief Set callback function when overlow CAN message.
 * \param cbk callback function when overflow CAN message
 */
void cd_set_overflow_callback( cd_overflow_callback_t cbk );

/*!
 * \brief Set callback function when sent CAN message.
 * \param cbk callback function when sent CAN message
 */
void cd_set_sent_callback( cd_sent_callback_t cbk );

/*! \} */   /* defgroup Cd */

#endif  /* CAN_DRIVER_H */
