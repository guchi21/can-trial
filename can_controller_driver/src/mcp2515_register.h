/*
 * (c) 2023 guchi21.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef MCP2515_REGISTER_H
#define MCP2515_REGISTER_H

/*!
 * \file mcp2515_register.h
 * \brief Definitions related the MCP2515 register.
 *
 * \author guchi21
 * \date 2023-11-06
 */

/*! 
 * \defgroup Mcp2515RegAddr MCP2515 register address
 */
/*! \{ */
#define REG_RXF0SIDH                    ( 0x00U )
#define REG_RXF0SIDL                    ( 0x01U )
#define REG_RXF0EID8                    ( 0x02U )
#define REG_RXF0EID0                    ( 0x03U )
#define REG_RXF1SIDH                    ( 0x04U )
#define REG_RXF1SIDL                    ( 0x05U )
#define REG_RXF1EID8                    ( 0x06U )
#define REG_RXF1EID0                    ( 0x07U )
#define REG_RXF2SIDH                    ( 0x08U )
#define REG_RXF2SIDL                    ( 0x09U )
#define REG_RXF2EID8                    ( 0x0AU )
#define REG_RXF2EID0                    ( 0x0BU )

/*!
 * @brief BFPCTRL
 *
 * bit 7 : reserved
 * bit 6 : reserved
 * bit 5 : B1BFS: RX1BF Pin State bit (Digital Output mode only).
 *                Reads as ‘0’ when RX1BF is configured as interrupt pin.
 * bit 4 : B0BFS: RX0BF Pin State bit (Digital Output mode only).
 *                Reads as ‘0’ when RX0BF is configured as interrupt pin.
 * bit 3 : B1BFE: RX1BF Pin Function Enable bit.
 *                1 = Pin function enabled, operation mode determined by B1BFM bit.
 *                0 = Pin function disabled, pin goes to high-impedance state.
 * bit 2 : B0BFE: RX0BF Pin Function Enable bit.
 *                1 = Pin function enabled, operation mode determined by B0BFM bit.
 *                0 = Pin function disabled, pin goes to high-impedance state.
 * bit 1 : B1BFM: RX1BF Pin Operation Mode bit.
 *                1 = Pin is used as interrupt when valid message loaded into RXB1.
 *                0 = Digital Output mode.
 * bit 0 : B0BFM: RX0BF Pin Operation Mode bit.
 *                1 = Pin is used as interrupt when valid message loaded into RXB0.
 *                0 = Digital Output mode.
 * 
 */
#define REG_BFPCTRL ( 0x0CU )   /*!< RXnBF PIN CONTROL AND STATUS */

