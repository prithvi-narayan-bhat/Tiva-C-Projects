# Overview
* Authors: Prithvi Bhat (pnb3598@mavs.uta.edu) Jason Losh (jlosh@uta.edu)
* Target Board: TI TM4C123G Launchpad
* Target uC: TI EK-TM4C123GXL

Some of these projects were developed as part of my course at UT Arlington under the guidance of Dr Jason Losh

### Requirements
* All projects are developed on the Code Composer Studio IDE and should be compatible as is

## SPI
* Expander: MCP23S08
### Summary
* Push button connected to MCP23S08 triggers an interrupt
* Interrupt propagated to TM4C123GXL via SPI
* ISR in TM4C123GXL flips LEDs, also interfaced to MCP23S08 over SPI
* SPI interface is configured for operation at a 2 MHz rate

## I2C
* Expander: MCP23008
### Summary
* Push button connected to MCP23008 triggers an interrupt
* Interrupt propagated to TM4C123GXL via I2C
* ISR in TM4C123GXL flips LEDs, also interfaced to MCP23008 over I2C
* SPI interface is configured for operation at a 100 kHz rate

## RTC
* Uses the internal RTC module on the microcontroller
### Summary
* Uses the two push buttons on the Tiva-C launchpad - one to put the board in low power hibernation and one to wake and resume normal operation
* Configures the RTC and hibernation modules appropriately to act on the button presses