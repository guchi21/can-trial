#ifndef PICO_CAN_H
#define PICO_CAN_H

#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/
enum CAN_BAUDRATE {
    CAN_BAUDRATE_INDEX_MIN = 0U,
    CAN_BAUDRATE_5KBPS = CAN_BAUDRATE_INDEX_MIN,
    CAN_BAUDRATE_10KBPS,
    CAN_BAUDRATE_20KBPS,
    CAN_BAUDRATE_50KBPS,
    CAN_BAUDRATE_100KBPS,
    CAN_BAUDRATE_125KBPS,
    CAN_BAUDRATE_250KBPS,
    CAN_BAUDRATE_500KBPS,
    CAN_BAUDRATE_800KBPS,
    CAN_BAUDRATE_1000KBPS,
    CAN_BAUDRATE_INDEX_MAX = CAN_BAUDRATE_1000KBPS,
    CAN_BAUDRATE_NUMOF_ITEMS
};

enum PICOCAN_OPERATION_MODE {
    PICOCAN_OPERATION_MODE_INDEX_MIN = 0U,
    PICOCAN_OPERATION_MODE_NORMAL = PICOCAN_OPERATION_MODE_INDEX_MIN,
    PICOCAN_OPERATION_MODE_SLEEP,
    PICOCAN_OPERATION_MODE_LOOPBACK,
    PICOCAN_OPERATION_MODE_LISTENONLY,
    PICOCAN_OPERATION_MODE_CONFIG,
    PICOCAN_OPERATION_MODE_INDEX_MAX = PICOCAN_OPERATION_MODE_CONFIG,
    PICOCAN_OPERATION_MODE_NUMOF_ITEMS
};

enum PICOCAN_RESULT {
    PICOCAN_SUCCESS = 0U,
    PICOCAN_FAILURE
};

enum PICOCAN_TX_PRIORITY {
    PICOCAN_TX_PRIORITY_INDEX_MIN = 0U,
    PICOCAN_TX_PRIORITY_LOW = PICOCAN_TX_PRIORITY_INDEX_MIN,
    PICOCAN_TX_PRIORITY_MIDDLE_LOW,
    PICOCAN_TX_PRIORITY_MIDDLE_HIGH,
    PICOCAN_TX_PRIORITY_HIGH,
    PICOCAN_TX_PRIORITY_INDEX_MAX = PICOCAN_TX_PRIORITY_HIGH,
    PICOCAN_TX_PRIORITY_NUMOF_ITEMS
};


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

bool picocan_reset();
bool picocan_change_can_baudrate( const enum CAN_BAUDRATE baudrate );
bool picocan_change_operation_mode( const enum PICOCAN_OPERATION_MODE mode );
bool picocan_set_tx0_buffer( const can_message_t *const msg );
bool picocan_set_tx1_buffer( const can_message_t *const msg );
bool picocan_set_tx2_buffer( const can_message_t *const msg );
bool picocan_request_send_tx0();
bool picocan_request_send_tx1();
bool picocan_request_send_tx2();
bool picocan_set_tx0_priority( enum PICOCAN_TX_PRIORITY priority );
bool picocan_set_tx1_priority( enum PICOCAN_TX_PRIORITY priority );
bool picocan_set_tx2_priority( enum PICOCAN_TX_PRIORITY priority );

// todo
// - request send prototype
// - write txbuf .. before confirm ready
// - write txbuf .. block send request while writing

#endif /* PICO_CAN_H */
