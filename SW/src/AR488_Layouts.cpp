#include <Arduino.h>

#include "AR488_Config.h"
#include "AR488_Layouts.h"

/***** AR488_Hardware.cpp, ver. 0.53.39, 29/01/2026 *****/

///=================================================///
///       Hardware layout function definitions      ///
///=================================================///

/*********************************/
/***** UNO/NANO BOARD LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvv *****/
#if defined(AR488_UNO) || defined(AR488_NANO)

/***** Control pin map *****/
/*
  Data pin map
  ------------
  DIO1_PIN  A0 : GPIB 1  : PC0
  DIO2_PIN  A1 : GPIB 2  : PC1
  DIO3_PIN  A2 : GPIB 3  : PC2
  DIO4_PIN  A3 : GPIB 4  : PC3
  DIO5_PIN  A4 : GPIB 13 : PC4
  DIO6_PIN  A5 : GPIB 14 : PC5
  DIO7_PIN   4 : GPIB 15 : PD4
  DIO8_PIN   5 : GPIB 16 : PD5

  Control pin map
  ---------------
  IFC_PIN   8  : GPIB  9 : PB0 : b0
  NDAC_PIN  9  : GPIB  8 : PB1 : b1
  NRFD_PIN  10 : GPIB  7 : PB2 : b2
  DAV_PIN   11 : GPIB  6 : PB3 : b3
  EOI_PIN   12 : GPIB  5 : PB4 : b4
  REN_PIN   3  : GPIB 17 : PD2 : b5
  SRQ_PIN   2  : GPIB 10 : PD2 : b6
  ATN_PIN   7  : GPIB 11 : PD7 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=LOW, 1=HIGH
    mask : 0=unaffected, 1=affected
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRD |= 0b00110000;
    DDRC |= 0b00111111;
    PORTD |= 0b00110000; // PORTD bits 5,4 high
    PORTC |= 0b00111111; // PORTC bits 5,4,3,2,1,0 high
    return;
  }

  // Set data pins to input
  DDRD &= 0b11001111 ;
  DDRC &= 0b11000000 ;
  PORTD |= 0b00110000; // PORTD bits 5,4 input_pullup
  PORTC |= 0b00111111; // PORTC bits 5,4,3,2,1,0 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~((PIND << 2 & 0b11000000) + (PINC & 0b00111111));
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set data pins as outputs
  DDRD |= 0b00110000;
  DDRC |= 0b00111111;

  // GPIB states are inverted
  db = ~db;

  // Set data bus
  PORTC = (PORTC & ~0b00111111) | (db & 0b00111111);
  PORTD = (PORTD & ~0b00110000) | ((db & 0b11000000) >> 2);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {
  if (!mask) return;  // Empty mask does nothing!
  // PORTB - use only the first (right-most) 5 bits (pins 8-12)
  uint8_t portBb = bits & 0x1F;
  uint8_t portBm = mask & 0x1F;
  // PORT D - keep bit 7, rotate bit 6 right 4 positions to set bit 2 on register
  uint8_t portDb = (bits & 0x80) + ((bits & 0x40) >> 4) + ((bits & 0x20) >> 2);
  uint8_t portDm = (mask & 0x80) + ((mask & 0x40) >> 4) + ((mask & 0x20) >> 2);

  // Set pin states using mask
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
  PORTD = ( (PORTD & ~portDm) | (portDb & portDm) );
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {
  // PORTB - use only the first (right-most) 5 bits (pins 8-12)
  uint8_t portBb = bits & 0x1F;
  uint8_t portBm = mask & 0x1F;
  // PORT D - keep bit 7, rotate bit 6 right 4 positions to set bit 2 on register
  uint8_t portDb = (bits & 0x80) + ((bits & 0x40) >> 4) + ((bits & 0x20) >> 2);
  uint8_t portDm = (mask & 0x80) + ((mask & 0x40) >> 4) + ((mask & 0x20) >> 2);

  uint8_t pmask;

  // Set pin direction registers using mask
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
  DDRD = ( (DDRD & ~portDm) | (portDb & portDm) );

  // Set inputs to input_pullup
  pmask = (~portBb & portBm);
  PORTB = ( (PORTB & ~pmask) | pmask );
  pmask = (~portDb & portDm);
  PORTD = ( (PORTD & ~pmask) | pmask );

}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //AR488UNO/AR488_NANO
/***** ^^^^^^^^^^^^^^^^^^^^^ *****/
/***** UNO/NANO BOARD LAYOUT *****/
/*********************************/



/*******************************************/
/***** MEGA2560 BOARD LAYOUT (Default) *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_D
/*
  Data pin map
  ------------
  DIO1_PIN  A0 : GPIB 1  : PF0
  DIO2_PIN  A1 : GPIB 2  : PF1
  DIO3_PIN  A2 : GPIB 3  : PF2
  DIO4_PIN  A3 : GPIB 4  : PF3
  DIO5_PIN  A4 : GPIB 13 : PF4
  DIO6_PIN  A5 : GPIB 14 : PF5
  DIO7_PIN  A6 : GPIB 15 : PF6
  DIO8_PIN  A7 : GPIB 16 : PF7

  Control pin map
  ---------------
  IFC_PIN   17 : GPIB  9 : PB0 : b0
  NDAC_PIN  16 : GPIB  8 : PB1 : b1
  NRFD_PIN   6 : GPIB  7 : PB2 : b2
  DAV_PIN    7 : GPIB  6 : PB3 : b3
  EOI_PIN    8 : GPIB  5 : PB4 : b4
  REN_PIN   10 : GPIB 17 : PD2 : b5
  SRQ_PIN    9 : GPIB 10 : PD2 : b6
  ATN_PIN   11 : GPIB 11 : PD7 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/

/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRF |= 0b00000000;
    PORTF |= 0b11111111; // set PORTF bits to high
    return;
  }

  DDRF &= 0b00000000 ;
  PORTF |= 0b11111111; // set PORTF bits to input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~(PINF & 0b11111111);
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set data pins as outputs
  DDRF |= 0b11111111;

  // Set data bus
  PORTF = ~db;
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // PORT H - keep bits 5-0. Move bits 5-2 left 1 position to set bits 6-3 and 1-0 on port
  uint8_t portHb = ((bits & 0x3C) << 1) + (bits & 0x03);
  uint8_t portHm = ((mask & 0x3C) << 1) + (mask & 0x03);

  // PORT B - keep bits 7 and 6, but rotate right 2 postions to set bits 5 and 4 on port 
  uint8_t portBb = ((bits & 0xC0) >> 2);
  uint8_t portBm = ((mask & 0xC0) >> 2);
 
  // Set pin states using mask
  PORTH = ( (PORTH & ~portHm) | (portHb & portHm) );
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // PORT H - keep bits 5-0. Move bits 5-2 left 1 position to set bits 6-3 and 1-0 on port
  uint8_t portHb = ((bits & 0x3C) << 1) + (bits & 0x03);
  uint8_t portHm = ((mask & 0x3C) << 1) + (mask & 0x03);

  // PORT B - keep bits 7 and 6, but rotate right 2 postions to set bits 5 and 4 on port 
  uint8_t portBb = ((bits & 0xC0) >> 2);
  uint8_t portBm = ((mask & 0xC0) >> 2);
 
  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin direction registers using mask
  DDRH = ( (DDRH & ~portHm) | (portHb & portHm) );
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //MEGA2560
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 BOARD LAYOUT (Default) *****/
/*******************************************/



/************************************/
/***** MEGA2560 BOARD LAYOUT E1 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_E1
/*
  Data pin map
  ------------
  DIO1_PIN  30 : GPIB 1  : PC1
  DIO2_PIN  32 : GPIB 2  : PC3
  DIO3_PIN  34 : GPIB 3  : PC5
  DIO4_PIN  36 : GPIB 4  : PC7
  DIO5_PIN  22 : GPIB 13 : PA6
  DIO6_PIN  24 : GPIB 14 : PA4
  DIO7_PIN  26 : GPIB 15 : PA2
  DIO8_PIN  28 : GPIB 16 : PA0

  Control pin map
  ---------------
  IFC_PIN   48 : GPIB  9 : PL1 : b0
  NDAC_PIN  48 : GPIB  8 : PL3 : b1
  NRFD_PIN  44 : GPIB  7 : PL5 : b2
  DAV_PIN   42 : GPIB  6 : PL7 : b3
  EOI_PIN   40 : GPIB  5 : PG1 : b4
  REN_PIN   50 : GPIB 17 : PB3 : b5
  SRQ_PIN   38 : GPIB 10 : PD7 : b6
  ATN_PIN   52 : GPIB 11 : PB1 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRA |= 0b01010101;
    DDRC |= 0b10101010;
    PORTA |= 0b01010101; // PORTA bits 6,4,2,0 high
    PORTC |= 0b10101010; // PORTC bits 7,5,3,1 high
    return;
  }

  // Set data pins to input
  DDRA &= 0b10101010 ;
  DDRC &= 0b01010101 ;
  PORTA |= 0b01010101; // PORTA bits 6,4,2,0 input_pullup
  PORTC |= 0b10101010; // PORTC bits 7,5,3,1 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  uint8_t val = 0;

  // Read the byte of data on the bus (GPIB states are inverted)
  val = ~((PINA & 0b01010101) + (PINC & 0b10101010));

  db |= (((val >> 1) & 1)<<3);
  db |= (((val >> 3) & 1)<<2);
  db |= (((val >> 5) & 1)<<1);
  db |= (((val >> 7) & 1)<<0);

  db |= (((val >> 6) & 1)<<7);
  db |= (((val >> 4) & 1)<<6);
  db |= (((val >> 2) & 1)<<5);
  db |= (((val >> 0) & 1)<<4);

  return db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint8_t val = 0;
  
  // Set data pins as outputs
  DDRA |= 0b01010101 ;
  DDRC |= 0b10101010 ;

  // GPIB states are inverted
  db = ~db;

  val |= (((db >> 3) & 1)<<1);
  val |= (((db >> 2) & 1)<<3);
  val |= (((db >> 1) & 1)<<5);
  val |= (((db >> 0) & 1)<<7);

  val |= (((db >> 7) & 1)<<6);
  val |= (((db >> 6) & 1)<<4);
  val |= (((db >> 5) & 1)<<2);
  val |= (((db >> 4) & 1)<<0);

  // Set data bus
  PORTA = (PORTA & ~0b01010101) | (val & 0b01010101);
  PORTC = (PORTC & ~0b10101010) | (val & 0b10101010);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // PORT B
  uint8_t portBb = (((bits >> 7 & 1))<<1) + (((bits >> 6 & 1))<<3);
  uint8_t portBm = (((mask >> 7 & 1))<<1) + (((mask >> 6 & 1))<<3);

  // PORT D
  uint8_t portDb = (((bits >> 5 & 1))<<7);
  uint8_t portDm = (((mask >> 5 & 1))<<7);

  // PORT G
  uint8_t portGb = (((bits >> 4 & 1))<<1);
  uint8_t portGm = (((mask >> 4 & 1))<<1);

  // PORT L
  uint8_t portLb = (((bits >> 0 & 1))<<1) + (((bits >> 1 & 1))<<3) + (((bits >> 2 & 1))<<5) + (((bits >> 3 & 1))<<7);
  uint8_t portLm = (((mask >> 0 & 1))<<1) + (((mask >> 1 & 1))<<3) + (((mask >> 2 & 1))<<5) + (((mask >> 3 & 1))<<7);

  // Set PORTs using mask to avoid affecting bits that should not be affected
  // and calculated and masked port byte
  // PORT B - bits 7 & 6 (ATN_PIN + SRQ_PIN)
  // PORT D - bit 5 (REN_PIN)
  // PORT G - bit 4 (EOI_PIN)
  // PORT L - bits 1,3,5,7 (IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN)
  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin states using mask
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
  PORTD = ( (PORTD & ~portDm) | (portDb & portDm) );
  PORTG = ( (PORTG & ~portGm) | (portGb & portGm) );
  PORTL = ( (PORTL & ~portLm) | (portLb & portLm) );
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // PORT B
  uint8_t portBb = (((bits >> 7 & 1))<<1) + (((bits >> 6 & 1))<<3);
  uint8_t portBm = (((mask >> 7 & 1))<<1) + (((mask >> 6 & 1))<<3);

  // PORT D
  uint8_t portDb = (((bits >> 5 & 1))<<7);
  uint8_t portDm = (((mask >> 5 & 1))<<7);

  // PORT G
  uint8_t portGb = (((bits >> 4 & 1))<<1);
  uint8_t portGm = (((mask >> 4 & 1))<<1);

  // PORT L
  uint8_t portLb = (((bits >> 0 & 1))<<1) + (((bits >> 1 & 1))<<3) + (((bits >> 2 & 1))<<5) + (((bits >> 3 & 1))<<7);
  uint8_t portLm = (((mask >> 0 & 1))<<1) + (((mask >> 1 & 1))<<3) + (((mask >> 2 & 1))<<5) + (((mask >> 3 & 1))<<7);

  // Set PORTs using mask to avoid affecting bits that should not be affected
  // and calculated and masked port byte
  // PORT B - bits 7 & 6 (ATN_PIN + SRQ_PIN)
  // PORT D - bit 5 (REN_PIN)
  // PORT G - bit 4 (EOI_PIN)
  // PORT L - bits 1,3,5,7 (IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN)
  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin direction registers using mask
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
  DDRD = ( (DDRD & ~portDm) | (portDb & portDm) );
  DDRG = ( (DDRG & ~portGm) | (portGb & portGm) );
  DDRL = ( (DDRL & ~portLm) | (portLb & portLm) );
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //MEGA2560
/***** ^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 BOARD LAYOUT E1 *****/
/************************************/



