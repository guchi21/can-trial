#ifndef CAN_H
#define CAN_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif


/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/
typedef enum {
    CAN_FORMAT_KIND_INDEX_MIN = 0U,
    CAN_FORMAT_KIND_STANDARD = CAN_FORMAT_KIND_INDEX_MIN,
    CAN_FORMAT_KIND_EXTENSION,
    CAN_FORMAT_KIND_INDEX_MAX = CAN_FORMAT_KIND_EXTENSION
} can_format_kind_t;

typedef struct {
    can_format_kind_t kind;
    uint32_t id;
    uint8_t length;
    uint8_t *content;
} can_message_t;


#endif  /* CAN_H */
