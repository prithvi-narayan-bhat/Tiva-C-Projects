/**
 *      @file can.h
 *      @author Prithvi Bhat
 *      @brief CAN module driver
 **/

#include "tm4c123gh6pm.h"
#include "stdbool.h"

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
    uint8_t messageNumber;              // Message number
    uint8_t canData[7];                 // Data for Tx/Rx
    uint16_t canArbitrationId[2];       // To resolve arbitration
    uint8_t frameStatus;                // Tx/Rx frame status [HX0] Tx Staged; [HXF] Tx Complete; [H0X] Rx Started; [HFX] Rx Complete
} canFrame_t;

void initCan(canModule_t canModule_t);
bool txCan(canModule_t canModule, canFrame_t canFrame);
