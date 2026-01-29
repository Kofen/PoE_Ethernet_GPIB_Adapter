#ifndef AR488_LAYOUTS_H
#define AR488_LAYOUTS_H

#include <Arduino.h>

#include "AR488_Config.h"


/***** AR488_Hardware.h, ver. 0.53.33, 12/12/2025 *****/


///=================================///
/// Hardware pin layout definitions ///
///=================================///


/*************************************/
/***** CUSTOM PIN LAYOUT SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_CUSTOM

/***** Configured in AR488_Config.h *****/

#endif
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** CUSTOM PIN LAYOUT SECTION *****/
/*************************************/



/**************************************/
/***** UNO/NANO LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#if defined(AR488_UNO) || defined(AR488_NANO)


/***** NOTE: UNO/NANO pinout *****/
#define DIO1_PIN  A0  /* GPIB 1  : PORTC bit 0 */
#define DIO2_PIN  A1  /* GPIB 2  : PORTC bit 1 */
#define DIO3_PIN  A2  /* GPIB 3  : PORTC bit 2 */
#define DIO4_PIN  A3  /* GPIB 4  : PORTC bit 3 */
#define DIO5_PIN  A4  /* GPIB 13 : PORTC bit 4 */
#define DIO6_PIN  A5  /* GPIB 14 : PORTC bit 5 */
#define DIO7_PIN   4  /* GPIB 15 : PORTD bit 4 */
#define DIO8_PIN   5  /* GPIB 16 : PORTD bit 5 */

#define IFC_PIN    8  /* GPIB 9  : PORTB bit 0 */
#define NDAC_PIN   9  /* GPIB 8  : PORTB bit 1 */
#define NRFD_PIN  10  /* GPIB 7  : PORTB bit 2 */
#define DAV_PIN   11  /* GPIB 6  : PORTB bit 3 */
#define EOI_PIN   12  /* GPIB 5  : PORTB bit 4 */

#define SRQ_PIN    2  /* GPIB 10 : PORTD bit 2 */
#define REN_PIN    3  /* GPIB 17 : PORTD bit 3 */
#define ATN_PIN    7  /* GPIB 11 : PORTD bit 7 */


#endif
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** UNO/NANO LAYOUT DEFINITION *****/
/**************************************/



/************************************************/
/***** MEGA2560 LAYOUT DEFINITION (Default) *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_D

// NOTE: MEGA2560 pinout last updated 28/07/2019
#define DIO1_PIN  A0  /* GPIB 1  : PORTF bit 0 */
#define DIO2_PIN  A1  /* GPIB 2  : PORTF bit 1 */
#define DIO3_PIN  A2  /* GPIB 3  : PORTF bit 2 */
#define DIO4_PIN  A3  /* GPIB 4  : PORTF bit 3 */
#define DIO5_PIN  A4  /* GPIB 13 : PORTF bit 4 */
#define DIO6_PIN  A5  /* GPIB 14 : PORTF bit 5 */
#define DIO7_PIN  A6  /* GPIB 15 : PORTF bit 4 */
#define DIO8_PIN  A7  /* GPIB 16 : PORTF bit 5 */

#define IFC_PIN   17  /* GPIB 9  : PORTH bit 0 */
#define NDAC_PIN  16  /* GPIB 8  : PORTH bit 1 */
#define NRFD_PIN   6  /* GPIB 7  : PORTH bit 3 */
#define DAV_PIN    7  /* GPIB 6  : PORTH bit 4 */
#define EOI_PIN    8  /* GPIB 5  : PORTH bit 5 */
#define REN_PIN    9  /* GPIB 17 : PORTD bit 6 */

#define SRQ_PIN   10  /* GPIB 10 : PORTB bit 4 */
#define ATN_PIN   11  /* GPIB 11 : PORTB bit 5 */

#endif  // AR488_MEGA2560_D
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 LAYOUT DEFINITION (Default) *****/
/************************************************/



/*****************************************/
/***** MEGA2560 LAYOUT DEFINITION E1 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_E1

// NOTE: MEGA2560 pinout last updated 28/07/2019
#define DIO1_PIN  30  /* GPIB 1  : PORTC bit 1 */
#define DIO2_PIN  32  /* GPIB 2  : PORTC bit 3 */
#define DIO3_PIN  34  /* GPIB 3  : PORTC bit 5 */
#define DIO4_PIN  36  /* GPIB 4  : PORTC bit 7 */
#define DIO5_PIN  22  /* GPIB 13 : PORTA bit 6 */
#define DIO6_PIN  24  /* GPIB 14 : PORTA bit 4 */
#define DIO7_PIN  26  /* GPIB 15 : PORTA bit 2 */
#define DIO8_PIN  28  /* GPIB 16 : PORTA bit 0 */

