#ifndef CAN_H
#define CAN_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif


/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

#define CAN_EMPTY                       ( 0U )
#define CAN_MINOF_LEN                   CAN_EMPTY                               /* Minimum length of the content of CAN message.  */
#define CAN_MAXOF_LEN                   ( 8U )                                  /* Maximum length of the content of CAN message.  */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef enum {
    CANID_KIND_MINOF_IDX = 0U,
    CANID_KIND_STD = CANID_KIND_MINOF_IDX,
    CANID_KIND_EXT,
    CANID_KIND_MAXOF_IDX = CANID_KIND_EXT
} canid_kind_t;

typedef struct {
    canid_kind_t id_kind;
    uint32_t id;
    bool is_remote;
    uint8_t dlc;
    uint8_t data[ CAN_MAXOF_LEN ];
} can_msg_t;


#endif  /* CAN_H */
