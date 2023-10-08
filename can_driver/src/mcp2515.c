#ifndef TYPES_H
#include "types.h"
#endif

#ifndef MCP2515_H
#include "mcp2515.h"
#endif

#ifndef RP2040_H
#include "rp2040.h"
#endif

#ifndef MCP2515_REG_ADDR_H
#include "mcp2515_reg_addr.h"
#endif

#ifndef MCP2515_SPI_CMD_H
#include "mcp2515_spi_cmd.h"
#endif

#ifndef _HARDWARE_SPI_H
#include "hardware/spi.h"
#endif

#ifndef _HARDWARE_SYNC_H
#include "hardware/sync.h"
#endif

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* RP2040 pin voltage. */
#define PIN_VOLTAGE_LOW             ( 0U )
#define PIN_VOLTAGE_HIGH            ( 1U )


/* RP2040 SPI clock. */
#define SPI_BAUDRATE                ( 1000000U )        /* SPI baudrate in Hz. */


/* RP2040 GPIO numbers. */
#define GPIO_MCP2515_MISO           (  4U )             /* RP2040 pin #06.  */
#define GPIO_MCP2515_CS             (  5U )             /* RP2040 pin #07.  */
#define GPIO_MCP2515_SCK            (  6U )             /* RP2040 pin #09.  */
#define GPIO_MCP2515_MOSI           (  7U )             /* RP2040 pin #10.  */
#define GPIO_MCP2515_INT1           ( 21U )             /* RP2040 pin #27.  */


/* Related CAN id definitions */
#define STANDARD_CAN_ID_MAX         ( 0x7FFULL )        /* Maximum value of the standard CAN ID.    */
#define EXTENSION_CAN_ID_MAX        ( 0x1FFFFFFFULL )   /* Maximum value of the extension CAN ID.   */


/* Related CAN content length definitions */
#define CAN_CONTENT_LEN_MIN         ( 0U )              /* Minimum length of the CAN message data.  */
#define CAN_CONTENT_LEN_MAX         ( 8U )              /* Maximum length of the CAN message data.  */


/*==================================================================*/
/* Type definitions                                                 */
/*==================================================================*/

typedef struct {
    uint8_t c1;
    uint8_t c2;
    uint8_t c3;
} can_baudrate_reg_t;


typedef struct{
    uint8_t sidh;
    uint8_t sidl;
    uint8_t eid8;
    uint8_t eid0;
} can_id_reg_t;


/*==================================================================*/
/* Const definitions                                                */
/*==================================================================*/
const can_baudrate_reg_t can_baudrate_5kbps    = { 0xA7U, 0XBFU, 0x07U };
const can_baudrate_reg_t can_baudrate_10kbps   = { 0x31U, 0XA4U, 0X04U };
const can_baudrate_reg_t can_baudrate_20kbps   = { 0x18U, 0XA4U, 0x04U };
const can_baudrate_reg_t can_baudrate_50kbps   = { 0x09U, 0XA4U, 0x04U };
const can_baudrate_reg_t can_baudrate_100kbps  = { 0x04U, 0x9EU, 0x03U };
const can_baudrate_reg_t can_baudrate_125kbps  = { 0x03U, 0x9EU, 0x03U };
const can_baudrate_reg_t can_baudrate_250kbps  = { 0x01U, 0x1EU, 0x03U };
const can_baudrate_reg_t can_baudrate_500kbps  = { 0x00U, 0x9EU, 0x03U };
const can_baudrate_reg_t can_baudrate_800kbps  = { 0x00U, 0x92U, 0x02U };
const can_baudrate_reg_t can_baudrate_1000kbps = { 0x00U, 0x82U, 0x02U };

/*==================================================================*/
/* Implements.                                                      */
/*==================================================================*/