#define IFC_PIN   48  /* GPIB 9  : PORTD bit 7 */
#define NDAC_PIN  46  /* GPIB 8  : PORTG bit 1 */
#define NRFD_PIN  44  /* GPIB 7  : PORTL bit 7 */
#define DAV_PIN   42  /* GPIB 6  : PORTL bit 5 */
#define EOI_PIN   40  /* GPIB 5  : PORTL bit 3 */
#define REN_PIN   38  /* GPIB 17 : PORTL bit 1 */

#define SRQ_PIN   50  /* GPIB 10 : PORTB bit 1 */
#define ATN_PIN   52  /* GPIB 11 : PORTB bit 3 */

#endif  // AR488_MEGA2560_E1
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 LAYOUT DEFINITION E1 *****/
/*****************************************/



/*****************************************/
/***** MEGA2560 LAYOUT DEFINITION E2 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_E2

// NOTE: MEGA2560 pinout last updated 28/07/2019
#define DIO1_PIN  37  /* GPIB 1  : PORTA bit 1 */
#define DIO2_PIN  35  /* GPIB 2  : PORTA bit 3 */
#define DIO3_PIN  33  /* GPIB 3  : PORTA bit 5 */
#define DIO4_PIN  31  /* GPIB 4  : PORTA bit 7 */
#define DIO5_PIN  29  /* GPIB 13 : PORTC bit 6 */
#define DIO6_PIN  27  /* GPIB 14 : PORTC bit 4 */
#define DIO7_PIN  25  /* GPIB 15 : PORTC bit 2 */
#define DIO8_PIN  23  /* GPIB 16 : PORTC bit 0 */

#define IFC_PIN   49  /* GPIB 9  : PORTG bit 0 */
#define NDAC_PIN  47  /* GPIB 8  : PORTG bit 2 */
#define NRFD_PIN  45  /* GPIB 7  : PORTL bit 6 */
#define DAV_PIN   43  /* GPIB 6  : PORTL bit 4 */
#define EOI_PIN   41  /* GPIB 5  : PORTL bit 2 */
#define REN_PIN   39  /* GPIB 17 : PORTL bit 0 */

#define SRQ_PIN   51  /* GPIB 10 : PORTB bit 0 */
#define ATN_PIN   53  /* GPIB 11 : PORTB bit 2 */

#endif  // AR488_MEGA2560_E2
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 LAYOUT DEFINITION E2 *****/
/*****************************************/



/****************************************************************/
/***** MICRO PRO (32u4) LAYOUT DEFINITION for MICRO (Artag) *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA32U4_MICRO

#define DIO1_PIN  3   /* GPIB 1  : PORTD bit 0   data pins assigned for minimum shifting */
#define DIO2_PIN  15  /* GPIB 2  : PORTB bit 1 */
#define DIO3_PIN  16  /* GPIB 3  : PORTB bit 2 */
#define DIO4_PIN  14  /* GPIB 4  : PORTB bit 3 */
#define DIO5_PIN  8   /* GPIB 13 : PORTB bit 4 */
#define DIO6_PIN  9   /* GPIB 14 : PORTB bit 5 */
#define DIO7_PIN  10  /* GPIB 15 : PORTB bit 6 */
#define DIO8_PIN  6   /* GPIB 16 : PORTD bit 7 */

#define IFC_PIN   4   /* GPIB 9  : PORTD bit 4 */
#define NDAC_PIN  A3  /* GPIB 8  : PORTF bit 4 */
#define NRFD_PIN  A2  /* GPIB 7  : PORTF bit 5 */
#define DAV_PIN   A1  /* GPIB 6  : PORTF bit 6 */
#define EOI_PIN   A0  /* GPIB 5  : PORTF bit 7 */
#define REN_PIN   5   /* GPIB 17 : PORTC bit 6 */
#define SRQ_PIN   7   /* GPIB 10 : PORTE bit 6 */
#define ATN_PIN   2   /* GPIB 11 : PORTD bit 1 */

#endif  // AR488_MEGA32U4_MICRO
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MICRO PRO (32u4) LAYOUT DEFINITION for MICRO (Artag) *****/
/****************************************************************/



