#pragma once

#include <Ethernet.h>
#include <StreamLib.h>
#include "config.h"

#define MAX_WEB_CLIENTS 1

#define MAX_START_LINE_LENGTH 256

class BasicWebServer {
public:
    BasicWebServer();
    void begin();
    void loop(int nrConnections);
    void killClients(void);

private:
    int nr_connections(void);
    bool have_free_connections(void);
    void handleRequest(EthernetClient& client, char* path, int nrConnections);
    void sendResponseErr(BufferedPrint& bp);
    void sendResponseOK(BufferedPrint& bp, int nrConnections);
    void sendResponseHeaderPlainText(BufferedPrint& bp);
    EthernetServer server = EthernetServer(80);
    EthernetClient clients[MAX_WEB_CLIENTS];
    bool currentLineIsBlank[MAX_WEB_CLIENTS]; // if the current line is blank (marks the end of the request)
    int charsRead[MAX_WEB_CLIENTS]; // The total number of characters read into the startreq buffer
    char startreq[MAX_WEB_CLIENTS][MAX_START_LINE_LENGTH]; // buffer for the request line

    void printOption(BufferedPrint& bp, const char* name);
    void printOption(BufferedPrint& bp, const char* name, int nr);
};
