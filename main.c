#include "can_driver.h"
#include <stdio.h>

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

#ifndef _HARDWARE_TIMER_H
#include "hardware/timer.h"
#endif

#include "hardware/sync.h"
#include <limits.h>
#include <string.h>

bool is_received = false;
can_message_t recv_msg = { CAN_KIND_STD, 0U, 0U, { 0U } };

void callback( uint gpio, uint32_t event_mask ) {

    candrv_get_rx_msg( CANDRV_RX_0, &recv_msg );
    is_received = true;

    // Begin LED.
    static int b = 1;
    b = ( 0 == b );
    gpio_put(PICO_DEFAULT_LED_PIN, b);
    // End LED.
}

int main() {

    bool is_available_local_msg = false;
    can_message_t local_msg = { CAN_KIND_STD, 0U, 0U, { 0U } };
    uint32_t rt = time_us_32();
    uint32_t prert = rt;

    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    busy_wait_us_32(3 * 1000 * 1000);

    if( false == candrv_init() ) {
        while(1)
            printf("初期化エラー");
    }

    // コールバックとうろく
    candrv_register_callback( callback );

    // そうしんめっせ作成
    can_message_t send_msg;
    uint8_t content[8] = { 0xA1, 0xB2, 0xC3, 0xFF, 0x00, 0x45, 0xEA, 0x55 };
    send_msg.kind = CAN_KIND_STD;
    send_msg.id = 0x295;
    send_msg.length = 8;
    send_msg.content = content;

    while(1) {

        if ( is_received ) {

            /* Begin supress interrupts. */
            uint32_t interrupts = save_and_disable_interrupts();

            local_msg.id = recv_msg.id;
            local_msg.kind = recv_msg.kind;
            local_msg.length = recv_msg.length;
            memcpy( local_msg.content, recv_msg.content, 8 );

            is_received = false;
            candrv_tmp_clr_rx0();

            restore_interrupts(interrupts);
            /* End supress interrupts. */

            is_available_local_msg = true;
        }

        

        if ( is_available_local_msg ) {

            rt = local_msg.content[4] + ( local_msg.content[5] << 8 ) + ( local_msg.content[6] << 16 ) + ( local_msg.content[7] << 24 );

            printf( "\nReceived new message. [%#X] %#X %#X %#X %#X %#X %#X %#X %#X | %d \n", local_msg.id,
                local_msg.content[0], local_msg.content[1], local_msg.content[2], local_msg.content[3],
                local_msg.content[4], local_msg.content[5], local_msg.content[6], local_msg.content[7], rt - prert );

            prert = rt;
            
            is_available_local_msg = false;
        }

        printf(".");
        sleep_ms( 50 );
        uint32_t current = time_us_32();

        // メッセージ更新
        uint8_t i1 = (uint8_t)(current & 0xff);
        uint8_t i2 = (uint8_t)((current >> 8) & 0xff);
        uint8_t i3 = (uint8_t)((current >> 16) & 0xff);
        uint8_t i4 = (uint8_t)((current >> 24) & 0xff);
        send_msg.content[4] = i1;
        send_msg.content[5] = i2;
        send_msg.content[6] = i3;
        send_msg.content[7] = i4;

        // 送信バッファにせっと
        if( false == candrv_set_tx_msg( CANDRV_TX_0, &send_msg ) ) {

            printf("seterr.");
        }

        // 送信要求
        if ( false == candrv_req_send_msg( CANDRV_TX_0 ) ) {

            printf("reqerr.");
        }
    }
}