/*****************************************/
/***** LEONARDO R3 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA32U4_LR3

#define DIO1_PIN  A0  /* GPIB 1  : PORTF bit 7 */
#define DIO2_PIN  A1  /* GPIB 2  : PORTF bit 6 */
#define DIO3_PIN  A2  /* GPIB 3  : PORTF bit 5 */
#define DIO4_PIN  A3  /* GPIB 4  : PORTF bit 4 */
#define DIO5_PIN  A4  /* GPIB 13 : PORTF bit 3 */
#define DIO6_PIN  A5  /* GPIB 14 : PORTF bit 2 */
#define DIO7_PIN   4  /* GPIB 15 : PORTD bit 4 */
#define DIO8_PIN   5  /* GPIB 16 : PORTC bit 6 */

#define IFC_PIN    8  /* GPIB 9  : PORTB bit 4 */
#define NDAC_PIN   9  /* GPIB 8  : PORTB bit 5 */
#define NRFD_PIN  10  /* GPIB 7  : PORTB bit 6 */
#define DAV_PIN   11  /* GPIB 6  : PORTD bit 6 */
#define EOI_PIN   12  /* GPIB 5  : PORTC bit 7 */

#define SRQ_PIN    2  /* GPIB 10 : PORTD bit 1 */
#define REN_PIN    3  /* GPIB 17 : PORTD bit 0 */
#define ATN_PIN    7  /* GPIB 11 : PORTE bit 6 */

uint8_t reverseBits(uint8_t dbyte);

#endif // AR488_MEGA32U4_LR3
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** LEONARDO R3 LAYOUT DEFINITION *****/
/*****************************************/



/***********************************************/
/***** MCP23S17 IC (SPI) LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MCP23S17

#include <SPI.h>

/***** NOTE: MCP23S17 pinout *****/
#define IFC_PIN    0  /* GPIB 9  : PORTA bit 0 */
#define NDAC_PIN   1  /* GPIB 8  : PORTA bit 1 */
#define NRFD_PIN   2  /* GPIB 7  : PORTA bit 2 */
#define DAV_PIN    3  /* GPIB 6  : PORTA bit 3 */
#define EOI_PIN    4  /* GPIB 5  : PORTA bit 4 */
#define REN_PIN    5  /* GPIB 17 : PORTA bit 5 */
#define SRQ_PIN    6  /* GPIB 10 : PORTA bit 6 */
#define ATN_PIN    7  /* GPIB 11 : PORTA bit 7 */

/***** NOTE: DIO1-DIO8 pins are assigned to ping 0 - 7 on port B *****/


/***** MCP23S17 defines *****/
// Direction registers
#define MCPDIRA  0x00
#define MCPDIRB  0x01

// Configuration register
#define MCPCON (0x0A)

// Pullup state register
#define MCPPUA 0x0C
#define MCPPUB 0x0D

// Port Register
#define MCPPORTA 0x12
#define MCPPORTB 0x13

// Interrupt registers
#define MCPINTENA 0x04    // Enable pin for interrupt on change (GPINTEN)
#define MCPINTCONA 0x08   // Configure interrupt: 0 = compare against previous; 1 = compare against DEFVAL
#define MCPINTFA 0x0E     // Flag that interrupt ocurred on pin (read-only)
#define MCPINTCAPA 0x10   // Read the status of the pin (read-only)
#define MCPINTPINA 2      // Pin assigned to catch MCP23S17 INTA on the Arduino controller

// MCP opcodes
#define MCPWRITE 0b01000000
#define MCPREAD  0b01000001

void mcpInit();
uint8_t mcpByteRead(uint8_t reg);
void mcpByteWrite(uint8_t reg, uint8_t db);
uint8_t mcpDigitalRead(uint8_t pin);
void mcpInterruptsEn();
void mcpIntHandler();
uint8_t getMcpIntAReg();

#endif // AR488_MCP23S17
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MCP23S17 IC (SPI) IC LAYOUT DEFINITION *****/
/**************************************************/



/********************************************************/
/***** PANDUINO/MIGHTYCORE MCGRAW LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA644P_MCGRAW

#define DIO1_PIN  10   /* GPIB 1  */
#define DIO2_PIN  11   /* GPIB 2  */
#define DIO3_PIN  12   /* GPIB 3  */
#define DIO4_PIN  13   /* GPIB 4  */
#define DIO5_PIN  14   /* GPIB 13 */
#define DIO6_PIN  15   /* GPIB 14 */
#define DIO7_PIN  16   /* GPIB 15 */
#define DIO8_PIN  17   /* GPIB 16 */

#define IFC_PIN   22   /* GPIB 9  */
#define NDAC_PIN  21   /* GPIB 8  */
#define NRFD_PIN  20   /* GPIB 7  */
#define DAV_PIN   19   /* GPIB 6  */
#define EOI_PIN   18   /* GPIB 5  */

#define SRQ_PIN   23   /* GPIB 10 */
#define REN_PIN   24   /* GPIB 17 */
#define ATN_PIN   31   /* GPIB 11 */

