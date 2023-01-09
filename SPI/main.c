/**
*      @file main.c
*      @author Prithvi Bhat
*      @brief Control LED through SPI IO expander
*      @date 2022-09-05
**/

#include "gpio.h"
#include "clock.h"
#include "nvic.h"
#include "spi1.h"
#include "wait.h"

// TM4C Pins
#define PIN_TM4C_SSI1_A1            PORTD,6     // A1   * Hard-wired in circuit
#define PIN_TM4C_SSI1_A0            PORTD,5     // A0   * Hard-wired in circuit
#define PIN_TM4C_SSI1_TX            PORTD,3     // SPI 1 Tx
#define PIN_TM4C_SSI1_RX            PORTD,2     // SPI 1 Rx
#define PIN_TM4C_PORTD_CHIP_SELECT  PORTD,1     // IO expander chip select
#define PIN_TM4C_PORTD_SSI1_CLOCK   PORTD,0     // SPI 1 Clock
#define PIN_TM4C_PORTE_INT          PORTE,1     // SPI interrupt

// TM4C special values
#define PORT_E_INTERRUPT_VECTOR     20          // Interrupt vector number for PORT E

// MCP23S08 Opcodes
#define OPCODE_MCP23S08_READ        0x47        // Opcode for reading values over SPI
#define OPCODE_MCP23S08_WRIT        0x46        // Opcode for writing values over SPI
#define VAL_MCP23S08_IODIR          0x80        // Value to set pin directions (bit 7 = input)
#define VAL_MCP23S08_GPINTEN        0x80        // Value to Enable GPIO input pin for interrupt-on-change even
#define VAL_MCP23S08_INTCON         0x80        // Value to indicate interrupt must be triggered on comparison to DEFVAL
#define VAL_MCP23S08_DEFVAL         0x80        // Value of ports to compare against for interrupt generation

// MCP23S08 Register Addresses
#define PIN_MCP23S08_IODIR          0x00        // Register address to set pin direction in IO expander
#define PIN_MCP23S08_GPINTEN        0x02        // Register address to Enable interrupts
#define PIN_MCP23S08_DEFVAL         0x03        // Register address to set interrupt compare value in IO expander
#define PIN_MCP23S08_INTCON         0x04        // Register address to set interrupt controller in IO expander
#define PIN_MCP23S08_IOCON          0x05        // Register address to enable/disable auto increment of address pointer
#define PIN_MCP23S08_INTCAP         0x08        // Register address to read interrupt capture values
#define PIN_MCP23S08_GPIO           0x09        // Register address to access GPIO bits

// Misc Values
#define LOGIC_LOW                   0
#define LOGIC_HIGH                  1
#define SYSTEM_CLK                  40e6        // System clock is configured for 40MHz operation
#define SPI_BAUD                    2e6         // SPI bus baud rate

/**
*      @brief Function to initialize SPI lines
**/
void initialise_spi_bus(void)
{
      initSpi1(USE_SSI_RX);
      setSpi1BaudRate(SPI_BAUD, SYSTEM_CLK);
      setSpi1Mode(LOGIC_HIGH, LOGIC_HIGH);
}

/**
*      @brief Function to initialize all necessary hardware on the device
**/
void init_TM4C_hardware(void)
{
      initSystemClockTo40Mhz();                       // Initialize system clock

      enablePort(PORTE);                              // Initialize clocks on PORTE

      initialise_spi_bus();                           // Initialize SPI bus

      enableNvicInterrupt(PORT_E_INTERRUPT_VECTOR);   // Initialize interrupt controller
      disablePinInterrupt(PIN_TM4C_PORTE_INT);        // Disable Interrupt on PE01 to configure
      selectPinDigitalInput(PIN_TM4C_PORTE_INT);      // Set Pin to input
      selectPinInterruptLowLevel(PIN_TM4C_PORTE_INT); // Initialize interrupt to trigger on rising edge
      enablePinPullup(PIN_TM4C_PORTE_INT);

      enablePinInterrupt(PIN_TM4C_PORTE_INT);         // Initialize Interrupt on PE01
      clearPinInterrupt(PIN_TM4C_PORTE_INT);          // Clear any older, stray interrupts
}

