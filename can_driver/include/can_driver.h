#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

#ifndef CAN_H
#include "can.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/
#define CANDRV_FAILURE ( 0U )
#define CANDRV_SUCCESS ( 1U )

#define CANDRV_INVALID_CAN_ID ( UINT32_MAX )

/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/
typedef bool candrv_result_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

enum CANDRV_TX {
    CANDRV_TX_INDEX_MIN = 0U,
    CANDRV_TX_0 = CANDRV_TX_INDEX_MIN,
    CANDRV_TX_1,
    CANDRV_TX_2,
    CANDRV_TX_INDEX_MAX = CANDRV_TX_2,
    CANDRV_TX_NUMOF_ITEMS
};

enum CANDRV_RX {
    CANDRV_RX_INDEX_MIN = 0U,
    CANDRV_RX_0 = CANDRV_RX_INDEX_MIN,
    CANDRV_RX_1,
    CANDRV_RX_INDEX_MAX = CANDRV_RX_1,
    CANDRV_RX_NUMOF_ITEMS
};


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

candrv_result_t candrv_init();
candrv_result_t candrv_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg );
candrv_result_t candrv_req_send_msg( const enum CANDRV_TX tx_idx );
candrv_result_t candrv_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg );


#endif  /* CAN_DRIVER_H */
