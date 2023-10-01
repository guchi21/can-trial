#include "hardware/spi.h"
#include "pico/stdlib.h"

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/

/* GPIO number definitions */
#define GPIONUM_SPI_RX              ( (uint) 4U )           // RP2040 pin #06.
#define GPIONUM_SPI_CS              ( (uint) 5U )           // RP2040 pin #07.
#define GPIONUM_SPI_SCK             ( (uint) 6U )           // RP2040 pin #09.
#define GPIONUM_SPI_TX              ( (uint) 7U )           // RP2040 pin #10.
#define GPIONUM_CAN_INT             ( (uint) 21U )          // RP2040 pin #27.

/* Pin voltage definitions */
#define PIN_LOW                     ( (bool) 0U )           // RP2040 pin low voltage.
#define PIN_HIGH                    ( (bool) 1U )           // RP2040 pin high voltage.

/* SPI clock */
#define SPI_CLOCK                   ( (uint) 1000000U )     // SPI baudrate definition in Hz.

/* SPI commands */
#define SPICMD_WRITE_REG            ( (uint8_t) 0x02U )     // Write to register at MCP2510.
#define SPICMD_READ_REG             ( (uint8_t) 0x03U )     // Read from register at MCP2510.
#define SPICMD_MODBITS_REG          ( (uint8_t) 0x05U )     // Modify register bits at MCP2510.
#define SPICMD_WRITE_TX0ID          ( (uint8_t) 0x40U )     // Write CAN ID to TX0 on MCP2510.
#define SPICMD_WRITE_TX0DATA        ( (uint8_t) 0x41U )     // Write data to TX0 on MCP2510.
#define SPICMD_WRITE_TX1ID          ( (uint8_t) 0x42U )     // Write CAN ID to TX1 on MCP2510.
#define SPICMD_WRITE_TX1DATA        ( (uint8_t) 0x43U )     // Write data to TX1 on MCP2510.
#define SPICMD_WRITE_TX2ID          ( (uint8_t) 0x44U )     // Write CAN ID to TX2 on MCP2510.
#define SPICMD_WRITE_TX2DATA        ( (uint8_t) 0x45U )     // Write data to TX2 on MCP2510.
#define SPICMD_REQ_TX0              ( (uint8_t) 0x81U )     // Request send CAN message from TX0 on MCP2510.
#define SPICMD_REQ_TX1              ( (uint8_t) 0x82U )     // Request send CAN message from TX1 on MCP2510.
#define SPICMD_REQ_TX2              ( (uint8_t) 0x83U )     // Request send CAN message from TX2 on MCP2510.
#define SPICMD_READ_RX0ID           ( (uint8_t) 0x90U )     // Read CAN ID from RX0 on MCP2510.
#define SPICMD_READ_RX0DATA         ( (uint8_t) 0x92U )     // Read data from RX0 on MCP2510.
#define SPICMD_READ_RX1ID           ( (uint8_t) 0x94U )     // Read CAN ID from RX1 on MCP2510.
#define SPICMD_READ_RX1DATA         ( (uint8_t) 0x96U )     // Read data from RX1 on MCP2510.
#define SPICMD_READ_STAT            ( (uint8_t) 0xA0U )     // Read status at MCP2510.
#define SPICMD_READ_RXSTAT          ( (uint8_t) 0xB0U )     // Read RX status at MCP2510.
#define SPICMD_RESET                ( (uint8_t) 0xC0U )     // Reset the MCP2510.

/* MCP2515 register write timeout */
#define MCP2515_TIMEOUT_WRITE_REG   ( (uint64_t) 1000ULL )  //  Timeout of write to register in microseconds on MCP2515.

/* MCP2515 operation modes */
#define MCP2515_MODE_NORMAL     ( (uint8_t) 0x00U )     // Normal mode.
#define MCP2515_MODE_SLEEP      ( (uint8_t) 0x20U )     // Sleep mode.
#define MCP2515_MODE_LOOPBACK   ( (uint8_t) 0x40U )     // Loop back mode.
#define MCP2515_MODE_LISTENONLY ( (uint8_t) 0x60U )     // Listen only mode.
#define MCP2515_MODE_CONFIG     ( (uint8_t) 0x80U )     // Config mode.

static void begin_spicmd() 
{
    gpio_put( GPIONUM_SPI_CS, PIN_LOW );
}

static void end_spicmd()
{
    gpio_put( GPIONUM_SPI_CS, PIN_HIGH );
}
