#ifndef CAN_DRIVER_IRQ_H
#define CAN_DRIVER_IRQ_H

#include "can.h"
#include "can_driver.h"

typedef void (*candrv_cbk_recv_t)( enum CANDRV_RX rx_idx );
typedef void (*candrv_cbk_sent_t)( enum CANDRV_TX tx_idx );
typedef void (*candrv_cbk_rbuf_ovf_t)( enum CANDRV_RX rx_idx );
typedef void (*candrv_cbk_busoff_t)( bool occured );

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/
void candrv_ind_irq( void );
void candrv_set_cbk_recv( candrv_cbk_recv_t cbk );
void candrv_set_cbk_sent( candrv_cbk_sent_t cbk );
void candrv_set_cbk_rbuf_ovf( candrv_cbk_rbuf_ovf_t cbk );
void candrv_set_cbk_busoff( candrv_cbk_busoff_t cbk );

#endif  /* CAN_DRIVER_IRQ_H */