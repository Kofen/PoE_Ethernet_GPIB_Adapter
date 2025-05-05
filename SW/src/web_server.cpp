#include "config.h"

#ifdef USE_WEBSERVER
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <Ethernet.h>
#include "web_server.h"
#include "AR488_ComPorts.h"
#include <StreamLib.h>

int decodeHexDigit(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    return -1;
}

char *ltrim(char *s) {
    while(isspace(*s)) s++;
    return s;
}

char *rtrim(char *s) {
    char* back = s + strlen(s);
    while(isspace(*--back));
    *(back+1) = '\0';
    return s;
}

char *trim(char *s) {
    return rtrim(ltrim(s)); 
}

// *********************************************** */
// SCPI bus read
// *********************************************** */

#include "AR488_GPIBbus.h"

extern GPIBbus gpibBus;

/**
 * @brief scan the GPIB bus and return all found instruments. 
 * It is heavily inspired by the fndl_h() function from the Prologix GPIB controller, with the parameter "all".
 * 
 * @return uint32_t bitmap: bit 1 = address 1, etc. Handy that addresses are 1-31
 */
uint32_t fndl(void) {
    uint16_t tmo = gpibBus.cfg.rtmo;
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t pri = 0xFF;
    uint32_t bitmap = 0;

    // Set minimal timeout
    gpibBus.cfg.rtmo = 35;

    // Request 'all'
    j = 31;

    // Poll the range of GPIB adresses
    while (i < j) {
        pri = i;

        // Ignore the controller address
        if (pri == gpibBus.cfg.caddr) {
            i++;
            continue;
        }

        // Serial.print("PRI: ");
        // Serial.println(pri);

        // Send UNL + UNT + LAD (addressDevice function adds 0x20 to pri)
        if (gpibBus.addressDevice(pri, 0xFF, TOLISTEN) == 1) {
#ifdef LOG_WEB_DETAILS
            debugPort.print(F("Error in addressDevice for primary address "));
            debugPort.println(pri);            
#endif
            break;
        }

        gpibBus.clearSignal(ATN_BIT);
        delayMicroseconds(1600);

        if (gpibBus.isAsserted(NDAC_PIN)) {
#ifdef LOG_WEB_DETAILS
            debugPort.print(F("Found Device with primary address "));
            debugPort.println(pri);            
#endif
            bitmap |= (1UL << pri);
            // else.... I do no scan for secondary addresses
        }  // End if NDAC aserted

        gpibBus.setControls(CIDS);
        //    delay(50);
        i++;

    }  // END while

    gpibBus.cfg.rtmo = tmo;

    gpibBus.setControls(CIDS);
    return bitmap;
}

// *********************************************** */
// the web server
// *********************************************** */

// This is a simple web server that handles a single connection at a time.
BasicWebServer::BasicWebServer() {
    // Constructor
}