#endif // AR488_MEGA644P_MCGRAW
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** PANDUINO/MIGHTYCORE MCGRAW LAYOUT DEFINITION *****/
/********************************************************/



/******************************************/
/***** POLOLU A-STAR 328PB ALT LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_328PB_ALT

/***** NOTE: UNO/NANO pinout *****/
#define DIO1_PIN  A0  /* GPIB 1  : PORTC bit 0 */
#define DIO2_PIN  A1  /* GPIB 2  : PORTC bit 1 */
#define DIO3_PIN  A2  /* GPIB 3  : PORTC bit 2 */
#define DIO4_PIN  A3  /* GPIB 4  : PORTC bit 3 */
#define DIO5_PIN  A4  /* GPIB 13 : PORTC bit 4 */
#define DIO6_PIN  A5  /* GPIB 14 : PORTC bit 5 */
#define DIO7_PIN  A6  /* GPIB 15 : PORTD bit 4 */
#define DIO8_PIN  A7  /* GPIB 16 : PORTD bit 5 */

#define IFC_PIN    5  /* GPIB 9  : PORTD bit 5 */
#define NDAC_PIN   6  /* GPIB 8  : PORTD bit 6 */
#define NRFD_PIN   7  /* GPIB 7  : PORTD bit 7 */
#define DAV_PIN    8  /* GPIB 6  : PORTB bit 0 */
#define EOI_PIN    9  /* GPIB 5  : PORTB bit 1 */

#define SRQ_PIN    3  /* GPIB 10 : PORTD bit 3 */
#define REN_PIN    2  /* GPIB 17 : PORTD bit 2 */
#define ATN_PIN    4  /* GPIB 11 : PORTD bit 4 */

#endif
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** POLOLU A-STAR 328PB ALT LAYOUT *****/
/******************************************/



/*************************************/
/***** POE_ETHERNET_GPIB_ADAPTOR *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef POE_ETHERNET_GPIB_ADAPTOR

uint8_t reverseBits(uint8_t dbyte);
uint8_t readPortPullupReg(PORT_t& port);
void setPortPullupBits(PORT_t& port, uint8_t reg);

/***** KOFEN's POE Ethernet Gpib Adaptor pinout *****/
#define DIO1_PIN  22  /* GPIB 1  : PORTD bit 0 */
#define DIO2_PIN  23  /* GPIB 2  : PORTD bit 1 */
#define DIO3_PIN  24  /* GPIB 3  : PORTD bit 2 */
#define DIO4_PIN  24  /* GPIB 4  : PORTD bit 3 */
#define DIO5_PIN  26  /* GPIB 13 : PORTD bit 4 */
#define DIO6_PIN  27  /* GPIB 14 : PORTD bit 5 */
#define DIO7_PIN  28  /* GPIB 15 : PORTD bit 4 */
#define DIO8_PIN  29  /* GPIB 16 : PORTD bit 5 */

#define IFC_PIN   18  /* GPIB 9  : PORTC bit 0 */
#define NDAC_PIN  17  /* GPIB 8  : PORTC bit 1 */
#define NRFD_PIN  16  /* GPIB 8  : PORTC bit 2 */
#define DAV_PIN   15  /* GPIB 6  : PORTC bit 3 */
#define EOI_PIN   14  /* GPIB 5  : PORTC bit 4 */
#define REN_PIN   21  /* GPIB 17 : PORTC bit 5 */
#define SRQ_PIN   19  /* GPIB 10 : PORTC bit 6 */
#define ATN_PIN   20  /* GPIB 11 : PORTC bit 7 */

#endif  // POE_ETHERNET_GPIB_ADAPTOR
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** POE_ETHERNET_GPIB_ADAPTOR *****/
/*************************************/



/***********************************/
/***** ESP32 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_DEVKIT1_WROOM

// Pin numbers represent GPIOnum

#define ESP32_NATIVE_FUNC

#define DIO1_PIN  32   /* GPIB 1  */
#define DIO2_PIN  33   /* GPIB 2  */
#define DIO3_PIN  25   /* GPIB 3  */
#define DIO4_PIN  26   /* GPIB 4  */
#define DIO5_PIN  27   /* GPIB 13 */
#define DIO6_PIN  14   /* GPIB 14 */
#define DIO7_PIN   4   /* GPIB 15 */
#define DIO8_PIN  13   /* GPIB 16 */

#define IFC_PIN   23   /* GPIB 9  */
#define NDAC_PIN  22   /* GPIB 8  */
#define NRFD_PIN  21   /* GPIB 7  */
#define DAV_PIN   19   /* GPIB 6  */
#define EOI_PIN   18   /* GPIB 5  */