/************************************/
/***** MEGA2560 BOARD LAYOUT E2 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA2560_E2
/*
  Data pin map
  ------------
  DIO1_PIN  37 : GPIB 1  : PA1
  DIO2_PIN  35 : GPIB 2  : PA3
  DIO3_PIN  33 : GPIB 3  : PA5
  DIO4_PIN  31 : GPIB 4  : PA7
  DIO5_PIN  29 : GPIB 13 : PC6
  DIO6_PIN  27 : GPIB 14 : PC4
  DIO7_PIN  25 : GPIB 15 : PC2
  DIO8_PIN  23 : GPIB 16 : PC0

  Control pin map
  ---------------
  IFC_PIN   49 : GPIB  9 : PG0 : b0
  NDAC_PIN  47 : GPIB  8 : PG2 : b1
  NRFD_PIN  45 : GPIB  7 : PL6 : b2
  DAV_PIN   43 : GPIB  6 : PL4 : b3
  EOI_PIN   41 : GPIB  5 : PL2 : b4
  REN_PIN   51 : GPIB 17 : PB0 : b5
  SRQ_PIN   39 : GPIB 10 : PL0 : b6
  ATN_PIN   53 : GPIB 11 : PB2 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRA |= 0b10101010;
    DDRA |= 0b01010101;
    PORTA |= 0b10101010; // PORTC bits 7,5,3,1 high
    PORTC |= 0b01010101; // PORTA bits 6,4,2,0 high
    return;
  }

  // Set data pins to input
  DDRA &= 0b01010101 ;
  DDRC &= 0b10101010 ;
  PORTA |= 0b10101010; // PORTC bits 7,5,3,1 input_pullup
  PORTC |= 0b01010101; // PORTA bits 6,4,2,0 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  uint8_t val = 0;
  
  // Read the byte of data on the bus (GPIB states are inverted)
  val = ~((PINA & 0b10101010) + (PINC & 0b01010101));

  db |= (((val >> 0) & 1)<<3);
  db |= (((val >> 2) & 1)<<2);
  db |= (((val >> 4) & 1)<<1);
  db |= (((val >> 6) & 1)<<0);

  db |= (((val >> 7) & 1)<<7);
  db |= (((val >> 5) & 1)<<6);
  db |= (((val >> 3) & 1)<<5);
  db |= (((val >> 1) & 1)<<4);

  return db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint8_t val = 0;
  
  // Set data pins as outputs
  DDRA |= 0b10101010 ;
  DDRC |= 0b01010101 ;

  // GPIB states are inverted
  db = ~db;

  val |= (((db >> 4) & 1)<<1);
  val |= (((db >> 5) & 1)<<3);
  val |= (((db >> 6) & 1)<<5);
  val |= (((db >> 7) & 1)<<7);

  val |= (((db >> 0) & 1)<<6);
  val |= (((db >> 1) & 1)<<4);
  val |= (((db >> 2) & 1)<<2);
  val |= (((db >> 3) & 1)<<0);

  // Set data bus
  PORTA = (PORTA & ~0b10101010) | (val & 0b10101010);
  PORTC = (PORTC & ~0b01010101) | (val & 0b01010101);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // PORT B
  uint8_t portBb = (((bits >> 7 & 1))<<0) + (((bits >> 6 & 1))<<2);
  uint8_t portBm = (((mask >> 7 & 1))<<0) + (((mask >> 6 & 1))<<2);

  // PORT G
  uint8_t portGb = (((bits >> 4 & 1))<<0) + (((bits >> 5 & 1))<<2);
  uint8_t portGm = (((mask >> 4 & 1))<<0) + (((mask >> 5 & 1))<<2);

  // PORT L
  uint8_t portLb = (((bits >> 0 & 1))<<0) + (((bits >> 1 & 1))<<2) + (((bits >> 2 & 1))<<4) + (((bits >> 3 & 1))<<6);
  uint8_t portLm = (((mask >> 0 & 1))<<0) + (((mask >> 1 & 1))<<2) + (((mask >> 2 & 1))<<4) + (((mask >> 3 & 1))<<6);

  // Set PORTs using mask to avoid affecting bits that should not be affected
  // and calculated and masked port byte
  // PORT B - bits 0 & 2 (ATN_PIN + SRQ_PIN)
  // PORT G - bits 0 & 2 (EOI_PIN, REN_PIN)
  // PORT L - bits 0,2,4,6 (IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN)
  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin states using mask
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
  PORTG = ( (PORTG & ~portGm) | (portGb & portGm) );
  PORTL = ( (PORTL & ~portLm) | (portLb & portLm) );
}



void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // PORT B
  uint8_t portBb = (((bits >> 7 & 1))<<0) + (((bits >> 6 & 1))<<2);
  uint8_t portBm = (((mask >> 7 & 1))<<0) + (((mask >> 6 & 1))<<2);

  // PORT G
  uint8_t portGb = (((bits >> 4 & 1))<<0) + (((bits >> 5 & 1))<<2);
  uint8_t portGm = (((mask >> 4 & 1))<<0) + (((mask >> 5 & 1))<<2);

  // PORT L
  uint8_t portLb = (((bits >> 0 & 1))<<0) + (((bits >> 1 & 1))<<2) + (((bits >> 2 & 1))<<4) + (((bits >> 3 & 1))<<6);
  uint8_t portLm = (((mask >> 0 & 1))<<0) + (((mask >> 1 & 1))<<2) + (((mask >> 2 & 1))<<4) + (((mask >> 3 & 1))<<6);

  // Set PORTs using mask to avoid affecting bits that should not be affected
  // and calculated and masked port byte
  // PORT B - bits 0 & 2 (ATN_PIN + SRQ_PIN)
  // PORT G - bits 0 & 2 (EOI_PIN, REN_PIN)
  // PORT L - bits 0,2,4,6 (IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN)
  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin direction registers using mask
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
  DDRG = ( (DDRG & ~portGm) | (portGb & portGm) );
  DDRL = ( (DDRL & ~portLm) | (portLb & portLm) );
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //MEGA2560
/***** ^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MEGA2560 BOARD LAYOUT E2 *****/
/************************************/



/***********************************************************/
/***** MICRO PRO (32u4) BOARD LAYOUT for MICRO (Artag) *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA32U4_MICRO
/*
  Data pin map
  ------------
  DIO1_PIN   3 : GPIB 1  : PD0
  DIO2_PIN  15 : GPIB 2  : PB1
  DIO3_PIN  16 : GPIB 3  : PB2
  DIO4_PIN  14 : GPIB 4  : PB3
  DIO5_PIN   8 : GPIB 13 : PB4
  DIO6_PIN   9 : GPIB 14 : PB5
  DIO7_PIN  10 : GPIB 15 : PB6
  DIO8_PIN   6 : GPIB 16 : PD7

  Control pin map
  ---------------
  IFC_PIN    4 : GPIB  9 : PD4 : b0
  NDAC_PIN  A3 : GPIB  8 : PF4 : b1
  NRFD_PIN  A2 : GPIB  7 : PF5 : b2
  DAV_PIN   A1 : GPIB  6 : PF6 : b3
  EOI_PIN   A0 : GPIB  5 : PF7 : b4
  REN_PIN    7 : GPIB 17 : PC6 : b5
  SRQ_PIN    5 : GPIB 10 : PE6 : b6
  ATN_PIN    2 : GPIB 11 : PD1 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/

/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRB |= 0b01111110;
    DDRD |= 0b10000001;
    PORTB |= 0b01111110; // PORTC bits 7,5,3,1 high
    PORTD |= 0b10000001; // PORTA bits 6,4,2,0 high
    return;
  }

  // Set data pins to input
  DDRB  &= 0b10000001 ;
  DDRD  &= 0b01111110 ;
  PORTB |= 0b01111110; // PORTB bits 6,5,4,3,2,1 input_pullup
  PORTD |= 0b10000001; // PORTD bits 7,0 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  return ~((PIND & 0b10000001) | (PINB & 0b01111110)) ;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  // Set data pins as outputs
  DDRB |= 0b01111110;
  DDRD |= 0b10000001;

  // GPIB states are inverted
  db = ~db;

  // Set data bus
  PORTB = (PORTB & ~0b01111110) | (db & 0b01111110) ;
  PORTD = (PORTD & ~0b10000001) | (db & 0b10000001);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // most of the time, only these bits change

  if (mask & 0b00011110) {

    // PORTF - NDAC_PIN, NRFD_PIN, DAV_PIN and EOI_PIN bits 1-4 rotated into bits 4-7
    uint8_t portFb = (bits & 0x1e) << 3;
    uint8_t portFm = (mask & 0x1e) << 3;

    // Set pin states using mask
    PORTF = ( (PORTF & ~portFm) | (portFb & portFm) );

  }

  if (mask & 0b11100001) {

    // PORTC - REN_PIN bit 5 rotated into bit 6
    uint8_t portCb = (bits & 0x20) << 1;
    uint8_t portCm = (mask & 0x20) << 1;
    // PORTD - IFC_PIN bit 0 rotated into bit 4 and ATN_PIN bit 7 rotated into 1
    uint8_t portDb = ((bits & 0x01) << 4) | ((bits & 0x80) >> 6);
    uint8_t portDm = ((mask & 0x01) << 4) | ((mask & 0x80) >> 6);
    // PORT E - SRQ_PIN bit 6  in bit 6
    uint8_t portEb = (bits & 0x40);
    uint8_t portEm = (mask & 0x40);

    // Set pin states using mask
    PORTC = ( (PORTC & ~portCm) | (portCb & portCm) );
    PORTD = ( (PORTD & ~portDm) | (portDb & portDm) );
    PORTE = ( (PORTE & ~portEm) | (portEb & portEm) );

  }

}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // most of the time, only these bits change

  if (mask & 0b00011110) {

    // PORTF - NDAC_PIN, NRFD_PIN, DAV_PIN and EOI_PIN bits 1-4 rotated into bits 4-7
    uint8_t portFb = (bits & 0x1e) << 3;
    uint8_t portFm = (mask & 0x1e) << 3;

    // Set pin direction registers using mask
    DDRF = ( (DDRF & ~portFm) | (portFb & portFm) );
  }

  if (mask & 0b11100001) {

    // PORTC - REN_PIN bit 5 rotated into bit 6
    uint8_t portCb = (bits & 0x20) << 1;
    uint8_t portCm = (mask & 0x20) << 1;
    // PORTD - IFC_PIN bit 0 rotated into bit 4 and ATN_PIN bit 7 rotated into 1
    uint8_t portDb = ((bits & 0x01) << 4) | ((bits & 0x80) >> 6);
    uint8_t portDm = ((mask & 0x01) << 4) | ((mask & 0x80) >> 6);
    // PORT E - SRQ_PIN bit 6  in bit 6
    uint8_t portEb = (bits & 0x40);
    uint8_t portEm = (mask & 0x40);

    // Set pin direction registers using mask
    DDRC = ( (DDRC & ~portCm) | (portCb & portCm) );
    DDRD = ( (DDRD & ~portDm) | (portDb & portDm) );
    DDRE = ( (DDRE & ~portEm) | (portEb & portEm) );

  }

}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif  // AR488_MEGA32U4_MICRO
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MICRO PRO (32u4) BOARD LAYOUT for MICRO (Artag) *****/
/***********************************************************/



