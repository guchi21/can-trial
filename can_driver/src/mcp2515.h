#ifndef MCP2515_H
#define MCP2515_H

#ifndef CAN_DRIVER_H
#include "can_driver.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* MCP2515 TX send priority */
#define TX_PRIORITY_LOW             ( 0x00U )
#define TX_PRIORITY_MIDDLE_LOW      ( 0x01U )
#define TX_PRIORITY_MIDDLE_HIGH     ( 0x02U )
#define TX_PRIORITY_HIGH            ( 0x03U )


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
    MCP2515_CAN_BAUDRATE_MINOF_IDX = 0U,
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
candrv_result_t mcp2515_reset_blocking( void );
uint8_t mcp2515_get_opr_mode( void );
candrv_result_t mcp2515_change_opr_mode_blocking( const uint8_t mode );
candrv_result_t mcp2515_change_can_baudrate( const enum MCP2515_CAN_BAUDRATE baudrate );
candrv_result_t mcp2515_change_tx_priority( const enum CANDRV_TX tx_idx, const uint8_t priority );

#endif /* MCP2515_H */
