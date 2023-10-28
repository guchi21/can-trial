#ifndef CAN_DRIVER_IRQ_H
#define CAN_DRIVER_IRQ_H

#include "can.h"

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/
void candrv_ind_irq( void );
void candrv_clr_all_send_req_if_err( void );
can_msg_t* candrv_get_recv_msg0( void );
can_msg_t* candrv_get_recv_msg1( void );

#endif  /* CAN_DRIVER_IRQ_H */