/************************************/
/***** LEONARDO R3 BOARD LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA32U4_LR3
/*
  Data pin map
  ------------
  DIO1_PIN  A0 : GPIB 1  : PF7
  DIO2_PIN  A1 : GPIB 2  : PF6
  DIO3_PIN  A2 : GPIB 3  : PF5
  DIO4_PIN  A3 : GPIB 4  : PF4
  DIO5_PIN  A4 : GPIB 13 : PF3
  DIO6_PIN  A5 : GPIB 14 : PF2
  DIO7_PIN   4 : GPIB 15 : PD4
  DIO8_PIN   5 : GPIB 16 : PC6

  Control pin map
  ---------------
  IFC_PIN    8 : GPIB  9 : PB4 : b0
  NDAC_PIN   9 : GPIB  8 : PB5 : b1
  NRFD_PIN  10 : GPIB  7 : PB6 : b2
  DAV_PIN   11 : GPIB  6 : PB7 : b3
  EOI_PIN   12 : GPIB  5 : PD6 : b4
  REN_PIN    3 : GPIB 17 : PD1 : b5
  SRQ_PIN    2 : GPIB 10 : PD0 : b6
  ATN_PIN    7 : GPIB 11 : PE6 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRC |= 0b01000000;
    DDRD |= 0b00010000;
    DDRF |= 0b11110011;
    PORTC |= 0b01000000; // PORTC bits 7,5,3,1 high
    PORTD |= 0b00010000; // PORTA bits 6,4,2,0 high
    PORTF |= 0b11110011; // PORTA bits 6,4,2,0 high
    return;
  }

  // Set data pins to input
  DDRC &= 0b10111111 ;
  DDRD &= 0b11101111 ;
  DDRF &= 0b00001100 ;
 
  PORTC |= 0b01000000; // PORTD bit 4 input_pullup
  PORTD |= 0b00010000; // PORTD bit 6 input_pullup
  PORTF |= 0b11110011; // PORTC bits 7,6,5,4,1,0 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  uint8_t portf = reverseBits( (PINF & 0b11110000) + ((PINF & 0b00000011) << 2) );
  return ~( ((PIND & 0b00010000) << 2) + ((PINC & 0b01000000) <<1) + portf );
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
//  uint8_t rdb;
  uint8_t portf;
  // Set data pins as outputs
  DDRC |= 0b01000000;
  DDRD |= 0b00010000;
  DDRF |= 0b11110011;

  // GPIB states are inverted
  db = ~db;

  // Port F require bits mapped to 0-1 and 4-7 in reverse order
  portf = reverseBits((db & 0b00001111) + ((db & 0b00110000) << 2));

  // Set data bus
  PORTC = (PORTC & ~0b01000000) | ((db & 0b10000000) >> 1);
  PORTD = (PORTD & ~0b00010000) | ((db & 0b01000000) >> 2);
  PORTF = (PORTF & ~0b11110011) | (portf & 0b11110011);
}


/***** Set the direction and state of the GPIB control lines ****/


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // PORTB - use bits 0 to 3, rotate bits 4 positions left to set bits 4-7 on register (pins 8-12)
  uint8_t portBb = ((bits & 0x0F) << 4);
  uint8_t portBm = ((mask & 0x0F) << 4);
  uint8_t portDb = ((bits & 0x10) << 2) + ((bits & 0x20) >> 5) + ((bits & 0x40) >> 5);
  uint8_t portDm = ((mask & 0x10) << 2) + ((mask & 0x20) >> 5) + ((mask & 0x40) >> 5);
  uint8_t portEb = ((bits & 0x80) >> 1);
  uint8_t portEm = ((mask & 0x80) >> 1);

  // Set pin states using mask
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
  PORTD = ( (PORTD & ~portDm) | (portDb & portDm) );
  PORTE = ( (PORTE & ~portEm) | (portEb & portEm) );
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // PORTB - use bits 0 to 3, rotate bits 4 positions left to set bits 4-7 on register (pins 8-12)
  uint8_t portBb = ((bits & 0x0F) << 4);
  uint8_t portBm = ((mask & 0x0F) << 4);
  uint8_t portDb = ((bits & 0x10) << 2) + ((bits & 0x20) >> 5) + ((bits & 0x40) >> 5);
  uint8_t portDm = ((mask & 0x10) << 2) + ((mask & 0x20) >> 5) + ((mask & 0x40) >> 5);
  uint8_t portEb = ((bits & 0x80) >> 1);
  uint8_t portEm = ((mask & 0x80) >> 1);

  // Set pin direction registers using mask
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
  DDRD = ( (DDRD & ~portDm) | (portDb & portDm) );
  DDRE = ( (DDRE & ~portEm) | (portEb & portEm) );
}


uint8_t reverseBits(uint8_t dbyte) {
   dbyte = (dbyte & 0xF0) >> 4 | (dbyte & 0x0F) << 4;
   dbyte = (dbyte & 0xCC) >> 2 | (dbyte & 0x33) << 2;
   dbyte = (dbyte & 0xAA) >> 1 | (dbyte & 0x55) << 1;
   return dbyte;
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //AR488_MEGA32U4_LR3
/***** ^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** LEONARDO R3 BOARD LAYOUT *****/
/************************************/



/******************************************/
/***** MCP23S17 EXPANDER (SPI) LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MCP23S17
/*
  Data pin map
  ------------
  DIO1_PIN   0 : GPIB 1  : B0
  DIO2_PIN   1 : GPIB 2  : B1
  DIO3_PIN   2 : GPIB 3  : B2
  DIO4_PIN   3 : GPIB 4  : B3
  DIO5_PIN   4 : GPIB 13 : B4
  DIO6_PIN   5 : GPIB 14 : B5
  DIO7_PIN   6 : GPIB 15 : B6
  DIO8_PIN   7 : GPIB 16 : B7

  Control pin map
  ---------------
  IFC_PIN    0 : GPIB  9 : PA0 : b0
  NDAC_PIN   1 : GPIB  8 : PA1 : b1
  NRFD_PIN   2 : GPIB  7 : PA2 : b2
  DAV_PIN    3 : GPIB  6 : PA3 : b3
  EOI_PIN    4 : GPIB  5 : PA4 : b4
  REN_PIN    5 : GPIB 17 : PA5 : b5
  SRQ_PIN    6 : GPIB 10 : PA6 : b6
  ATN_PIN    7 : GPIB 11 : PA7 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


// MCP23S17 hardware config
const uint8_t chipSelect = MCP_SELECTPIN;
const uint8_t mcpAddr = MCP_ADDRESS;      // Must be between 0 and 7
uint8_t mcpIntAReg = 0;


/***** Ready the SPI bus *****/
void mcpInit(){
  SPI.begin();
  // Optional: Clock divider (slow down the bus speed [optional])
  SPI.setClockDivider(SPI_CLOCK_DIV8);
  // Set expander configuration register
  // (Bit 1=0 sets active low for Int A)
  // (Bit 3=1 enables hardware address pins (MCP23S17 only)
  // (Bit 7=0 sets registers to be in same bank)
  mcpByteWrite(MCPCON, 0b00001000);
  // Enable MCP23S17 interrupts
  mcpInterruptsEn();
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (stat == OUTPUT) {
    mcpByteWrite(MCPDIRB, 0b00000000);  // Port direction: 0 = output; 1 = input;
    // Set output pins HIGH
    mcpByteWrite(MCPPORTB, 0b11111111);
  }
  // Set data pins to input
  mcpByteWrite(MCPDIRB, 0b11111111);  // Port direction: 0 = output; 1 = input;
  mcpByteWrite(MCPPUB, 0b11111111);   // 1 = Pullup resistors enabled
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~mcpByteRead(MCPPORTB);
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set data pins as outputs
  mcpByteWrite(MCPDIRB, 0b00000000);  // Port direction: 0 = output; 1 = input;

  // GPIB states are inverted
  db = ~db;

  // Set data bus
  mcpByteWrite(MCPPORTB, db);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  uint8_t portAb = bits;
  uint8_t portAm = mask;

  uint8_t regByte = 0;
  uint8_t regMod = 0; 


  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin states using mask
  regByte = mcpByteRead(MCPPORTA);
  regMod = (regByte & ~portAm) | (portAb & portAm);
  mcpByteWrite(MCPPORTA, regMod);
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  uint8_t portAb = bits;
  uint8_t portAm = mask;

  uint8_t regByte = 0;
  uint8_t regMod = 0; 


  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin direction registers using mask
  regByte = ~mcpByteRead(MCPDIRA);   // Note: on MCP23S17 0 = output, 1 = input
  regMod = (regByte & ~portAm) | (portAb & portAm);
  mcpByteWrite(MCPDIRA, ~regMod);    // Note: on MCP23S17 0 = output, 1 = input

}


/***** MCP23S17 interrupt handler *****/
/*
 * Interrput pin on Arduino configure with attachInterrupt
 */
void mcpIntHandler() {
//  mcpIntA = true;
//  Serial.println(F("MCP Interrupt triggered"));
  mcpIntAReg = mcpByteRead(MCPINTCAPA);
}


/***** Read from the MCP23S17 *****/
/*
 * reg : register we want to read , e.g. MCPPORTA or MCPPORTB
 */
uint8_t mcpByteRead(uint8_t reg){
  uint8_t db;
//Serial.print(F("Rx addr: "));
//Serial.print(mcpAddr);
  digitalWrite(chipSelect, LOW);            // Enable MCP communication
  SPI.transfer(MCPREAD | (mcpAddr << 1));   // Write opcode + chip address + write bit
  SPI.transfer(reg);                        // Write the register we want to read
  db = SPI.transfer(0x00);                  // Send any byte. Function returns low byte (port A value) which is ignored
  digitalWrite(chipSelect, HIGH);           // Enable MCP communication
//uint8_t dbinv = ~db;
//Serial.print(F("\tRCV: "));
//Serial.println(dbinv, HEX);
  return db;
}


/***** Write to the MCP23S17 *****/
void mcpByteWrite(uint8_t reg, uint8_t db){
//Serial.print(F("Tx addr: "));
//Serial.print(mcpAddr);
  digitalWrite(chipSelect, LOW);            // Enable MCP communication
  SPI.transfer(MCPWRITE | (mcpAddr << 1));  // Write opcode (with write bit set) + chip address
  SPI.transfer(reg);                        // Write register we want to change
  SPI.transfer(db);                         // Write data byte
  digitalWrite(chipSelect, HIGH);           // Stop MCP communication
//Serial.print(F("\tWRT: "));
//Serial.print(db, HEX);
//uint8_t dbinv = ~db;
//Serial.print(F("\tWRTinv: "));
//Serial.println(dbinv, HEX);
}


/***** Read status of control port pins *****/
/*
 * Pin value between 0 and 7
 * Control bus = port A)
 */
uint8_t mcpDigitalRead(uint8_t pin) {
  // If the pin value is larger than 7 then do nothing and return
  // Zero or larger value is implied by the variable type
  if (pin > 7) return 0x0;
  // Read the port A pin state, extract and return HIGH/LOW state for the requested pin
  return mcpByteRead(MCPPORTA) & (1 << pin) ? HIGH : LOW;
}


/***** Get the status of an MCP23S17 pin) *****/
uint8_t getGpibPinState(uint8_t pin){
  if (mcpIntAReg & (1 << gpibsig)) return LOW;    // 1 means interrupt triggered, so asserted (LOW)
  return HIGH;
}


/***** Configure pins that will generate an interrupt *****/
void mcpInterruptsEn(){
  // Set to interrupt mode for compare to previous
  mcpByteWrite(MCPINTCONA, 0b00000000);
  // Enable interrupt to detect pin state change on pins 4, 6 and 7 (EOI_PIN, SRQ_PIN and ATN_PIN)
  mcpByteWrite(MCPINTENA, 0b11010000);
}

#endif //AR488_MCP23S17
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** MCP23S17 EXPANDER (SPI) LAYOUT *****/
/******************************************/



/***************************************************/
/***** PANDUINO/MIGHTYCORE MCGRAW BOARD LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_MEGA644P_MCGRAW
/*
  Data pin map
  ------------
  DIO1_PIN  10 : GPIB 1  : PD2
  DIO2_PIN  11 : GPIB 2  : PD3
  DIO3_PIN  12 : GPIB 3  : PD4
  DIO4_PIN  13 : GPIB 4  : PD5
  DIO5_PIN  14 : GPIB 13 : PD6
  DIO6_PIN  15 : GPIB 14 : PD7
  DIO7_PIN  16 : GPIB 15 : PC0
  DIO8_PIN  17 : GPIB 16 : PC1

  Control pin map
  ---------------
  IFC_PIN   22 : GPIB  9 : PC6 : b0
  NDAC_PIN  21 : GPIB  8 : PC5 : b1
  NRFD_PIN  20 : GPIB  7 : PC4 : b2
  DAV_PIN   19 : GPIB  6 : PC3 : b3
  EOI_PIN   18 : GPIB  5 : PC2 : b4
  REN_PIN   23 : GPIB 17 : PA0 : b5
  SRQ_PIN   24 : GPIB 10 : PC7 : b6
  ATN_PIN   31 : GPIB 11 : PA7 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRD |= 0b11111100;
    DDRC |= 0b00000011;
    PORTD |= 0b11111100; // PORTC bits 7,5,3,1 high
    PORTC |= 0b00000011; // PORTA bits 6,4,2,0 high
    return;
  }

  // Set data pins to input
  DDRD &= 0b00000011;  
  DDRC &= 0b11111100;  
  
  PORTD |= 0b11111100; // PORTD bits 7, 6, 5, 4, 3, 2 input_pullup
  PORTC |= 0b00000011; // PORTC bits 1, 0 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~((PIND >> 2 & 0b00111111) + (PINC << 6 & 0b11000000));
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set data pins as outputs
  DDRD |= 0b11111100;
  DDRC |= 0b00000011;

  // GPIB states are inverted
  db = ~db;

  // Set data bus
  PORTC = (PORTC & ~0b00000011) | ((db & 0b11000000) >> 6);
  PORTD = (PORTD & ~0b11111100) | ((db & 0b00111111) << 2);
}


/***** Reverse the order of the bits in a byte *****/
uint8_t reverseBits(uint8_t dbyte) {
   dbyte = (dbyte & 0xF0) >> 4 | (dbyte & 0x0F) << 4;
   dbyte = (dbyte & 0xCC) >> 2 | (dbyte & 0x33) << 2;
   dbyte = (dbyte & 0xAA) >> 1 | (dbyte & 0x55) << 1;
   return dbyte;
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // PORT A - use bits 5 and 7. Map to port A bits 0 and 7
  uint8_t portAb = ((bits & 0x20) >> 5) + (bits &  0x80);
  uint8_t portAm = ((mask & 0x20) >> 5) + (mask &  0x80);

  // PORT C- use the 5 right-most bits (bits 0 - 4) and bit 6
  // Reverse bits 0-4 and map to bits 2-6. Map bit 6 to bit 7
  uint8_t portCb = (reverseBits(bits & 0x1F) >> 1) + ((bits & 0x40) << 1);
  uint8_t portCm = (reverseBits(mask & 0x1F) >> 1) + ((mask & 0x40) << 1);

  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin states using mask
  PORTA = ( (PORTA & ~portAm) | (portAb & portAm) );
  PORTC = ( (PORTC & ~portCm) | (portCb & portCm) );

}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // PORT A - use bits 5 and 7. Map to port A bits 0 and 7
  uint8_t portAb = ((bits & 0x20) >> 5) + (bits &  0x80);
  uint8_t portAm = ((mask & 0x20) >> 5) + (mask &  0x80);

  // PORT C- use the 5 right-most bits (bits 0 - 4) and bit 6
  // Reverse bits 0-4 and map to bits 2-6. Map bit 6 to bit 7
  uint8_t portCb = (reverseBits(bits & 0x1F) >> 1) + ((bits & 0x40) << 1);
  uint8_t portCm = (reverseBits(mask & 0x1F) >> 1) + ((mask & 0x40) << 1);

  // Set registers: register = (register & ~bitmask) | (value & bitmask)
  // Mask: 0=unaffected; 1=to be changed

  // Set pin direction registers using mask
  DDRA = ( (DDRA & ~portAm) | (portAb & portAm) );
  DDRC = ( (DDRC & ~portCm) | (portCb & portCm) );

}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif // AR488_MEGA644P_MCGRAW
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** PANDUINO/MIGHTYCORE MCGRAW BOARD LAYOUT *****/
/***************************************************/



