#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

#ifndef PICO_CAN_H
#include "pico_can.h"
#endif

bool candrv_initialize() {

    /* Reset CAN controller. */
    if ( false == picocan_reset() ) {

        return false;
    }

    if ( false == picocan_set_tx0_priority( PICOCAN_TX_PRIORITY_LOW ) ) {

        return false;
    }

    if ( false == picocan_set_tx1_priority( PICOCAN_TX_PRIORITY_MIDDLE_LOW ) ) {

        return false;
    }

    if ( false == picocan_set_tx2_priority( PICOCAN_TX_PRIORITY_MIDDLE_HIGH ) ) {

        return false;
    }
    

    // clear tec;
    // clear rec;


    // clear cat stat;
    // clear can ctrl;
    // clear eflg;

    // clear caninte;
    // clear can intf;
    


    // clear tx0;
    // clear rx 1;

    // set rxf[0-5]
    // set rxm[0-1]




    // set can rate;
    if ( false == picocan_change_can_baudrate( CAN_BAUDRATE_1000KBPS ) ) {

        return false;
    }

    // clear registers;


    picocan_change_operation_mode( PICOCAN_OPERATION_MODE_NORMAL );
}

bool clear_tx1buf() {
    
}
