#ifndef PICO_CAN_H
#define PICO_CAN_H

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
    CANRATE_MIN = 0,
    CANRATE_5KBPS = CANRATE_MIN,
    CANRATE_10KBPS,
    CANRATE_20KBPS,
    CANRATE_50KBPS,
    CANRATE_100KBPS,
    CANRATE_125KBPS,
    CANRATE_250KBPS,
    CANRATE_500KBPS,
    CANRATE_800KBPS,
    CANRATE_1000KBPS,
    CANRATE_MAX = CANRATE_1000KBPS,
    CANRATE_SIZE
} picocan_canrate;

typedef enum {
    OPMODE_MIN = 0,
    OPMODE_NORMAL = OPMODE_MIN,
    OPMODE_SLEEP,
    OPMODE_LOOPBACK,
    OPMODE_LISTENONLY,
    OPMODE_CONFIG,
    OPMODE_MAX = OPMODE_CONFIG,
    OPMODE_SIZE
} picocan_opmode;


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

bool picocan_reset();
bool picocan_set_canrate( const picocan_canrate rate );
bool picocan_set_mode( const picocan_opmode mode );

#endif
