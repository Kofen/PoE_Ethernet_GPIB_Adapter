#include "config.h"

#ifdef USE_WEBSERVER
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Ethernet.h>
#include "web_server.h"
#include "AR488_ComPorts.h"
#include <StreamLib.h>

BasicWebServer::BasicWebServer() {
    // Constructor
}

void BasicWebServer::begin(bool debug = false) {
    this->debug = debug;
    server.begin();
}

int BasicWebServer::nr_connections(void) {
    int count = 0;
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (clients[i]) {
            count++;
        }
    }
    return count;
}

bool BasicWebServer::have_free_connections(void) {
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (!clients[i]) {
            return true;
        }
    }
    return false;
}

void BasicWebServer::loop(int nrConnections) {
    // simple TCP server based on 'server.accept()', meaning I must handle the lifecycle of the client
    // It is not blocking for input, but blocks for output

    // close any clients that are not connected
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        if (clients[i] && !clients[i].connected()) {
            if (debug) {
                debugPort.print(F("Force Closing Web connection of slot "));
                debugPort.print(i);
                debugPort.print(F(" from remote port "));
                debugPort.println(clients[i].remotePort());
            }
            clients[i].stop();            
        }
    }

    if (have_free_connections()) {
        // check if a new client is available
        EthernetClient newClient = server.accept();
        if (newClient) {
            bool found = false;
            for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
                if (!clients[i]) {
                    clients[i] = newClient;
                    found = true;
                    // init parser
                    currentLineIsBlank[i] = true;
                    charsRead[i] = 0;
                    memset(startreq[i], 0, sizeof(startreq[i]));
                    if (debug) {
                        debugPort.print(F("New Web connection in slot "));
                        debugPort.print(i);
                        debugPort.print(F(" from remote port "));
                        debugPort.println(newClient.remotePort());
                    }
                    break;
                }
            }

            if (!found) {
                // shouldn't happen, but still....
                if (debug) {
                    debugPort.print(F("Web connection limit reached from remote port "));
                    debugPort.println(newClient.remotePort());
                }
                newClient.stop();
            }
        }
    }

    // handle any incoming data
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        // an http request ends with a blank line
        while (clients[i].connected() && clients[i].available()) {
            char c = clients[i].read();
            // read the first line, until \newline or end of buffer.
            // The buffer was set to \0, so I'll always have a null terminated string, no matter the stop reason
            if (charsRead[i] < sizeof(startreq[i]) - 1) {
                if (c == '\r' || c == '\n') {
                    // end the line
                    charsRead[i] = sizeof(startreq[i]) - 1; // mark as full
                } else {
                    // store the character in the buffer
                    startreq[i][charsRead[i]] = c;
                    charsRead[i]++;
                }
            }
            // if you've gotten to the end of the line (received a newline
            // character) and the line is blank, the http request has ended,
            // so you can send a reply
            if (c == '\n' && currentLineIsBlank[i]) {
                if (debug) {
                    debugPort.print(F("Got complete request on slot "));
                    debugPort.print(i);
                    debugPort.print(F(": \""));
                    for (int j = 0; j < sizeof(startreq[i]); j++) {
                        char c = startreq[i][j];
                        if (c == 0) {
                            break;
                        }
                        debugPort.print(c);
                    }
                    debugPort.println(F("\""));
                }                
                // got all data. Check what the request was for
                // I only support GET requests
                char *path = NULL;
                if (startreq[i][0] == 'G' && startreq[i][1] == 'E' && startreq[i][2] == 'T' && startreq[i][3] == ' ') {
                    // This was GET
                    // now read until the first non space
                    int j = 4;
                    while ((j < sizeof(startreq[i])-1) && (startreq[i][j] == ' ') && (startreq[i][j] != 0)) {
                        j++;
                    }
                    path = (char *)(&startreq[i][j]);
                    // and then the first space after that
                    while ((j < sizeof(startreq[i])-1) && (startreq[i][j] != ' ') && (startreq[i][j] != 0)) {
                        j++;
                    }
                    // terminate the string
                    startreq[i][j] = 0;
                }
                // TODO: add a lookup table for the path, pointing to external functions
                // TODO: this here is just a test with 2 valid URLs
                // TODO: add context (like the connected instruments found) to the loop function or use an external function that has access to all of that
                if ((path != NULL) && ((strcmp(path,"/") == 0) || (strcmp(path,"/scan") == 0))) {
                    // this is the root path
                    // send a response
                    sendResponseOK(clients[i], nrConnections);
                } else {
                    // send an error response
                    sendResponseErr(clients[i]);
                }
                delay(10);  // yield to send reply
                if (debug) {
                    debugPort.print(F("Sent data and Closing Web connection of slot "));
                    debugPort.print(i);
                    debugPort.print(F(" from remote port "));
                    debugPort.println(clients[i].remotePort());                    
                }
                clients[i].stop();
                break;
            }
            if (c == '\n') {
                // you're starting a new line
                currentLineIsBlank[i] = true;
            } else if (c != '\r') {
                // you've gotten a character on the current line
                currentLineIsBlank[i] = false;
            }
        }
    }
};

void BasicWebServer::sendResponseErr(EthernetClient &client) {
    char buff[40];
    BufferedPrint bp(client, buff, sizeof(buff));

    bp.print(F("HTTP/1.1 404 Not Found\n"));
    bp.flush();
}

void BasicWebServer::sendResponseOK(EthernetClient &client, int nrConnections) {
    // This is the main page of the server. All other valid URLs return to this page.
    // The refresh is done via meta, allowing the page URL to be "reset" to the main page upon refresh
    //
    // The server construct the HTML streaming via buffer, as that greatly lowers the amount of network packets
    // If I do client.print, 1 character is sent at a time, using up 2 network packets per character

    char buff[512];
    BufferedPrint bp(client, buff, sizeof(buff));
    // TODO: consider moving to picocss or bootstrap for the buttons etc. But that would add an external dependency.

    bp.print(F("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n"));  
    bp.print(F("<!DOCTYPE HTML>\n<html><head><title>Ethernet2GPIB</title>"));
    bp.print(F("<style>body { font-family: Arial, sans-serif; }</style>"));
    bp.print(F("<meta http-equiv=\"refresh\" content=\"5;URL=/\">")); // refresh automatically, while going to the home page after 5 sec
    bp.print(F("</head><body>"));
    bp.print(F("<h1>" DEVICE_NAME "</h1>"));
    bp.print(F("<p>Number of client connections: "));
    bp.print(nrConnections);
    bp.print(F("</p>"));
#ifdef INTERFACE_VXI11
    bp.print(F("<h2>VXI-11 Ethernet Server</h2>"));
    bp.print(F("<p>VISA connection strings:</p><p>Controller: <b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::INSTR</b> (unless you have set the default instrument address to something else than 0)</p><p>Instruments: <b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::gpib,<i>N</i>::INSTR</b> or <b>...::inst<i>N</i>::INSTR</b>, where <i>N</i> is their address on the GPIB bus (1..30)</p>"));
#endif
#ifdef INTERFACE_PROLOGIX
    bp.print(F("<h2>Prologix GPIB Ethernet Server</h2>"));
    bp.print(F("<p>IP Address: "));
    bp.print(Ethernet.localIP());
    bp.print(F("</p>"));
#endif
    bp.print("</body></html>\n\n");
    bp.flush();
}
#endif