/**
*      @brief Function to initialize necessary pins and registers on IO expander
*                  Write steps
*                   - Clear CS
*                   - Opcode into SPI register
*                   - Address into SPI register
*                   - Data into SPI register
*                   - Set CS
*      @param register_address address to write into
*      @param data data to write
**/
void write_MCP23S08(uint32_t register_address, uint32_t data)
{
      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_HIGH);  // Set CS line
      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_LOW);   // Clear CS line

      writeSpi1Data(OPCODE_MCP23S08_WRIT);                  // Transmit write opcode into IO expander
      readSpi1Data();                                       // Read SPI register to clear register

      writeSpi1Data(register_address);                      // Transmit address of register in IO expander to write into
      readSpi1Data();                                       // Read SPI register to clear register

      writeSpi1Data(data);                                  // Transmit data into IO expander
      readSpi1Data();                                       // Read SPI register to clear register

      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_HIGH);  // Set CS to indicate transmission complete
}

/**
*      @brief Function to read necessary pins and registers on IO expander in order to write to it
*                  Write steps
*                   - Clear CS
*                   - Opcode into SPI register
*                   - Address into SPI register
*                   - Data from respective register address
*                   - Set CS
*      @param register_address address to write into
*      @param data data to write
**/
uint32_t read_MCP23S08(uint32_t register_address)
{
      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_HIGH);  // Set CS line
      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_LOW);   // Clear CS line

      writeSpi1Data(OPCODE_MCP23S08_READ);                  // Transmit read opcode into IO expander
      readSpi1Data();                                       // Read SPI register to clear register

      writeSpi1Data(register_address);                      // Set the address of the register to read form
      readSpi1Data();                                       // Read contents of interested register

      writeSpi1Data(0x00);                                  // Dummy value to induce read
      uint32_t retVal = readSpi1Data();

      setPinValue(PIN_TM4C_PORTD_CHIP_SELECT, LOGIC_HIGH);  // Set CS to indicate transmission complete

      return retVal;
}


/**
*      @brief Construct a new Interrupt Handler object
**/
void PORTE_ISR()
{
      read_MCP23S08(PIN_MCP23S08_GPIO);
      write_MCP23S08(PIN_MCP23S08_GPIO, 0x00);  // Set LED pins if button has been pressed

      waitMicrosecond(100000);

      write_MCP23S08(PIN_MCP23S08_GPIO, 0x20);  // Set LED pins if button has been pressed
      clearPinInterrupt(PIN_TM4C_PORTE_INT);    // Clear interrupt

      waitMicrosecond(100000);

      read_MCP23S08(PIN_MCP23S08_INTCAP);       // Read Interrupt register
      write_MCP23S08(PIN_MCP23S08_GPIO, 0x40);  // Set Red LED

      clearPinInterrupt(PIN_TM4C_PORTE_INT);
}

/**
*      @brief main function
**/
void main(void)
{

      init_TM4C_hardware();
      // GPIO controls
      write_MCP23S08(PIN_MCP23S08_IODIR, VAL_MCP23S08_IODIR);           // Set pin directions (bit 07 = input; others = output)

      // Interrupt controls
      write_MCP23S08(PIN_MCP23S08_DEFVAL, VAL_MCP23S08_DEFVAL);         // Set interrupt trigger condition
      write_MCP23S08(PIN_MCP23S08_INTCON, VAL_MCP23S08_INTCON);         // Controls how the associated pin value is compared for interrupt-on-change
      write_MCP23S08(PIN_MCP23S08_GPINTEN, VAL_MCP23S08_GPINTEN);       // Enable GPIO input pin for interrupt-on-change event

      write_MCP23S08(PIN_MCP23S08_GPIO, 0x40);                          // Set Green LED pins

      while(true)                                                       // Run infinitely
      {}
}
