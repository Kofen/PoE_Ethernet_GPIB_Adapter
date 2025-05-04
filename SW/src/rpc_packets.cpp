/*!
  @file   rpc_packets.cpp
  @brief  Definitions of variables and basic functions
          to receive and send RPC/VXI packets.
*/

#include "rpc_packets.h"
#include "rpc_enums.h"

/*  The definition of the buffers to hold packet data */

uint8_t udp_read_buffer[UDP_READ_SIZE]; // only for udp bind requests
uint8_t udp_send_buffer[UDP_SEND_SIZE]; // only for udp bind responses
uint8_t tcp_read_buffer[TCP_READ_SIZE]; // only for tcp bind requests
uint8_t tcp_send_buffer[TCP_SEND_SIZE]; // only for tcp bind responses
uint8_t vxi_read_buffer[VXI_READ_SIZE]; // only for vxi requests
uint8_t vxi_send_buffer[VXI_SEND_SIZE]; // only for vxi responses

/*!
  @brief  Receive an RPC bind request packet via UDP.

  This function is called only when the udp connection has
  data available. It reads the data into the udp_read_buffer.

  @param  udp		The EthernetUDP connection from which to read.
  @return The length of data received.
*/
uint32_t get_bind_packet(EthernetUDP &udp)
{
    uint32_t len = udp.read(udp_request_packet_buffer, UDP_READ_SIZE);
    return len;
}

/*!
  @brief  Receive an RPC bind request packet via TCP.

  This function is called only when the tcp client has data
  available. It reads the data into the tcp_read_buffer.

  @param  tcp   The EthernetClient connection from which to read.
  @return The length of data received.
*/
uint32_t get_bind_packet(EthernetClient &tcp)
{
    uint32_t len;

    tcp_request_prefix->length = 0; // set the length to zero in case the following read fails

    tcp.readBytes(tcp_request_prefix_buffer, 4); // get the FRAG + LENGTH field

    len = (tcp_request_prefix->length & 0x7fffffff); // mask out the FRAG bit

    if (len > 4) {
        len = min(len, (uint32_t)(sizeof(tcp_read_buffer) - 4)); // do not read more than the buffer can hold

        tcp.readBytes(tcp_request_packet_buffer, len);
    } else {
        len = 0; // no data read
    }

    return len;
}

/*!
  @brief  Receive an RPC/VXI command request packet via TCP.

  This function is called only when the tcp client has data
  available. It reads the data into the vxi_read_buffer.

  @param  tcp   The EthernetClient connection from which to read.

  @return The length of data received. Will be 0xffffffff if the packet is too large.
*/
uint32_t get_vxi_packet(EthernetClient &tcp)
{
    uint32_t read_len;
    uint32_t return_len;

    vxi_request_prefix->length = 0; // set the length to zero in case the following read fails

    tcp.readBytes(vxi_request_prefix_buffer, 4); // get the FRAG + LENGTH field

    return_len = (vxi_request_prefix->length & 0x7fffffff); // mask out the FRAG bit

    if (return_len > 4) {
        if (return_len >= sizeof(vxi_read_buffer) - 4) {
            return_len = 0xffffffff; // packet too large
            read_len = (uint32_t)(sizeof(vxi_read_buffer) - 4); // do not read more than the buffer can hold
        }
        else {
            read_len = return_len;
        }

        tcp.readBytes(vxi_request_packet_buffer, read_len); 
    } else {
        return_len = 0; // no data to read
    }

    return return_len;
}

/*!
  @brief  Send an RPC bind response packet via UDP.

  This function is called to return the port number on which
  the VXI_Server is listening. It uses the udp_send_buffer.

  @param  udp   The udp connection on which to send.
  @param  len	  The length of the response to send.
*/
void send_bind_packet(EthernetUDP &udp, uint32_t len)
{
    fill_response_header(udp_response_packet_buffer, udp_request->xid); // get the xid from the request

    udp.beginPacket(udp.remoteIP(), udp.remotePort());
    udp.write(udp_response_packet_buffer, len);
    udp.endPacket();
}

/*!
  @brief  Send an RPC bind response packet via TCP.

  This function is called to return the port number on which
  the VXI_Server is listening. It uses the tcp_send_buffer.

  @param  tcp		The EthernetClient to which to send.
  @param  len		The length of the response to send.
*/
void send_bind_packet(EthernetClient &tcp, uint32_t len)
{
    fill_response_header(tcp_response_packet_buffer, tcp_request->xid); // get the xid from the request

    // adjust length to multiple of 4, appending 0's to fill the dword

    while ((len & 3) > 0) {
        tcp_response_packet_buffer[len++] = 0;
    }

    tcp_response_prefix->length = 0x80000000 | len; // set the FRAG bit and the length;

    while (tcp.availableForWrite() == 0)
        ; // wait for tcp to be available

    tcp.write(tcp_response_prefix_buffer, len + 4); // add 4 to the length to account for the tcp_response_prefix
    tcp.flush();
}

/*!
  @brief  Send a VXI command response packet via TCP.

  This function is called to return the response to the
  previous command request; the packet includes at least
  the basic response header plus an error code, but may
  include additional data as appropriate. It uses the
  vxi_send_buffer.

  @param  tcp		The EthernetClient to which to send.
  @param  len		The length of the response to send.
*/
void send_vxi_packet(EthernetClient &tcp, uint32_t len)
{
    fill_response_header(vxi_response_packet_buffer, vxi_request->xid);

    // adjust length to multiple of 4, appending 0's to fill the dword

    while ((len & 3) > 0) {
        vxi_response_packet_buffer[len++] = 0;
    }

    vxi_response_prefix->length = 0x80000000 | len; // set the FRAG bit and the length;

    while (tcp.availableForWrite() == 0)
        ; // wait for tcp to be available

    tcp.write(vxi_response_prefix_buffer, len + 4); // add 4 to the length to account for the vxi_response_prefix
    tcp.flush();
}

/*!
  @brief  Fill in the standard response header data.

  This function is called by the various send_XYZ functions
  to fill in the "generic" parts of the response.

  @param  buffer	The buffer to use for the response
  @param	xid			The transaction id copied from the request
*/
void fill_response_header(uint8_t *buffer, uint32_t xid)
{
    rpc_response_packet *rpc_response = (rpc_response_packet *)buffer;

    rpc_response->xid = xid;                       // transaction id supplied by the request
    rpc_response->msg_type = rpc::REPLY;           // CALL = 0; REPLY = 1
    rpc_response->reply_state = rpc::MSG_ACCEPTED; // MSG_ACCEPTED = 0; MSG_DENIED = 1
    rpc_response->verifier_l = 0;
    rpc_response->verifier_h = 0;
}
