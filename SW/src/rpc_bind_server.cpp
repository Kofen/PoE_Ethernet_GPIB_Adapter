/*!
  @file   rpc_bind_server.cpp
  @brief  Defines the methods of the RCP_Bind_Server class.
*/

#include "rpc_bind_server.h"
#include "rpc_enums.h"
#include "rpc_packets.h"
#include "vxi_server.h"

void RPC_Bind_Server::begin()
{
    /*
      Initialize the UDP and TCP servers to listen on
      the BIND_PORT port.
    */
    udp.begin(rpc::BIND_PORT);
    tcp.begin();

#ifdef LOG_VXI_DETAILS
    debugPort.println(F("Listening for RPC_BIND requests on UDP and TCP port 111"));
#endif
}

/*!
  The loop() member function should be called by
  the main loop of the program to process any UDP or
  TCP bind requests. It will only process requests if
  the vxi_server is available. If so, it will hand off the
  TCP or UDP request to process_request() for validation
  and response. The response will be assembled by
  process_request(), but it will be sent from loop() since
  we know whether to send it via UDP or TCP.
*/
void RPC_Bind_Server::loop()
{
    /*  What to do if the vxi_server is busy?

        There is no "out of resources" error code return from the
        RPC BIND request. We could respond with PROC_UNAVAIL, but
        that might suggest that the ESP simply cannot do RPC BIND
        at all (as opposed to not right now). Another optioon is to
        reject the message, but the enumerated reasons for rejection
        (RPC_MISMATCH, AUTH_ERROR) do not seem appropriate. For now,
        the solution is to ignore (not read) incoming requests until
        a vxi_server becomes available.
    */

    if (vxi_server.have_free_connections()) {
        
        int len;

        if (udp.parsePacket() > 0) {
            len = get_bind_packet(udp);
            if (len > 0) {
#ifdef LOG_VXI_DETAILS                
                debugPort.println(F("UDP packet received"));
#endif                
                process_request(true);
                send_bind_packet(udp, sizeof(bind_response_packet));
            }
        } else {
            EthernetClient tcp_client;
            tcp_client = tcp.accept();
            if (tcp_client) {
                len = get_bind_packet(tcp_client);
                if (len > 0) {
#ifdef LOG_VXI_DETAILS                    
                    debugPort.println(F("TCP packet received"));
#endif                    
                    process_request(false);
                    send_bind_packet(tcp_client, sizeof(bind_response_packet));
                }
                tcp_client.stop(); // close the connection
            }
        }
    }
}

/*!
  @brief  Handle the details of processing an incoming request
          for both TCP and UDP servers.

  This function checks to see if the incoming request is a valid
  PORT_MAP request. It assembles the response including a
  success or error code and the port passed by the VXI_Server.
  Actually sending the response is handled by the caller.

  @param  onUDP   Indicates whether the server calling on this
                  function is UDP or TCP.
*/
void RPC_Bind_Server::process_request(bool onUDP)
{
    uint32_t rc = rpc::SUCCESS;
    uint32_t port = 0;

    rpc_request_packet *rpc_request = (onUDP ? udp_request : tcp_request);
    bind_response_packet *bind_response = (onUDP ? udp_bind_response : tcp_bind_response);

    if (rpc_request->program != rpc::PORTMAP) {
        rc = rpc::PROG_UNAVAIL;

#ifdef LOG_VXI_DETAILS
        debugPort.print(F("ERROR: Invalid program (expected PORTMAP = 0x186A0; received 0x"));
        debugPort.printf("%08x)\n", (uint32_t)(rpc_request->program));
#endif
    } else if (rpc_request->procedure != rpc::GET_PORT) {
        rc = rpc::PROC_UNAVAIL;

#ifdef LOG_VXI_DETAILS
        debugPort.print(F("ERROR: Invalid procedure (expected GET_PORT = 3; received "));
        debugPort.printf("%u)\n", (uint32_t)(rpc_request->procedure));
#endif
    } else {
        // i.e., if it is a valid PORTMAP request

#ifdef LOG_VXI_DETAILS
        debugPort.print(F("PORTMAP command received on "));
        debugPort.println(onUDP?F("UDP"):F("TCP"));
#endif        
        port = vxi_server.allocate();

        /*  The logic in the loop() routine should not allow
            the port returned to be zero, since we first checked
            to see if the vxi_server was available. However, we are
            including the following test just in case.
        */

        if (port == 0) {
            rc = rpc::GARBAGE_ARGS; // not really the appropriate response, but we need to signal failure somehow!
#ifdef LOG_VXI_DETAILS
            debugPort.println(F("ERROR: PORTMAP failed: vxi_server not available."));
        } else {
            debugPort.print(F("PORTMAP: assigned to port "));
            debugPort.printf("%d\n", port);
#endif
        }
    }

    bind_response->rpc_status = rc;
    bind_response->vxi_port = port;
}
