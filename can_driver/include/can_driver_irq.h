#ifndef CAN_DRIVER_IRQ_H
#define CAN_DRIVER_IRQ_H

#include "can.h"
#include "can_driver.h"

typedef void (*candrv_cbk_recv_t)( enum CANDRV_RX rx_idx );

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/
void candrv_ind_irq( void );
void candrv_set_cbk_recv( candrv_cbk_recv_t cbk );

#endif  /* CAN_DRIVER_IRQ_H */