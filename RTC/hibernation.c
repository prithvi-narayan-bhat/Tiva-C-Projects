/**
 *      @file hibernation.c
 *      @author Prithvi Bhat
 *      @brief Configurations for the Hibernation module
 *      @date 2022-09-27
**/

#include "tm4c123gh6pm.h"
#include "hibernation.h"
#include <stdint.h>
#include "gpio.h"

/**
*      @brief Enumeration of hibernation wake types
**/
typedef enum
{
    WAKE_ON_RTC_MATCH   = HIB_CTL_RTCWEN,
    WAKE_ON_GPIO_PIN    = HIB_CTL_PINWEN,
    WAKE_ON_LOW_BATT    = HIB_CTL_BATWKEN,
}wake_mode_t;

/**
*      @brief Function to wait while register write is in progress
**/
void wait_write(void)
{
    while(!(HIB_CTL_R & HIB_CTL_WRC));
}

/**
*      @brief Function to initialize registers in the hibernation module
**/
void init_hibernation_module(void)
{
    SYSCTL_RCGCHIB_R |= SYSCTL_RCGCHIB_R0;
    _delay_cycles(3);
    HIB_IM_R |= HIB_IM_WC;                          // Set the required RTC match interrupt mask
    wait_write();
    HIB_CTL_R |= HIB_CTL_CLK32EN;                   // Enable 32kHz clock for the hibernation module
    wait_write();
    // _delay_cycles(600);
    HIB_CTL_R |= HIB_CTL_RTCEN | HIB_CTL_VDD3ON;    // Enable RTC and VDD in the hibernation module
}

/**
*      @brief Function to force uC to go into hibernate
**/
void hibernate(uint32_t seconds)
{
   disablePort(PORTF);
    HIB_CTL_R |= WAKE_ON_GPIO_PIN | WAKE_ON_RTC_MATCH;
    HIB_IC_R |= HIB_IC_WC | HIB_IC_EXTW;

    wait_write();
    HIB_RTCM0_R = seconds;                          // Set match value to trigger interrupt
    wait_write();
    HIB_RTCSS_R = (seconds << 16);                  // Set value in sub-second register
    wait_write();
    HIB_RTCLD_R = 0;                                // Set load value for hibernation RTC
    HIB_CTL_R |= HIB_CTL_HIBREQ;                    // Request Board to go into hibernation
    // HIB_CTL_R = 0x0000015B;
}

/**
*      @brief Get the hibernation wake mode object
*      @return uint32_t register value indicating the type of event that triggered the wake
**/
uint32_t get_hibernation_wake_mode(void)
{
    return HIB_RIS_R;
}
