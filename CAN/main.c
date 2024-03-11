/**
*      @file main.c
*      @author Prithvi Bhat
*      @brief Demo project to test CAN driver
**/

// System Includes
#include <stdint.h>

// Board Includes
#include "tm4c123gh6pm.h"
#include "clock.h"
#include "wait.h"
#include "can.h"

/**
*      @brief Main driver for the entire project
**/
void main(void)
{
	initSystemClock(SYS_CLK_40MHZ);		// Initialise system clock to 40MHz

	initCan(CAN0);						// Initialise CAN0 module

	canFrame_t canFrame;

	canFrame.canArbitrationId[0] = 1;	// Set Arbitration ID
	canFrame.canData[0] = 0xFF;			// Set CAN Data
	canFrame.dataByteCount = 1;			// Request transmission of only one byte
	canFrame.messageNumber = 1;			// Something between 0x01 and 0x20

	txCan(CAN0, canFrame);				// Pack and transmit frame

	return;
}
