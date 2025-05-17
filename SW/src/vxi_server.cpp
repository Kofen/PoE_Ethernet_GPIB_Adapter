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
 */
void VXI_Server::begin(uint32_t port)
{
    this->vxi_port = port;

    if (tcp_server) {
        delete tcp_server;
        tcp_server = NULL;
    }

    tcp_server = new EthernetServer(vxi_port);
    if (!tcp_server) {
#ifdef LOG_VXI_DETAILS
        debugPort.print(F("ERROR: Failed to create TCP server on port "));
        debugPort.printf("%u\n", (uint32_t)vxi_port);
#endif
        return;
    }

#ifdef LOG_VXI_DETAILS
    debugPort.print(F("VXI server listening on port "));
    debugPort.printf("%u\n", (uint32_t)vxi_port);
#endif
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
#ifdef LOG_VXI_DETAILS
            debugPort.print(F("Force Closing VXI connection on port "));
            debugPort.print((uint32_t)vxi_port);
            debugPort.print(F(" of slot "));
            debugPort.print(i);
            debugPort.print(F(" from remote port "));
            debugPort.println(clients[i].remotePort());
#endif
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
#ifdef LOG_VXI_DETAILS
                    debugPort.print(F("New VXI connection on port "));
                    debugPort.print((uint32_t)vxi_port);
                    debugPort.print(F(" in slot "));
                    debugPort.print(i);
                    debugPort.print(F(" from remote port "));
                    debugPort.println(newClient.remotePort());
#endif
                    break;
                }
            }
            if (!found) {
                // shouldn't happen, but still....
#ifdef LOG_VXI_DETAILS
                debugPort.print(F("VXI connection limit reached on port "));
                debugPort.print((uint32_t)vxi_port);
                debugPort.print(F(" from remote port "));
                debugPort.println(newClient.remotePort());
#endif
                newClient.stop();
            }
        }
    }

    // handle any incoming data
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (clients[i] && clients[i].available()) // if a connection has been established on port
        {
            bool bClose = false;
            bool overflow = false;
            // read the entire packet, blocking if needed. The packet is small in general, so should have arrived completely
            // TODO: make this work in a non blocking way, but then you'd need non-static memory buffers
            uint32_t len = get_vxi_packet(clients[i]);

            // do not handle overflow for now, let the protocol handle it, as there is checking on max_receive_size
            if (len != 0) {
                bClose = handle_packet(clients[i], i, overflow);
            }

            if (bClose) {
#ifdef LOG_VXI_DETAILS
                debugPort.print(F("Closing VXI connection on port "));
                debugPort.print((uint32_t)vxi_port);
                debugPort.print(F(" of slot "));
                debugPort.print(i);
                debugPort.print(F(" from remote port "));
                debugPort.println(clients[i].remotePort());
#endif
                clients[i].stop();
            }
        }
    }
    return nr_connections();
}

void VXI_Server::killClients(void)
{
    for (int i = 0; i < MAX_VXI_CLIENTS; i++) {
        if (clients[i]) {
            clients[i].stop();
        }
    }
}

