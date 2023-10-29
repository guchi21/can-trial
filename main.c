#include "can_driver.h"
#include "can_driver_irq.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include <limits.h>
#include <string.h>

#define MAXOF_RBUF ( 4U )

static uint8_t cntof_rbuf = 0U;
static can_msg_t recvs[ MAXOF_RBUF ];

static void cbk_recv( const enum CANDRV_RX rx_idx ) {

    if ( MAXOF_RBUF <= cntof_rbuf )
        return;

    uint8_t i = cntof_rbuf;

    candrv_result_t res = candrv_get_rx_msg( rx_idx, recvs[ i ] );

    if( CANDRV_SUCCESS == res )
        cntof_rbuf++;
}

int main() {

    candrv_set_cbk_recv( cbk_recv );

    if( CANDRV_FAILURE == candrv_init() ) {
        while(1) 
            printf("初期化エラー");
    }

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
        }

        uint32_t current = time_us_32();

        // メッセージ更新
        can_message_t s;
        s.id_kind = CAN_KIND_STD;
        s.id = 0x295;
        s.is_remote = false;
        s.dlc = 8;
        s.data[0] = (uint8_t)(current & 0xff);
        s.data[1] = (uint8_t)((current >> 8) & 0xff);
        s.data[2] = (uint8_t)((current >> 16) & 0xff);
        s.data[3] = (uint8_t)((current >> 24) & 0xff);
        s.data[4] = (uint8_t)(current & 0xff);
        s.data[5] = (uint8_t)((current >> 8) & 0xff);
        s.data[6] = (uint8_t)((current >> 16) & 0xff);
        s.data[7] = (uint8_t)((current >> 24) & 0xff);

        // 送信バッファにせっと
        if( false == candrv_set_tx_msg( CANDRV_TX_0, &s ) ) {

            printf("seterr.");
        }

        // 送信要求
        if ( false == candrv_req_send_msg( CANDRV_TX_0 ) ) {

            printf("reqerr.");
        }
    }
}