#define SRQ_PIN    5   /* GPIB 10 */
#define REN_PIN   17   /* GPIB 17 */
#define ATN_PIN   16   /* GPIB 11 */

#endif // ESP32_DEVKIT1_WROOM_32
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32 LAYOUT DEFINITION *****/
/***********************************/



/***********************************************/
/***** ESP32_TTGO_T8_161 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_TTGO_T8_161

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN  34   /* GPIB 1  */
#define DIO2_PIN  35   /* GPIB 2  */
#define DIO3_PIN  32   /* GPIB 3  */
#define DIO4_PIN  33   /* GPIB 4  */
#define DIO5_PIN  25   /* GPIB 13 */
#define DIO6_PIN  26   /* GPIB 14 */
#define DIO7_PIN  27   /* GPIB 15 */
#define DIO8_PIN  14   /* GPIB 16 */

#define IFC_PIN   15   /* GPIB 9  */
#define NDAC_PIN  22   /* GPIB 8  */
#define NRFD_PIN  19   /* GPIB 7  */
#define DAV_PIN   23   /* GPIB 6  */
#define EOI_PIN   18   /* GPIB 5  */

#define SRQ_PIN    2   /* GPIB 10 */
#define REN_PIN   13   /* GPIB 17 */
#define ATN_PIN    5   /* GPIB 11 */

#endif // ESP32_TTGO_T8_161
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32_TTGO_T8_161 LAYOUT DEFINITION *****/
/****************************************************/



/********************************************/
/***** ESP32_ESP32DEV LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_ESP32DEV

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN  33   /* GPIB 1  */
#define DIO2_PIN  32   /* GPIB 2  */
#define DIO3_PIN  26   /* GPIB 3  */
#define DIO4_PIN  25   /* GPIB 4  */
#define DIO5_PIN  14   /* GPIB 13 */
#define DIO6_PIN  27   /* GPIB 14 */
#define DIO7_PIN  13   /* GPIB 15 */
#define DIO8_PIN  12   /* GPIB 16 */

#define IFC_PIN   22   /* GPIB 9  */
#define NDAC_PIN  21   /* GPIB 8  */
#define NRFD_PIN  19   /* GPIB 7  */
#define DAV_PIN   18   /* GPIB 6  */
#define EOI_PIN   17   /* GPIB 5  */

#define SRQ_PIN   16   /* GPIB 10 */
#define REN_PIN   23   /* GPIB 17 */
#define ATN_PIN    4   /* GPIB 11 */

#endif // ESP32_ESP32DEV
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32_ESP32DEV LAYOUT DEFINITION *****/
/********************************************/



/***********************************************/
/***** ESP32_LOLIN32_161 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_LOLIN32_161

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN  32   /* GPIB 1  */
#define DIO2_PIN  33   /* GPIB 2  */
#define DIO3_PIN  25   /* GPIB 3  */
#define DIO4_PIN  26   /* GPIB 4  */
#define DIO5_PIN  27   /* GPIB 13 */
#define DIO6_PIN  14   /* GPIB 14 */
#define DIO7_PIN  12   /* GPIB 15 */
#define DIO8_PIN  13   /* GPIB 16 */

#define IFC_PIN   21   /* GPIB 9  */
#define NDAC_PIN  19   /* GPIB 8  */
#define NRFD_PIN  23   /* GPIB 7  */
#define DAV_PIN   18   /* GPIB 6  */
#define EOI_PIN   17   /* GPIB 5  */

#define SRQ_PIN    4   /* GPIB 10 */
#define REN_PIN   22   /* GPIB 17 */
#define ATN_PIN   16   /* GPIB 11 */

#endif // ESP32_LOLIN32_161
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32_LOLIN32_161 LAYOUT DEFINITION *****/
/***********************************************/



/***********************************************/
/***** ESP32_S2_161 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_S2_161

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN   1   /* GPIB 1  */
#define DIO2_PIN   2   /* GPIB 2  */
#define DIO3_PIN   3   /* GPIB 3  */
#define DIO4_PIN   4   /* GPIB 4  */
#define DIO5_PIN   5   /* GPIB 13 */
#define DIO6_PIN   6   /* GPIB 14 */
#define DIO7_PIN   7   /* GPIB 15 */
#define DIO8_PIN   8   /* GPIB 16 */

#define IFC_PIN   10   /* GPIB 9  */
#define NDAC_PIN  11   /* GPIB 8  */
#define NRFD_PIN  12   /* GPIB 7  */
#define DAV_PIN   13   /* GPIB 6  */
#define EOI_PIN   14   /* GPIB 5  */

