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

typedef void (*can_int_callback_t)( uint gpio, uint32_t event_mask );


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

/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

candrv_result_t candrv_init();
candrv_result_t candrv_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg );
candrv_result_t candrv_req_send_msg( const enum CANDRV_TX tx_idx );
candrv_result_t candrv_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg );
void candrv_register_callback( const can_int_callback_t callback );
void candrv_set_irq_enabled( enum CANDRV_IRQ irq, bool enabled );

void candrv_tmp1( can_message_t *msg );
bool candrv_tmp2();
void candrv_tmp3();
void candrv_tmp4();
void candrv_tmp5();
void candrv_tmp6();
void candrv_tmp_clr_exerr();
void candrv_tmp_cancel_tx_if_msgerr( void );
bool candrv_tmp_is_merr( void );
bool candrv_tmp_is_err( void );
void candrv_tmp_clr_rx0( void );

#endif  /* CAN_DRIVER_H */