/******************************************/
/***** POLOLU A-STAR 328PB ALT LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_328PB_ALT
/*
  Data pin map
  ------------
  DIO1_PIN  A0 : GPIB 1  : PC0
  DIO2_PIN  A1 : GPIB 2  : PC1
  DIO3_PIN  A2 : GPIB 3  : PC2
  DIO4_PIN  A3 : GPIB 4  : PC3
  DIO5_PIN  A4 : GPIB 13 : PC4
  DIO6_PIN  A5 : GPIB 14 : PC5
  DIO7_PIN  A6 : GPIB 15 : PD4
  DIO8_PIN  A7 : GPIB 16 : PD5

  Control pin map
  ---------------
  IFC_PIN    5 : GPIB  9 : PD5 : b0
  NDAC_PIN   6 : GPIB  8 : PD6 : b1
  NRFD_PIN   7 : GPIB  7 : PD7 : b2
  DAV_PIN    8 : GPIB  6 : PB0 : b3
  EOI_PIN    9 : GPIB  5 : PB1 : b4
  REN_PIN    2 : GPIB 17 : PD2 : b5
  SRQ_PIN    3 : GPIB 10 : PD3 : b6
  ATN_PIN    4 : GPIB 11 : PD4 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    DDRC |= 0b00111111;
    DDRE |= 0b00001100;
    PORTC |= 0b00111111; // PORTC bits 7,5,3,1 high
    PORTE |= 0b00001100; // PORTA bits 6,4,2,0 high
    return;
  }

  // Set data pins to input
  DDRC &= 0b11000000;
  DDRE &= 0b11110011;
  PORTC |= 0b00111111; // PORTC bits 5,4,3,2,1,0 input_pullup
  PORTE |= 0b00001100; // PORTE bits 3,2 input_pullup
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~((PINE << 4 & 0b11000000) + (PINC & 0b00111111));
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  // Set data pins as outputs
  DDRE |= 0b00001100;
  DDRC |= 0b00111111;

  // GPIB states are inverted
  db = ~db;

  // Set data bus
  PORTC = (PORTC & ~0b00111111) | (db & 0b00111111);
  PORTE = (PORTE & ~0b00001100) | ((db & 0b11000000) >> 4);
}


/***** Set the state of the GPIB control lines ****/
void setGpibCtrlState(uint8_t bits, uint8_t mask) {
  if (!mask) return;  // Empty mask does nothing!

  // PORTB - take bits 3 and 4 or value and move to the first two bits (pins 8, 9)
  uint8_t portBb = (bits & 0x18) >> 3;
  uint8_t portBm = (mask & 0x18) >> 3;
  // PORT D - take bits 0-2, move to 5-7, take bits 5-7 and move to 2-4
  uint8_t portDb = ((bits & 0x07) << 5) + ((bits & 0xE0) >> 3);
  uint8_t portDm = ((mask & 0x07) << 5) + ((mask & 0xE0) >> 3);

  // Set pin states using mask
  PORTB = ( (PORTB & ~portBm) | (portBb & portBm) );
  PORTD = ( (PORTD & ~portDm) | (portDb & portDm) );
}


/***** Set the direction of the GPIB control lines ****/
void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  uint8_t pmask;
  
  // PORTB - take bits 3 and 4 or value and move to the first two bits (pins 8, 9)
  uint8_t portBb = (bits & 0x18) >> 3;
  uint8_t portBm = (mask & 0x18) >> 3;
  // PORT D - take bits 0-2, move to 5-7, take bits 5-7 and move to 2-4
  uint8_t portDb = ((bits & 0x07) << 5) + ((bits & 0xE0) >> 3);
  uint8_t portDm = ((mask & 0x07) << 5) + ((mask & 0xE0) >> 3);

  // Set pin direction registers using mask
  DDRB = ( (DDRB & ~portBm) | (portBb & portBm) );
  DDRD = ( (DDRD & ~portDm) | (portDb & portDm) );

  // Set inputs to input_pullup
  pmask = (~portBb & portBm);
  PORTB = ( (PORTB & ~pmask) | pmask );
  pmask = (~portDb & portDm);
  PORTD = ( (PORTD & ~pmask) | pmask );

}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif //AR488_328PB_ALT
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** POLOLU A-STAR 328PB ALT LAYOUT *****/
/******************************************/



/*************************************/
/***** POE_ETHERNET_GPIB_ADAPTOR *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef POE_ETHERNET_GPIB_ADAPTOR
/***** Control pin map *****/
/*
  Data pin map
  ------------
  DIO1_PIN  22 : GPIB 1  : PD0
  DIO2_PIN  23 : GPIB 2  : PD1
  DIO3_PIN  24 : GPIB 3  : PD2
  DIO4_PIN  25 : GPIB 4  : PD3
  DIO5_PIN  26 : GPIB 13 : PD4
  DIO6_PIN  27 : GPIB 14 : PD5
  DIO7_PIN  28 : GPIB 15 : PD6
  DIO8_PIN  29 : GPIB 16 : PD7

  Control pin map
  ---------------
  IFC_PIN   18 : GPIB  9 : PC4 : b0
  NDAC_PIN  17 : GPIB  8 : PC3 : b1
  NRFD_PIN  16 : GPIB  7 : PC2 : b2
  DAV_PIN   15 : GPIB  6 : PC1 : b3
  EOI_PIN   14 : GPIB  5 : PC0 : b4
  REN_PIN   21 : GPIB 17 : PC7 : b5
  SRQ_PIN   19 : GPIB 10 : PC5 : b6
  ATN_PIN   20 : GPIB 11 : PC6 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=LOW, 1=HIGH
    mask : 0=unaffected, 1=affected
*/


uint8_t readPortPullupReg(PORT_t& port){
  uint8_t reg = 0;
  reg |= (port.PIN0CTRL & PORT_PULLUPEN_bm) >> 3;
  reg |= (port.PIN1CTRL & PORT_PULLUPEN_bm) >> 2;
  reg |= (port.PIN2CTRL & PORT_PULLUPEN_bm) >> 1;
  reg |= (port.PIN3CTRL & PORT_PULLUPEN_bm);
  reg |= (port.PIN4CTRL & PORT_PULLUPEN_bm) << 1;
  reg |= (port.PIN5CTRL & PORT_PULLUPEN_bm) << 2;
  reg |= (port.PIN6CTRL & PORT_PULLUPEN_bm) << 3;
  reg |= (port.PIN7CTRL & PORT_PULLUPEN_bm) << 4;
  return reg;
}


void setPortPullupBits(PORT_t& port, uint8_t reg){
  port.PIN0CTRL |= ((reg<<3) & PORT_PULLUPEN_bm);
  port.PIN1CTRL |= ((reg<<2) & PORT_PULLUPEN_bm);
  port.PIN2CTRL |= ((reg<<1) & PORT_PULLUPEN_bm);
  port.PIN3CTRL |= (reg & PORT_PULLUPEN_bm);
  port.PIN4CTRL |= ((reg>>1) & PORT_PULLUPEN_bm);
  port.PIN5CTRL |= ((reg>>2) & PORT_PULLUPEN_bm);
  port.PIN6CTRL |= ((reg>>3) & PORT_PULLUPEN_bm);
  port.PIN7CTRL |= ((reg>>4) & PORT_PULLUPEN_bm);
  
}


/***** Set the GPIB data bus to input pullup *****/

void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    // Set data pins to output
    PORTD.DIR &= 0b11111111;
    // Set PORTD bits to high
    PORTD.OUT = 0b11111111;
    return;
  }

  // Set data pins to input
  PORTD.DIR &= 0b00000000;
  // Set PORTD bits to input_pullup
  setPortPullupBits(PORTD, 0b11111111);
}


/***** Read the GPIB data bus wires to collect the byte of data *****/

uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  return ~PORTD.IN;
}


/***** Set the GPIB data bus to output and with the requested byte *****/

void setGpibDbus(uint8_t db) {

  // Set data pins as outputs
  PORTD.DIR |= 0b11111111;

  // Set data bus (GPIB states are inverted)
  PORTD.OUT = ~db;
}


/***** Reverse the order of the bits in a byte *****/
uint8_t reverseBits(uint8_t dbyte) {
   dbyte = (dbyte & 0xF0) >> 4 | (dbyte & 0x0F) << 4;
   dbyte = (dbyte & 0xCC) >> 2 | (dbyte & 0x33) << 2;
   dbyte = (dbyte & 0xAA) >> 1 | (dbyte & 0x55) << 1;
   return dbyte;
}


