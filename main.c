#include "can_driver.h"
#include "mcp2515.h"
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include <stdio.h>

int main()
{
    const uint16_t id = 0x0390;
    const uint8_t data[8] = { 0x01, 0x02, 0x03, 0xFF, 0x00, 0x45, 0xEA, 0x55 };

    stdio_init_all();

    mcp2515_init();

    mcp2515_set_tx0( id, data );

    while(1) {

        if(true == mcp2515_is_ready_tx0()) {

            // sleep_ms(100);

            mcp2515_int_tx0_clear();
            mcp2515_req_tx0_send();
        }
    }
}
