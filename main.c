#include "can_driver.h"
#include <stdio.h>

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

#ifndef _HARDWARE_TIMER_H
#include "hardware/timer.h"
#endif

#include "hardware/sync.h"

int32_t call = 0;
bool merr = false;
bool err = false;

void callback( uint gpio, uint32_t event_mask ) {

    uint32_t ints = save_and_disable_interrupts();

    // Begin LED.
    static int b = 1;

    if ( 0 < b ) {

        b = 0;
    }
    else {

        b = 1;
    }

    call++;

    gpio_put(PICO_DEFAULT_LED_PIN, b);
    // End LED.

    if( true == candrv_tmp_is_merr() ) {
        merr = true;
    }

    if( true == candrv_tmp_is_err() ) {
        err = true;
    }

    candrv_tmp_cancel_tx_if_msgerr();

    restore_interrupts(ints);
}

int main() {

    stdio_init_all();
    
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    

    busy_wait_us_32(3 * 1000 * 1000);

    if( false == candrv_init() ) {
        while(1)
        printf("初期化エラー");
    }
    
    // わりこみしょきか MERR, ERR, TX0
    candrv_tmp6();

    // コールバックとうろく
    candrv_register_callback( callback );

    // そうしんめっせ作成
    can_message_t msg;
    uint8_t content[8] = { 0x01, 0x02, 0x03, 0xFF, 0x00, 0x45, 0xEA, 0x55 };

    msg.kind = CAN_FORMAT_KIND_STANDARD;
    msg.id = 0x295;
    msg.length = 8;
    msg.content = content;


    while(1) {

        sleep_ms( 500 );

        printf("b.");
        candrv_tmp4(); // Print status.

        printf("%d回の割り込みよびだしがあったようです\n", call);

        if ( true == merr ) {

            printf("MERRだったようです\n");
            merr = false;
        }

        if ( true == err ) {

            printf("_ERRだったようです\n");
            err = false;
        }

        // メッセージ更新
        const uint32_t current = time_us_32();
        uint8_t i1 = (uint8_t)(current & 0xff);
        uint8_t i2 = (uint8_t)((current >> 8) & 0xff);
        uint8_t i3 = (uint8_t)((current >> 16) & 0xff);
        uint8_t i4 = (uint8_t)((current >> 24) & 0xff);
        msg.content[0] = i1;
        msg.content[1] = i2;
        msg.content[6] = i3;
        msg.content[7] = i4;

        // 送信バッファにせっと
        if( false == candrv_set_tx_msg( CANDRV_TX_0, &msg ) ) {

            printf("seterr.");
        }

        // 送信要求
        // candrv_tmp_clr_exerr();
        printf("そうしんようきゅうをだします\n");
        if ( false == candrv_req_send_msg( CANDRV_TX_0 ) ) {

            printf("reqerr.");
        }

        printf("e.%d\n", current);
    }
}
