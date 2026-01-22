# PoE Ethernet GPIB Adapter Software

## Programming

In order to program the adapter you first need to flash the Optiboot bootloader via Jtag with an UPDI programmer(Atmel ICE, serial updi or similar).
After this is done at least once, it can be programmed through the usb-c interface.

In order to program via usb-c, you can use platformio (from the `\SW` directory), or `avrdude` or `avrdudess`.

If you use `avrdude`, select the correct serial port of your device, and the `.elf` or `.hex` file you want to use, and run as follows:

`avrdude -c arduino -p atmega4809 -P {Your_serial_port} -D -U flash:w:{path_to_your_hex__or_elf_file}`

If you use `avrdudess`, the GUI version of avrdude, note that for ELF firmware files, use the `*.*` filter in file open dialog, there is no `*.elf` option for some reason.

# AR488, what has changed and how to integrate a new version of AR488

The GPIB part of this program is "forked" from https://github.com/Twilight-Logic/AR488, from ver. 0.53.34, 15/04/2026. It was enhanced with ethernet support, VXI-11.2 and a couple of User Interface options.

Since a proper fork was not possible seen the amount of changes and the specifics of the hardware, this file documents how the integration of the AR488 code was performed. Hoping it will help future code updates.

The original README from AR488 is here: [README_AR488](README_AR488.md), as is the [original AR488 manual](AR488-manual.pdf). Only the part regarding the Prologix protocol would be of interest here.

The files starting with `AR488` were copied to `\SW\src` and modified:

## AR488.ino

This is/was the 'main' file. It received most changes:

* changed the setup section, as the structure was not compatible with cohabitation with other socket servers
* was lacking forward declarations, making it incompatible with 'standard' compilers.

The file was renamed to 'prologix_server.cpp'. The code sections that were modified, are marked as such, with explanation of what was changed.

Anticipated solving AR488 issues #75 and #76 ("dataPort vs Serial in fndl_h", and "printHelp small issues")

## AR488_ComPorts.cpp and AR488_ComPorts.h

* Removed the sections with DEVNULL at the start of the files, it is not used.
* Added include to `EthernetStream.h`
* `startDataPort(unsigned long baud)` version added that points to a `EthernetStream`, surrounded by `#ifdef AR_ETHERNET_PORT/#else/#endif`
* `int maintainDataPort()` added
* `printBuf()` added
* Anticipated solving AR488 issue #78 ("debugPrint does not use debugPort")

## AR488_Config.h

* Added `#include "config.h"` at the top
* Comment out `#define USE_MACROS`

## AR488_Eeprom.cpp and AR488_Eeprom.h

no changes

## AR488_GPIBbus.cpp and AR488_GPIBbus.h

* Added a couple of sections with `#ifdef AR488_GPIBconf_EXTEND`, in order to store the IP address in the config.
* Adapted some debug prints behind `DEBUG_GPIBbus_RECEIVE`, marked with `// >>> CHANGED >>>`

## AR488_Layouts.cpp and AR488_Layouts.h

no changes, other than anticipation of AR488 issue #77 ("POE_ETHERNET_GPIB_ADAPTOR pullup control is ineffective")