#define SRQ_PIN   16   /* GPIB 10 */
#define REN_PIN    9   /* GPIB 17 */
#define ATN_PIN   15   /* GPIB 11 */

#endif // ESP32_S2_161
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32_S2_161 LAYOUT DEFINITION *****/
/***********************************************/



/*************************************************************/
/***** ESP32_Wilhelm_AR488_ESP32S2_Rx LAYOUT DEFINITIONS *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef ESP32_Wilhelm_AR488_ESP32S2_R4

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN   8    /* GPIB  8 */
#define DIO2_PIN   9    /* GPIB  9 */
#define DIO3_PIN   10   /* GPIB 10 */
#define DIO4_PIN   11   /* GPIB 11 */
#define DIO5_PIN   12   /* GPIB 12 */
#define DIO6_PIN   13   /* GPIB 13 */
#define DIO7_PIN   14   /* GPIB 14 */
#define DIO8_PIN   15   /* GPIB 15 */

#define IFC_PIN    38   /* GPIB 34 */
#define NDAC_PIN   33   /* GPIB 33 */
#define NRFD_PIN   34   /* GPIB 34 */
#define DAV_PIN    40   /* GPIB 40 */
#define EOI_PIN    35   /* GPIB 35 */

#define SRQ_PIN    37   /* GPIB 37 */
#define REN_PIN    26   /* GPIB 21 */
#define ATN_PIN    36   /* GPIB 36 */

#endif // ESP32_Wilhelm_AR488_ESP32S2_R4

#ifdef ESP32_Wilhelm_AR488_ESP32S2_R5

#define ESP32_ARDUINO_FUNC

#define DIO1_PIN   8    /* GPIB  8 */
#define DIO2_PIN   9    /* GPIB  9 */
#define DIO3_PIN   10   /* GPIB 10 */
#define DIO4_PIN   11   /* GPIB 11 */
#define DIO5_PIN   12   /* GPIB 12 */
#define DIO6_PIN   13   /* GPIB 13 */
#define DIO7_PIN   14   /* GPIB 14 */
#define DIO8_PIN   15   /* GPIB 15 */

#define IFC_PIN     4   /* GPIB  4 */
#define NDAC_PIN   33   /* GPIB 33 */
#define NRFD_PIN   34   /* GPIB 34 */
#define DAV_PIN     5   /* GPIB  5 */
#define EOI_PIN    35   /* GPIB 35 */

#define SRQ_PIN    37   /* GPIB 37 */
#define REN_PIN    21   /* GPIB 21 */
#define ATN_PIN    36   /* GPIB 36 */

#endif // ESP32_Wilhelm_AR488_ESP32S2_R5
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32_Wilhelm_AR488_ESP32S2_Rx LAYOUT DEFINITIONS *****/
/*************************************************************/



/***************************************/
/***** RAS PICO LAYOUT 1 DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L1

void gpioFuncList();

#define DIO1_PIN   6   /* GPIB 1  */
#define DIO2_PIN   7   /* GPIB 2  */
#define DIO3_PIN   8   /* GPIB 3  */
#define DIO4_PIN   9   /* GPIB 4  */
#define DIO5_PIN  10   /* GPIB 13 */
#define DIO6_PIN  11   /* GPIB 14 */
#define DIO7_PIN  12   /* GPIB 15 */
#define DIO8_PIN  13   /* GPIB 16 */

#define IFC_PIN   14   /* GPIB 9  */
#define NDAC_PIN  15   /* GPIB 8  */
#define NRFD_PIN  16   /* GPIB 7  */
#define DAV_PIN   17   /* GPIB 6  */
#define EOI_PIN   18   /* GPIB 5  */

#define REN_PIN   19   /* GPIB 17 */
#define SRQ_PIN   20   /* GPIB 10 */
#define ATN_PIN   21   /* GPIB 11 */

#endif // RAS_PICO_L1
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO LAYOUT 1 DEFINITION *****/
/****************************************/



/****************************************/
/***** RAS PICO LAYOUT 2 DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L2

void gpioFuncList();

#define DIO1_PIN  14   /* GPIB 1  */
#define DIO2_PIN  15   /* GPIB 2  */
#define DIO3_PIN  16   /* GPIB 3  */
#define DIO4_PIN  17   /* GPIB 4  */
#define DIO5_PIN  18   /* GPIB 13 */
#define DIO6_PIN  19   /* GPIB 14 */
#define DIO7_PIN  20   /* GPIB 15 */
#define DIO8_PIN  21   /* GPIB 16 */

