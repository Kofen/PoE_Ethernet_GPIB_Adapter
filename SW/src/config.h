#pragma once

#define DEVICE_NAME "Ethernet2GPIB Gateway v1.1 (AR488 v0.53.03)\n"

#define LED_R 13
#define LED_G 39
#define LED_B 38

// This is needed, and debugPort should point to Serial if you want to use the serial menu
#define DEBUG_ENABLE
// define USE_SERIALMENU if you want a basic menu on the serial console for amongst others IP address setting.
#define USE_SERIALMENU

// define USE_WEBSERVER if you want to use a very basic web page server that serves a static explanation page.
// It is not interactive, just a cherry on the cake with help text.
// This is mainly meant to be used with the VXI server, but it should work with the Prologix server as long as ROM permits (it will be very close if not over the limit).
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
// Max MAX_SOCK_NUM sockets on the device. You will likely not even be able to reach that number, because of other sockets open or busy closing
#define MAX_VXI_CLIENTS MAX_SOCK_NUM
// define LOG_VXI_DETAILS, if you want to see VXI details on the debugPort
// It will mess up the serial menu a bit
#define LOG_VXI_DETAILS

// set LOG_WEB_DETAILS to 0 or 1, depending on whether you want to see Web server details on the debugPort
// setting to 1 messes up the serial menu a bit
#define LOG_WEB_DETAILS 0

// if you activate this, the serial menu will be messier. 
// Only activate this when you want to see memory usage 
// and other details in auto refresh on the console.
#define LOG_STATS_ON_CONSOLE

// EEPROM use: 
// Writing the 24AA256 is somehow broken, so we can also write via the GPIB configuration
#define AR488_GPIBconf_EXTEND
