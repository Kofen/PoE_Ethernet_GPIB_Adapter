#include "24AA256UID.h"
#include "AR488_ComPorts.h"

// device address range of 24AA256UID is 0x50 to 0x57
// The device is a 32kB (256Kbit) EEPROM with pages of 64 bytes each. 
// Do not write over page boundaries.
// Content address range is 0x0000 to 0x7FFF
// Page start addresses are: 0x00, 0x40, 0x80, 0xC0, ...
#define PAGE_SIZE 64
// This code makes no effort to do wear levelling, as writing is supposed to be extremely rare.
// It also doesn't take the busy flag into account, which one should do if you write a lot.
//
//
// Content organisation:

// MAC address: 0x7F7A-0x7F7F (6 bytes)
#define MAC_ADDRESS_START 0x7F7A

// unique ID:   0x7FFA-0x7FFD (4 bytes)
#define UNIQUE_ID_START 0x7FFA

// IP address:   0x7F00-0x7F03 (4 bytes)
#define IP_ADDRESS_START 0x7F00

// Default instrument: 0x7F04 (1 byte)
#define DEFAULT_INSTRUMENT_START 0x7F04



_24AA256UID::_24AA256UID(uint8_t address, bool pinswap, bool debug) : deviceAddress(address), pinswap(pinswap), debugEnabled(debug) {}

void _24AA256UID::begin() {
    //Wire.swap(pinswap);
    Wire.begin();
}

void _24AA256UID::getMACAddress(uint8_t* mac) {
    // Get MAC address EUI-48
    readBytes(MAC_ADDRESS_START, mac, 6);
    printDebug("MAC Address read.");
}

void _24AA256UID::getUniqueID(uint8_t* uid) {
    // Read unique ID 32 bit serial
    readBytes(UNIQUE_ID_START, uid, 4);
    printDebug("Unique ID read.");
}

void _24AA256UID::getIPAddress(uint8_t* ip) {
    readBytes(IP_ADDRESS_START, ip, 4);
    if (ip[0] == 255 && ip[1] == 255 && ip[2] == 255 & ip[3] == 255) {
        ip[0] = 0;
        ip[1] = 0;
        ip[2] = 0;
        ip[3] = 0;
    }
    printDebug("IP Address read.");
}

void _24AA256UID::setIPAddress(uint8_t* ip) {
    writeBytes(IP_ADDRESS_START, ip, 4);
    printDebug("IP Address written.");
}

uint8_t _24AA256UID::getDefaultInstrument(void){
    uint8_t r = readByte(DEFAULT_INSTRUMENT_START);
    if (r > 30) { 
        r = 0; // GPIB bus cannot go over 30
    }
    return r;
}

void _24AA256UID::setDefaultInstrument(uint8_t instrument){
    writeByte(DEFAULT_INSTRUMENT_START, instrument);
}

uint8_t _24AA256UID::readByte(uint16_t address) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, (size_t)1);
    printDebug("Byte read.");
    return Wire.available() ? Wire.read() : 0xFF; // Return 0xFF if no data is available
}

void _24AA256UID::writeByte(uint16_t address, uint8_t data) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.write(data & 0xFF);
    Wire.endTransmission();
    delay(10);
    printDebug("Byte written.");
}

void _24AA256UID::readBytes(uint16_t address, uint8_t* buffer, size_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, (size_t)length);
    for (int i = 0; i < length && Wire.available(); i++) {
        buffer[i] = Wire.read();
    }
    printDebug("Bytes read.");
}

void _24AA256UID::writeBytes(uint16_t address, const uint8_t* buffer, size_t length) {
    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    for (int i = 0; i < length; i++) {
        Wire.write(buffer[i] & 0xFF);
    }
    Wire.endTransmission();
    delay(10);
    printDebug("Bytes written.");
}

void _24AA256UID::writePage(uint16_t address, const uint8_t* data, size_t length) {
    if (length > PAGE_SIZE) {
        printDebug("Error: Length exceeds page size!");
        return;
    }

    uint16_t endAddress = address + length - 1;
    if ((address / PAGE_SIZE) != (endAddress / PAGE_SIZE)) {
        printDebug("Error: Write operation crosses page boundary!");
        return;
    }

    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    for (uint8_t i = 0; i < length; i++) {
        Wire.write(data[i]);
    }
    Wire.endTransmission();
    delay(10);
    printDebug("Page written within boundary.");
}

void _24AA256UID::readPage(uint16_t address, uint8_t* data) {
    if (address % PAGE_SIZE != 0) {
        printDebug("Error: Address is not page aligned!");
        return;
    }

    Wire.beginTransmission(deviceAddress);
    Wire.write((address >> 8) & 0xFF);
    Wire.write(address & 0xFF);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, (size_t)PAGE_SIZE);
    for (uint8_t i = 0; i < PAGE_SIZE && Wire.available(); i++) {
        data[i] = Wire.read();
    }
    printDebug("Page read.");
}

void _24AA256UID::printDebug(const String& message) {
    if (debugEnabled) {
        DB_RAW_PRINTLN(message);
    }
}

