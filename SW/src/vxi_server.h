#pragma once

#include "utilities.h"
#include <Ethernet.h>
#include "config.h"
#include "rpc_packets.h"

/**
 * @brief a helper class to capture data from the instruments, and send it through to the VXI client.
 * This class only supports basic write operations (for now)
 */
class vxiBufStream : public Stream {
   public:
    vxiBufStream(EthernetClient &tcp, int slot) : tcp(tcp), slot(slot), buffer_pos(0), total_len(0) {}

    size_t write(uint8_t ch) override {
        // debugPort.print((char)ch);
        if (buffer_pos >= sizeof(buffer) - 1) {
            flushVXI(false);  // buffer is full, flush it to vxi. Let flush() do the repositioning of the buffer pointer
        }
        buffer[buffer_pos++] = ch;
        total_len++;
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override {
        size_t written = 0;
        for (size_t i = 0; i < size; i++) {
            if (write(buffer[i]) == 1) {
                written++;
            } else {
                break;
            }
        }
        return written;
    };

    int available() { return 0; }  // dummy
    int read() { return 0; }       // dummy
    int peek() { return 0; }       // dummy

    size_t len(void) { return buffer_pos; }
    size_t total_len = 0;  // total length of the data

    // This flush is called when the buffer is full
    void flush() {
        flushVXI(false);  // send the data to the VXI client
    }
    // This is the real flush
    void flushVXI(bool close = false);

   private:
    char buffer[MAX_READ_RESPONSE_DATA_SIZE];  // buffer for the data
    size_t buffer_pos = 0;
    EthernetClient &tcp;
    int slot = 0;
};

/*!
  @brief  Interface with the devices.
*/
class SCPI_handler_interface
{
  public:
    virtual ~SCPI_handler_interface() {} 
    // write a command to the SCPI parser or device
    virtual void write(int address, const char *data, size_t len) = 0;

    // read a response from the SCPI parser or device and write to a Stream (thereby allowing large replies)
    virtual bool read(int address, vxiBufStream &dataStream) = 0;    
    
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
    void begin(uint32_t port);
    int nr_connections(void);
    bool have_free_connections(void);

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
    bool handle_packet(EthernetClient &tcp, int slot, bool overflow = false);
    void parse_scpi(char *buffer);

    EthernetServer *tcp_server;
    EthernetClient clients[MAX_VXI_CLIENTS];
    uint8_t addresses[MAX_VXI_CLIENTS];
    Read_Type read_type;
    uint32_t rw_channel;
    uint32_t vxi_port;
    SCPI_handler_interface &scpi_handler;
};

