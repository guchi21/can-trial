#include "can_driver.h"
#include "can_driver_irq.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/sync.h"
#include <limits.h>
#include <string.h>

int main() {

    uint8_t local_content[8] = { 0U };
    can_message_t local_msg = { CAN_KIND_STD, 0U, 0U, local_content };

    uint32_t rt = time_us_32();
    uint32_t prert = rt;


    if( CANDRV_FAILURE == candrv_init() ) {
        while(1) 
            printf("初期化エラー");
    }

    test();

    // そうしんめっせ作成
    can_message_t s;
    uint8_t content[8] = { 0xA1, 0xB2, 0xC3, 0xFF, 0x00, 0x45, 0xEA, 0x55 };
    s.kind = CAN_KIND_STD;
    s.id = 0x295;
    s.length = 8;
    s.content = content;

    while(1) {

        can_message_t* m = candrv_get_recv_msg0();
        rt = m->content[4] + ( m->content[5] << 8 ) + ( m->content[6] << 16 ) + ( m->content[7] << 24 );
        printf( "Received message. [%#X] %#X %#X %#X %#X %#X %#X %#X %#X | %d \n", m->id,
                m->content[0], m->content[1], m->content[2], m->content[3],
                m->content[4], m->content[5], m->content[6], m->content[7], rt - prert );
        prert = rt;

        busy_wait_us_32(2 * 1000 * 1000);

        uint32_t current = time_us_32();

        // メッセージ更新
        s.content[4] = (uint8_t)(current & 0xff);
        s.content[5] = (uint8_t)((current >> 8) & 0xff);
        s.content[6] = (uint8_t)((current >> 16) & 0xff);
        s.content[7] = (uint8_t)((current >> 24) & 0xff);

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
