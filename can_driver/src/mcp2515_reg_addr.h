#ifndef MCP2515_REG_ADDR_H
#define MCP2515_REG_ADDR_H

/*==================================================================*/
/* Macro definitions                                                */
/*==================================================================*/


/*----------------------*/
/* Register addresses   */
/*----------------------*/
#define REG_RXF0SIDH        ( 0x00U )
#define REG_RXF0SIDL        ( 0x01U )
#define REG_RXF0EID8        ( 0x02U )
#define REG_RXF0EID0        ( 0x03U )
#define REG_RXF1SIDH        ( 0x04U )
#define REG_RXF1SIDL        ( 0x05U )
#define REG_RXF1EID8        ( 0x06U )
#define REG_RXF1EID0        ( 0x07U )
#define REG_RXF2SIDH        ( 0x08U )
#define REG_RXF2SIDL        ( 0x09U )
#define REG_RXF2EID8        ( 0x0AU )
#define REG_RXF2EID0        ( 0x0BU )
#define REG_BFPCTRL_FLGS    ( 0x0CU )           /* RXnBF PIN CONTROL AND STATUS */
    /* bit 7 : reserved */
    /* bit 6 : reserved */
    /* bit 5 : B1BFS: RX1BF Pin State bit (Digital Output mode only).                   */
    /*                Reads as ‘0’ when RX1BF is configured as interrupt pin.            */
    /* bit 4 : B0BFS: RX0BF Pin State bit (Digital Output mode only).                   */
    /*                Reads as ‘0’ when RX0BF is configured as interrupt pin.            */
    /* bit 3 : B1BFE: RX1BF Pin Function Enable bit.                                    */
    /*                1 = Pin function enabled, operation mode determined by B1BFM bit. */
    /*                0 = Pin function disabled, pin goes to high-impedance state.      */
    /* bit 2 : B0BFE: RX0BF Pin Function Enable bit.                                    */
    /*                1 = Pin function enabled, operation mode determined by B0BFM bit. */
    /*                0 = Pin function disabled, pin goes to high-impedance state.      */
    /* bit 1 : B1BFM: RX1BF Pin Operation Mode bit.                                     */
    /*                1 = Pin is used as interrupt when valid message loaded into RXB1. */
    /*                0 = Digital Output mode.                                          */
    /* bit 0 : B0BFM: RX0BF Pin Operation Mode bit.                                     */
    /*                1 = Pin is used as interrupt when valid message loaded into RXB0. */
    /*                0 = Digital Output mode.                                          */

