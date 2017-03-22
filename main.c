//*****************************************************************************
//
// Standard Includes
//
//*****************************************************************************

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "userlib/gpio.h"
#include "userlib/uart.h"
#include "userlib/udma.h"

#include "peripherals/led.h"
#include "peripherals/misc.h"

//*****************************************************************************
//
// Peripheral defines
//
//*****************************************************************************

// GPS
#define GPS_ENABLE true
#define GPS_BASE                UART1_BASE
#define GPS_NAV_LOCK_SENSE_BASE GPIO_PORTB_BASE
#define GPS_NAV_LOCK_SENSE_PIN  GPIO_PIN_5
#define GPS_BAUD 9600
#define GPS_UART_CONFIG (UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE)

//*****************************************************************************
//
// Function prototypes
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void __error__(char *pcFilename, uint32_t ui32Line) {

}
#endif

// Int Handlers
void BusFaultHandler(void);
void UsageFaultHandler(void);
void GPSIntHandler(void);
void COMIntHandler(void);

//*****************************************************************************
//
// Global vars
//
//*****************************************************************************

//*****************************************************************************
//
// Main function
//
//*****************************************************************************
int32_t main(void) {
  //
  ///////////////////Initialize//////////////////////////////
  //

  //
  // Set the clocking to run at 80MHz.
  //
  SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

  //
  // Set Fault Interrupts in the NVIC
  //
  IntRegister(FAULT_BUS, BusFaultHandler);
  IntRegister(FAULT_USAGE, UsageFaultHandler);

  //
  // Enable the FPU
  //
  FPUInit();

  //
  // Enable processor interrupts.
  //
  IntMasterEnable();

  //
  // Enable the Console IO
  //
  UARTInit(UART0_BASE, GPS_BAUD, false, (UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE | UART_CONFIG_WLEN_8));
  UARTIntInit(UART0_BASE, UART_INT_RX | UART_INT_RT, -1, COMIntHandler);

  UARTInit(GPS_BASE, GPS_BAUD, true, GPS_UART_CONFIG);
  UARTIntInit(GPS_BASE, UART_INT_RX | UART_INT_RT, -1, GPSIntHandler);

  while (true);
}

void GPSIntHandler(void) {
  UARTIntClear(GPS_BASE, UART_INT_RX | UART_INT_RT);
  do {
    int32_t i32LocalChar = UARTCharGet(GPS_BASE);
    if (i32LocalChar != -1) UARTCharPut(UART0_BASE, i32LocalChar);
    } while (UARTCharsAvail(GPS_BASE));
}

void COMIntHandler(void) {
  UARTIntClear(UART0_BASE, UART_INT_RX | UART_INT_RT);
  do {
    int32_t i32LocalChar = UARTCharGet(UART0_BASE);
    if (i32LocalChar != -1) UARTCharPut(GPS_BASE, i32LocalChar);
  } while (UARTCharsAvail(UART0_BASE));
}

void BusFaultHandler(void) {
  while (true)
    ;
}

void UsageFaultHandler(void) {
  while (true)
    ;
}
