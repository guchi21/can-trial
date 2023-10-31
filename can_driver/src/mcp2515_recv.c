#include "mcp2515.h"
#include "mcp2515_register.h"
#include "mcp2515_spicmd.h"
#include "can.h"
#include "pico_can.h"

/*==================================================================*/
/* Define.                                                          */
/*==================================================================*/

#define IDXOF_RX_SPICMD_READ_HDR        ( 0U )
#define IDXOF_RX_SPICMD_READ_BODY       ( 1U )
#define NUMOF_RX_SPICMD_ITEMS           ( 2U )


/*==================================================================*/
/* Type.                                                            */
/*==================================================================*/


/*==================================================================*/
/* Const.                                                           */
/*==================================================================*/

static const uint8_t RX_SPICMD_TBL[ CANDRV_RX_NUMOF_ITEMS ][ NUMOF_RX_SPICMD_ITEMS ] = {
    { SPICMD_READ_RX0_HDR, SPICMD_READ_RX0_BODY },
    { SPICMD_READ_RX1_HDR, SPICMD_READ_RX1_BODY }
};


/*==================================================================*/
/* Variable.                                                        */
/*==================================================================*/


/*==================================================================*/
/* Prototype.                                                       */
/*==================================================================*/


/*==================================================================*/
/* Implement.                                                       */
/*==================================================================*/

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

static uint32_t build_canid( const uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ], const canid_kind_t kind ) {

    return ( kind == CANID_KIND_EXT ) ? build_ext_canid( hdr ) : build_std_canid( hdr );
}

static bool is_invalid_rx_idx( const enum CANDRV_RX rx_idx ) {

    return ( ( CANDRV_RX_MINOF_IDX > rx_idx ) || ( CANDRV_RX_MAXOF_IDX < rx_idx ) ) ? true : false;
}

candrv_result_t mcp2515_get_rx_msg( const enum CANDRV_RX rx_idx, can_msg_t *const msg ) {

    uint8_t hdr[ MCP2515_CANHDR_NUMOF_ITEMS ] = { 0U };
    const uint8_t maskof_irq = ( CANDRV_RX_0 == rx_idx ) ? MASKOF_CANINT_RX0IF : MASKOF_CANINT_RX1IF;

    /* Fails if illegal arguments. */
    if ( is_invalid_rx_idx( rx_idx ) || ( NULL == msg ) )
        return CANDRV_FAILURE;

    /* Read CAN header from register. */
    picocan_begin_spi_commands();
    picocan_write_spi( RX_SPICMD_TBL[ rx_idx ][ IDXOF_RX_SPICMD_READ_HDR ] );
    picocan_read_array_spi( MCP2515_CANHDR_NUMOF_ITEMS, hdr );
    picocan_end_spi_commands();

    /* CAN id kind. */
    msg->id_kind = ( 0U != ( hdr[ MCP2515_CANHDR_SIDL ] & MASKOF_SIDL_IDE ) ) ? CANID_KIND_EXT : CANID_KIND_STD;
    
    /* Build CAN id. */
    msg->id = build_canid( hdr, msg->id_kind );
    
    /* Remote frame or data frame. */
    msg->is_remote = ( 0U != ( hdr[ MCP2515_CANHDR_DLC ] & MASKOF_RTR ) ) ? true : false;
    
    /* CAN DLC. */
    msg->dlc = (uint8_t)( hdr[ MCP2515_CANHDR_DLC ] & MASKOF_DLC );

    if ( CAN_MAXOF_LEN < msg->dlc )
        msg->dlc = CAN_MAXOF_LEN;

    /* CAN data. */
    if ( ( false == msg->is_remote ) && ( CAN_EMPTY < msg->dlc ) ) {

        picocan_begin_spi_commands();
        picocan_write_spi( RX_SPICMD_TBL[ rx_idx ][ IDXOF_RX_SPICMD_READ_BODY ] );
        picocan_read_array_spi( CAN_MAXOF_LEN, msg->data );
        picocan_end_spi_commands();
    }

    /* Clear interruption by received. */
    mcp2515_modbits_register( REG_CANINTF, maskof_irq, 0U );

    return CANDRV_SUCCESS;
}
