#ifndef _24AA256UID_H
#define _24AA256UID_H

#include <Arduino.h>
#include <Wire.h>

class _24AA256UID {
    
public:
    _24AA256UID(uint8_t address = 0x50, bool pinswap = false, bool debug = false);

    void begin();
    void getMACAddress(uint8_t* mac);
    void getUniqueID(uint8_t* uid);
    void getIPAddress(uint8_t* ip);
    void setIPAddress(uint8_t* ip);
    uint8_t getDefaultInstrument(void);
    void setDefaultInstrument(uint8_t instrument);

private:
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t data);
    void readBytes(uint16_t address, uint8_t* buffer, size_t length);
    void writeBytes(uint16_t address, const uint8_t* buffer, size_t length);

    void writePage(uint16_t address, const uint8_t* data, size_t length);
    void readPage(uint16_t address, uint8_t* data);

    uint8_t deviceAddress;
    bool debugEnabled;
    bool pinswap;

    void printDebug(const String& message);
};

#endif

