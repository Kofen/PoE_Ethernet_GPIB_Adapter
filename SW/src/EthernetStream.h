#ifndef ETHERNETSTREAM_H
#define ETHERNETSTREAM_H

#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

class EthernetStream : public Stream {
public:
    EthernetStream();
  
    bool begin(uint32_t port);
    int maintain(void);
    int available() override;
    int read() override;
    int peek() override;
    void flush() override;
    size_t write(uint8_t b) override;
    //size_t write(const uint8_t *buffer, size_t size) override;  // Override for writing buffers
    using Print::write;  // Bring in other overloads of write from Print

private:
    byte* mac;
    IPAddress ip;
    uint16_t port;
    void checkClient();
    EthernetServer *server;
    EthernetClient client;
    String buffer;
    unsigned long lastActivityTime;  // Track the last activity time
    const unsigned long timeout;  // Timeout period in milliseconds
    unsigned long lastWriteTime; // Track time since last data to buffer
    const unsigned long timeout_write; // Flush buffer if no newline after timeout period

};

#endif

