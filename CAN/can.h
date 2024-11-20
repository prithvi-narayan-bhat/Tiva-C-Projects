/**
 *      @file can.h
 *      @author Prithvi Bhat
 *      @brief CAN module driver
 **/

#ifndef CAN_H
#define CAN_H

#include "tm4c123gh6pm.h"
#include "stdbool.h"

#define CAN_FRAME_STAGING       0x01
#define CAN_FRAME_TRANSMITTED   0x0F
#define CAN_FRAME_RECEIVING     0x10
#define CAN_FRAME_RECEIVED      0xF0

/**
*      @brief Enumeration of CAN available modules
**/
typedef enum
{
    CAN0,
    CAN1
} canModule_t;

/**
*      @brief Structure of User facing elements of a CAN frame
**/
typedef struct
{
    uint8_t dataByteCount;              // Number of bytes to transmit or received
    uint8_t messageNumber;              // Selects one of the 32 message objects in the message RAM for data transfer
        uint8_t canData[7];             // Data for Tx/Rx
    uint16_t canArbitrationId[2];       // To resolve arbitration
    uint8_t frameStatus;                // Tx/Rx frame status Staging, Transmitted, Receiving, Received
} canFrame_t;

// CAN driver APIs
void initCan(canModule_t canModule_t);
void txCan(canModule_t canModule, canFrame_t canFrame);

#endif                                  // CAN_H
