/**
*      @file main.c
*      @author Prithvi Bhat
*      @brief Control LED through SPI based IO expander
*      @date 2022-09-05
**/

#include "gpio.h"
#include "clock.h"
#include "nvic.h"
#include "i2c0.h"
#include "wait.h"

// TM4C Pins
#define PIN_TM4C_PORTD_CHIP_SELECT  PORTD,1     // IO expander chip select
#define PIN_TM4C_PORTD_SSI1_CLOCK   PORTD,0     // SPI 1 Clock
#define PIN_TM4C_PORTE_INT          PORTE,1     // I2C interrupt
#define PIN_TM4C_IIC_SCL            PORTB,2     // I2C SCL
#define PIN_TM4C_IIC_SDA            PORTB,3     // I2C SDA

// TM4C special values
#define PORT_E_INTERRUPT_VECTOR     20          // Interrupt vector number for PORT E

// MCP23008 Opcodes
#define OPCODE_MCP23S08_READ        0x47        // Opcode for reading values over SPI
#define OPCODE_MCP23S08_WRIT        0x46        // Opcode for writing values over SPI

// MCP23008 Register Values
#define VAL_MCP23008_IODIR          0x80        // Value to set pin directions (bit 7 = input)
#define VAL_MCP23008_GPINTEN        0x80        // Value to Enable GPIO input pin for interrupt-on-change even
#define VAL_MCP23008_INTCON         0x80        // Value to indicate interrupt must be triggered on comparison to DEFVAL
#define VAL_MCP23008_DEFVAL         0x80        // Value of ports to compare against for interrupt generation

// MCP23008 Device Address
#define SLAVE_MCP23008_ADDR         0x20        // Device slave address

// MCP23008 Register Addresses
#define PIN_MCP23008_IODIR          0x00        // Register address to set pin direction in IO expander
#define PIN_MCP23008_GPINTEN        0x02        // Register address to Enable interrupts
#define PIN_MCP23008_DEFVAL         0x03        // Register address to set interrupt compare value in IO expander
#define PIN_MCP23008_INTCON         0x04        // Register address to set interrupt controller in IO expander
#define PIN_MCP23008_IOCON          0x05        // Register address to enable/disable auto increment of address pointer
#define PIN_MCP23008_INTCAP         0x08        // Register address to read interrupt capture values
#define PIN_MCP23008_GPIO           0x09        // Register address to access GPIO bits

// Misc Values
#define LOGIC_LOW                   0
#define LOGIC_HIGH                  1
#define SYSTEM_CLK                  40e6        // System clock is configured for 40MHz operation

void initialise_interrupt_pins(void)
{
    disableNvicInterrupt(PORT_E_INTERRUPT_VECTOR);       // Initialize interrupt controller
    disablePinInterrupt(PIN_TM4C_PORTE_INT);             // Disable Interrupt on PE01 to configure
    selectPinDigitalInput(PIN_TM4C_PORTE_INT);           // Set Pin to input
    selectPinInterruptLowLevel(PIN_TM4C_PORTE_INT);      // Initialize interrupt to trigger on rising edge
    enablePinPullup(PIN_TM4C_PORTE_INT);

    clearPinInterrupt(PIN_TM4C_PORTE_INT);               // Clear any older, stray interrupts
    enablePinInterrupt(PIN_TM4C_PORTE_INT);              // Initialize Interrupt on PE01
    enableNvicInterrupt(PORT_E_INTERRUPT_VECTOR);
}

/**
*      @brief Function to initialize all necessary hardware on the device
**/
void init_TM4C_hardware(void)
{
      initSystemClockTo40Mhz();                           // Initialize system clock
      enablePort(PORTE);                                  // Initialize clocks on PORTE
      initI2c0();                                         // Initialize IIC interface
      initialise_interrupt_pins();                        // Initialize interrupt
}

/**
*      @brief Construct a new Interrupt Handler object
**/
void PORTE_ISR()
{
    writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_GPIO, 0x00);      // Set LED pins if button has been pressed
    waitMicrosecond(100000);

    writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_GPIO, 0x20);      // Set LED pins if button has been pressed

    waitMicrosecond(100000);

    readI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_INTCAP);           // Read interrupt register on the IO expander to clear interrupt

    writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_GPIO, 0x40);      // Set Red LED
    clearPinInterrupt(PIN_TM4C_PORTE_INT);
}

/**
*      @brief main function
**/
void main(void)
{

      init_TM4C_hardware();
      // GPIO controls
      writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_IODIR, VAL_MCP23008_IODIR);       // Set pin directions (bit 07 = input; others = output)

      // Interrupt controls
      writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_DEFVAL, VAL_MCP23008_DEFVAL);     // Set interrupt trigger condition
      writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_INTCON, VAL_MCP23008_INTCON);     // Controls how the associated pin value is compared for interrupt-on-change
      writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_GPINTEN, VAL_MCP23008_GPINTEN);   // Enable GPIO input pin for interrupt-on-change event

      writeI2c0Register(SLAVE_MCP23008_ADDR, PIN_MCP23008_GPIO, 0x40);                      // Set Green LED pins

      while(true)                                                                           // Run infinitely
      {}
}
