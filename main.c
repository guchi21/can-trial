#include "can.h"
#include "can_driver.h"
#include "can_driver_irq.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include <stdio.h>
#include <malloc.h>

#define MAXOF_RBUF ( 4U )

static uint8_t cntof_rbuf = 0U;
static can_msg_t recvs[ MAXOF_RBUF ];


uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();

   return getTotalHeap() - m.uordblks;
}

void cbk( enum CANDRV_RX rx_idx ) {

    if ( MAXOF_RBUF <= cntof_rbuf )
        return;

    uint8_t i = cntof_rbuf;

    candrv_result_t res = candrv_get_rx_msg( rx_idx, &recvs[ i ] );

    if( CANDRV_SUCCESS == res )
        cntof_rbuf++;
}

int main() {

    printf("Free is %d", getFreeHeap() );

    candrv_set_cbk_recv( (candrv_cbk_recv_t)cbk );

    if( CANDRV_FAILURE == candrv_init() ) {
        while(1) 
            printf("初期化エラー");
    }

    uint8_t c = 1U;

    while ( true ) {

        if ( 0U < cntof_rbuf ) {

            uint32_t irqs = save_and_disable_interrupts();

            printf( "Received message...\n");

            for ( uint8_t i = 0U; cntof_rbuf > i; i++ ) {

                can_msg_t* m = &recvs[ i ];
                printf( "[%#X] %d %#X %#X %#X %#X %#X %#X %#X %#X\n", m->id, m->dlc,
                    m->data[0], m->data[1], m->data[2], m->data[3],
                    m->data[4], m->data[5], m->data[6], m->data[7] );
            }

            cntof_rbuf = 0U;

            restore_interrupts(irqs);

            printf("Free is %d", getFreeHeap() );
        }


        if ( CANDRV_SUCCESS == candrv_is_tx_available( CANDRV_TX_0 ) ) {

            // メッセージ更新
            uint32_t current = time_us_32();
            can_msg_t s;
            s.id_kind = CANID_KIND_STD;
            s.id = 0x295 + c;
            s.is_remote = false;
            s.dlc = 8;
            s.data[0] = 0xFFU;
            s.data[1] = 0x00U;
            s.data[2] = 0xA5U;
            s.data[3] = 0xF7U;
            s.data[4] = (uint8_t)(current & 0xff);
            s.data[5] = (uint8_t)((current >> 8) & 0xff);
            s.data[6] = (uint8_t)((current >> 16) & 0xff);
            s.data[7] = (uint8_t)((current >> 24) & 0xff);

            if ( 0xFFU == c )
                c = 0U;
            
            c++;

            // 送信バッファにせっと
            if ( false == candrv_set_tx_msg( CANDRV_TX_0, &s, CANDRV_TX_PRIORITY_MIDLOW ) ) {

                // printf("seterr.");
            }

            // 送信要求
            if ( false == candrv_set_send_req( CANDRV_TX_0 ) ) {

                // printf("reqerr.");
            }
        }
    }
}
