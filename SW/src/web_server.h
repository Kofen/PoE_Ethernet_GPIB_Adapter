#pragma once

#include <Ethernet.h>
#include <StreamLib.h>
#include "config.h"

#define MAX_WEB_CLIENTS 1

#define MAX_START_LINE_LENGTH 64

class BasicWebServer {
public:
    BasicWebServer();
    void begin(bool debug = false);
    void loop(int nrConnections);

private:
    bool debug;
    int nr_connections(void);
    bool have_free_connections(void);
    void sendResponseErr(BufferedPrint& bp);
    void sendResponseOK(BufferedPrint& bp, int nrConnections);
    void sendResponseHeaderPlainText(BufferedPrint& bp);
    EthernetServer server = EthernetServer(80);
    EthernetClient clients[MAX_WEB_CLIENTS];
    bool currentLineIsBlank[MAX_WEB_CLIENTS];
    int charsRead[MAX_WEB_CLIENTS];
    uint8_t startreq[MAX_WEB_CLIENTS][MAX_START_LINE_LENGTH]; // buffer for the request line

    void printOption(BufferedPrint& bp, const char* name);
};
