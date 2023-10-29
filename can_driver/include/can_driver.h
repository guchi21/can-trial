#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include "pico/stdlib.h"
#include "can.h"

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

#define CANDRV_FAILURE ( 0U )
#define CANDRV_SUCCESS ( 1U )


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef bool candrv_result_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

enum CANDRV_TX {
    CANDRV_TX_MINOF_IDX = 0U,
    CANDRV_TX_0 = CANDRV_TX_MINOF_IDX,
    CANDRV_TX_1,
    CANDRV_TX_2,
    CANDRV_TX_MAXOF_IDX = CANDRV_TX_2,
    CANDRV_TX_NUMOF_ITEMS
};

enum CANDRV_RX {
    CANDRV_RX_MINOF_IDX = 0U,
    CANDRV_RX_0 = CANDRV_RX_MINOF_IDX,
    CANDRV_RX_1,
    CANDRV_RX_MAXOF_IDX = CANDRV_RX_1,
    CANDRV_RX_NUMOF_ITEMS
};

enum CANDRV_IRQ {
    CANDRV_IRQ_MINOF_IDX = 0U,
    CANDRV_IRQ_RX0_FULL = CANDRV_IRQ_MINOF_IDX,
    CANDRV_IRQ_RX1_FULL,
    CANDRV_IRQ_TX0_EMPTY,
    CANDRV_IRQ_TX1_EMPTY,
    CANDRV_IRQ_TX2_EMPTY,
    CANDRV_IRQ_ERR,
    CANDRV_IRQ_WAKEUP,
    CANDRV_IRQ_MSGERR,
    CANDRV_IRQ_MAXOF_IDX = CANDRV_IRQ_MSGERR,
    CANDRV_IRQ_NUMOF_ITEMS
};

enum CANDRV_TX_PRIORITY {
    CANDRV_TX_PRIORITY_MINOF_IDX = 0,
    CANDRV_TX_PRIORITY_LOW = CANDRV_TX_PRIORITY_MINOF_IDX,
    CANDRV_TX_PRIORITY_MIDLOW,
    CANDRV_TX_PRIORITY_MIDHIGH,
    CANDRV_TX_PRIORITY_HIGH,
    CANDRV_TX_PRIORITY_MAXOF_IDX = CANDRV_TX_PRIORITY_HIGH,
    CANDRV_TX_PRIORITY_NUMOF_ITEMS
};

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

candrv_result_t candrv_init();
candrv_result_t candrv_get_rx_msg( const enum CANDRV_RX rx_idx, can_msg_t *const msg );
candrv_result_t candrv_set_tx_msg( const enum CANDRV_TX tx_idx,
    const can_msg_t *const msg, const enum CANDRV_TX_PRIORITY priority );
candrv_result_t candrv_set_send_req( const enum CANDRV_TX tx_idx );
candrv_result_t candrv_clr_send_req( const enum CANDRV_TX tx_idx );

#endif  /* CAN_DRIVER_H */
