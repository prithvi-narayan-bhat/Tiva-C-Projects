/**
 *       @brief GPIO Library (ports A-F)
 *          Target Platform:    EK-TM4C123GXL
 *          Target uC:          TM4C123GH6PM
 *       @author Prithvi Bhat (with references from Jason Losh's Library)
 **/

#include "nvic.h"
#include "tm4c123gh6pm.h"

/**
*      @brief Function to enable NVIC interrupts of a particular kind
*      @param vectorNumber the mask of the interrupt to be enabled
**/
void enableNvicInterrupt(uint8_t vectorNumber)
{
    volatile uint32_t* p = (uint32_t*) &NVIC_EN0_R;
    vectorNumber -= 16;
    p += vectorNumber >> 5;
    *p = 1 << (vectorNumber & 31);
}

/**
 *      @brief Function to disable NVIC interrupts of a particular kind
 *      @param vectorNumber the mask of the interrupt to be disabled
 **/
void disableNvicInterrupt(uint8_t vectorNumber)
{
    volatile uint32_t* p = (uint32_t*) &NVIC_DIS0_R;
    vectorNumber -= 16;
    p += vectorNumber >> 5;
    *p = 1 << (vectorNumber & 31);
}

/**
*      @brief Set the NVIC Interrupt Priority object
*      @param vectorNumber mask of the interrupt
*      @param priority priority
**/
void setNvicInterruptPriority(uint8_t vectorNumber, uint8_t priority)
{
    volatile uint32_t* p = (uint32_t*) &NVIC_PRI0_R;
    vectorNumber -= 16;
    uint32_t shift = 5 + (vectorNumber & 3) * 8;
    p += vectorNumber >> 2;
    *p &= ~(7 << shift);
    *p |= priority << shift;
}

/**
*      @brief Function to disable all interrupts
**/
void disableInterrupts(void)
{
    /**
     *   @brief By executing the CPSID I instruction, the processor clears the I-bit in the CPSR, disabling all interrupts
     *   @param CPS Change Processor State
     *   @param I refers to the "Interrupt Disable" flag in the CPSR (Current Program Status Register)
     *   @param CPSID is used to clear the I-bit, which disables interrupts
     *
     **/
    __asm("     CPSID I");      // Invoke the ARM Cortex M instruction to disable all instructions
}

/**
 *      @brief Function to disable all interrupts
 **/
void enableInterrupts(void)
{
    /**
     *   @brief By executing the CPSIE I instruction, the processor sets the I-bit in the CPSR, enabling interrupts
     *   @param CPS Change Processor State
     *   @param I refers to the "Interrupt Disable" flag in the CPSR (Current Program Status Register)
     *   @param CPSIE is used to set the I-bit, which enables interrupts
     *
     **/
    __asm("     CPSIE I"); // Invoke the ARM Cortex M instruction to enable all instructions
}