#define REG_TXRTSCTRL                   ( 0x0DU )
#define REG_CANSTAT                     ( 0x0EU )               /*!< CAN status     */
#define REG_CANCTRL                     ( 0x0FU )               /*!< CAN control    */
#define REG_RXF3SIDH                    ( 0x10U )
#define REG_RXF3SIDL                    ( 0x11U )
#define REG_RXF3EID8                    ( 0x12U )
#define REG_RXF3EID0                    ( 0x13U )
#define REG_RXF4SIDH                    ( 0x14U )
#define REG_RXF4SIDL                    ( 0x15U )
#define REG_RXF4EID8                    ( 0x16U )
#define REG_RXF4EID0                    ( 0x17U )
#define REG_RXF5SIDH                    ( 0x18U )
#define REG_RXF5SIDL                    ( 0x19U )
#define REG_RXF5EID8                    ( 0x1AU )
#define REG_RXF5EID0                    ( 0x1BU )
#define REG_TEC                         ( 0x1CU )
#define REG_REC                         ( 0x1DU )
#define REG_RXM0SIDH                    ( 0x20U )
#define REG_RXM0SIDL                    ( 0x21U )
#define REG_RXM0EID8                    ( 0x22U )
#define REG_RXM0EID0                    ( 0x23U )
#define REG_RXM1SIDH                    ( 0x24U )
#define REG_RXM1SIDL                    ( 0x25U )
#define REG_RXM1EID8                    ( 0x26U )
#define REG_RXM1EID0                    ( 0x27U )
#define REG_CNF3                        ( 0x28U )               /*!< CAN bus interface bit timing #3 */
#define REG_CNF2                        ( 0x29U )               /*!< CAN bus interface bit timing #2 */
#define REG_CNF1                        ( 0x2AU )               /*!< CAN bus interface bit timing #1 */
#define REG_CANINTE                     ( 0x2BU )               /*!< CAN interrupt enables           */
#define REG_CANINTF                     ( 0x2CU )               /*!< CAN interrupt flags             */
#define REG_EFLG                        ( 0x2DU )
#define REG_TXB0CTRL                    ( 0x30U )
#define REG_TXB0SIDH                    ( 0x31U )
#define REG_TXB0SIDL                    ( 0x32U )
#define REG_TXB0EID8                    ( 0x33U )
#define REG_TXB0EID0                    ( 0x34U )
#define REG_TXB0DLC                     ( 0x35U )
#define REG_TXB0D0                      ( 0x36U )
#define REG_TXB0D1                      ( 0x37U )
#define REG_TXB0D2                      ( 0x38U )
#define REG_TXB0D3                      ( 0x39U )
#define REG_TXB0D4                      ( 0x3AU )
#define REG_TXB0D5                      ( 0x3BU )
#define REG_TXB0D6                      ( 0x3CU )
#define REG_TXB0D7                      ( 0x3DU )
#define REG_TXB1CTRL                    ( 0x40U )
#define REG_TXB1SIDH                    ( 0x41U )
#define REG_TXB1SIDL                    ( 0x42U )
#define REG_TXB1EID8                    ( 0x43U )
#define REG_TXB1EID0                    ( 0x44U )
#define REG_TXB1DLC                     ( 0x45U )
#define REG_TXB1D0                      ( 0x46U )
#define REG_TXB1D1                      ( 0x47U )
#define REG_TXB1D2                      ( 0x48U )
#define REG_TXB1D3                      ( 0x49U )
#define REG_TXB1D4                      ( 0x4AU )
#define REG_TXB1D5                      ( 0x4BU )
#define REG_TXB1D6                      ( 0x4CU )
#define REG_TXB1D7                      ( 0x4DU )
#define REG_TXB2CTRL                    ( 0x50U )
#define REG_TXB2SIDH                    ( 0x51U )
#define REG_TXB2SIDL                    ( 0x52U )
#define REG_TXB2EID8                    ( 0x53U )
#define REG_TXB2EID0                    ( 0x54U )
#define REG_TXB2DLC                     ( 0x55U )
#define REG_TXB2D0                      ( 0x56U )
#define REG_TXB2D1                      ( 0x57U )
#define REG_TXB2D2                      ( 0x58U )
#define REG_TXB2D3                      ( 0x59U )
#define REG_TXB2D4                      ( 0x5AU )
#define REG_TXB2D5                      ( 0x5BU )
#define REG_TXB2D6                      ( 0x5CU )
#define REG_TXB2D7                      ( 0x5DU )
#define REG_RXB0CTRL                    ( 0x60U )
#define REG_RXB0SIDH                    ( 0x61U )
#define REG_RXB0SIDL                    ( 0x62U )
#define REG_RXB0EID8                    ( 0x63U )
#define REG_RXB0EID0                    ( 0x64U )
#define REG_RXB0DLC                     ( 0x65U )
#define REG_RXB0D0                      ( 0x66U )
#define REG_RXB0D1                      ( 0x67U )
#define REG_RXB0D2                      ( 0x68U )
#define REG_RXB0D3                      ( 0x69U )
#define REG_RXB0D4                      ( 0x6AU )
#define REG_RXB0D5                      ( 0x6BU )
#define REG_RXB0D6                      ( 0x6CU )
#define REG_RXB0D7                      ( 0x6DU )
#define REG_RXB1CTRL                    ( 0x70U )
#define REG_RXB1SIDH                    ( 0x71U )
#define REG_RXB1SIDL                    ( 0x72U )
#define REG_RXB1EID8                    ( 0x73U )
#define REG_RXB1EID0                    ( 0x74U )
#define REG_RXB1DLC                     ( 0x75U )
#define REG_RXB1D0                      ( 0x76U )
#define REG_RXB1D1                      ( 0x77U )
#define REG_RXB1D2                      ( 0x78U )
#define REG_RXB1D3                      ( 0x79U )
#define REG_RXB1D4                      ( 0x7AU )
#define REG_RXB1D5                      ( 0x7BU )
#define REG_RXB1D6                      ( 0x7CU )
#define REG_RXB1D7                      ( 0x7DU )
/*! \} */

