#ifndef MCP2515_H
#define MCP2515_H

#include "can_driver.h"

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* MCP2515 operation modes. */
#define OPR_MODE_NORMAL             ( 0x00U )           /* Normal mode.                     */
#define OPR_MODE_SLEEP              ( 0x20U )           /* Sleep mode.                      */
#define OPR_MODE_LOOPBACK           ( 0x40U )           /* Loop back mode.                  */
#define OPR_MODE_LISTENONLY         ( 0x60U )           /* Listen only mode.                */
#define OPR_MODE_CONFIG             ( 0x80U )           /* Config mode.                     */
#define OPR_MODE_INVALID            ( 0xE0U )
#define TIMEOUTOF_OPR_MODE_CHANGE   ( 1000000UL )       /* Timeout of change operation mode in micro seconds. */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/

enum MCP2515_CAN_BAUDRATE {
    MCP2515_CAN_BAUDRATE_MINOF_IDX = 0,
    MCP2515_CAN_BAUDRATE_5KBPS = MCP2515_CAN_BAUDRATE_MINOF_IDX,
    MCP2515_CAN_BAUDRATE_10KBPS,
    MCP2515_CAN_BAUDRATE_20KBPS,
    MCP2515_CAN_BAUDRATE_50KBPS,
    MCP2515_CAN_BAUDRATE_100KBPS,
    MCP2515_CAN_BAUDRATE_125KBPS,
    MCP2515_CAN_BAUDRATE_250KBPS,
    MCP2515_CAN_BAUDRATE_500KBPS,
    MCP2515_CAN_BAUDRATE_800KBPS,
    MCP2515_CAN_BAUDRATE_1000KBPS,
    MCP2515_CAN_BAUDRATE_MAXOF_IDX = MCP2515_CAN_BAUDRATE_1000KBPS,
    MCP2515_CAN_BAUDRATE_NUMOF_ITEMS
};


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

uint8_t mcp2515_read_register( const uint8_t addr );
void mcp2515_write_register( const uint8_t addr, const uint8_t val );
void mcp2515_modbits_register( const uint8_t addr, const uint8_t maskof_write, const uint8_t val );
uint8_t mcp2515_get_opr_mode( void );
candrv_result_t mcp2515_reset( void );
uint8_t mcp2515_get_opr_mode( void );
candrv_result_t mcp2515_set_opr_mode( const uint8_t mode );
candrv_result_t mcp2515_set_baudrate( const enum MCP2515_CAN_BAUDRATE baudrate );
candrv_result_t mcp2515_get_rx_msg( const enum CANDRV_RX rx_idx, can_message_t *const msg );
candrv_result_t mcp2515_set_send_req( const enum CANDRV_TX tx_idx );
candrv_result_t mcp2515_set_tx_msg( const enum CANDRV_TX tx_idx, const can_message_t *const msg, const enum CANDRV_TX_PRIORITY priority );

#endif /* MCP2515_H */
