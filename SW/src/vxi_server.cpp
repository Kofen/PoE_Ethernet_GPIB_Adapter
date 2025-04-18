#include "vxi_server.h"
#include "rpc_enums.h"
#include "rpc_packets.h"


VXI_Server::VXI_Server(SCPI_handler_interface &scpi_handler)
    : scpi_handler(scpi_handler)
{
    tcp_server = NULL;
}

VXI_Server::~VXI_Server()
{
}

int VXI_Server::nr_connections(void) {
    int count = 0;
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (clients[i]) {
            count++;
        }
    }
    return count;
}

bool VXI_Server::have_free_connections(void) {
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (!clients[i]) {
            return true;
        }
    }
    return false;
}

uint32_t VXI_Server::allocate()
{
    uint32_t port = 0;

    if (have_free_connections()) {
        port = vxi_port; // This can be a cyclic counter, not a simple integer
    }
    return port;
}

/**
 * @brief Start the VXI server on the specified port.
 * 
 * @param port TCP port to listen on
 * @param debug true when debug messages are to be printed
 */
void VXI_Server::begin(uint32_t port, bool debug = false)
{
    this->vxi_port = port;
    this->debug = debug;

    if (tcp_server) {
        delete tcp_server;
        tcp_server = NULL;
    }

    tcp_server = new EthernetServer(vxi_port);
    if (!tcp_server) {
        if (debug) {
            debugPort.print(F("ERROR: Failed to create TCP server on port "));
            debugPort.printf("%u\n", (uint32_t)vxi_port);
        }
        return;
    }

    if (debug) {
        debugPort.print(F("VXI server listening on port "));
        debugPort.printf("%u\n", (uint32_t)vxi_port);
    }
    tcp_server->begin();
}

/**
 * @brief run the VXI RPC server loop.
 * 
 * @return int the active number of clients
 */
int VXI_Server::loop()
{
    // This is a TCP server based on 'server.accept()', meaning I must handle the lifecycle of the client 
    // It is blocking for input and output

    // close any clients that are not connected
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (clients[i] && !clients[i].connected()) {
            clients[i].stop();
            if (debug) {
                debugPort.print(F("Force Closing VXI connection on port "));
                debugPort.print((uint32_t)vxi_port);
                debugPort.print(F(" of slot "));
                debugPort.print(i);
                debugPort.print(F(" from remote port "));
                debugPort.println(clients[i].remotePort());
            }               
        }
    }

    if (have_free_connections()) {
        // check if a new client is available
        EthernetClient newClient = tcp_server->accept();
        if (newClient) {
            bool found = false;
            for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
                if (!clients[i]) {
                    clients[i] = newClient;
                    found = true;
                    if (debug) {
                        debugPort.print(F("New VXI connection on port "));
                        debugPort.print((uint32_t)vxi_port);
                        debugPort.print(F(" in slot "));
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
                    debugPort.print(F("VXI connection limit reached on port "));
                    debugPort.print((uint32_t)vxi_port);
                    debugPort.print(F(" from remote port "));
                    debugPort.println(newClient.remotePort());
                }
                newClient.stop();
            }
        }
    }

    // handle any incoming data
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (clients[i] && clients[i].available()) // if a connection has been established on port
        {
            bool bClose = false;
            // read the entire packet, blocking if needed. The packet is small in general, so should have arrived completely
            // TODO: make this work in a non blocking way
            int len = get_vxi_packet(clients[i]);

            if (len > 0) {
                bClose = handle_packet(clients[i], i);
            }

            if (bClose) {
                if (debug) {
                    debugPort.print(F("Closing VXI connection on port "));
                    debugPort.print((uint32_t)vxi_port);
                    debugPort.print(F(" of slot "));
                    debugPort.print(i);
                    debugPort.print(F(" from remote port "));
                    debugPort.println(clients[i].remotePort());
                }
                clients[i].stop();
            }
        }
    }
    return nr_connections();
}

bool VXI_Server::handle_packet(EthernetClient &client, int slot)
{
    bool bClose = false;
    uint32_t rc = rpc::SUCCESS;

    if (vxi_request->program != rpc::VXI_11_CORE) {
        rc = rpc::PROG_UNAVAIL;

        if (debug) {
            debugPort.print(F("ERROR: Invalid program (expected VXI_11_CORE = 0x607AF; received 0x"));
            debugPort.printf("%08x)\n", (uint32_t)(vxi_request->program));
        }

    } else
        switch (vxi_request->procedure) {
        case rpc::VXI_11_CREATE_LINK:
            create_link(client, slot);
            break;
        case rpc::VXI_11_DEV_READ:
            read(client, slot);
            break;
        case rpc::VXI_11_DEV_WRITE:
            write(client, slot);
            break;
        case rpc::VXI_11_DESTROY_LINK:
            destroy_link(client, slot);
            bClose = true;
            break;
        default:
            if (debug) {
                debugPort.print(F("Invalid VXI-11 procedure (received "));
                debugPort.printf("%u)\n", (uint32_t)(vxi_request->procedure));
            }
            rc = rpc::PROC_UNAVAIL;
            break;
        }

    /*  Response messages will be sent by the various routines above
        when the program and procedure are recognized (and therefore
        rc == rpc::SUCCESS). We only need to send a response here
        if rc != rpc::SUCCESS.  */

    if (rc != rpc::SUCCESS) {
        vxi_response->rpc_status = rc;
        send_vxi_packet(client, sizeof(rpc_response_packet));
    }

    /*  signal to caller whether the connection should be close (i.e., DESTROY_LINK)  */

    return bClose;
}