#define REG_TXRTSCTRL_FLGS  ( 0x0DU )
#define REG_CANSTAT         ( 0x0EU )           /* CAN status.                      */
#define REG_CANCTRL_FLGS    ( 0x0FU )           /* CAN control.                     */
#define REG_RXF3SIDH        ( 0x10U )
#define REG_RXF3SIDL        ( 0x11U )
#define REG_RXF3EID8        ( 0x12U )
#define REG_RXF3EID0        ( 0x13U )
#define REG_RXF4SIDH        ( 0x14U )
#define REG_RXF4SIDL        ( 0x15U )
#define REG_RXF4EID8        ( 0x16U )
#define REG_RXF4EID0        ( 0x17U )
#define REG_RXF5SIDH        ( 0x18U )
#define REG_RXF5SIDL        ( 0x19U )
#define REG_RXF5EID8        ( 0x1AU )
#define REG_RXF5EID0        ( 0x1BU )
#define REG_TEC             ( 0x1CU )
#define REG_REC             ( 0x1DU )
#define REG_RXM0SIDH        ( 0x20U )
#define REG_RXM0SIDL        ( 0x21U )
#define REG_RXM0EID8        ( 0x22U )
#define REG_RXM0EID0        ( 0x23U )
#define REG_RXM1SIDH        ( 0x24U )
#define REG_RXM1SIDL        ( 0x25U )
#define REG_RXM1EID8        ( 0x26U )
#define REG_RXM1EID0        ( 0x27U )
#define REG_CNF3_FLGS       ( 0x28U )           /* CAN bus interface bit timing #3. */
#define REG_CNF2_FLGS       ( 0x29U )           /* CAN bus interface bit timing #2. */
#define REG_CNF1_FLGS       ( 0x2AU )           /* CAN bus interface bit timing #1. */
#define REG_CANINTE_FLGS    ( 0x2BU )           /* CAN interrupt enables.           */
#define REG_CANINTF_FLGS    ( 0x2CU )           /* CAN interrupt flags.             */
#define REG_EFLG_FLGS       ( 0x2DU )
#define REG_TXB0CTRL_FLGS   ( 0x30U )
#define REG_TXB0SIDH        ( 0x31U )
#define REG_TXB0SIDL        ( 0x32U )
#define REG_TXB0EID8        ( 0x33U )
#define REG_TXB0EID0        ( 0x34U )
#define REG_TXB0DLC         ( 0x35U )
#define REG_TXB0D0          ( 0x36U )
#define REG_TXB0D1          ( 0x37U )
#define REG_TXB0D2          ( 0x38U )
#define REG_TXB0D3          ( 0x39U )
#define REG_TXB0D4          ( 0x3AU )
#define REG_TXB0D5          ( 0x3BU )
#define REG_TXB0D6          ( 0x3CU )
#define REG_TXB0D7          ( 0x3DU )
#define REG_TXB1CTRL_FLGS   ( 0x40U )
#define REG_TXB1SIDH        ( 0x41U )
#define REG_TXB1SIDL        ( 0x42U )
#define REG_TXB1EID8        ( 0x43U )
#define REG_TXB1EID0        ( 0x44U )
#define REG_TXB1DLC         ( 0x45U )
#define REG_TXB1D0          ( 0x46U )
#define REG_TXB1D1          ( 0x47U )
#define REG_TXB1D2          ( 0x48U )
#define REG_TXB1D3          ( 0x49U )
#define REG_TXB1D4          ( 0x4AU )
#define REG_TXB1D5          ( 0x4BU )
#define REG_TXB1D6          ( 0x4CU )
#define REG_TXB1D7          ( 0x4DU )
#define REG_TXB2CTRL_FLGS   ( 0x50U )
#define REG_TXB2SIDH        ( 0x51U )
#define REG_TXB2SIDL        ( 0x52U )
#define REG_TXB2EID8        ( 0x53U )
#define REG_TXB2EID0        ( 0x54U )
#define REG_TXB2DLC         ( 0x55U )
#define REG_TXB2D0          ( 0x56U )
#define REG_TXB2D1          ( 0x57U )
#define REG_TXB2D2          ( 0x58U )
#define REG_TXB2D3          ( 0x59U )
#define REG_TXB2D4          ( 0x5AU )
#define REG_TXB2D5          ( 0x5BU )
#define REG_TXB2D6          ( 0x5CU )
#define REG_TXB2D7          ( 0x5DU )
#define REG_RXB0CTRL_FLGS   ( 0x60U )
#define REG_RXB0SIDH        ( 0x61U )
#define REG_RXB0SIDL        ( 0x62U )
#define REG_RXB0EID8        ( 0x63U )
#define REG_RXB0EID0        ( 0x64U )
#define REG_RXB0DLC         ( 0x65U )
#define REG_RXB0D0          ( 0x66U )
#define REG_RXB0D1          ( 0x67U )
#define REG_RXB0D2          ( 0x68U )
#define REG_RXB0D3          ( 0x69U )
#define REG_RXB0D4          ( 0x6AU )
#define REG_RXB0D5          ( 0x6BU )
#define REG_RXB0D6          ( 0x6CU )
#define REG_RXB0D7          ( 0x6DU )
#define REG_RXB1CTRL_FLGS   ( 0x70U )
#define REG_RXB1SIDH        ( 0x71U )
#define REG_RXB1SIDL        ( 0x72U )
#define REG_RXB1EID8        ( 0x73U )
#define REG_RXB1EID0        ( 0x74U )
#define REG_RXB1DLC         ( 0x75U )
#define REG_RXB1D0          ( 0x76U )
#define REG_RXB1D1          ( 0x77U )
#define REG_RXB1D2          ( 0x78U )
#define REG_RXB1D3          ( 0x79U )
#define REG_RXB1D4          ( 0x7AU )
#define REG_RXB1D5          ( 0x7BU )
#define REG_RXB1D6          ( 0x7CU )
#define REG_RXB1D7          ( 0x7DU )

/*--------------------------*/
/* Mask of register value   */
/*--------------------------*/
#define MASKOF_CANSTAT_OPMOD    ( 0xE0U )                   /* Operation status.    */
#define MASKOF_CANSTAT_ICOD     ( 0x0EU )                   /* Inturrupt kind.      */

