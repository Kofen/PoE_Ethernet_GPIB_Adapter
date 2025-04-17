#include "EthernetStream.h"


EthernetStream::EthernetStream()
    : lastActivityTime(0), lastWriteTime(0), timeout(10000), timeout_write(500) {}


bool EthernetStream::begin(uint32_t port) {
        
    this->port = port;
    server = new EthernetServer(port);
    if (!server) return false;

    server->begin();
    lastActivityTime = millis();
    return true;
}

void EthernetStream::checkClient() {
    // This uses the 'server.available()' method. A client will become available once I have received data. 
    // You can also only have one client this way
    if (client && !client.connected()) {
        client.stop();
        client = EthernetClient();
    }
    if (!client) {
        client = server->available();
    }
    if (client) {
        lastActivityTime = millis();
    }
}

int EthernetStream::available() {
    if (!server) return 0;
    checkClient();
    if (!client) {
        client = server->available();
        if (client) {
        }        
    }
    if (client) {
        return client.available();
    }
    return 0;
}

int EthernetStream::read() {
    checkClient();
    if (client) {
        return client.read();
    }
    return -1;
}

int EthernetStream::peek() {
    checkClient();
    if (client) {
        return client.peek();
    }
    return -1;
}

void EthernetStream::flush() {
    if (client) {
        client.flush();
    }
}

size_t EthernetStream::write(uint8_t b) {
    if (client) {
        buffer += static_cast<char>(b);

        unsigned long now = millis();
        if (b == '\n' || buffer.length() >= 1024 || (now - lastWriteTime) > timeout_write) {
            client.print(buffer);
            buffer = "";
            lastWriteTime = now;
        }

        return 1;
    }
    return 0;
}


int EthernetStream::maintain(void) {
    unsigned long currentMillis = millis();
    if (client && (currentMillis - lastActivityTime > timeout)) {
        client.stop();
        client = EthernetClient();
    }
    // TOOD: would it be better to just call checkClient?
    if (client) {
        return 1;
    } else {
        return 0;
    }
}