void VXI_Server::create_link(EthernetClient &client, int slot)
{
    /*  The data field in a link request should contain a string
        with the name of the requesting device. It may already
        be null-terminated, but just in case, we will put in
        the terminator.  */

    if (!scpi_handler.claim_control()) {
        create_response->rpc_status = rpc::SUCCESS;
        create_response->error = rpc::OUT_OF_RESOURCES; // not DEVICE_LOCKED because that would require lock_timeout etc
        create_response->link_id = 0;
        create_response->abort_port = 0;
        create_response->max_receive_size = 0;
        send_vxi_packet(client, sizeof(create_response_packet));
        return;
    }

    create_request->data[create_request->data_len] = 0;
    if (debug) {
        debugPort.print(F("CREATE LINK request from \""));
        debugPort.print(create_request->data);
        debugPort.print(F("\" on port "));
        debugPort.print((uint32_t)vxi_port);
        debugPort.print(F(" -> LID="));
        debugPort.print(slot);
        debugPort.println();
    }
    // interpret and store the request data so that I can use it on the GPIB bus
    // make lowercase
    for(int i = 0; i < create_request->data_len; i++) {
        create_request->data[i] = tolower(create_request->data[i]);
    }
    int my_nr = 0;
    int r = sscanf(create_request->data, "inst%d", &my_nr);
    // if not check (g|h)pib[0-9],[0-9] after the comma
    if (r != 1 && ((create_request->data[0] == 'g' || create_request->data[0] == 'h') &&
                    create_request->data[1] == 'p' && 
                    create_request->data[2] == 'i' &&
                    create_request->data[3] == 'b')) {
        char *cptr;
        for(int i = 4; i < create_request->data_len; i++) {
            if (create_request->data[i] == ',') {
                cptr = &create_request->data[i+1];
                my_nr = atoi(cptr);
                break;
            }
            create_request->data[i] = tolower(create_request->data[i]);
        }
    }  
    if (my_nr < 0 || my_nr > 31) {
        create_response->rpc_status = rpc::SUCCESS;
        create_response->error = rpc::PARAMETER_ERROR;
        create_response->link_id = 0;
        create_response->abort_port = 0;
        create_response->max_receive_size = 0;
        send_vxi_packet(client, sizeof(create_response_packet));
        return;
    }
    // store
    addresses[slot] = my_nr;
    
    /*  Generate the response  */
    create_response->rpc_status = rpc::SUCCESS;
    create_response->error = rpc::NO_ERROR;
    create_response->link_id = slot;
    create_response->abort_port = 0;
    create_response->max_receive_size = VXI_READ_SIZE - 4;
    send_vxi_packet(client, sizeof(create_response_packet));
}

void VXI_Server::destroy_link(EthernetClient &client, int slot)
{
    if (debug) {
        debugPort.print(F("DESTROY LINK LID="));
        debugPort.print(slot);
        debugPort.print(F(" on port "));
        debugPort.print((uint32_t)vxi_port);
        debugPort.println();        
    }
    destroy_response->rpc_status = rpc::SUCCESS;
    destroy_response->error = rpc::NO_ERROR;
    send_vxi_packet(client, sizeof(destroy_response_packet));
    scpi_handler.release_control();
}

void VXI_Server::read(EthernetClient &client, int slot)
{
    // This is where we read from the device
    char outbuffer[256];
    size_t len = 0;
    bool rv = scpi_handler.read(addresses[slot], outbuffer, &len, sizeof(outbuffer));

    // FIXME handle error codes, maybe even pick up errors from the SCPI Parser

    if (debug) {
        debugPort.print(F("READ DATA LID="));
        debugPort.print(slot);
        debugPort.print(F(" on port "));
        debugPort.printf("%u", (uint32_t)vxi_port);
        debugPort.print(F("; gpib_address="));
        debugPort.print(addresses[slot]);
        debugPort.print(F("; data = "));
        printBuf(outbuffer, (int)len);
    }
    read_response->rpc_status = rpc::SUCCESS;
    read_response->error = rpc::NO_ERROR;
    read_response->reason = rpc::END;
    read_response->data_len = (uint32_t)len;
    strcpy(read_response->data, outbuffer);

    send_vxi_packet(client, sizeof(read_response_packet) + len);
}

void VXI_Server::write(EthernetClient &client, int slot)
{
    // This is where we write to the device
    uint32_t wlen = write_request->data_len;
    uint32_t len = wlen;
    // right trim. SCPI parser doesn't like \r\n
    while (len > 0 && isspace(write_request->data[len - 1])) {
        len--;
    }
    write_request->data[len] = 0;
    if (debug) {
        debugPort.print(F("WRITE DATA LID="));
        debugPort.print(slot);
        debugPort.print(F(" on port "));
        debugPort.print((uint32_t)vxi_port);
        debugPort.print(F("; gpib_address="));
        debugPort.print(addresses[slot]);        
        debugPort.print(F("; data = "));
        printBuf(write_request->data, (int)len);
    }
    /*  Parse and respond to the SCPI command  */
    scpi_handler.write(addresses[slot], write_request->data, len);

    /*  Generate the response  */
    write_response->rpc_status = rpc::SUCCESS;
    write_response->error = rpc::NO_ERROR;
    write_response->size = wlen; // with the original length
    send_vxi_packet(client, sizeof(write_response_packet));
}

// const char *VXI_Server::get_visa_resource()
// {
//     static char visa_resource[40];
//     sprintf(visa_resource, "TCPIP::%s::INSTR", WiFi.localIP().toString().c_str());
//     return visa_resource;
// }

