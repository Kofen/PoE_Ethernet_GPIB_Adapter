#pragma once

#define DEVICE_NAME "Ethernet2GPIB Gateway v1.2 (AR488 v0.53.03)\n"

#define LED_R 13
#define LED_G 39
#define LED_B 38

// This is needed, and debugPort should point to Serial if you want to use the serial menu
#define DEBUG_ENABLE
// define USE_SERIALMENU if you want a basic menu on the serial console for amongst others IP address setting.
#define USE_SERIALMENU

// define DISABLE_WEB_SERVER if you do not want to use the web server. 
// The web server serves a static explanation page and maybe some interactive use (see below).
#ifndef DISABLE_WEB_SERVER
#define USE_WEBSERVER
#endif

#ifndef INTERFACE_PROLOGIX
#define INTERFACE_VXI11
#endif
// and if you define both, well, you'll have to deal with the compiler telling you there is not enough ROM.


// for the Prologix server: 
#define AR_ETHERNET_PORT
#define PROLOGIX_PORT 1234

// For the VXI server:
#define VXI11_PORT 9010
// Maximum number of clients for the VXI server:
// Max sockets on the device. You will likely not even be able to reach that number, because of other sockets open or busy closing
// MAX_SOCK_NUM is defined in the Ethernet library, and is 4 for W5100 and 8 for W5200 and W5500.
#define MAX_VXI_CLIENTS MAX_SOCK_NUM

// define LOG_VXI_DETAILS, if you want to see VXI details on the debugPort
// It will mess up the serial menu a bit
// #define LOG_VXI_DETAILS

// define LOG_WEB_DETAILS if you want to see Web server details on the debugPort
// setting to 1 messes up the serial menu a bit
//#define LOG_WEB_DETAILS

// define WEB_INTERACTIVE for the interactive web server
// This should also work with the Prologix, but at present it is over the limit, hence the inclusion only with VXI11.
#ifdef INTERFACE_VXI11
#define WEB_INTERACTIVE
#endif

// Only activate this when you want to see memory usage 
// and other details in auto refresh on the console.
// #define LOG_STATS_ON_CONSOLE

// EEPROM use: 
// Writing the 24AA256 is somehow broken, so we can also write via the GPIB configuration via AR488_GPIBconf_EXTEND
#define AR488_GPIBconf_EXTEND
