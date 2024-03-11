/**
*      @file can.c
*      @author Prithvi Bhat
*      @brief CAN module driver
**/

#include "gpio.h"
#include "can.h"

// Pin mappings for CAN modules
#define CAN0_Rx PORTB,4
#define CAN0_Tx PORTB,5
#define CAN1_Rx
#define CAN1_Tx

/**
*      @brief Function to initialise CAN module
*      @param canModule 1 of two modules to be initialised
**/
void initCan(canModule_t canModule)
{
    switch (canModule)
    {
        // CAN 0 is available on Port-B and Port-E. This initialises CAN 0 on PB4 and PB5 by default
        case CAN0:
        {
            SYSCTL_RCGC0_R |= SYSCTL_RCGC0_CAN0;                    // Clock CAN0 peripheral
            _delay_cycles(3);

            enablePort(PORTB);                                      // Clock Port B
            setPinAuxFunction(CAN0_Rx, GPIO_PCTL_PB4_CAN0RX);       // Set alternate function to CAN Rx
            setPinAuxFunction(CAN0_Tx, GPIO_PCTL_PB5_CAN0TX);       // Set alternate function to CAN Tx

            CAN0_CTL_R = CAN_CTL_INIT;                              // Set Tx High. Block all Rx and Tx
            CAN0_CTL_R |= CAN_CTL_CCE;                              // Allow Write configuration to CAN_BIT register
            CAN0_CTL_R |= CAN_CTL_DAR;                              // Allow retransmission on failure
            CAN0_CTL_R |= CAN_CTL_SIE;                              // Enable Status interrupts
            CAN0_CTL_R |= CAN_CTL_IE;                               // Enable CAN interrupts
            CAN0_CTL_R |= CAN_CTL_EIE;                              // Enable Error interrupts

            /**
             *      @brief Set bits in the CAN_BIT register
             *          Assumptions:
             *              1. System Clock: 40MHz
             *              2. CAN Baud: 500Kbps
             *              3. Sample point: 87.5%
             *              4. SJW : 1
             *          Calculations:
             *              1. TQ = 1/f_can = 1/500k = 2us
             *              2. TSEG1 + TSEG2 + 1 = bit_preiod = 87.5%
             *              3. Assume TSEG1 = 6 and TSEG2 = 1
             *              4. BRP = ((sys_clk / (f_can * bit_period)) - 1)
             *                 BRP = (((40*10^6) / ((500*10^3) * (6+1+1))) - 1) = 10
             **/

            // @todo Verify these values
            CAN0_BIT_R =  10 << 0;                                  // BRP: Baud Rate Prescaler
            CAN0_BIT_R |= 1 << 6;                                   // SJW: Synchronisation Jump Width
            CAN0_BIT_R |= 6 << 8;                                   // TSEG1: Time Segment 1
            CAN0_BIT_R |= 1 << 12;                                  // TSEG2: Time Segment 2

            CAN0_CTL_R &= ~CAN_CTL_INIT;                            // Set Tx Low
            CAN0_CTL_R &= ~CAN_CTL_CCE;                             // Disallow Write configuration to CAN_BIT register
        }
    }
}

/**
*      @brief Function to transmit a packet of data over CAN
*      @param canModule module to transmit over
*      @param canFrame elements of data to be transmitted
*      @return true return on success
*      @return false return on failure
**/
bool txCan(canModule_t canModule, canFrame_t canFrame)
{
    canFrame.frameStatus = 0x00;                                    // Indicate Frame staging has begun

    // Configure CAN to transmit
    uint8_t i;
    CAN0_CTL_R = CAN_CTL_INIT;                                      // Set Tx High. Block all Rx and Tx

    while (!(CAN0_IF1CRQ_R & CAN_IF1CRQ_BUSY));                     // Wait until the CAN registers are free

    CAN0_IF1CMSK_R |= CAN_IF1CMSK_WRNRD;                            // Write, Not Read
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_CONTROL;                          // Transfer control bits from CANIFnMCTL register into Interface registers

    CAN0_IF1CMSK_R |= CAN_IF1CMSK_ARB;                              // Transfer ID + DIR + XTD + MSGVAL into Interface registers
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_DATAA;                            // Access Data Byte 0 to 3
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_DATAB;                            // Access Data Byte 4 to 7
    CAN0_IF1CMSK_R |= CAN_IF1CMSK_TXRQST;                           // Transmission Request

    CAN0_IF1MCTL_R |= CAN_IF1MCTL_TXIE;                             // Set INTPND bit in CANIFnMCTL on successful transmission of frame

    // Load user provided data into CAN registers
    // Define a macro pointing to the base of the first data register
#ifndef CAN0_DATA_R_BASE
#define CAN0_DATA_R_BASE  ((volatile uint32_t *)0x4004003C)

    for (i = 0; i < canFrame.dataByteCount; i++)
    {
        (*(CAN0_DATA_R_BASE + i)) = canFrame.canData[i];            // Iteratively assign each data byte to successive register locations
    }

#undef CAN0_DATA_R_BASE
#endif

    CAN0_IF1MCTL_R |= canFrame.dataByteCount;                       // Set Data length code

    CAN0_IF1ARB1_R = canFrame.canArbitrationId[0];                  // Set Arbitration ID for message
    CAN0_IF1ARB2_R = canFrame.canArbitrationId[1];                  // Set Arbitration ID for message

    CAN0_CTL_R &= ~CAN_CTL_INIT;                                    // Set Tx High. Block all Rx and Tx
    CAN0_IF1CRQ_R |= canFrame.messageNumber;                        // Set the Message Number. CAN begins transmission on setting this bit

    return true;
}