#define IFC_PIN    6   /* GPIB 9  */
#define NDAC_PIN   7   /* GPIB 8  */
#define NRFD_PIN   8   /* GPIB 7  */
#define DAV_PIN    9   /* GPIB 6  */
#define EOI_PIN   10   /* GPIB 5  */

#define REN_PIN   11   /* GPIB 17 */
#define SRQ_PIN   12   /* GPIB 10 */
#define ATN_PIN   13   /* GPIB 11 */

#endif // RAS_PICO_L2
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO LAYOUT 2 DEFINITION *****/
/****************************************/



/****************************************/
/***** RAS PICO LAYOUT 3 DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L3

void gpioFuncList();

#define DIO1_PIN   2   /* GPIB 1  */
#define DIO2_PIN   3   /* GPIB 2  */
#define DIO3_PIN   4   /* GPIB 3  */
#define DIO4_PIN   5   /* GPIB 4  */
#define DIO5_PIN   6   /* GPIB 13 */
#define DIO6_PIN   7   /* GPIB 14 */
#define DIO7_PIN   8   /* GPIB 15 */
#define DIO8_PIN   9   /* GPIB 16 */

#define IFC_PIN   10   /* GPIB 9  */
#define NDAC_PIN  11   /* GPIB 8  */
#define NRFD_PIN  12   /* GPIB 7  */
#define DAV_PIN   13   /* GPIB 6  */
#define EOI_PIN   14   /* GPIB 5  */

#define REN_PIN   15   /* GPIB 17 */
#define SRQ_PIN   20   /* GPIB 10 */
#define ATN_PIN   21   /* GPIB 11 */

#endif // RAS_PICO_L3
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO LAYOUT 3 DEFINITION *****/
/****************************************/



/****************************************/
/***** RAS PICO LAYOUT 4 DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L4

void gpioFuncList();

#define DIO1_PIN  10   /* GPIB 1  */
#define DIO2_PIN  11   /* GPIB 2  */
#define DIO3_PIN  12   /* GPIB 3  */
#define DIO4_PIN  13   /* GPIB 4  */
#define DIO5_PIN  14   /* GPIB 13 */
#define DIO6_PIN  15   /* GPIB 14 */
#define DIO7_PIN  20   /* GPIB 15 */
#define DIO8_PIN  21   /* GPIB 16 */

#define IFC_PIN    2   /* GPIB 9  */
#define NDAC_PIN   3   /* GPIB 8  */
#define NRFD_PIN   4   /* GPIB 7  */
#define DAV_PIN    5   /* GPIB 6  */
#define EOI_PIN    6   /* GPIB 5  */
#define REN_PIN    7   /* GPIB 17 */
#define SRQ_PIN    8   /* GPIB 10 */
#define ATN_PIN    9   /* GPIB 11 */

#endif // RAS_PICO_L4
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO LAYOUT 4 DEFINITION *****/
/****************************************/



/****************************************/
/***** RAS PICO LAYOUT 5 DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L5

#define RAS_PICO_LAUTO

void gpioFuncList();

#define DIO1_PIN  10   /* GPIB 1  */
#define DIO2_PIN  11   /* GPIB 2  */
#define DIO3_PIN  12   /* GPIB 3  */
#define DIO4_PIN  13   /* GPIB 4  */
#define DIO5_PIN  14   /* GPIB 13 */
#define DIO6_PIN  15   /* GPIB 14 */
#define DIO7_PIN  20   /* GPIB 15 */
#define DIO8_PIN  21   /* GPIB 16 */

#define IFC_PIN    9   /* GPIB 9  */
#define NDAC_PIN   8   /* GPIB 8  */
#define NRFD_PIN   7   /* GPIB 7  */
#define DAV_PIN    6   /* GPIB 6  */
#define EOI_PIN    5   /* GPIB 5  */
#define REN_PIN    3   /* GPIB 17 */
#define SRQ_PIN    4   /* GPIB 10 */
#define ATN_PIN    2   /* GPIB 11 */

#endif // RAS_PICO_L5
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO LAYOUT 5 DEFINITION *****/
/****************************************/



/*********************************************/
/***** NANO R4 RENESAS LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RA4M1_NANO_R4

/* NOTE: Renesas RA4M1 boards work only with SN7516x buffer chips */

/***** UNO/NANO R4 (Renesas) pinout *****/
#define DIO1_PIN  14  /* GPIB 1  : P014 */
#define DIO2_PIN  15  /* GPIB 2  : P000 */
#define DIO3_PIN  16  /* GPIB 3  : P001 */
#define DIO4_PIN  17  /* GPIB 4  : P002 */
#define DIO5_PIN  18  /* GPIB 13 : P101 */
#define DIO6_PIN  19  /* GPIB 14 : P100 */
#define DIO7_PIN   4  /* GPIB 15 : P103 */
#define DIO8_PIN   5  /* GPIB 16 : P102 */

