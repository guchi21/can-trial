/*
 * (c) 2023 guchi21.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef MCP2515_H
#define MCP2515_H

#include "can_driver.h"
#include "pico/stdlib.h"



enum MCP2515_BAUDRATE {
    MCP2515_BAUDRATE_MINOF_IDX = 0,
    MCP2515_BAUDRATE_5KBPS = MCP2515_BAUDRATE_MINOF_IDX,
    MCP2515_BAUDRATE_10KBPS,
    MCP2515_BAUDRATE_20KBPS,
    MCP2515_BAUDRATE_50KBPS,
    MCP2515_BAUDRATE_100KBPS,
    MCP2515_BAUDRATE_125KBPS,
    MCP2515_BAUDRATE_250KBPS,
    MCP2515_BAUDRATE_500KBPS,
    MCP2515_BAUDRATE_800KBPS,
    MCP2515_BAUDRATE_1000KBPS,
    MCP2515_BAUDRATE_MAXOF_IDX = MCP2515_BAUDRATE_1000KBPS,
    MCP2515_BAUDRATE_NUMOF_ITEMS
};

enum MCP2515_OPMODE {
    MCP2515_OPMODE_MINOF_IDX = 0,
    MCP2515_OPMODE_NORMAL = MCP2515_OPMODE_MINOF_IDX,
    MCP2515_OPMODE_SLEEP,
    MCP2515_OPMODE_LOOPBACK,
    MCP2515_OPMODE_LISTENONLY,
    MCP2515_OPMODE_CONFIG,
    MCP2515_OPMODE_MAXOF_IDX = MCP2515_OPMODE_CONFIG,
    MCP2515_OPMODE_NUMOF_ITEMS
};



/*==================================================================*/
/* Prototype.                                                       */
/*==================================================================*/

uint8_t mcp2515_read_register( const uint8_t addr );
void mcp2515_write_register( const uint8_t addr, const uint8_t val );
void mcp2515_modbits_register( const uint8_t addr, const uint8_t maskof_write, const uint8_t val );

cd_result_t mcp2515_reset( void );
cd_result_t mcp2515_set_opmode( const enum MCP2515_OPMODE mode );
cd_result_t mcp2515_set_baudrate( const enum MCP2515_BAUDRATE baudrate );

uint8_t mcp2515_get_numof_tx_err( void );
uint8_t mcp2515_get_numof_rx_err( void );

void mcp2515_cbk_can_controller( uint32_t event_mask );

void mcp2515_clear_all_send_req_if_error( void );

#endif /* MCP2515_H */
