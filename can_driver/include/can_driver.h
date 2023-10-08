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


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/
typedef bool candrv_result_t;

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

candrv_result_t candrv_init();
candrv_result_t candrv_set_tx0_msg( const can_message_t *const msg );
candrv_result_t candrv_set_tx1_msg( const can_message_t *const msg );
candrv_result_t candrv_set_tx2_msg( const can_message_t *const msg );
candrv_result_t candrv_req_send_tx0();
candrv_result_t candrv_req_send_tx1();
candrv_result_t candrv_req_send_tx2();
candrv_result_t candrv_get_rx0_msg( can_message_t *const msg );
candrv_result_t candrv_get_rx1_msg( can_message_t *const msg );

#endif  /* CAN_DRIVER_H */
