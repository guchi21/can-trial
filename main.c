#include "can_driver.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include <stdio.h>
#include <malloc.h>

#define MAXOF_RBUF ( 100U )

static uint8_t cntof_rbuf = 0U;
static struct cd_can_message recvs[ MAXOF_RBUF ];
static uint8_t cntof_print = 0U;
static uint32_t print[ MAXOF_RBUF ];


static uint32_t countof_recv = 0U;
static uint32_t countof_sent = 0U;
static uint32_t countof_ovf = 0U;
static struct cd_can_message recv_msg;
static bool is_busoff = false;

uint32_t getTotalHeap(void) {
   extern char __StackLimit, __bss_end__;
   
   return &__StackLimit  - &__bss_end__;
}

uint32_t getFreeHeap(void) {
   struct mallinfo m = mallinfo();

   return getTotalHeap() - m.uordblks;
}

bool cbk_print_stat( repeating_timer_t *rt ) {

    printf( "recv: %d, sent: %d, overflow: %d, tx_err: %d, rx_err: %d\n",
        countof_recv, countof_sent, countof_ovf, candrv_get_numof_tx_err(), candrv_get_numof_rx_err() );

    countof_recv = 0U;
    countof_sent = 0U;
    countof_ovf = 0U;

    return ( true );
}

void cbk_recv( enum CD_RX rx_idx ) {

    if( CD_SUCCESS == candrv_get_rx_msg( rx_idx, &recv_msg ) )
        countof_recv++;
}

void cbk_sent( enum CD_TX tx_idx ) {

    countof_sent++;
}

void cbk_rbuf_ovf( enum CD_RX rx_idx ) {

    countof_ovf++;
}

void cbk_busoff( bool occured ) {

    is_busoff = occured;
}

int main() {

    (void)stdio_init_all();

    printf("Total is %d, Free is %d\n", getTotalHeap(), getFreeHeap() );

    candrv_set_cbk_recv( (candrv_cbk_recv_t)cbk_recv );
    candrv_set_cbk_sent( (candrv_cbk_sent_t)cbk_sent );
    candrv_set_cbk_rbuf_ovf( (candrv_cbk_rbuf_ovf_t)cbk_rbuf_ovf );
    candrv_set_cbk_busoff( (candrv_cbk_busoff_t)cbk_busoff );

    if( CD_FAILURE == candrv_init() ) {
        while(1) 
            printf("初期化エラー");
    }

    static repeating_timer_t timer;

    add_repeating_timer_ms( -1000, &cbk_print_stat, NULL, &timer );


    while ( true ) {

        // sleep_us(2000);
        if ( !is_busoff ) {

            enum CD_TX tx_idx = candrv_get_available_tx();

            if ( CD_TX_INVALID != tx_idx ) {

                // メッセージ更新
                uint32_t current = time_us_32();
                struct cd_can_message s;
                s.id_kind = CD_CANID_KIND_STD;
                s.id = 0x295;
                s.is_remote = false;
                s.dlc = 8;
                s.data[0] = 0xFFU;
                s.data[1] = 0x00U;
                s.data[2] = 0xA5U;
                s.data[3] = 0xF7U;
                s.data[4] = (uint8_t)((current >> 24) & 0xff);
                s.data[5] = (uint8_t)((current >> 16) & 0xff);
                s.data[6] = (uint8_t)((current >> 8) & 0xff);
                s.data[7] = (uint8_t)(current & 0xff);

                // 送信バッファにせっと
                if ( false == candrv_set_tx_msg( tx_idx, &s, CD_TX_PRIORITY_MIDLOW ) ) {

                    printf("seterr.");
                }

                // 送信要求
                if ( false == candrv_set_send_req( tx_idx ) ) {

                    printf("reqerr.");
                }
            }
        } else {
            printf("おやすみ\n");
        }
    }
}