void BasicWebServer::begin() {
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
#ifdef LOG_WEB_DETAILS
            debugPort.print(F("Force Closing Web connection of slot "));
            debugPort.print(i);
            debugPort.print(F(" from remote port "));
            debugPort.println(clients[i].remotePort());
#endif
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
#ifdef LOG_WEB_DETAILS
                    debugPort.print(F("New Web connection in slot "));
                    debugPort.print(i);
                    debugPort.print(F(" from remote port "));
                    debugPort.println(newClient.remotePort());
#endif
                    break;
                }
            }

            if (!found) {
                // shouldn't happen, but still....
#ifdef LOG_WEB_DETAILS
                debugPort.print(F("Web connection limit reached from remote port "));
                debugPort.println(newClient.remotePort());
#endif
                newClient.stop();
            }
        }
    }

    // handle any incoming data
    for (int i = 0; i < MAX_WEB_CLIENTS; i++) {
        // an http request ends with a blank line
        while (clients[i].connected() && clients[i].available()) {
            char c = clients[i].read();
            // read the first line, until newline or end of buffer.
            // The buffer was set to \0, and I leave 1 free at the end, so I'll always have a null terminated string, no matter the stop reason
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
                // doubly make sure we have a null terminated string
                startreq[i][sizeof(startreq[i])-1] = '\0';
#ifdef LOG_WEB_DETAILS
                debugPort.print(F("Got complete web request on slot "));
                debugPort.print(i);
                debugPort.print(F(": \""));
                debugPort.print(startreq[i]);
                debugPort.println(F("\""));
#endif
                // got all data. Check what the request was for
                // I only support GET requests
                char *path = NULL;
                char* token;
                char* rest = startreq[i];

                // get the first token, must be GET
                token = strtok_r(rest, " ", &rest);
                if (token && (strcasecmp(token, "GET") == 0)) {
                    // This is a GET request
                    // get the path
                    token = strtok_r(rest, " ", &rest);
                    if (token) {
                        // this is the path
                        path = token;
                    }
                }
                handleRequest(clients[i], path, nrConnections);

                delay(10);  // yield to send reply
#ifdef LOG_WEB_DETAILS
                debugPort.print(F("Sent data and Closing Web connection of slot "));
                debugPort.print(i);
                debugPort.print(F(" from remote port "));
                debugPort.println(clients[i].remotePort());                    
#endif
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

void BasicWebServer::handleRequest(EthernetClient& client, char* path, int nrConnections) {
    // This is the main function that handles the request
    // It will send a response to the client based on the request

    char buff[512];
    BufferedPrint bp(client, buff, sizeof(buff));
    bool isOK = false;       
    if (path != NULL) {
        if (strcmp(path,"/") == 0) {
            // this is the root path
            // send a response
            sendResponseOK(bp, nrConnections);
            isOK = true;                     
        } else if (strcmp(path,"/cnx") == 0) {
            sendResponseHeaderPlainText(bp);
            bp.print(nrConnections);
            isOK = true;
        } else if (strcmp(path,"/fnd") == 0) {
            sendResponseHeaderPlainText(bp);
            // this is the find command
            // search for instruments on the bus, see fndl_h()
            uint32_t bitmap = fndl();
            for (int i = 0; i < 32; i++) {
                if (bitmap & (1UL << i)) {
                    // this is a found instrument
                    printOption(bp, "gpib,", i);
                }
            }
            isOK = true;
        } else if (strncmp(path,"/ex",3) == 0) {
            int cmd_type = -1;
            int addr = -1;
            int num_chars = -1;
            int r = sscanf(path, "/ex%d/gpib,%d/%n", &cmd_type, &addr, &num_chars);
            if (r == 2 && num_chars > 0 && cmd_type >= 0 && cmd_type < 3 && addr > 0 && addr < 32) {
                // I have a path like /ex1/gpib,1/123
                char *cmd = path + num_chars;
                // now decode the command, in place. It can have %NN encoded characters
                char *src = cmd;
                char *dst = cmd;
                bool copied = false;
                while (*src) {
                    copied = false;
                    if (*src == '%') {
                        // this is a %NN encoded character
                        // convert the next two characters from a hex value
                        // and store it in the destination
                        char ch1 = *(src + 1);
                        char ch2 = *(src + 2);
                        if (ch1 && ch2) {
                            int val1 = decodeHexDigit(ch1);
                            int val2 = decodeHexDigit(ch2);
                            if (val1 >= 0 && val2 >= 0) {
                                // this is a valid %NN encoded character
                                // convert it to the character and store it in the destination
                                *dst = (val1 << 4) | val2;
                                src += 2; // move the source pointer
                                copied = true;
                            }
                        }
                    }
                    if (!copied) {
                        // this is a normal character or something went wrong
                        // just copy it to the destination
                        *dst = *src;
                    }
                    src++;
                    dst++;
                }
                *dst = '\0'; // null terminate the string
                // now trim it
                cmd = trim(cmd);

#ifdef LOG_WEB_DETAILS
                debugPort.print(F("Got cmdtype "));
                debugPort.print(cmd_type);
                debugPort.print(F(" for address "));
                debugPort.print(addr);
                debugPort.print(F(": \""));
                debugPort.print(cmd);
                debugPort.println(F("\""));
#endif                

                // Handle the command
                sendResponseHeaderPlainText(bp);
                if (cmd_type == 0 || cmd_type == 1) {
                    // this is the send command
                    // TODO filter out instrument and parameters
                    // and send the command to the instrument
                    isOK = true;
                }
                if ((cmd_type == 2) || (cmd_type == 0 && cmd[strlen(cmd)-1] == '?')) {
                    // this is the read command
                    // TODO filter out instrument
                    // and get the reply from the instrument
                    // mock the response for now
                    bp.print(F("bla"));
                    isOK = true;
                }
            }
        }
    }
    if (!isOK) {
        // send an error response
        sendResponseErr(bp);
    }
    bp.flush();
}

void BasicWebServer::sendResponseErr(BufferedPrint& bp) {
    bp.print(F("HTTP/1.1 404 Not Found\n"));
}

void BasicWebServer::sendResponseHeaderPlainText(BufferedPrint& bp){
    // This is a simple response, no HTML
    bp.print(F("HTTP/1.1 200 OK\nContent-Type: text/plain\nConnection: close\n\n"));
}

void BasicWebServer::printOption(BufferedPrint& bp, const char* name) {
    bp.print(F("<option value=\""));
    bp.print(name);
    bp.print(F("\">"));
    bp.print(name);
    bp.print(F("</option>"));
}

void BasicWebServer::printOption(BufferedPrint& bp, const char* name, int nr) {
    bp.print(F("<option value=\""));
    bp.print(name);
    bp.print(nr);
    bp.print(F("\">"));
    bp.print(name);
    bp.print(nr);
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
    bp.print(F("<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\" /> "
        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" /> "
        "<title>Ethernet2GPIB</title><style> body { font-family: Arial, sans-serif; } "
#ifdef INTERFACE_VXI11
        "table { text-align: left; border-collapse: collapse; } "
        "th, td { padding: 2px 8px; white-space: nowrap; vertical-align: top; } "
        "button { margin: 0px 2px; color: white; background-color: gray; padding: 2px 12px; border-radius: 4px; border: 1px solid #ccc; cursor: pointer; } "
        "textarea { width: 100%; } input { width: 100%; } "
#endif
        "</style></head><body><h1>" DEVICE_NAME "</h1><p>Number of client connections: <span id=\"cnx\">"));
    bp.print(nrConnections);
    bp.print(F("</span></p>\n"));
#ifdef INTERFACE_PROLOGIX
    bp.print(F("<h2>Prologix GPIB Ethernet Server</h2><p>IP Address: "));
    bp.print(Ethernet.localIP());
    bp.print(F("</p>"));
#endif
#ifdef INTERFACE_VXI11
    bp.print(F("<h2>VXI-11 Ethernet Server</h2><h3>VISA connection strings:</h3><table><tr><td>Controller:</td><td><b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::INSTR</b> (unless you have set the default instrument address to something else than 0)</td></tr><tr><td>Instruments:</td><td><b>TCPIP::"));
    bp.print(Ethernet.localIP());
    bp.print(F("::gpib,<i>N</i>::INSTR</b> or <b>...::inst<i>N</i>::INSTR</b>, where <i>N</i> is their address on the GPIB bus (1..30)</td></tr></table>"
        "<h3>Interactive IO:</h3><table><tr><th>Instruments</th><th colspan=\"2\">Command</th></tr> "
        "<tr><td rowspan=\"4\"><select id=\"inst\" size=\"4\" style=\"width: 9ch;\"></select><br /><button onclick=\"find()\">Find</button></td>"
        "<td width=\"80%\"><input type=\"text\" id=\"cmd\" maxlength=100 value=\"\" /></td><td><button onclick=\"self.cmd.value=self.pre.value\">&lt;</button>"
        "<select id=\"pre\">"));
    printOption(bp, "*IDN?");
    printOption(bp, "*RST");
    printOption(bp, "*OPC?");
    printOption(bp, "*CLS");
    printOption(bp, ":SYSTem:ERRor?");
    bp.print(F("</select></td></tr><tr><td colspan=\"2\">"
        "<button onclick=\"ex(0)\">Execute</button>"
        "&nbsp;&nbsp;(<button onclick=\"ex(1)\">Send</button> <button onclick=\"ex(2)\">Read</button>) "
        "</td></tr>"
        "<tr><th colspan=\"2\">History</th></tr><tr><td colspan=\"2\"><textarea id=\"r\" rows=\"10\" cols=\"80\" readonly></textarea><br /> "
        "<button onclick=\"self.r.value=''; scroll()\">Clear</button></td></tr></table>\n"
        "<script>\nfunction tick() { fetch(\"/cnx\") .then((response) => { if (!response.ok) { return \"?\"; } return response.text(); }) .then((data) => { self.cnx.innerHTML = data; }); }\nsetInterval(tick, 5000);"
        "function find() { fetch(\"/fnd\") .then((response) => { if (!response.ok) { throw new Error(\"ERR: \" + response.statusText); } return response.text(); }) .then((data) => { self.inst.innerHTML = data; }); };\n"
        "function ex(t) { const inst = self.inst.value; const cmd = self.cmd.value;\nif (inst === \"\") { alert(\"Please select an instrument\"); return; }\n"
        "var m = \"/ex\" + t.toString() + \"/\" + inst + \"/\";\n"
        "if (t < 2) { if (cmd === \"\") { alert(\"Please enter a command\"); return; } m += cmd; }\n"  // no encodeURIComponent here, decoding that would require a lot of code and ROM
        "fetch(m) .then((response) => { if (!response.ok) { throw new Error(\"ERR: \" + response.statusText); } return response.text(); })\n"
        ".then((data) => { if (t < 2) { self.r.value += \"> \" + inst + \": \" + cmd + \"\\n\"; }\n"
        "if ((t === 2)||((t === 0)&&(data !== \"\"))) { self.r.value += \"< \" + inst + \": \" + data + \"\\n\"; }\n"
        "scroll(); }); };\n"
        "function scroll() { self.r.scrollTop = self.r.scrollHeight; }\n</script>\n"));
#endif
    bp.print(F("</body></html>\n\n"));
}
#endif