bool VXI_Server::handle_packet(EthernetClient &client, int slot, bool overflow = false)
{
    // Handle a low level VXI packet

    // Use of shared memory zones:
    // vxi_request points to the static buffer vxi_read_buffer
    // vxi_response points to the static buffer vxi_send_buffer

    bool bClose = false;
    uint32_t rc = rpc::SUCCESS;

    if (vxi_request->program != rpc::VXI_11_CORE) {
        rc = rpc::PROG_UNAVAIL;

#ifdef LOG_VXI_DETAILS
        debugPort.print(F("ERROR: Invalid program (expected VXI_11_CORE = 0x607AF; received 0x"));
        debugPort.printf("%08x)\n", (uint32_t)(vxi_request->program));
#endif

    } else if (overflow) {
        rc = rpc::GARBAGE_ARGS; // this is probably not the right error code, the reason for overflow can be anything
        bClose = true;
#ifdef LOG_VXI_DETAILS
        debugPort.print(F("ERROR: Buffer overflow on inbound VXI packet\n"));
#endif
    } else {
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
#ifdef LOG_VXI_DETAILS
            debugPort.print(F("Invalid VXI-11 procedure (received "));
            debugPort.printf("%u)\n", (uint32_t)(vxi_request->procedure));
#endif
            rc = rpc::PROC_UNAVAIL;
            break;
        }
    }

    /*  Response messages will be sent by the various routines above
        when the program and procedure are recognized (and therefore
        rc == rpc::SUCCESS). We only need to send a response here
        if rc != rpc::SUCCESS.  */

    if (rc != rpc::SUCCESS) {
        // no need to do memset, rpc_response_packet will be filled for the rest by the send_vxi_packet function 
        vxi_response->rpc_status = rc;
        send_vxi_packet(client, sizeof(rpc_response_packet));
    }

    /*  signal to caller whether the connection should be close (i.e., DESTROY_LINK)  */

    return bClose;
}