uint8_t bitsToPort(uint8_t bits){
  // PORT C - keep bits 0-4, rotate bit 5 right 3 positions, bit 6 & 7 left 1 position on register
  return (reverseBits(bits & 0x1F) >> 3) | ((bits & 0x20) << 2) | ((bits & 0x40) >> 1) | ((bits & 0x80) >> 1);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {
  if (!mask) return;  // Empty mask does nothing!
  uint8_t portCb = bitsToPort(bits);
  uint8_t portCm = bitsToPort(mask);  

  // Set pin states using mask
  PORTC.OUT = ( (PORTC.OUT & ~portCm) | (portCb & portCm) );
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {
  uint8_t portCb = bitsToPort(bits);
  uint8_t portCm = bitsToPort(mask);  

  // Set pin direction registers using mask
  PORTC.DIR = ( (PORTC.DIR & ~portCm) | (portCb & portCm) );

  // Set inputs to input_pullup, outputs  to output
  uint8_t reg = (readPortPullupReg(PORTC) & ~portCm);
  uint8_t toset = (~portCb & portCm);
  reg |= toset;
  setPortPullupBits(PORTC, reg);
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif  // POE_ETHERNET_GPIB_ADAPTOR
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** POE_ETHERNET_GPIB_ADAPTOR *****/
/*************************************/



/***********************************/
/***** ESP32 LAYOUT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#if defined(ESP32_NATIVE_FUNC)

/***** Struct used to hold GPIO register values *****/
struct gpioregister_t {
  uint32_t reg0 = 0;
  uint32_t reg1 = 0;
};


/***** Array holding pin map *****/
const uint8_t databus[8] = { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN };
const uint8_t ctrlreg[8] = { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN };


/***** 64-bit control and data bus pin masks *****/
uint64_t gpioDbMask = 0;
uint64_t gpioCtrlMask = 0;


/***** ESP GPIO configuratiom object *****/
gpio_config_t gpioCfg;
const gpio_config_t * gpioCfgPtr = &gpioCfg;


/***** Covert 64-bit mask to 2 x 32-bit regster values *****/
void mask64ToReg(gpioregister_t& gpioreg, uint64_t mask) {
  gpioreg.reg0 = (mask & 0xFFFFFFFF);
  gpioreg.reg1 = (mask >> 32);
}


/***** Convert 2 x 32-bit register values to 64-bit mask *****/
uint64_t regToMask64(gpioregister_t& gpioreg) {
  uint64_t gpiomask = 0;
  gpiomask = gpioreg.reg0;
  gpiomask |= ((uint64_t)gpioreg.reg1 << 32);
  return gpiomask;
}


/***** Generate GPIO mask from assigned pin map *****/
uint64_t genGpioMask(const uint8_t buspins[], uint8_t bitmask) {
  uint64_t gpioreg = 0;
  for (uint8_t i=0; i<8; i++) {
    if (bitmask & (1 << i)) {
      gpioreg |= ( 1ULL << buspins[i] );
    }
  }
  return gpioreg;
}


void setGpioDirMasked(const uint8_t bus[], uint8_t mask, uint8_t state = INPUT_PULLUP) {

  uint64_t gpiomask = 0;

  for (uint8_t i=0; i<8; i++){
    if ( mask & (1U << i) ) gpiomask |= (1ULL<<bus[i]);
  }

  if (state == OUTPUT) {
  // OUTPUT mode
    gpioCfg.pin_bit_mask = gpiomask;
    gpioCfg.mode = GPIO_MODE_INPUT_OUTPUT;
    gpioCfg.pull_up_en = GPIO_PULLUP_DISABLE;
  }else{
    // INPUT_PULLUP mode
    gpioCfg.pin_bit_mask = gpiomask;
    gpioCfg.mode = GPIO_MODE_INPUT;
    gpioCfg.pull_up_en = GPIO_PULLUP_ENABLE;
  }

  gpio_config(gpioCfgPtr);

}


uint8_t digitalReadReg(uint8_t pin){
  if (pin>SOC_GPIO_PIN_COUNT) return 0;   // Pins above 48 are invalid
  if (GPIO_IS_VALID_GPIO(pin)) {
    uint64_t gpioall = 0;
    gpioregister_t reg;
  
    reg.reg0 = REG_READ(GPIO_IN_REG);
#ifdef GPIO_OUT1_REG
    reg.reg1 = REG_READ(GPIO_IN1_REG);
#endif
    gpioall = regToMask64(reg);
    if ( gpioall & (1ULL<<pin) ) return 1;
  }
  return 0;
}


/***** Init GPIO registers *****/
void initEspGpioPins(){
  // Generate masks
  gpioDbMask = genGpioMask(databus, 0xFF);
  gpioCtrlMask = genGpioMask(ctrlreg, 0xFF);

  // Configure all GPIOs to input pullup (default?)
  uint64_t gpioall = (gpioDbMask | gpioCtrlMask);  
  gpioCfg.pin_bit_mask = gpioall;
  gpioCfg.mode = GPIO_MODE_INPUT;
  gpioCfg.pull_up_en = GPIO_PULLUP_ENABLE;
  gpioCfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
  gpioCfg.intr_type =  GPIO_INTR_DISABLE;
  gpio_config(gpioCfgPtr);
  
}


/**** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {

  gpioregister_t gpiodb;
  
  mask64ToReg(gpiodb, gpioDbMask);
 
  // Set pins to OUTPUT
  if (state == OUTPUT) {
    setGpioDirMasked(databus, 0xFF, OUTPUT);
    // Set to HIGH
    REG_WRITE(GPIO_OUT_W1TS_REG, gpiodb.reg0);
#ifdef GPIO_OUT1_W1TS_REG
    REG_WRITE(GPIO_OUT1_W1TS_REG, gpiodb.reg1);
#endif
    return;
  }
  
  // Set pins to INPUT_PULLUP
  setGpioDirMasked(databus, 0xFF, INPUT_PULLUP);

}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint64_t gpioall = 0;
  gpioregister_t gpioreg;
  
  // Read the byte of data on the bus`
  gpioreg.reg0 = REG_READ(GPIO_IN_REG);
  gpioreg.reg1 = REG_READ(GPIO_IN1_REG);
  gpioall = regToMask64(gpioreg);

  // Calculate and return result
  uint8_t result = 0;
  for (uint8_t i=0; i<8; i++) {
    if ( gpioall & (1ULL<<databus[i]) ) result = (result | (1U<<i));
  }
  return ~result;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  uint64_t busmask = genGpioMask(databus, 0xFF);
  uint64_t valmask = genGpioMask(databus, db);
  gpioregister_t busreg;
  gpioregister_t valreg;
  mask64ToReg(busreg, busmask);
  mask64ToReg(valreg, valmask);
  
  // Set databus to output
  readyGpibDbus(OUTPUT);
  
  // Set required value
  if (db>0) {
    REG_WRITE( GPIO_OUT_W1TC_REG, valreg.reg0 );
#ifdef GPIO_OUT1_W1TC_REG
    REG_WRITE( GPIO_OUT1_W1TC_REG, valreg.reg1 );
#endif
  }
}


/***** Set GPIB ctrl pin state *****/
void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint8_t hbits = bits & mask;
  uint8_t lbits = ~(bits | ~mask);

  uint64_t gpiomask = 0;

  if (hbits) {
    gpioregister_t gpioHbits;
    gpiomask = genGpioMask(ctrlreg, hbits);
    mask64ToReg(gpioHbits, gpiomask);

    if (gpioHbits.reg0) REG_WRITE( GPIO_OUT_W1TS_REG, gpioHbits.reg0 );
#ifdef GPIO_OUT1_W1TS_REG
    if (gpioHbits.reg1) REG_WRITE( GPIO_OUT1_W1TS_REG, gpioHbits.reg1 );
#endif
  }
  
  if (lbits){
    gpioregister_t gpioLbits;
    gpiomask = genGpioMask(ctrlreg, lbits);
    mask64ToReg(gpioLbits, gpiomask);

    if (gpioLbits.reg0) REG_WRITE( GPIO_OUT_W1TC_REG, gpioLbits.reg0 );
#ifdef GPIO_OUT1_W1TC_REG
    if (gpioLbits.reg1) REG_WRITE( GPIO_OUT1_W1TC_REG, gpioLbits.reg1 );
#endif
  }
}


/***** Set the direction of control pins *****/

void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint8_t obits = bits & mask;
  uint8_t ibits =  ~(bits | ~mask);


  if (obits) setGpioDirMasked(ctrlreg, obits, OUTPUT);

  if (ibits) setGpioDirMasked(ctrlreg, ibits, INPUT_PULLUP);
  
}


uint8_t getGpibPinState(uint8_t pin){
//  return digitalRead(pin);
  return digitalReadReg(pin);
}

#endif      // ESP32_NATIVE_FUNC



#if defined(ESP32_ARDUINO_FUNC)


uint8_t databus[8] = { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN };

uint8_t ctrlbus[8] = { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN };


/***** Dummy function *****/
void initEspGpioPins(){
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    for (uint8_t i=0; i<8; i++){
      pinMode(databus[i], OUTPUT);
      digitalWrite(databus[i], HIGH);
    }
    return;
  }
  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], INPUT_PULLUP);
  }
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  for (uint8_t i=0; i<8; i++){
    db = db + (digitalRead(databus[i]) ? 0 : 1<<i );
  }
  return db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], OUTPUT);
    digitalWrite(databus[i], ((db&(1<<i)) ? LOW : HIGH) );
  }
  
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // Set pin state
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) digitalWrite( ctrlbus[i], ((bits&(1<<i)) ? HIGH : LOW) );
  }

}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // Set pin direction
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) pinMode( ctrlbus[i], ((bits&(1<<i)) ? OUTPUT : INPUT_PULLUP) );
  }

}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}


#endif  // ESP32_ARDUINO_FUNC
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** ESP32 LAYOUT DEFINITION *****/
/***********************************/



/***********************************/
/***** RAS PICO BOARD LAYOUT 1 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L1
/*
  Data pin map
  ------------
  DIO1_PIN   6 : GPIB 1  : GPIO6
  DIO2_PIN   7 : GPIB 2  : GPIO7
  DIO3_PIN   8 : GPIB 3  : GPIO8
  DIO4_PIN   9 : GPIB 4  : GPIO9
  DIO5_PIN  10 : GPIB 13 : GPIO10
  DIO6_PIN  11 : GPIB 14 : GPIO11
  DIO7_PIN  12 : GPIB 15 : GPIO12
  DIO8_PIN  13 : GPIB 16 : GPIO13

  Control pin map
  ---------------
  IFC_PIN   14 : GPIB 9  : GPIO14 : b0
  NDAC_PIN  15 : GPIB 8  : GPIO15 : b1
  NRFD_PIN  16 : GPIB 7  : GPIO16 : b2
  DAV_PIN   17 : GPIB 6  : GPIO17 : b3
  EOI_PIN   18 : GPIB 5  : GPIO18 : b4
  REN_PIN   19 : GPIB 17 : GPIO19 : b5
  SRQ_PIN   20 : GPIB 10 : GPIO20 : b6
  ATN_PIN   21 : GPIB 11 : GPIO21 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


const uint32_t gpioDbMask = 0x00003FC0;
const uint32_t gpioCtrlMask = 0x003FC000;
const uint8_t gpioDbOffset = 6;
const uint8_t gpioCtrlOffset = 14;


#ifdef USE_RP2040_FUNC_LIST
void gpioFuncList(){
  Serial.println();
  Serial.print(F("Pull: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_pulled_up(i));
  }
  Serial.println();
  Serial.print(F("Outs: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_dir_out(i));
  }
  Serial.println();
}
#endif


/***** Set GPIO internal pullup resistors *****/
void gpio_set_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_pull_up(i);
  }
}


/***** Disable GPIO pull-up resistors *****/
void gpio_clear_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_disable_pulls(i);
  }
}


/***** Initialise all GPIO pins *****/
void initRpGpioPins(){
  uint32_t gpiomask = gpioCtrlMask | gpioDbMask;  // Scope of GPIO pins to be allocated to GPIB
  gpio_init_mask(gpiomask);                       // Configure as GPIO pins those allocated to GPIB
  gpio_set_dir_in_masked(gpiomask);               // Configure all as inputs by default
  gpio_set_pullups_masked(gpiomask);              // Enable all pullups (un-asserted) by default
  delay(200);                                     // Allow some settling time
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    gpio_set_dir_out_masked(gpioDbMask);
    // Set outputs to high
    gpio_put_masked(gpioDbMask, gpioDbMask);
  }

  // Set data pins to input
  gpio_set_dir_in_masked(gpioDbMask);
  gpio_set_pullups_masked(gpioDbMask); 
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  uint32_t gpioall = gpio_get_all();
  gpioall = (gpioall & gpioDbMask) >> gpioDbOffset;
  return (uint8_t)~gpioall;  
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint32_t gpioall = ((uint8_t)~db) << gpioDbOffset;
  gpio_clear_pullups_masked(gpioDbMask);
  gpio_set_dir_out_masked(gpioDbMask);
  gpio_put_masked(gpioDbMask, gpioall);
}


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    - bits : 0=LOW, 1=HIGH
    - mask : 0=unaffected, 1=affected
    Has relevance only to output pins
*/
void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint32_t gpiobits = (uint8_t)(bits & mask) << gpioCtrlOffset;
  uint32_t gpioOmask = (mask << gpioCtrlOffset);

  gpio_put_masked(gpioOmask, gpiobits);

}


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=input pullup, 1=output
    mask : 0=unaffected, 1=affected
*/
void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint32_t gpioOmask = ( (bits & mask) << gpioCtrlOffset );
  uint32_t gpioImask = ( ((uint8_t)~bits & mask) << gpioCtrlOffset );

  if (gpioOmask){
    gpio_clear_pullups_masked(gpioOmask);
    gpio_set_dir_out_masked(gpioOmask);
  }

  if (gpioImask){
    gpio_set_pullups_masked(gpioImask);
    gpio_set_dir_in_masked(gpioImask);
  }
  
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif // RAS_PICO_L1
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO BOARD LAYOUT 1 *****/
/***********************************/



/***********************************/
/***** RAS PICO BOARD LAYOUT 2 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L2
/*
  Data pin map
  ------------
  DIO1_PIN  14 : GPIB 1  : GPIO14
  DIO2_PIN  15 : GPIB 2  : GPIO15
  DIO3_PIN  16 : GPIB 3  : GPIO16
  DIO4_PIN  17 : GPIB 4  : GPIO17
  DIO5_PIN  18 : GPIB 13 : GPIO18
  DIO6_PIN  19 : GPIB 14 : GPIO19
  DIO7_PIN  20 : GPIB 15 : GPIO20
  DIO8_PIN  21 : GPIB 16 : GPIO21

  Control pin map
  ---------------
  IFC_PIN    6 : GPIB 9  : GPIO6  : b0
  NDAC_PIN   7 : GPIB 8  : GPIO7  : b1
  NRFD_PIN   8 : GPIB 7  : GPIO8  : b2
  DAV_PIN    9 : GPIB 6  : GPIO9  : b3
  EOI_PIN   10 : GPIB 5  : GPIO10 : b4
  REN_PIN   11 : GPIB 17 : GPIO11 : b5
  SRQ_PIN   12 : GPIB 10 : GPIO12 : b6
  ATN_PIN   13 : GPIB 11 : GPIO13 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


const uint32_t gpioDbMask = 0x003FC000;
const uint32_t gpioCtrlMask = 0x00003FC0;
const uint8_t gpioDbOffset = 14;
const uint8_t gpioCtrlOffset = 6;


#ifdef USE_RP2040_FUNC_LIST
void gpioFuncList(){
  Serial.println();
  Serial.print(F("Pull: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_pulled_up(i));
  }
  Serial.println();
  Serial.print(F("Outs: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_dir_out(i));
  }
  Serial.println();
}
#endif


/***** Set GPIO internal pullup resistors *****/
void gpio_set_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_pull_up(i);
  }
}