#define IFC_PIN    8  /* GPIB 9  : P303 */
#define NDAC_PIN   9  /* GPIB 8  : P304 */
#define NRFD_PIN  10  /* GPIB 7  : P107 */
#define DAV_PIN   11  /* GPIB 6  : P106 */
#define EOI_PIN   12  /* GPIB 5  : P102 */

#define SRQ_PIN    2  /* GPIB 10 : P105 */
#define REN_PIN    3  /* GPIB 17 : P104 */
#define ATN_PIN    7  /* GPIB 11 : P109 */

#endif  // RA4M1_NANO_R4
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** NANO R4 RENESAS LAYOUT DEFINITION *****/
/*********************************************/



/****************************************/
/***** TEENSY 4.1 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef IMXRT1062_TEENSY41_01

/***** NOTE: Teensy 4.1 board *****/
#define DIO1_PIN  23  /* GPIB 1  : AD_B1_09 : GPIO1_25 */
#define DIO2_PIN  22  /* GPIB 2  : AD_B1_08 : GPIO1_24 */
#define DIO3_PIN  21  /* GPIB 3  : AD_B1_11 : GPIO1_27 */
#define DIO4_PIN  20  /* GPIB 4  : AD_B1_10 : GPIO1_26 */
#define DIO5_PIN  19  /* GPIB 13 : AD_B1_00 : GPIO1_16 */
#define DIO6_PIN  18  /* GPIB 14 : AD_B1_01 : GPIO1_17 */
#define DIO7_PIN  17  /* GPIB 15 : AD_B1_06 : GPIO1_22 */
#define DIO8_PIN  16  /* GPIB 16 : AD_B1_07 : GPIO1_23 */

#define IFC_PIN   15  /* GPIB 9  : AD_B1_03 : GPIO1_19 */
#define NDAC_PIN  14  /* GPIB 8  : AD_B1_02 : GPIO1_18 */
#define NRFD_PIN  41  /* GPIB 7  : AD_B1_05 : GPIO1_21 */
#define DAV_PIN   40  /* GPIB 6  : AD_B1_04 : GPIO1_20 */
#define EOI_PIN   39  /* GPIB 5  : AD_B1_13 : GPIO1_29 */

#define SRQ_PIN   38  /* GPIB 10 : AD_B1_12 : GPIO1_28 */
#define REN_PIN   26  /* GPIB 17 : AD_B1_14 : GPIO1_30 */
#define ATN_PIN   27  /* GPIB 11 : AD_B1_15 : GPIO1_31 */

#define PIN_PULLUP_ENABLE   IOMUXC_PAD_PUS(3) | IOMUXC_PAD_PUE | IOMUXC_PAD_PKE | IOMUXC_PAD_DSE(6)
#define PIN_PULLUP_DISABLE  ~IOMUXC_PAD_PUE & ~IOMUXC_PAD_PKE
#define PIN_SION_ENABLE  (1UL<<4)
#define PIN_SION_DISABLE  ~(1UL<<4)

#endif  // IMXRT1062_TEENSY41_01 
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** TEENSY 4.1 LAYOUT DEFINITION *****/
/****************************************/



/**************************************/
/***** GLOBAL DEFINITIONS SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvv *****/

#if defined (AR488_CUSTOM)
  #if defined (ESP32)
    #define ESP32_NATIVE_FUNC
  #elif defined (ARDUINO_ARCH_RP2040)
    #define RAS_PICO_LAUTO
  #else
    #define AR488_CUSTOM_DEFAULT
  #endif
#endif

void readyGpibDbus(uint8_t state);
uint8_t readGpibDbus();
void setGpibDbus(uint8_t db);
//void setGpibState(uint8_t bits, uint8_t mask, uint8_t mode);
void setGpibCtrlState(uint8_t bits, uint8_t mask);
void setGpibCtrlDir(uint8_t bits, uint8_t mask);
uint8_t getGpibPinState(uint8_t pin);

#ifdef LEVEL_SHIFTER
  void initLevelShifter();
  void shiftEnable(bool stat);
#endif

#if defined(ARDUINO_ARCH_RP2040)
  void initRpGpioPins();
#endif

#if defined(ESP32)
  void initEspGpioPins();
#endif

#if defined(__IMXRT1062__) && not defined(AR488_CUSTOM) // Teensy 4.1
  void initTsyGpioPins();
#endif

/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** GLOBAL DEFINITIONS SECTION *****/
/**************************************/


#endif // AR488_LAYOUTS_H
