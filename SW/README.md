# PoE Ethernet GPIB Adapter Software

## Programming

In order to program the adapter you first need to flash the Optiboot bootloader via Jtag with an UPDI programmer(Atmel ICE, serial updi or similar).
After this is done, it can be programmed through the usb-c interface.

# AR488, what has changed and how to integrate a new version of AR488

The GPIB part of this program is "forked" from https://github.com/Twilight-Logic/AR488, from ver. 0.53.03, 08/04/2025. It was enhanced with ethernet support, VXI-11.2 and a couple of User Interface options.

Since a proper fork was not possible seen the amount of changes and the specifics of the hardware, this file documents how the integration of the AR488 code was performed. Hoping it will help future code updates.

The original README from AR488 is here: [README_AR488](README_AR488.md), as is the [original AR488 manual](AR488-manual.pdf). Only the part regarding the Prologix protocol would be of interest here.

The files starting with `AR488` were copied to `\SW\src` and modified:

## AR488.ino

This is/was the 'main' file. It received most changes:

* changed the setup section, as the structure was not compatible with cohabitation with other socket servers
* was lacking forward declarations, making it incompatible with 'standard' compilers.

The file was renamed to 'prologix_server.cpp'. The code sections that were modified, are marked as such, with explanation of what was changed.

## AR488_ComPorts.cpp and AR488_ComPorts.h

* DEVNULL externalised to become a dependency (this is minor, as we do not use it, hence DEVNULL.h is commented out)
* Added include to `EthernetStream.h`
* `startDataPort()` version added that points to a `EthernetStream`.
* `int maintainDataPort()` added
* `printBuf()` added

## AR488_Config.h

* Set `#define AR488_CUSTOM`, plus content of the 2 relevant sections

## AR488_Eeprom.cpp and AR488_Eeprom.h

no changes

## AR488_GPIBbus.cpp and AR488_GPIBbus.h

* Changed `void sendData(char *data, uint8_t dsize, );`: added `const` qualifier to `*data` and added `bool isLastPacket = true` parameter
* Changed `receiveData(Stream &dataStream, bool detectEoi, bool detectEndByte, uint8_t endByte)`, added `int maxSize = 0` parameter, plus handling of the return value
* Added `enum receiveState`
* Added a couple of sections with `#ifdef AR488_GPIBconf_EXTEND`, in order to store the IP address in the config.

All but AR488_GPIBconf_EXTEND is under way for inclusion in the upstream repo, and is partially integrated in version 0.53.11. See Twilight-Logic/AR488 PR #61  [Allow maxSize in receiveData()](https://github.com/Twilight-Logic/AR488/pull/61)

## AR488_Layouts.cpp and AR488_Layouts.h

Replaced the entire `CUSTOM PIN LAYOUT SECTION` in the .cpp file.