/*! 
 * \defgroup Mcp2515RegMask MCP2515 register mask 
 */
/*! \{ */

#define MASKOF_OPMOD                    ( 0xE0U )               /*!< Operation mode      */
#define MASKOF_CANSTAT_ICOD             ( 0x0EU )               /*!< Inturrupt kind      */

#define MASKOF_CANCTRL_ABAT             ( 0x10U )               /*!< Abort send CAN message     */
#define MASKOF_CANCTRL_OSM              ( 0x08U )               /*!< One shot send CAN message  */

#define MASKOF_CANINT_MERRF             ( 0x80U )
#define MASKOF_CANINT_WAKIF             ( 0x40U )
#define MASKOF_CANINT_ERRIF             ( 0x20U )
#define MASKOF_CANINT_TX2IF             ( 0x10U )
#define MASKOF_CANINT_TX1IF             ( 0x08U )
#define MASKOF_CANINT_TX0IF             ( 0x04U )
#define MASKOF_CANINT_RX1IF             ( 0x02U )
#define MASKOF_CANINT_RX0IF             ( 0x01U )

#define MASKOF_EFLG_RX1OVR              ( 0x80U )
#define MASKOF_EFLG_RX0OVR              ( 0x40U )
#define MASKOF_EFLG_TXBO                ( 0x20U )
#define MASKOF_EFLG_TXEP                ( 0x10U )
#define MASKOF_EFLG_RXEP                ( 0x08U )
#define MASKOF_EFLG_TXWAR               ( 0x04U )
#define MASKOF_EFLG_RXWAR               ( 0x02U )
#define MASKOF_EFLG_EWARN               ( 0x01U )

#define MASKOF_TXBCTRL_ABTF             ( 0x40U )
#define MASKOF_TXBCTRL_MLOA             ( 0x20U )
#define MASKOF_TXBCTRL_TXERR            ( 0x10U )
#define MASKOF_TXBCTRL_TXREQ            ( 0x08U )
#define MASKOF_TXBCTRL_TXP              ( 0x03U )

#define MASKOF_RXBCTRL_RXM              ( 0x60U )
#define MASKOF_RXBCTRL_RXRTR            ( 0x08U )
#define MASKOF_RXB0CTRL_BUKT            ( 0x04U )
#define MASKOF_RXB0CTRL_FILHIT          ( 0x01U )
#define MASKOF_RXB1CTRL_FILHIT          ( 0x07U )

#define MASKOF_SIDL_IDE                 ( 0x08U )
#define MASKOF_RTR                      ( 0x40U )
#define MASKOF_DLC                      ( 0x0FU )
/*! \} */


/*==================================================================*/
/* Type.                                                            */
/*==================================================================*/

/*--------------------------------------*/
/* Index of CAN header on the register. */
/*--------------------------------------*/
/*! 
 * \enum MCP2515_CANHDR
 * \brief Buffer index of CAN id and DLC.
 */
enum MCP2515_CANHDR {
    MCP2515_CANHDR_SIDH = 0,    /*! CAN id */
    MCP2515_CANHDR_SIDL = 1,    /*! CAN id */
    MCP2515_CANHDR_EID8 = 2,    /*! CAN id */
    MCP2515_CANHDR_EID0 = 3,    /*! CAN id */
    MCP2515_CANHDR_DLC = 4,     /*! DLC */
    MCP2515_CANHDR_NUMOF_ITEMS = 5,
    MCP2515_CANHDR_MINOF_IDX = MCP2515_CANHDR_SIDH,
    MCP2515_CANHDR_MAXOF_IDX = MCP2515_CANHDR_DLC
};


#endif /* MCP2515_REGISTER_H */
