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


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

bool candrv_init();
bool candrv_set_tx0_msg( const can_message_t *const msg );
bool candrv_set_tx1_msg( const can_message_t *const msg );
bool candrv_set_tx2_msg( const can_message_t *const msg );
bool candrv_req_send_tx0();
bool candrv_req_send_tx1();
bool candrv_req_send_tx2();
bool candrv_get_rx0_msg( can_message_t *const msg );
bool candrv_get_rx1_msg( can_message_t *const msg );

#endif  /* CAN_DRIVER_H */