/***** Disable GPIO pull-up resistors *****/
void gpio_clear_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_disable_pulls(i);
  }
}

/*
uint8_t reverseBits(uint8_t dbyte) {
   dbyte = (dbyte & 0xF0) >> 4 | (dbyte & 0x0F) << 4;
   dbyte = (dbyte & 0xCC) >> 2 | (dbyte & 0x33) << 2;
   dbyte = (dbyte & 0xAA) >> 1 | (dbyte & 0x55) << 1;
   return dbyte;
}
*/

/***** Initialise all GPIO pins *****/
void initRpGpioPins(){
  uint32_t gpiomask = gpioCtrlMask | gpioDbMask;  // Scope of GPIO pins to be allocated to GPIB
  gpio_init_mask(gpiomask);                       // Configure as GPIO pins those allocated to GPIB
  gpio_set_dir_in_masked(gpiomask);               // Configure all as inputs by default
  gpio_set_pullups_masked(gpiomask);              // Enable all pullups (un-asserted) by default
  delay(200);                                     // Allow some settling time
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    gpio_set_dir_out_masked(gpioDbMask);
    // Set outputs to high
    gpio_put_masked(gpioDbMask, gpioDbMask);
  }

  // Set data pins to input
  gpio_set_dir_in_masked(gpioDbMask);
  gpio_set_pullups_masked(gpioDbMask);
  
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  uint32_t gpioall = gpio_get_all();
  gpioall = (gpioall & gpioDbMask) >> gpioDbOffset;
  return (uint8_t)~gpioall;  
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint32_t gpioall = ((uint8_t)~db) << gpioDbOffset;
  gpio_clear_pullups_masked(gpioDbMask);
  gpio_set_dir_out_masked(gpioDbMask);
  gpio_put_masked(gpioDbMask, gpioall);
}


/***** Set the direction and state of the GPIB control lines ****/
/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
    mode (mode)     : 0=set pin state, 1=set pin direction
   Arduino Uno/Nano pin to Port/bit to direction/state byte map:
   IFC_PIN   06  byte bit 0
   NDAC_PIN  07  byte bit 1
   NRFD_PIN  08  byte bit 2
   DAV_PIN   09  byte bit 3
   EOI_PIN   10  byte bit 4
   REN_PIN   11  byte bit 5
   SRQ_PIN   12  byte bit 6
   ATN_PIN   13  byte bit 7
*/


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    - bits : 0=LOW, 1=HIGH
    - mask : 0=unaffected, 1=affected
    Has relevance only to output pins
*/
void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint32_t gpiobits = (uint8_t)(bits & mask) << gpioCtrlOffset;
  uint32_t gpioOmask = (mask << gpioCtrlOffset);

  gpio_put_masked(gpioOmask, gpiobits);

}


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=input pullup, 1=output
    mask : 0=unaffected, 1=affected
*/
void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint32_t gpioOmask = ( (bits & mask) << gpioCtrlOffset );
  uint32_t gpioImask = ( ((uint8_t)~bits & mask) << gpioCtrlOffset );

  if (gpioOmask){
    gpio_clear_pullups_masked(gpioOmask);
    gpio_set_dir_out_masked(gpioOmask);
  }

  if (gpioImask){
    gpio_set_pullups_masked(gpioImask);
    gpio_set_dir_in_masked(gpioImask);
  }
  
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif // RAS_PICO_L2
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO BOARD LAYOUT 2 *****/
/***********************************/



/***********************************/
/***** RAS PICO BOARD LAYOUT 3 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L3
/*
  Data pin map
  ------------
  DIO1_PIN   2 : GPIB 1  : GPIO14
  DIO2_PIN   3 : GPIB 2  : GPIO15
  DIO3_PIN   4 : GPIB 3  : GPIO16
  DIO4_PIN   5 : GPIB 4  : GPIO17
  DIO5_PIN   6 : GPIB 13 : GPIO18
  DIO6_PIN   7 : GPIB 14 : GPIO19
  DIO7_PIN   8 : GPIB 15 : GPIO20
  DIO8_PIN   9 : GPIB 16 : GPIO21

  Control pin map
  ---------------
  IFC_PIN   10 : GPIB 9  : GPIO6  : b0
  NDAC_PIN  11 : GPIB 8  : GPIO7  : b1
  NRFD_PIN  12 : GPIB 7  : GPIO8  : b2
  DAV_PIN   13 : GPIB 6  : GPIO9  : b3
  EOI_PIN   14 : GPIB 5  : GPIO10 : b4
  REN_PIN   15 : GPIB 17 : GPIO11 : b5
  SRQ_PIN   20 : GPIB 10 : GPIO12 : b6
  ATN_PIN   21 : GPIB 11 : GPIO13 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/

const uint32_t gpioDbMask = 0x000003FC;
const uint32_t gpioCtrlMask = 0x0030FC00;
const uint8_t gpioDbOffset = 2;
const uint8_t gpioCtrlOffset = 10;


#ifdef USE_RP2040_FUNC_LIST
void gpioFuncList(){
  Serial.println();
  Serial.print(F("Pull: "));
  for (uint8_t i=21; i>19; i--){
    Serial.print(gpio_is_pulled_up(i));
  }
  for (uint8_t i=15; i>1; i--){
    Serial.print(gpio_is_pulled_up(i));
  }
  Serial.println();
  Serial.print(F("Outs: "));
  for (uint8_t i=21; i>19; i--){
    Serial.print(gpio_is_dir_out(i));
  }
  for (uint8_t i=15; i>1; i--){
    Serial.print(gpio_is_dir_out(i));
  }
  Serial.println();
}
#endif

/***** Set GPIO internal pullup resistors *****/
void gpio_set_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_pull_up(i);
  }
}


/***** Disable GPIO pull-up resistors *****/
void gpio_clear_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_disable_pulls(i);
  }
}


/***** Initialise all GPIO pins *****/
void initRpGpioPins(){
  uint32_t gpiomask = gpioCtrlMask | gpioDbMask;  // Scope of GPIO pins to be allocated to GPIB
  gpio_init_mask(gpiomask);                       // Configure as GPIO pins those allocated to GPIB
  gpio_set_dir_in_masked(gpiomask);               // Configure all as inputs by default
  gpio_set_pullups_masked(gpiomask);              // Enable all pullups (un-asserted) by default
  delayMicroseconds(200);                          // Allow some settling time
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    gpio_set_dir_out_masked(gpioDbMask);
    // Set outputs to high
    gpio_put_masked(gpioDbMask, gpioDbMask);
  }

  // Set data pins to input
  gpio_set_dir_in_masked(gpioDbMask);
  gpio_set_pullups_masked(gpioDbMask);
  
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  uint32_t gpioall = gpio_get_all();
  gpioall = (gpioall & gpioDbMask) >> gpioDbOffset;
  return (uint8_t)~gpioall;  
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint32_t gpioall = ((uint8_t)~db) << gpioDbOffset;
  gpio_clear_pullups_masked(gpioDbMask);
  gpio_set_dir_out_masked(gpioDbMask);
  gpio_put_masked(gpioDbMask, gpioall);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint8_t masked = bits & mask;

  uint32_t gpiobits = ( ((masked & 0x3F) << gpioCtrlOffset) | ((masked & 0xC0) << (gpioCtrlOffset + 4) ) );
  uint32_t gpiomask = ( ((mask & 0x3F) << gpioCtrlOffset) | ((mask & 0xC0) << (gpioCtrlOffset + 4)) );

  gpio_put_masked(gpiomask, gpiobits);

}


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=input pullup, 1=output
    mask : 0=unaffected, 1=affected
*/
void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint8_t masked = bits & mask;
  uint8_t rmasked = ~bits & mask;

  uint32_t gpioOmask = ( ((masked & 0x3F) << gpioCtrlOffset) | ((masked & 0xC0) << (gpioCtrlOffset + 4) ) );
  uint32_t gpioImask = ( ((rmasked & 0x3F) << gpioCtrlOffset) | ((rmasked & 0xC0) << (gpioCtrlOffset + 4) ) );

  if (gpioOmask){
    gpio_clear_pullups_masked(gpioOmask);
    gpio_set_dir_out_masked(gpioOmask);
  }

  if (gpioImask){
    gpio_set_pullups_masked(gpioImask);
    gpio_set_dir_in_masked(gpioImask);
  }
  
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}


#endif // RAS_PICO_L3
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO BOARD LAYOUT 3 *****/
/***********************************/



/***********************************/
/***** RAS PICO BOARD LAYOUT 4 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_L4
/*
  Data pin map
  ------------
  DIO1_PIN  10 : GPIB 10 : GPIO14
  DIO2_PIN  11 : GPIB 11 : GPIO15
  DIO3_PIN  12 : GPIB 12 : GPIO16
  DIO4_PIN  13 : GPIB 13 : GPIO17
  DIO5_PIN  14 : GPIB 14 : GPIO18
  DIO6_PIN  15 : GPIB 15 : GPIO19
  DIO7_PIN  20 : GPIB 20 : GPIO20
  DIO8_PIN  21 : GPIB 21 : GPIO21

  Control pin map
  ---------------
  IFC_PIN    2 : GPIB 9  : GPIO6  : b0
  NDAC_PIN   3 : GPIB 8  : GPIO7  : b1
  NRFD_PIN   4 : GPIB 7  : GPIO8  : b2
  DAV_PIN    5 : GPIB 6  : GPIO9  : b3
  EOI_PIN    6 : GPIB 5  : GPIO10 : b4
  REN_PIN    7 : GPIB 17 : GPIO11 : b5
  SRQ_PIN    8 : GPIB 10 : GPIO12 : b6
  ATN_PIN    9 : GPIB 11 : GPIO13 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


const uint32_t gpioDbMask = 0x0030FC00;
const uint32_t gpioCtrlMask = 0x000003FC;
const uint8_t gpioDbOffset = 10;
const uint8_t gpioCtrlOffset = 2;


#ifdef USE_RP2040_FUNC_LIST
void gpioFuncList(){
  Serial.println();
  Serial.print(F("Pull: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_pulled_up(i));
  }
  Serial.println();
  Serial.print(F("Outs: "));
  for (uint8_t i=21; i>5; i--){
    Serial.print(gpio_is_dir_out(i));
  }
  Serial.println();
}
#endif


/***** Set GPIO internal pullup resistors *****/
void gpio_set_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_pull_up(i);
  }
}


/***** Disable GPIO pull-up resistors *****/
void gpio_clear_pullups_masked(uint32_t mask){
  for (uint8_t i=0; i<32; i++){
    if ( mask & (1<<i) ) gpio_disable_pulls(i);
  }
}


/***** Initialise all GPIO pins *****/
void initRpGpioPins(){
  uint32_t gpiomask = gpioCtrlMask | gpioDbMask;  // Scope of GPIO pins to be allocated to GPIB
  gpio_init_mask(gpiomask);                       // Configure as GPIO pins those allocated to GPIB
  gpio_set_dir_in_masked(gpiomask);               // Configure all as inputs by default
  gpio_set_pullups_masked(gpiomask);              // Enable all pullups (un-asserted) by default
  delayMicroseconds(50);                          // Allow some settling time
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    gpio_set_dir_out_masked(gpioDbMask);
    // Set outputs to high
    gpio_put_masked(gpioDbMask, gpioDbMask);
  }

  // Set data pins to input
  gpio_set_dir_in_masked(gpioDbMask);
  gpio_set_pullups_masked(gpioDbMask);
  
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  // Read the byte of data on the bus
  uint32_t gpioall = gpio_get_all();
  gpioall = ((gpioall & 0x0000FC00) >> gpioDbOffset) | ((gpioall & 0x00300000) >> (gpioDbOffset + 4));
  return (uint8_t)~gpioall;  
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  uint8_t rdb = ~db;
  uint32_t gpioall = ( ((rdb & 0x3F) << gpioDbOffset) | ((rdb & 0xC0) << (gpioDbOffset + 4)) );
  gpio_clear_pullups_masked(gpioDbMask);
  gpio_set_dir_out_masked(gpioDbMask);
  gpio_put_masked(gpioDbMask, gpioall);
}


void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint8_t maskedbits = bits & mask;
  uint32_t gpiobits = (maskedbits << gpioCtrlOffset);
  uint32_t gpiomask = (mask << gpioCtrlOffset);

  gpio_put_masked(gpiomask, gpiobits);

}


/*
   Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=input pullup, 1=output
    mask : 0=unaffected, 1=affected
*/
void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint8_t masked = bits & mask;
  uint8_t rmasked = ~bits & mask;

  uint32_t gpioOmask = ( masked << gpioCtrlOffset );
  uint32_t gpioImask = ( rmasked << gpioCtrlOffset );

  if (gpioOmask){
    gpio_clear_pullups_masked(gpioOmask);
    gpio_set_dir_out_masked(gpioOmask);
  }

  if (gpioImask){
    gpio_set_pullups_masked(gpioImask);
    gpio_set_dir_in_masked(gpioImask);
  }
  
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif // RAS_PICO_L4
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO BOARD LAYOUT 4 *****/
/***********************************/



