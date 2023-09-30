#ifndef MCP2515_H
#define MCP2515_H

#ifndef _PICO_STDLIB_H
#include "pico/stdlib.h"
#endif

void mcp2515_init();
void mcp2515_clear_txbuf1();
bool mcp2515_to_normalmode();
bool mcp2515_to_sleepmode();

void mcp2515_int_tx0_enable();
void mcp2515_int_tx0_disable();
void mcp2515_int_tx0_clear();
void mcp2515_req_tx0_send();

void mcp2515_set_tx0( const uint16_t id, const uint8_t data[8] );
bool mcp2515_is_ready_tx0();

#endif