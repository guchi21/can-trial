#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef PICO_CAN_H
#include "pico_can.h"
#endif

bool candrv_init() {

    /* Reset CAN controller. */
    if ( false == picocan_reset() ) {

        return false;
    }

    clear tec;
    clear rec;


    clear cat stat;
    clear can ctrl;
    clear eflg;

    clear caninte;
    clear can intf;
    


    clear tx0;
    clear rx 1;

    set rxf[0-5]
    set rxm[0-1]




    // set can rate;
    if ( false == picocan_set_canrate( CANRATE_1000KBPS ) ) {

        return false;
    }

    // clear registers;


    picocan_set_mode( OPMODE_NORMAL );
}
