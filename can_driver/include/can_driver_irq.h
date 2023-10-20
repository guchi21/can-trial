#ifndef CAN_DRIVER_IRQ_H
#define CAN_DRIVER_IRQ_H

#include "can.h"

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/
void candrv_ind_irq( void );
void mcp2515_clr_all_send_req_if_err( void );
can_message_t* candrv_get_recv_msg0( void );
can_message_t* candrv_get_recv_msg1( void );

#endif  /* CAN_DRIVER_IRQ_H */