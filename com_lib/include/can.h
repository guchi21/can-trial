#ifndef CAN_H
#define CAN_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif


/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* Related CAN content length definitions */
#define CAN_MINOF_LEN                   ( 0U )                  /* Minimum length of the content of CAN message.  */
#define CAN_MAXOF_LEN                   ( 8U )                  /* Maximum length of the content of CAN message.  */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef enum {
    CAN_KIND_MINOF_IDX = 0U,
    CAN_KIND_STD = CAN_KIND_MINOF_IDX,
    CAN_KIND_EXT,
    CAN_KIND_MAXOF_IDX = CAN_KIND_EXT
} can_format_kind_t;

typedef struct {
    can_format_kind_t kind;
    uint32_t id;
    uint8_t length;
    uint8_t *content;
} can_message_t;


#endif  /* CAN_H */
