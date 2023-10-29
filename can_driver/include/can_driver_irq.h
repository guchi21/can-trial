#ifndef CAN_DRIVER_IRQ_H
#define CAN_DRIVER_IRQ_H

#include "can.h"
#include "can_driver.h"

typedef void (*candrv_cbk_recv_t)( const enum CANDRV_RX rx_idx );

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/
void candrv_ind_irq( void );
void candrv_set_cbk_recv( const candrv_cbk_recv_t *const cbk );

#endif  /* CAN_DRIVER_IRQ_H */