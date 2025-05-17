#pragma once

/*!
  @file   rpc_bind_server.h
  @brief  Declares the RPC_Bind_Server class
*/

#include "utilities.h"
#include "vxi_server.h"
#include <Ethernet.h>
#include "rpc_enums.h"

/*!
  @brief  Listens for and responds to PORT_MAP requests.

  The RPC_Bind_Server class listens for incoming PORT_MAP requests
  on port 111, both on UDP and TCP. When a request comes in, it asks
  the VXI_Server (passed as part of the construction of the class)
  for the current port and returns a response accordingly. Note that
  the VXI_Server must be constructed before the RPC_Bind_Server.
*/
class RPC_Bind_Server
{

  public:
    /*!
      @brief  Constructor's only task is to save a reference
              to the VXI_Server.

      @param  vs  A reference to the VXI_Server
    */
    RPC_Bind_Server(VXI_Server &vs)
        : vxi_server(vs)
    {
    }

    /*!
      @brief  Destructor only needs to stop the listening services.
    */
    ~RPC_Bind_Server()
    {
        udp.stop();
        // tcp.stop(); // stop doesn't exist on lightweight AVR
    };

    /*!
      @brief  Initializes the RPC_Bind_Server by setting up
              the TCP and UDP servers.
    */
    void begin();

    /*!
      @brief  Call this at least once per main loop to
              process any incoming PORT_MAP requests.
    */
    void loop();

    void killClients(void) {};

  protected:
    void process_request(bool onUDP);

    VXI_Server &vxi_server;   ///< Reference to the VXI_Server
    EthernetUDP udp;          ///< UDP server
    EthernetServer tcp = EthernetServer(rpc::BIND_PORT);       ///< TCP server
};