#define MASKOF_CANCTRL_OPMOD    MASKOF_CANSTAT_OPMOD        /* Operation request.   */
#define MASKOF_CANCTRL_ABAT     ( 0x10U )                   /* Abort all TX.        */
#define MASKOF_CANCTRL_OSM      ( 0x08U )                   /* One shot TX.         */

#define MASKOF_CANINTE_MERRF    ( 0x80U )
#define MASKOF_CANINTE_WAKIF    ( 0x40U )
#define MASKOF_CANINTE_ERRIF    ( 0x20U )
#define MASKOF_CANINTE_TX2IF    ( 0x10U )
#define MASKOF_CANINTE_TX1IF    ( 0x08U )
#define MASKOF_CANINTE_TX0IF    ( 0x04U )
#define MASKOF_CANINTE_RX1IF    ( 0x02U )
#define MASKOF_CANINTE_RX0IF    ( 0x01U )

#define MASKOF_CANINTF_MERRF    MASKOF_CANINTE_MERRF
#define MASKOF_CANINTF_WAKIF    MASKOF_CANINTE_WAKIF
#define MASKOF_CANINTF_ERRIF    MASKOF_CANINTE_ERRIF
#define MASKOF_CANINTF_TX2IF    MASKOF_CANINTE_TX2IF
#define MASKOF_CANINTF_TX1IF    MASKOF_CANINTE_TX1IF
#define MASKOF_CANINTF_TX0IF    MASKOF_CANINTE_TX0IF
#define MASKOF_CANINTF_RX1IF    MASKOF_CANINTE_RX1IF
#define MASKOF_CANINTF_RX0IF    MASKOF_CANINTE_RX0IF

#define MASKOF_EFLG_RX1OVR      ( 0x80U )
#define MASKOF_EFLG_RX0OVR      ( 0x40U )
#define MASKOF_EFLG_TXBO        ( 0x20U )
#define MASKOF_EFLG_TXEP        ( 0x10U )
#define MASKOF_EFLG_RXEP        ( 0x08U )
#define MASKOF_EFLG_TXWAR       ( 0x04U )
#define MASKOF_EFLG_RXWAR       ( 0x02U )
#define MASKOF_EFLG_EWARN       ( 0x01U )

#define MASKOF_TXB0CTRL_ABTF    ( 0x40U )
#define MASKOF_TXB0CTRL_MLOA    ( 0x20U )
#define MASKOF_TXB0CTRL_TXERR   ( 0x10U )
#define MASKOF_TXB0CTRL_TXREQ   ( 0x08U )
#define MASKOF_TXB0CTRL_TXP     ( 0x03U )

#define MASKOF_TXB1CTRL_ABTF    MASKOF_TXB0CTRL_ABTF
#define MASKOF_TXB1CTRL_MLOA    MASKOF_TXB0CTRL_MLOA
#define MASKOF_TXB1CTRL_TXERR   MASKOF_TXB0CTRL_TXERR
#define MASKOF_TXB1CTRL_TXREQ   MASKOF_TXB0CTRL_TXREQ
#define MASKOF_TXB1CTRL_TXP     MASKOF_TXB0CTRL_TXP

#define MASKOF_TXB2CTRL_ABTF    MASKOF_TXB0CTRL_ABTF
#define MASKOF_TXB2CTRL_MLOA    MASKOF_TXB0CTRL_MLOA
#define MASKOF_TXB2CTRL_TXERR   MASKOF_TXB0CTRL_TXERR
#define MASKOF_TXB2CTRL_TXREQ   MASKOF_TXB0CTRL_TXREQ
#define MASKOF_TXB2CTRL_TXP     MASKOF_TXB0CTRL_TXP

#define MASKOF_RXB0CTRL_RXM     ( 0x60 )
#define MASKOF_RXB0CTRL_RXRTR   ( 0x08 )
#define MASKOF_RXB0CTRL_BUKT    ( 0x04 )
#define MASKOF_RXB0CTRL_FILHIT  ( 0x01 )

#define MASKOF_RXB1CTRL_RXM     ( 0x60 )
#define MASKOF_RXB1CTRL_RXRTR   ( 0x08 )
#define MASKOF_RXB1CTRL_FILHIT  ( 0x07 )


#endif /* MCP2515_REG_ADDR_H */
