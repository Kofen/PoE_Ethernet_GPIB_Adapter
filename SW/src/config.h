#pragma once

#ifndef __AVR_ATmega4809__
#error "The compiler must set __AVR_ATmega4809__ for ATmega4809 based boards!"
// __AVR_ATmega4809__ will force a define of POE_ETHERNET_GPIB_ADAPTOR, repeat it here for the IDEs that do not set it automatically
#define POE_ETHERNET_GPIB_ADAPTOR
#endif

#define DEVICE_NAME "Ethernet2GPIB Gateway v2.2 (AR488 v0.53.34)\n"

#define LED_R 13
#define LED_G 39
#define LED_B 38

// This is needed, and debugPort should point to Serial if you want to use the serial menu
#define DEBUG_ENABLE
// define USE_SERIALMENU if you want a basic menu on the serial console for amongst others IP address setting.
#define USE_SERIALMENU

// Choice of interface type: Prologix or VXI11
// Only one should be defined. If none is defined, VXI11 will be used.
#ifndef INTERFACE_PROLOGIX
#define INTERFACE_VXI11
#endif
// If you define both, well, you'll have to deal with the compiler telling you there is not enough ROM.

// Debugging:

// define LOG_VXI_DETAILS if you want to see VXI details on USB Serial (debugPort)
// It will mess up the serial menu a bit
//#define LOG_VXI_DETAILS

// define LOG_WEB_DETAILS if you want to see Web server details on USB Serial (debugPort)
// It will mess up the serial menu a bit
//#define LOG_WEB_DETAILS

// define LOG_STATS_ON_CONSOLE when you want to see memory usage
// and other details in auto refresh on the USB Serial (debugPort)
//#define LOG_STATS_ON_CONSOLE

// Debugging AR488:
// These options will log to USB Serial (debugPort)
//#define DEBUG_GPIBbus_SEND
//#define DEBUG_GPIBbus_RECEIVE
//#define DEBUG_GPIBbus_CONTROL
//#define DEBUG_GPIBbus_DEVICE
//#define DEBUG_GPIB_COMMANDS
//#define DEBUG_GPIB_ADDRESSING

// define DISABLE_WEB_SERVER if you do NOT want to use the web server or if you activated too many debug options above. 
//#define DISABLE_WEB_SERVER

// Prologix has no ROM space for the web server, so disabling it if you have selected Prologix:
#ifdef INTERFACE_PROLOGIX
#define DISABLE_WEB_SERVER
#endif

// The web server serves a static explanation page and maybe some interactive use (see below).
#ifdef DISABLE_WEB_SERVER
#ifdef USE_WEBSERVER
#undef USE_WEBSERVER
#endif
#else
#define USE_WEBSERVER
#endif

// define WEB_INTERACTIVE for the interactive web server
#ifdef USE_WEBSERVER
#define WEB_INTERACTIVE
#endif

// PORTS

// For the Prologix server: 
#define AR_ETHERNET_PORT
#define PROLOGIX_PORT 1234

// For the VXI server:
#define VXI11_PORT 9010
// Maximum number of clients for the VXI server:
// Max sockets on the device. You will likely not even be able to reach that number, because of other sockets open or busy closing
// MAX_SOCK_NUM is defined in the Ethernet library, and is 4 for W5100 and 8 for W5200 and W5500.
#define MAX_VXI_CLIENTS MAX_SOCK_NUM

// EEPROM use: 
// Writing the 24AA256 is somehow broken, so we can also write via the GPIB configuration via AR488_GPIBconf_EXTEND
#define AR488_GPIBconf_EXTEND