void VXI_Server::create_link(EthernetClient &client, int slot)
{
    /*  The data field in a link request should contain a string
        with the name of the requesting device. */
    
    // Use of shared memory zones:
    // create_request points to the static buffer vxi_read_buffer
    // create_response points to the static buffer vxi_send_buffer

    memset(create_response, 0, sizeof(create_response_packet));

    uint32_t len = create_request->data_len;
    if (len >= MAX_WRITE_REQUEST_DATA_SIZE) {
        len = MAX_WRITE_REQUEST_DATA_SIZE - 1; // I do not have more than that. The input buffer will have been truncated before.
        // And I remove 1 extra to add a null terminator, as I do string operations below and the size if never that big
    }
    create_request->data[len] = 0; // make sure the string is null terminated

    if (!scpi_handler.claim_control()) {
        create_response->rpc_status = rpc::SUCCESS;
        create_response->error = rpc::OUT_OF_RESOURCES; // not DEVICE_LOCKED because that would require lock_timeout etc
        create_response->link_id = 0;
        create_response->abort_port = 0;
        create_response->max_receive_size = 0;
        send_vxi_packet(client, sizeof(create_response_packet));
        return;
    }

#ifdef LOG_VXI_DETAILS
    debugPort.print(F("CREATE LINK request from "));
    printBuf(create_request->data, len);
    debugPort.print(F(" on port "));
    debugPort.print((uint32_t)vxi_port);
    debugPort.print(F(" -> LID="));
    debugPort.print(slot);
    debugPort.println();
#endif
    // interpret and store the request data so that I can use it on the GPIB bus
    // make lowercase
    for(int i = 0; i < len; i++) {
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
        for(int i = 4; i < len; i++) {
            if (create_request->data[i] == 0) {
                break;
            }
            if (create_request->data[i] == ',') {
                cptr = &create_request->data[i+1];
                my_nr = atoi(cptr);
                break;
            }
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
    create_response->max_receive_size = MAX_WRITE_REQUEST_DATA_SIZE;
    send_vxi_packet(client, sizeof(create_response_packet));
}

void VXI_Server::destroy_link(EthernetClient &client, int slot)
{
    // Use of shared memory zones:
    // destroy_response points to the static buffer vxi_send_buffer

#ifdef LOG_VXI_DETAILS
    debugPort.print(F("DESTROY LINK LID="));
    debugPort.print(slot);
    debugPort.print(F(" on port "));
    debugPort.print((uint32_t)vxi_port);
    debugPort.println();        
#endif
    memset(destroy_response, 0, sizeof(destroy_response_packet));
    destroy_response->rpc_status = rpc::SUCCESS;
    destroy_response->error = rpc::NO_ERROR;
    send_vxi_packet(client, sizeof(destroy_response_packet));
    scpi_handler.release_control();
}

void VXI_Server::read(EthernetClient &client, int slot)
{
    // This is where we read from the device
    
    // Use of shared memory zones:
    // read_request points to the static buffer vxi_read_buffer
    // read_response points to the static buffer vxi_send_buffer

    uint32_t max_len = MAX_READ_RESPONSE_DATA_SIZE; // I do not have more than that. The output buffer will overflow
    uint32_t request_len = (uint32_t)read_request->request_size;
    if (request_len > 0 && request_len < max_len) {
        max_len = request_len;
    }

    memset(read_response, 0, sizeof(read_response_packet));
    // If I surpass my max size, I just cut off and the client will have to issue another read 
    vxiBufStream vxiStream(read_response->data, max_len);  ///< using the static buffer's data area
    SCPI_handler_read_stop_reasons rv = scpi_handler.read(addresses[slot], vxiStream, max_len);
    // FIXME handle error codes, maybe even pick up errors from the SCPI Parser
#ifdef LOG_VXI_DETAILS
    debugPort.print(F("READ DATA LID="));
    debugPort.print(slot);
    debugPort.print(F(" on port "));
    debugPort.print((uint32_t)vxi_port);
    debugPort.print(F("; gpib_address="));
    debugPort.print(addresses[slot]);
    debugPort.print(F("; data_len = "));
    debugPort.print((uint32_t)vxiStream.len());
    debugPort.print(F("; max_len="));
    debugPort.print(max_len);
    debugPort.print(F("; stop_reason="));
    debugPort.print(rv);    
    debugPort.print(F("; data="));
    printBuf(read_response->data, (int)vxiStream.len());    
#endif

    read_response->rpc_status = rpc::SUCCESS;
    read_response->error = rpc::NO_ERROR;
    if (rv == SRS_MAXSIZE) {
        read_response->reason = 0; // tell the user to read again
    } else {
        read_response->reason = rpc::END;
    }
    read_response->data_len = (uint32_t)vxiStream.len();

    send_vxi_packet(client, sizeof(read_response_packet) + read_response->data_len);
}

void VXI_Server::write(EthernetClient &client, int slot)
{
    // This is where we write to the device

    // Use of shared memory zones:
    // write_request points to the static buffer vxi_read_buffer
    // write_response points to the static buffer vxi_send_buffer
    
    uint32_t len = write_request->data_len;
    if (len >= MAX_WRITE_REQUEST_DATA_SIZE) {
        len = MAX_WRITE_REQUEST_DATA_SIZE; // I do not have more than that. The input buffer will have been truncated before.
    }

    uint32_t wlen = len;
    
    // Is this the end of the command?
    uint32_t flags = (uint32_t)write_request->flags;

    bool is_eoi = (flags & 8) != 0;
    if (is_eoi) { 
        // this is the end of the command, so I can trim the data
        // right trim. Some instruments don't like \r\n
        while (wlen > 0 && isspace(write_request->data[wlen - 1])) {
            wlen--;
        }
    }

#ifdef LOG_VXI_DETAILS
    debugPort.print(F("WRITE DATA LID="));
    debugPort.print(slot);
    debugPort.print(F(" on port "));
    debugPort.print((uint32_t)vxi_port);
    debugPort.print(F("; gpib_address="));
    debugPort.print(addresses[slot]);        
    debugPort.print(F("; is_eoi="));
    debugPort.print(is_eoi);        
    debugPort.print(F("; data="));
    printBuf(write_request->data, (int)wlen);
#endif
    /*  Parse and respond to the SCPI command  */
    scpi_handler.write(addresses[slot], write_request->data, wlen, is_eoi);

    /*  Generate the response  */
    memset(write_response, 0, sizeof(write_response_packet));
    write_response->rpc_status = rpc::SUCCESS;
    write_response->error = rpc::NO_ERROR;
    write_response->size = len; // with the potentially truncated original (non trimmed) length
    send_vxi_packet(client, sizeof(write_response_packet));
}

// const char *VXI_Server::get_visa_resource()
// {
//     static char visa_resource[40];
//     sprintf(visa_resource, "TCPIP::%s::INSTR", WiFi.localIP().toString().c_str());
//     return visa_resource;
// }

