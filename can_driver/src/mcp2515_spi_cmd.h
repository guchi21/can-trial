#ifndef MCP2515_SPI_CMD_H
#define MCP2515_SPI_CMD_H

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

#define SPICMD_WRITE_REG            ( 0x02U )           /* Write bytes to register.           */
#define SPICMD_READ_REG             ( 0x03U )           /* Read bytes from register.          */
#define SPICMD_MODBITS_REG          ( 0x05U )           /* Modify register bits in a byte.    */
#define SPICMD_WRITE_TX0ID          ( 0x40U )           /* Write bytes from TX0 CAN id.       */
#define SPICMD_WRITE_TX0DATA        ( 0x41U )           /* Write bytes from TX0 CAN content.  */
#define SPICMD_WRITE_TX1ID          ( 0x42U )           /* Write bytes from TX1 CAN id.       */
#define SPICMD_WRITE_TX1DATA        ( 0x43U )           /* Write bytes from TX1 CAN content.  */
#define SPICMD_WRITE_TX2ID          ( 0x44U )           /* Write bytes from TX2 CAN id.       */
#define SPICMD_WRITE_TX2DATA        ( 0x45U )           /* Write bytes from TX2 CAN content.  */
#define SPICMD_REQ_TX0              ( 0x81U )           /* Request send CAN message from TX0. */
#define SPICMD_REQ_TX1              ( 0x82U )           /* Request send CAN message from TX1. */
#define SPICMD_REQ_TX2              ( 0x83U )           /* Request send CAN message from TX2. */
#define SPICMD_READ_RX0ID           ( 0x90U )           /* Read bytes from RX0 CAN id.        */
#define SPICMD_READ_RX0DATA         ( 0x92U )           /* Read bytes from RX0 CAN content.   */
#define SPICMD_READ_RX1ID           ( 0x94U )           /* Read bytes from RX1 CAN id.        */
#define SPICMD_READ_RX1DATA         ( 0x96U )           /* Read bytes from RX1 CAN content.   */
#define SPICMD_READ_STAT            ( 0xA0U )           /* Read status of the MCP2515.        */
#define SPICMD_READ_RXSTAT          ( 0xB0U )           /* Read RX status.                    */
#define SPICMD_RESET                ( 0xC0U )           /* Reset the MCP2515.                 */


#endif /* MCP2515_SPI_CMD_H */
