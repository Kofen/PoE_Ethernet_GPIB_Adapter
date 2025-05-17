#pragma once

#include "utilities.h"
#include <Ethernet.h>
#include "config.h"

/*!
  @brief  Interface with the devices.
*/
class SCPI_handler_interface
{
  public:
    virtual ~SCPI_handler_interface() {} 
    // write a command to the SCPI parser or device
    virtual void write(int address, const char *data, size_t len) = 0;
    // read a response from the SCPI parser or device
    virtual bool read(int address, char *data, size_t *len, size_t max_len) = 0;
    // claim_control() should return true if the SCPI parser is ready to accept a command
    virtual bool claim_control() = 0;
    // release_control() should be called when the SCPI parser is no longer needed
    virtual void release_control() = 0;
};

/*!
  @brief  Listens for and responds to VXI-11 requests.
*/
class VXI_Server
{

  public:
    enum Read_Type {
        rt_none = 0,
        rt_identification = 1,
        rt_parameters = 2
    };

  public:
    VXI_Server(SCPI_handler_interface &scpi_handler);
    // VXI_Server(SCPI_handler_interface &scpi_handler, uint32_t port_min, uint32_t port_max);
    // VXI_Server(SCPI_handler_interface &scpi_handler, uint32_t port);
    ~VXI_Server();

    int loop();
    void begin(uint32_t port, bool debug = false);
    int nr_connections(void);
    bool have_free_connections(void);
    void killClients(void);

    uint32_t allocate();
    uint32_t port() { return vxi_port; }
    // const char *get_visa_resource();
    // std::list<IPAddress> get_connected_clients();
    // void disconnect_client(const IPAddress &ip);

  protected:
    void create_link(EthernetClient &tcp, int slot);
    void destroy_link(EthernetClient &tcp, int slot);
    void read(EthernetClient &tcp, int slot);
    void write(EthernetClient &tcp, int slot);
    bool handle_packet(EthernetClient &tcp, int slot);
    void parse_scpi(char *buffer);
    bool debug;

    EthernetServer *tcp_server;
    EthernetClient clients[MAX_VXI_CLIENTS];
    uint8_t addresses[MAX_VXI_CLIENTS];
    Read_Type read_type;
    uint32_t rw_channel;
    uint32_t vxi_port;
    SCPI_handler_interface &scpi_handler;
};