/***********************************/
/***** RAS PICO BOARD LAYOUT 5 *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RAS_PICO_LAUTO
/*

  SN7516x Buffers
  ---------------
  TE = 22
  TC = REN

  Data pin map
  ------------
  DIO1_PIN  10 : GPIB 10 : GPIO14
  DIO2_PIN  11 : GPIB 11 : GPIO15
  DIO3_PIN  12 : GPIB 12 : GPIO16
  DIO4_PIN  13 : GPIB 13 : GPIO17
  DIO5_PIN  14 : GPIB 14 : GPIO18
  DIO6_PIN  15 : GPIB 15 : GPIO19
  DIO7_PIN  20 : GPIB 20 : GPIO20
  DIO8_PIN  21 : GPIB 21 : GPIO21

  Control pin map
  ---------------
  IFC_PIN    9 : GPIB 9  : GPIO6  : b0
  NDAC_PIN   8 : GPIB 8  : GPIO7  : b1
  NRFD_PIN   7 : GPIB 7  : GPIO8  : b2
  DAV_PIN    6 : GPIB 6  : GPIO9  : b3
  EOI_PIN    5 : GPIB 5  : GPIO10 : b4
  REN_PIN    3 : GPIB 17 : GPIO11 : b5
  SRQ_PIN    4 : GPIB 10 : GPIO12 : b6
  ATN_PIN    2 : GPIB 11 : GPIO13 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/

uint8_t minPIN = 0xFF;
uint8_t maxPIN = 0;

const uint8_t databus[8] = { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN };
const uint8_t ctrlreg[8] = { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN };

uint32_t gpioDbMask = 0;
uint32_t gpioCtrlMask = 0;


/***** Generate a GPIO mask from 8-bit mask *****/
uint32_t genGpioMask(const uint8_t buspins[], uint8_t bitmask){
  uint32_t gpiomask = 0;
  for (uint8_t i=0; i<8; i++) {
    if (bitmask & (1 << i)) gpiomask += (1 << buspins[i]);
  }
  return gpiomask;
}


/***** Calculate the minimum and maximum pin number *****/
void getMinMax(){
  uint8_t i = 0;
  for (i=0; i<8; i++) {
    if (databus[i] > maxPIN) maxPIN = databus[i];
    if (ctrlreg[i] > maxPIN) maxPIN = ctrlreg[i];
    if (databus[i] < minPIN) minPIN = databus[i];
    if (ctrlreg[i] < minPIN) minPIN = ctrlreg[i];   
  }
  
}

/*
void printMinMax(){
  Serial.print("Min: ");
  Serial.println(minPIN);
  Serial.print("Max: ");
  Serial.println(maxPIN);
}


void printPins(){
  bool state;
  for (uint8_t i=minPIN; i<=maxPIN; i++){
    state = digitalRead(i);
    Serial.print(F("Pin "));
    Serial.print(i);
    Serial.print(F(":\t"));
    Serial.println(state);
  }
}
*/

/***** Set GPIO internal pullup resistors *****/
void gpio_set_pullups_masked(uint32_t mask){
  const uint8_t min = minPIN;
  const uint8_t max = maxPIN + 1;
  for (uint8_t i=min; i<max; i++){
    if ( mask & (1<<i) ) gpio_pull_up(i);
  }
}


/***** Disable GPIO pull-up resistors *****/
void gpio_clear_pullups_masked(uint32_t mask){
  const uint8_t min = minPIN;
  const uint8_t max = maxPIN + 1;
  for (uint8_t i=min; i<max; i++){
    if ( mask & (1<<i) ) gpio_disable_pulls(i);
  }
}


/***** Initialise all GPIO pins *****/
void initRpGpioPins(){
  uint32_t gpiomask = 0;
  gpioDbMask = genGpioMask(databus, 0xFF);
  gpioCtrlMask = genGpioMask(ctrlreg, 0xFF);
  
  gpiomask = (gpioCtrlMask | gpioDbMask);     // Scope of GPIO pins to be allocated to GPIB
  getMinMax();                                // Get lowest and highest pin number
  gpio_init_mask(gpiomask);                   // Configure as GPIO pins those allocated to GPIB
  gpio_set_dir_in_masked(gpiomask);           // Configure all as inputs by default
  gpio_set_pullups_masked(gpiomask);          // Enable all pullups (un-asserted) by default
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {

  // Set pins to OUTPUT
  if (state == OUTPUT) {
    // Set pins to output
    gpio_set_dir_out_masked(gpioDbMask);
    // Set outputs to high
    gpio_put_masked(gpioDbMask, gpioDbMask);
  }

  // Set data pins to input
  gpio_set_dir_in_masked(gpioDbMask);
  gpio_set_pullups_masked(gpioDbMask);
  
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {

  // Read the byte of data on the bus
  uint32_t gpioall = gpio_get_all();
  uint8_t result = 0;

  for (uint8_t i=0; i<8; i++) {
    if ( gpioall & (1<<databus[i]) ) result |= (1<<i);
  }

  return ~result;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  uint32_t gpiodb = 0;
  gpiodb = genGpioMask(databus, ~db);
  
  gpio_clear_pullups_masked(gpioDbMask);
  gpio_set_dir_out_masked(gpioDbMask);
  gpio_put_masked(gpioDbMask, gpiodb);

}


/***** Set the state of GPIB control pins *****/
void setGpibCtrlState(uint8_t bits, uint8_t mask){

  uint32_t gpiobits = genGpioMask(ctrlreg, bits);
  uint32_t gpiomask = genGpioMask(ctrlreg, mask);

  gpio_put_masked(gpiomask, gpiobits);
  
}


/***** Set the direction of GPIB control pins *****/
void setGpibCtrlDir(uint8_t bits, uint8_t mask){

  uint32_t gpioObits = genGpioMask(ctrlreg, (bits&mask));
  uint32_t gpioIbits = genGpioMask(ctrlreg, (~bits&mask));

  if (gpioObits){
    gpio_clear_pullups_masked(gpioObits);
    gpio_set_dir_out_masked(gpioObits);
  }

  if (gpioIbits){
    gpio_set_pullups_masked(gpioIbits);
    gpio_set_dir_in_masked(gpioIbits);
  }
  
}


/***** Get the state of a single pin *****/
uint8_t getGpibPinState(uint8_t pin){
  return digitalRead(pin);
}

#endif // RAS_PICO_LAUTO
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** RAS PICO BOARD LAYOUT 5 *****/
/***********************************/



/********************************************/
/***** UNO/NANO R4 RENESAS BOARD LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef RA4M1_NANO_R4

/* NOTE: Renesas RA4M1 boards work only with SN7516x buffer chips */

/***** Control pin map *****/
/*
  Data pin map
  ------------
  DIO1_PIN  A0 : GPIB 1  : PC0
  DIO2_PIN  A1 : GPIB 2  : PC1
  DIO3_PIN  A2 : GPIB 3  : PC2
  DIO4_PIN  A3 : GPIB 4  : PC3
  DIO5_PIN  A4 : GPIB 13 : PC4
  DIO6_PIN  A5 : GPIB 14 : PC5
  DIO7_PIN   4 : GPIB 15 : PD4
  DIO8_PIN   5 : GPIB 16 : PD5

  Control pin map
  ---------------
  IFC_PIN   8  : GPIB  9 : PB0 : b0
  NDAC_PIN  9  : GPIB  8 : PB1 : b1
  NRFD_PIN  10 : GPIB  7 : PB2 : b2
  DAV_PIN   11 : GPIB  6 : PB3 : b3
  EOI_PIN   12 : GPIB  5 : PB4 : b4
  REN_PIN   3  : GPIB 17 : PD2 : b5
  SRQ_PIN   2  : GPIB 10 : PD2 : b6
  ATN_PIN   7  : GPIB 11 : PD7 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits : 0=LOW, 1=HIGH
    mask : 0=unaffected, 1=affected

  digitalReadFast and digitalWriteFast functions based on UNOR4_digitalWritefast.h
  by KurtE:
  https://github.com/KurtE/UNOR4-stuff/blob/main/libraries/UNOR4_digitalWriteFast/UNOR4_digitalWriteFast.h#L50-L59

*/

uint8_t databus[8] = { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN };
uint8_t ctrlbus[8] = { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN };


/**
static R_PORT0_Type *port_table[] = { R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7 };
static const uint16_t mask_table[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7,
                                       1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15 };
*/

/***** Fast write digital pin *****/
/*
static inline void digitalWriteFast(uint8_t pin, uint8_t val) {
  uint16_t port_pin = g_pin_cfg[pin].pin;
  uint16_t pin_mask = mask_table[port_pin & 0x0F];
  port_pin = port_pin >> 8;
  if (val) {
    port_table[port_pin]->POSR = pin_mask;
  } else {
    port_table[port_pin]->PORR = pin_mask;
  }
}
*/

/***** Fast read digital pin *****/
/*
static inline uint16_t digitalReadFast(pin_size_t pin) {
  uint16_t port_pin = g_pin_cfg[pin].pin;
  uint16_t pin_mask = mask_table[port_pin & 0x0F];
  port_pin = port_pin >> 8;
    if (port_table[port_pin]->PIDR & pin_mask) {
    return HIGH;
  }else{
    return LOW;
  }
}
*/

/***** Fast toggle digital pin *****/
/*
static inline void digitalToggleFast(pin_size_t pin) {
  uint16_t port_pin = g_pin_cfg[pin].pin;
  uint16_t pin_mask = mask_table[port_pin & 0x0F];
  port_pin = port_pin >> 8;

  if (port_table[port_pin]->PODR & pin_mask) {
    port_table[port_pin]->PORR = pin_mask;
  }else{
    port_table[port_pin]->POSR = pin_mask;
  }
}
*/

/***** Set the GPIB data bus to input pullup *****/
/*
void readyGpibDbus() {
  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], INPUT_PULLUP);
  }
}
*/

/***** Read the GPIB data bus wires to collect the byte of data *****/
/*
uint8_t readGpibDbus() {
  uint8_t db = 0;
  for (uint8_t i=0; i<8; i++){
    if (!digitalReadFast(databus[i])) db += (1<<i);

  }
  return db;
}
*/

/***** Set the GPIB data bus to output and with the requested byte *****/
/*
void setGpibDbus(uint8_t db) {

  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], OUTPUT);
    digitalWriteFast(databus[i], ((db&(1<<i)) ? LOW : HIGH) );
  }
  
}
*/

/*
void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // Set pin state
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) digitalWriteFast( ctrlbus[i], ((bits&(1<<i)) ? HIGH : LOW) );
  }

}
*/

/*
void setGpibCtrlDir(uint8_t bits, uint8_t mask) {
  // Set pin direction
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) pinMode( ctrlbus[i], ((bits&(1<<i)) ? OUTPUT : INPUT_PULLUP) );
  }
}
*/

/*
uint8_t getGpibPinState(uint8_t pin){
//  return digitalRead(pin);
  return digitalReadFast(pin);
}
*/



/***** PinMode Fast digital pin *****/

static inline void pinModeFast(pin_size_t pin, uint8_t val) {

  byte _port = g_pin_cfg[pin].pin >> 8;
  byte _pin = g_pin_cfg[pin].pin & 0xFF;

    // Unlock PFS registers
//  R_PMISC->PWPR = 0x00; // Write 0x00 to PWPR_B0WI to disable register write protection.
//  R_PMISC->PWPR = 0x40; // Write 0x40 to PWPR_PFSWE to enable PFS register write access.
//  R_PMISC->PWPR &= ~0x80; // Clear PWPR_B0WI to disable register write protection.
//  R_PMISC->PWPR |= 0x40;  // Set PWPR_PFSWE to enable PFS register write access.
    
  if (val == OUTPUT) {

    R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PDR = 1;  // Set direction OUTPUT
    R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PODR = 1; // Set output HIGH
    R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PCR = 0;  // Disable pull-up
    return;

  }
  
  R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PMR = 0;  // Set direction input
  R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PDR = 0;  // Set direction input
  R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PCR = 1;  // Enable pull-up

  // Re-lock PFS registers
//  R_PMISC->PWPR = 0x00; // Write 0x00 to PWPR_PFSWE to disable PFS register write access.
//  R_PMISC->PWPR = 0x80; // Write 0x80 to re-enable register write protection.
//  R_PMISC->PWPR &= ~0x40; // Clear PWPR_PFSWE to disable PFS register write access.
//  R_PMISC->PWPR |= 0x80;  // Set PWPR_BOWI re-enable register write protection.

}





/***** Fast write digital pin *****/
static inline void digitalWriteFast(pin_size_t pin, uint8_t val) {
  
  byte _port = g_pin_cfg[pin].pin >> 8;
  byte _pin = g_pin_cfg[pin].pin & 0xFF;

  if (val) {
    R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PODR = 1;
  }else{
    R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PODR = 0;
  }
}


/***** Fast read digital pin *****/
static inline uint8_t digitalReadFast(pin_size_t pin) {

  byte _port = g_pin_cfg[pin].pin >> 8;
  byte _pin = g_pin_cfg[pin].pin & 0xFF;
  
  if (R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PIDR) {
    return HIGH;
  }else{
    return LOW;
  }

}


/***** Fast toggle digital pin *****/
static inline void digitalToggleFast(pin_size_t pin) {
  
  byte _port = g_pin_cfg[pin].pin >> 8;
  byte _pin = g_pin_cfg[pin].pin & 0xFF;

  R_PFS->PORT[_port].PIN[_pin].PmnPFS_b.PODR ^= 1;
  
}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    for (uint8_t i=0; i<8; i++){
      pinModeFast(databus[i], OUTPUT);
      digitalWriteFast(databus[i], HIGH);
    }
    return;
  }
  for (uint8_t i=0; i<8; i++){
    pinModeFast(databus[i], INPUT_PULLUP);
  }
//  lastbyte = 0;
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  for (uint8_t i=0; i<8; i++){
    if (!digitalReadFast(databus[i])) db += (1<<i);
  }
  return db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {
  for (uint8_t i=0; i<8; i++){
    digitalWriteFast(databus[i], ((db&(1<<i)) ? LOW : HIGH) );
  }
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {
  // Set pin state
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) digitalWriteFast( ctrlbus[i], ((bits&(1<<i)) ? HIGH : LOW) );
  }
}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {
  // Set pin direction
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) pinModeFast( ctrlbus[i], ((bits&(1<<i)) ? OUTPUT : INPUT_PULLUP) );
  }
}


