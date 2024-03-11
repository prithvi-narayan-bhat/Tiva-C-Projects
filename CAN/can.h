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
    uint8_t dataByteCount;
    uint8_t messageNumber;
    uint8_t canData[7];
    uint16_t canArbitrationId[2];

} canFrame_t;

void initCan(canModule_t canModule_t);
bool txCan(canModule_t canModule, canFrame_t canFrame);
