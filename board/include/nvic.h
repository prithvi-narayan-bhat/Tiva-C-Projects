/**
 *       @brief GPIO Library (ports A-F)
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *       @author Prithvi Bhat (with references from Jason Losh's Library)
 **/

#ifndef NVIC_H
#define NVIC_H

#include <stdint.h>

void enableNvicInterrupt(uint8_t vectorNumber);
void disableNvicInterrupt(uint8_t vectorNumber);
void setNvicInterruptPriority(uint8_t vectorNumber, uint8_t priority);
void enableInterrupts(void);
void disableInterrupts(void);

#endif
