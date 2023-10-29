#ifndef MCP2515_H
#define MCP2515_H

#include "can_driver.h"

/*==================================================================*/
/* Define.                                                          */
/*==================================================================*/

/* MCP2515 operation modes. */
#define OPMODE_INVALID            ( 0xE0U )
#define TIMEOUTOF_OPMODE_CHANGE   ( 1000000UL )       /* Timeout of change operation mode in micro seconds. */


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

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
candrv_result_t mcp2515_reset( void );
candrv_result_t mcp2515_set_opmode( const enum MCP2515_OPMODE mode );
candrv_result_t mcp2515_set_baudrate( const enum MCP2515_BAUDRATE baudrate );
candrv_result_t mcp2515_clr_send_req( const enum CANDRV_TX tx_idx );
candrv_result_t mcp2515_get_rx_msg( const enum CANDRV_RX rx_idx, can_msg_t *const msg );
candrv_result_t mcp2515_set_tx_msg( const enum CANDRV_TX tx_idx,
    const can_msg_t *const msg, const enum CANDRV_TX_PRIORITY priority );
candrv_result_t mcp2515_set_send_req( const enum CANDRV_TX tx_idx );

void mcp2515_clr_all_send_req_if_err( void ); // todo:trial
candrv_result_t mcp2515_is_tx_available( const enum CANDRV_TX tx_idx ); // todo:trial

#endif /* MCP2515_H */
