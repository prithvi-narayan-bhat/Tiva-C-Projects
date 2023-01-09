# Overview
* Authors:      Prithvi Bhat (pnb3598@mavs.uta.edu) Jason Losh (jlosh@uta.edu)
* Target Board: TI TM4C123G Launchpad
* Target uC:	TI EK-TM4C123GXL

## SPI
* Expander: 	SPI: MCP23S08
### Summary
* Push button connected to MCP23S08 triggers an interrupt
* Interrupt propagated to TM4C123GXL via SPI
* ISR in TM4C123GXL turns on LEDs, also interfaced to MCP23S08 over SPI

### Operation
1. Using the SSI interface of your choice on the TM4C123GXL board, connect an MCP23S08 bus expander to the appropriate clock, TX, RX, and chip select lines for the selected SSI interface. Connect the interrupt output of the bus expander to a GPI; Pull the A1 and A0 pins on the bus expander high
2. Interface two LEDs and a pushbutton to the bus expander
3. Write code to configure the SSI interface for operation at a 2 MHz rate
4. Write code to initialize the bus expander over the SPI bus so that the LED pins are outputs and the push-button pin is an input
5. Write code for the following behavior: only one of the LEDs glows when the button is pressed and the other when button released
6. Configure the oscilloscope in a single trace mode, with normal trigger on the falling edge of the chip select, and capture the clock and TX lines during both read and write to the device
7. Enable interrupts on the bus expander and the GPIO pin being used for the interrupt connection. Add an ISR to handle this interrupt
8. Modify the stop go code in Step 5 to wait 1s after the one of the LEDs turns on. At that time, clear any existing interrupts, enable the NVIC interrupt control, and then turn on the the other LED again and turn off the first LED
9. Write an ISR that is called when the push-button is pressed which turns off the first and turns on the second LED, effectively moving the check for a push-button to an interrupt event and the LED color change to the ISR
