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
#define TIMEOUTOF_OPR_MODE_CHANGE   ( 1000UL )

/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/


/*==================================================================*/
/* Prototypes                                                       */
/*==================================================================*/

bool mcp2515_reset_blocking();

#endif /* MCP2515_H */