uint8_t getGpibPinState(uint8_t pin){
  return digitalReadFast(pin);
}



#endif // RA4M1_NANO_R4
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** UNO/NANO R4 RENESAS BOARD LAYOUT *****/
/********************************************/



/***********************************/
/***** TEENSY 4.1 BOARD LAYOUT *****/
/***** vvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef IMXRT1062_TEENSY41_01
/*
  Data pin map
  ------------
  DIO1_PIN  23 : GPIB 1  : AD_B1_09 : GPIO1_25
  DIO2_PIN  22 : GPIB 2  : AD_B1_08 : GPIO1_24
  DIO3_PIN  21 : GPIB 3  : AD_B1_11 : GPIO1_27
  DIO4_PIN  20 : GPIB 4  : AD_B1_10 : GPIO1_26
  DIO5_PIN  19 : GPIB 13 : AD_B1_00 : GPIO1_16
  DIO6_PIN  18 : GPIB 14 : AD_B1_01 : GPIO1_17
  DIO7_PIN  17 : GPIB 15 : AD_B1_06 : GPIO1_22
  DIO8_PIN  16 : GPIB 16 : AD_B1_07 : GPIO1_23

  Control pin map
  ---------------
  IFC_PIN   15 : GPIB 9  : AD_B1_03 : GPIO1_19 : b0
  NDAC_PIN  14 : GPIB 8  : AD_B1_02 : GPIO1_18 : b1
  NRFD_PIN  41 : GPIB 7  : AD_B1_05 : GPIO1_21 : b2
  DAV_PIN   40 : GPIB 6  : AD_B1_04 : GPIO1_20 : b3
  EOI_PIN   39 : GPIB 5  : AD_B1_13 : GPIO1_29 : b4

  REN_PIN   26 : GPIB 17 : AD_B1_14 : GPIO1_30 : b5
  SRQ_PIN   38 : GPIB 10 : AD_B1_12 : GPIO1_28 : b6
  ATN_PIN   27 : GPIB 11 : AD_B1_15 : GPIO1_31 : b7

  Bits control lines as follows: 7-ATN_PIN, 6-SRQ_PIN, 5-REN_PIN, 4-EOI_PIN, 3-DAV_PIN, 2-NRFD_PIN, 1-NDAC_PIN, 0-IFC_PIN
    bits (databits) : State - 0=LOW, 1=HIGH/INPUT_PULLUP; Direction - 0=input, 1=output;
    mask (mask)     : 0=unaffected, 1=enabled
*/


/***************************************/
/*****   TEENSY NATIVE FUNCTIONS   *****/
/***************************************/
/*
 * While the GPIO direction is set to input, (GDIR[GDIR]=0), a
 * read access to DR does not return DR data. Instead it returns
 * the PSR data, which is the corresponding input signal value.
 *
 * GDIR - direction register
 * DR   - data register
 * PSR  - pad sample register
*/

/***** GPIB data and control buses ******/
const uint8_t databus[2][8] = {
                { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN },
                { 25, 24, 27, 26, 16, 17, 22, 23 }  // Corresponding GPIO6 bits
};

const uint8_t ctrlbus[2][8] = {
                { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN },
                { 19, 18, 21, 20, 29, 30, 28, 31 }  // Corresponding GPIO6 bits
};


/***** GPIO6 register value *****/
uint32_t gpioDbReg = 0;
uint32_t gpioctrlbus = 0;

volatile uint32_t * iomuxcPadGpio6[] = {
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_00,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_01,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_02,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_03,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_04,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_05,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_06,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_07,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_08,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_09,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_10,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_11,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_12,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_13,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_14,
  &IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B1_15
};

volatile uint32_t * iomuxcMuxGpio6[] = {
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_00,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_01,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_02,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_03,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_04,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_05,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_06,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_07,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_08,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_09,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_10,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_11,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_12,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_13,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_14,
  &IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B1_15
};


/***** Generate GPIO mask from assigned pin map *****/
uint64_t genGpioMask(const uint8_t buspins[][8], uint8_t bitmask) {
  uint64_t gpioreg = 0;
  for (uint8_t i=0; i<8; i++) {
    if (bitmask & (1 << i)) {
      gpioreg |= ( 1ULL << buspins[1][i] );
    }
  }
  return gpioreg;
}


/***** Set pullups on all bus pins *****/
void setBusPullups(const uint8_t bus[][8], bool enable) {
  volatile uint32_t * regptr = 0;
  if (enable) {
    for (uint8_t i=0; i<8; i++){
      regptr = iomuxcPadGpio6[ (bus[1][i] - 16) ];
      *regptr |= PIN_PULLUP_ENABLE;
    }
    return;
  }
  for (uint8_t i=0; i<8; i++){
    regptr = iomuxcPadGpio6[ (bus[1][i] - 16) ];
    *regptr &= PIN_PULLUP_DISABLE;  
  }
}


/***** Set SION bit on all bus pins *****/
void setBusSionBit(const uint8_t bus[][8], bool enable) {
  volatile uint32_t * regptr = 0;
  if (enable) {
    for (uint8_t i=0; i<8; i++){
      regptr = iomuxcMuxGpio6[ (bus[1][i] - 16) ];
      *regptr |= PIN_SION_ENABLE; 
    }
    return;
  }
  for (uint8_t i=0; i<8; i++){
    regptr = iomuxcMuxGpio6[ (bus[1][i] - 16) ];
    *regptr &= PIN_SION_DISABLE;    
  }
}


/***** Print 32-bit register *****/
void printReg(const char * regname, uint32_t regvalue){
  char buffer[64];
  char valbuf[34]; 
  
  memset(buffer, '\0', sizeof(buffer));
  memset(valbuf, '\0', sizeof(valbuf));
  memset(valbuf, 0x30, 32);

  for (uint8_t i=0; i<32; i++) {
    if (regvalue & (1UL<<i)) valbuf[31-i]++;
  }
  
  sprintf(buffer, "%-10s: %s", regname, valbuf);
  Serial.println(buffer);
}


/***** Initialise GPIO pins *****/
void initTsyGpioPins(){

  uint32_t gpioall;

  // Generate mask that identifies bits for each bus
  gpioDbReg = genGpioMask(databus, 0xFF);
  gpioctrlbus = genGpioMask(ctrlbus, 0xFF);
  gpioall = (gpioDbReg | gpioctrlbus);
  
  // Set all required GPIOs to input_pullup
  GPIO1_GDIR &= ~gpioall;
  setBusSionBit(databus, true);
  setBusSionBit(ctrlbus, true);
  setBusPullups(databus, true);
  setBusPullups(ctrlbus, true);

}


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {

  if (state == OUTPUT) {
    GPIO6_GDIR |= gpioDbReg;
    setBusPullups(databus, false);
    GPIO6_DR |= gpioDbReg;    // Outputs high by default
    return;
  }

  GPIO6_GDIR &= ~gpioDbReg;
  setBusPullups(databus, true);

}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  uint32_t reg = GPIO6_PSR;
  
  for (uint8_t i=0; i<8; i++){
    if ( reg & (1UL<<databus[1][i]) ) db |= (1U<<i);
  }
  
  return ~db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  uint32_t gpioval = genGpioMask(databus, ~db);

  GPIO6_GDIR |= gpioDbReg;            // Set as outputs
  GPIO6_DR |= gpioDbReg;              // Set high
  setBusPullups(databus, false);   // Disable pull-ups  
  GPIO6_DR &= (gpioval | ~gpioDbReg); // Set value

}


/***** Set control pin output level *****/
void setGpibCtrlState(uint8_t bits, uint8_t mask) {
  uint32_t gpiobit = 0;
  for (uint8_t i=0; i<8; i++) {    
    gpiobit = (1UL<<ctrlbus[1][i]);
    if (mask&(1U<<i)) {
      if (bits&(1U<<i)) {
        GPIO6_DR |= gpiobit;
      }else{
        GPIO6_DR &= ~gpiobit;
      }
    }
  }
}


/***** Set control pin direction *****/
void setGpibCtrlDir(uint8_t bits, uint8_t mask) {
  volatile uint32_t * regptr = 0;
  uint32_t gpiobit = 0;
  uint8_t idx = 0;
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1U<<i)) {
      gpiobit = (1UL<<ctrlbus[1][i]);
      idx = ctrlbus[1][i] - 16;
      if (bits&(1U<<i)) {
        // Output
        GPIO6_GDIR |= gpiobit;
        regptr = iomuxcPadGpio6[idx];
        *regptr &= PIN_PULLUP_DISABLE;
      }else{
        // Input_pullup
        GPIO6_GDIR &= ~gpiobit;
        regptr = iomuxcPadGpio6[idx];
        *regptr |= PIN_PULLUP_ENABLE;
      }
    }    
  }
}


uint8_t getGpibPinState(uint8_t pin){
//  return digitalRead(pin);
  return digitalReadFast(pin);
}


#endif  // IMXRT1062_TEENSY41_01
/***** ^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** TEENSY 4.1 BOARD LAYOUT *****/
/***********************************/




/*************************************/
/***** CUSTOM PIN LAYOUT SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvv *****/
#if defined (AR488_CUSTOM_DEFAULT) || defined (NON_ARDUINO)

uint8_t databus[8] = { DIO1_PIN, DIO2_PIN, DIO3_PIN, DIO4_PIN, DIO5_PIN, DIO6_PIN, DIO7_PIN, DIO8_PIN };
uint8_t ctrlbus[8] = { IFC_PIN, NDAC_PIN, NRFD_PIN, DAV_PIN, EOI_PIN, REN_PIN, SRQ_PIN, ATN_PIN };


/***** Set the GPIB data bus to input pullup *****/
void readyGpibDbus(uint8_t state = INPUT_PULLUP) {
  if (state == OUTPUT) {
    for (uint8_t i=0; i<8; i++){
      pinMode(databus[i], OUTPUT);
      // Set outputs to high?
      digitalWrite(databus[i], HIGH);
    }
  }

  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], INPUT_PULLUP);
  }
}


/***** Read the GPIB data bus wires to collect the byte of data *****/
uint8_t readGpibDbus() {
  uint8_t db = 0;
  for (uint8_t i=0; i<8; i++){
    db = db + (digitalRead(databus[i]) ? 0 : 1<<i );
  }
  return db;
}


/***** Set the GPIB data bus to output and with the requested byte *****/
void setGpibDbus(uint8_t db) {

  for (uint8_t i=0; i<8; i++){
    pinMode(databus[i], OUTPUT);
    digitalWrite(databus[i], ((db&(1<<i)) ? LOW : HIGH) );
  }
  
}


void setGpibCtrlState(uint8_t bits, uint8_t mask) {

  // Set pin state
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) digitalWrite( ctrlbus[i], ((bits&(1<<i)) ? HIGH : LOW) );
  }

}


void setGpibCtrlDir(uint8_t bits, uint8_t mask) {

  // Set pin direction
  for (uint8_t i=0; i<8; i++) {
    if (mask&(1<<i)) pinMode( ctrlbus[i], ((bits&(1<<i)) ? OUTPUT : INPUT_PULLUP) );
  }

}

uint8_t getGpibPinState(uint8_t pin){
  return digitalReadFast(pin);
}

#endif
/***** ^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** CUSTOM PIN LAYOUT SECTION *****/
/*************************************/




/************************************/
/***** COMMON FUNCTIONS SECTION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvv *****/

#ifdef LEVEL_SHIFTER

void initLevelShifter(){
  pinMode(LVL_SHIFT_EN, OUTPUT);
  digitalWrite(LVL_SHIFT_EN, HIGH);
}

void shiftEnable(bool stat){
  if (stat) {
    digitalWrite(LVL_SHIFT_EN, HIGH);
//    Serial.println(F("Level shifters ON"));

  }else{
    digitalWrite(LVL_SHIFT_EN, LOW);
//    Serial.println(F("Level shifters OFF"));
  }
}

#endif

/***** ^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** COMMON FUNCTIONS SECTION *****/
/************************************/
