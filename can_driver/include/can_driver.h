#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

enum CAN_FORMAT_KIND {
    CAN_FORMAT_KIND_INDEX_MIN = 0U,
    CAN_FORMAT_KIND_STANDARD = CAN_FORMAT_KIND_INDEX_MIN,
    CAN_FORMAT_KIND_EXTENSION,
    CAN_FORMAT_KIND_INDEX_MAX = CAN_FORMAT_KIND_EXTENSION
};

typedef struct {
    enum CAN_FORMAT_KIND kind;
    uint32_t id;
    uint8_t length;
    uint8_t *content;
} can_message_t;

bool candrv_initialize();

#endif
