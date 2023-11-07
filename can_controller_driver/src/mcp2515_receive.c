#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "can_driver.h"
#include "hardware_manager.h"

/*! 
 * \defgroup RxSpiCmdIdx Index within SPI command table for CAN receiver
 */
/*! \{ */
#define IDXOF_RX_SPICMD_READ_HDR        ( 0U )          /*!< Read CAN id and DLC to register    */
#define IDXOF_RX_SPICMD_READ_BODY       ( 1U )          /*!< Read CAN data                      */
#define NUMOF_RX_SPICMD_ITEMS           ( 2U )
/*! \} */

static const uint8_t RX_SPICMD_TBL[ CD_RX_NUMOF_ITEMS ][ NUMOF_RX_SPICMD_ITEMS ] = {
    { SPICMD_READ_RX0_HDR, SPICMD_READ_RX0_BODY },
    { SPICMD_READ_RX1_HDR, SPICMD_READ_RX1_BODY }
};

static uint32_t build_ext_canid( const uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    return (uint32_t)(
        (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_SIDH ] << 21U ) & 0x1FE00000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_SIDL ] << 13U ) & 0x001C0000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_SIDL ] << 16U ) & 0x00030000UL )
      | (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_EID8 ] <<  8U ) & 0x0000FF00UL )
      | (uint32_t)(             (uint32_t)hdr[ MCP2515_CANHDR_EID0 ]          & 0x000000FFUL )
    );
}

static uint32_t build_std_canid( const uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] ) {

    return (uint32_t)(
        (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_SIDH ] << 3U ) & 0x000007F8UL )
      | (uint32_t)( (uint32_t)( (uint32_t)hdr[ MCP2515_CANHDR_SIDL ] >> 5U ) & 0x00000007UL )
    );
}

static uint32_t build_canid( const uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ], const enum CD_CANID_KIND kind ) {

    return ( kind == CD_CANID_KIND_EXT ) ? build_ext_canid( hdr ) : build_std_canid( hdr );
}

static bool is_invalid_rx_idx( const enum CD_RX rx_idx ) {

    return ( ( CD_RX_MINOF_IDX > rx_idx ) || ( CD_RX_MAXOF_IDX < rx_idx ) ) ? true : false;
}

cd_result_t cd_get_message( const enum CD_RX rx_idx, struct cd_can_message *const msg ) {

    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ];
    uint8_t maskof_irq;

    /* Fails if illegal arguments. */
    if ( is_invalid_rx_idx( rx_idx ) || ( NULL == msg ) )
        return CD_FAILURE;

    /* Read CAN header from register. */
    picocan_begin_spi_commands();
    picocan_write_spi( RX_SPICMD_TBL[ rx_idx ][ IDXOF_RX_SPICMD_READ_HDR ] );
    picocan_read_array_spi( MCP2515_CANHDR_NUMOF_ITEMS, hdr );
    picocan_end_spi_commands();

    /* CAN id kind. */
    msg->id_kind = ( 0U != ( hdr[ MCP2515_CANHDR_SIDL ] & MASKOF_SIDL_IDE ) )
        ? CD_CANID_KIND_EXT : CD_CANID_KIND_STD;
    
    /* Build CAN id. */
    msg->id = build_canid( hdr, msg->id_kind );
    
    /* Remote frame or data frame. */
    msg->is_remote = ( 0U != ( hdr[ MCP2515_CANHDR_DLC ] & MASKOF_RTR ) ) ? true : false;
    
    /* CAN DLC. */
    msg->dlc = (uint8_t)( hdr[ MCP2515_CANHDR_DLC ] & MASKOF_DLC );

    if ( CD_CANDLC_MAXOF_LEN < msg->dlc )
        msg->dlc = CD_CANDLC_MAXOF_LEN;

    /* CAN data. */
    picocan_begin_spi_commands();
    picocan_write_spi( RX_SPICMD_TBL[ rx_idx ][ IDXOF_RX_SPICMD_READ_BODY ] );
    picocan_read_array_spi( CD_CANDLC_MAXOF_LEN, msg->data );
    picocan_end_spi_commands();

    maskof_irq = ( CD_RX_0 == rx_idx ) ? MASKOF_CANINT_RX0IF : MASKOF_CANINT_RX1IF;

    /* Clear interruption by received. */
    mcp2515_modbits_register( REG_CANINTF, maskof_irq, 0U );

    return CD_SUCCESS;
}
