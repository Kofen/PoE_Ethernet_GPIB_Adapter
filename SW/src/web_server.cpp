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
                // TODO fill in /fnd, /snd, /rd, and see how to make "query" easier than 2 buttons
                // TODO make textarea readonly for the users
                char buff[512];
                BufferedPrint bp(clients[i], buff, sizeof(buff));                
                if (path != NULL) {
                    if (strcmp(path,"/") == 0) {
                        // this is the root path
                        // send a response
                        sendResponseOK(bp, nrConnections);                        
                    } else if (strcmp(path,"/cnx") == 0) {
                        sendResponsePlainNumber(bp, nrConnections);
                    } else {
                        // this is not a valid path
                        // send an error response
                        sendResponseErr(bp);
                    }
                } else {
                    // send an error response
                    sendResponseErr(bp);
                }
                bp.flush();
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

void BasicWebServer::sendResponseErr(BufferedPrint& bp) {
    bp.print(F("HTTP/1.1 404 Not Found\n"));
}

void BasicWebServer::sendResponsePlainText(BufferedPrint& bp, const char* text){
    // This is a simple response, no HTML
    bp.print(F("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\n"));
    bp.print(text);
}

void BasicWebServer::sendResponsePlainNumber(BufferedPrint& bp, int nr){
    // This is a simple response, no HTML
    bp.print(F("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\n"));
    bp.print(nr);
}

void BasicWebServer::printOption(BufferedPrint& bp, const char* name) {
    bp.print(F("<option value=\""));
    bp.print(name);
    bp.print(F("\">"));
    bp.print(name);
    bp.print(F("</option>"));
}

void BasicWebServer::sendResponseOK(BufferedPrint& bp, int nrConnections) {
    // This is the main page of the server. All other valid URLs return to this page.
    // The refresh is done via meta, allowing the page URL to be "reset" to the main page upon refresh
    //
    // The server construct the HTML streaming via buffer, as that greatly lowers the amount of network packets
    // If I do client.print, 1 character is sent at a time, using up 2 network packets per character

    // TODO: thoughts about ROM size optimisation:
    // a function call in AVR GCC costs 6 or 8 bytes (depending if the called function is far away: RCALL or CALL)
    // So: there is no use in splitting the HTML template into many smaller functions
    
    bp.print(F("HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: close\n\n"));  
    bp.print(F("<!DOCTYPE html><html lang=\"en\"> <head> <meta charset=\"UTF-8\" /> "
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /> "
        "<title>Ethernet2GPIB</title> <style> body { font-family: Arial, sans-serif; } "
#ifdef INTERFACE_VXI11
        "table { text-align: left; border-collapse: collapse; } "
        "th, td { padding: 2px 8px; white-space: nowrap; vertical-align: top; } "
        "button { margin: 0px 2px; color: white; background-color: gray; padding: 2px 12px; border-radius: 4px; border: 1px solid #ccc; cursor: pointer; } "
        "textarea { width: 100%; } input { width: 100%; } "
#endif
        "</style> </head> <body> <h1>" DEVICE_NAME "</h1> <p>Number of client connections: <span id=\"cnx\">"));
    bp.print(nrConnections);
    bp.print(F("</span></p> "));
#ifdef INTERFACE_PROLOGIX
    bp.print(F("<h2>Prologix GPIB Ethernet Server</h2><p>IP Address: "));
    bp.print(Ethernet.localIP());
    bp.print(F("</p>"));
#endif
#ifdef INTERFACE_VXI11
    bp.print(F("<h2>VXI-11 Ethernet Server</h2> <h3>VISA connection strings:</h3> <table> <tr> <td>Controller:</td> <td> <b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::INSTR</b> (unless you have set the default instrument address to something else than 0) </td> </tr> <tr> <td>Instruments:</td> <td> <b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::gpib,<i>N</i>::INSTR</b> or <b>...::inst<i>N</i>::INSTR</b>, where <i>N</i> is their address on the GPIB bus (1..30) </td> </tr> </table> "));
    bp.print(F("<h3>Interactive IO:</h3> <table> <tr> <th>Instruments</th> <th colspan=\"2\">Command</th> </tr> "
        "<tr> <td rowspan=\"4\"> <select id=\"inst\" size=\"4\"> </select> <br /> <button onclick=\"find()\">Find</button> </td> "
        "<td width=\"80%\"><input type=\"text\" id=\"cmd\" value=\"\" /></td> <td> <button onclick=\"self.cmd.value=self.pre.value\"><</button> "
        "<select id=\"pre\">"));
    printOption(bp, "*IDN?");
    printOption(bp, "*RST");
    printOption(bp, "*OPC?");
    printOption(bp, "*CLS");
    printOption(bp, ":SYSTem:ERRor?");
    bp.print(F("</select> </td> </tr> <tr> <td colspan=\"2\"> <button onclick=\"send()\">Send</button>"
        "<button onclick=\"read()\">Read</button> </td> </tr> "
        "<tr> <th colspan=\"2\">History</th> </tr> <tr> <td colspan=\"2\"> <textarea id=\"r\" rows=\"10\" cols=\"80\"></textarea><br /> "
        "<button onclick=\"self.r.value=''; scroll()\">Clear</button> </td> </tr> </table> "
        "<script>\nfunction tick() { fetch(\"/cnx\") .then((response) => { if (!response.ok) { return \"?\"; } return response.text(); }) .then((data) => { self.cnx.innerHTML = data; }); }\nsetInterval(tick, 5000);"
        "function find() { fetch(\"/fnd\") .then((response) => { if (!response.ok) { throw new Error(\"ERR: \" + response.statusText); } return response.text(); }) .then((data) => { self.inst.innerHTML = data; }); };\n"
        "function send() { const inst = self.inst.value; const cmd = self.cmd.value;\nif (cmd === \"\" || inst === \"\") { return; }\n"
        "fetch(\"/snd\" + \"/\" + inst + \"/\" + cmd) .then((response) => { if (!response.ok) { throw new Error(\"ERR: \" + response.statusText); } return response.text(); }) .then((data) => { self.r.value += \"> \" + inst + \": \" + cmd + \"\\n\"; scroll(); }); };\n"
        "function read() { const inst = self.inst.value;\nif (inst === \"\") { return; }\n"
        "fetch(\"/rd\" + \"/\" + inst) .then((response) => { if (!response.ok) { throw new Error(\"ERR: \" + response.statusText); } return response.text(); }) .then((data) => { self.r.value += \"< \" + inst + \": \" + data + \"\\n\"; scroll(); }); };\n"
        "function scroll() { self.r.scrollTop = self.r.scrollHeight; }\n</script>\n"));
#endif
    bp.print(F("</body></html>\n\n"));
}
#endif