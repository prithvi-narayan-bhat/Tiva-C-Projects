/**
 *      @file main.c
 *      @author Prithvi Bhat
 *      @brief Demonstrate Hibernation on TM4c123 using the RTC module
 *      @date 2022-10-05
 **/

#include "clock.h"
#include "gpio.h"
#include "hibernation.h"
#include "wd0.h"
#include "tm4c123gh6pm.h"

#define LED_RED             PORTF,1
#define LED_BLUE            PORTF,2
#define LED_GREEN           PORTF,3
#define PUSH_BUTTON_WAKE    PORTF,0
#define PUSH_BUTTON_SLEEP   PORTF,4

#define EXT_WAKE            (get_hibernation_wake_mode() & HIB_RIS_EXTW)
#define RTC_WAKE            (get_hibernation_wake_mode() & HIB_RIS_RTCALT0)

/**
*      @brief Function block operation until a button is pressed by user
**/
void wait_for_button_press(void)
{
    while (getPinValue(PUSH_BUTTON_SLEEP))  {}  // Check for button press (Low on Press)

    // Set LEDs for visual confirmations
    setPinValue(LED_BLUE, 0);
    setPinValue(LED_RED, 0);
    setPinValue(LED_GREEN, 0);
    disablePort(PORTF);
}

/**
*      @brief Function to initialize all necessary hardware on the device
**/
void init_TM4C_hardware(void)
{
    initSystemClockTo40Mhz();                 // Initialize system clock

    enablePort(PORTF);                        // Initialize clocks on PORTF

    selectPinPushPullOutput(LED_BLUE);        // Initialize PORTF pin 1 as an output
    selectPinPushPullOutput(LED_RED);         // Initialize PORTF pin 2 as an output
    selectPinPushPullOutput(LED_GREEN);       // Initialize PORTF pin 3 as an output
    selectPinDigitalInput(PUSH_BUTTON_SLEEP); // Initialize PORTF pin 4 as an input
    selectPinDigitalInput(PUSH_BUTTON_WAKE);  // Initialize PORTF pin 0 as an input
    enablePinPullup(PUSH_BUTTON_SLEEP);       // Set Pull mode for push button input (Normally High)
    enablePinPullup(PUSH_BUTTON_WAKE);        // Set Pull mode for push button input (Normally High)
}

/**
*      @brief main function
**/
void main(void)
{
    init_TM4C_hardware();
    setPinValue(LED_RED, 1);

    if (!(HIB_CTL_R & HIB_CTL_CLK32EN))     // Initialise hibernation module only once
    {
        init_hibernation_module();
    }

    if(EXT_WAKE)                            // Check if wake was caused by external button press
    {
        setPinValue(LED_RED, 0);
        setPinValue(LED_GREEN, 0);
        setPinValue(LED_BLUE, 1);
    }
    else if (RTC_WAKE)                      // Check if wake was caused by timeout of RTC module
    {
        setPinValue(LED_RED, 1);
        setPinValue(LED_BLUE, 0);
        setPinValue(LED_GREEN, 0);
    }

    wait_for_button_press();

    hibernate(5);
    while(1)    {}
}
