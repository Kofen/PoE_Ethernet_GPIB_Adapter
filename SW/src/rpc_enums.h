#pragma once

/*!
  @file   rpc_enums.h
  @brief  Enumerations of RPC and VXI protocol codes (message types, reply status,
          error codes, etc.).

    Helpful information on the protocol of the basic RPC packet is available
    at https://www.ibm.com/docs/it/aix/7.2?topic=concepts-remote-procedure-call.
    For information on the VXI specific protocol, see the VXIbus TCP/IP Instrument
    Protocol Specification at https://vxibus.org/specifications.html.
*/

/*!
  @brief  The rpc namespace is used group the RPC/VXI protocol codes.
*/
namespace rpc
{

/*!
  @brief  Port numbers used in the RPC and VXI communication.

  Bind requests always come in on port 111, via either UDP or TCP.
  Some clients (Siglent oscilloscopes for example) require a different port per link; therefore
  it is possible to cycle through a block of ports, changing each time VXI_Server
  begins listening for a new link request.
  Keeping START and END the same will allow for mDNS publication of the VXI_Server port.
*/
enum ports {

    BIND_PORT = 111,       ///< Port to listen on for bind requests
    VXI_PORT_START = 9010, ///< Start of a block of ports to use for VXI transactions
    VXI_PORT_END = 9010    ///< End of a block of ports to use for VXI transactions
};

/*!
  @brief  Message types are either CALL (request) or REPLY (response).
*/
enum msg_type {

    CALL = 0, ///< The message contains a CALL (request)
    REPLY = 1 ///< The message contains a REPLY (response)
};

/*!
  @brief  Indicates whether the request was accepted or denied.

  Note that messages should be denied only for mismatch of RPC protocol
  or problems with authorization - neither of which are used in espBode.
  If accepted, messages will generate a response status which can
  indicate other types of errors.
*/
enum reply_state {

    MSG_ACCEPTED = 0, ///< Message has been accepted; its status is indicated in the rpc_status field
    MSG_DENIED = 1    ///< Message has been denied
};

/*!
  @brief  Reasons that a message was denied.
*/
enum reject_status {

    RPC_MISMATCH = 0, ///< Message denied due to mismatch in RPC protocol
    AUTH_ERROR = 1    ///< Message denied due to invalid authorization
};

/*!
  @brief  Additional detail if a message is denied due to invalid authorization.
*/
enum auth_status {

    AUTH_BADCRED = 1,      ///< Credentials were given in an invalid format
    AUTH_REJECTEDCRED = 2, ///< Credentials were formatted correctly but were rejected
    AUTH_BADVERF = 3,      ///< Verification was given in an invalid format
    AUTH_REJECTEDVERF = 4, ///< Verification was formatted corrrectly but was rejected
    AUTH_TOOWEAK = 5       ///< Authorization level is inadequate
};

/*!
  @brief  Response status for accepted messages.

  This status response can indicate success or a limited set of errors. Many
  response types will also include another, more detailed, error field.
*/
enum rpc_status {

    SUCCESS = 0,       ///< Request was successfully executed
    PROG_UNAVAIL = 1,  ///< The requested program is not available
    PROG_MISMATCH = 2, ///< There is a mismatch in the program request (perhaps version number??)
    PROC_UNAVAIL = 3,  ///< The requested procedure is not available
    GARBAGE_ARGS = 4   ///< Invalid syntax / format
};

/*!
  @brief  espBode responds only to PORTMAP and VXI_11_CORE programs.
*/
enum programs {

    PORTMAP = 0x186A0,    ///< Request for the port on which the VXI_Server is listening
    VXI_11_CORE = 0x607AF ///< Request for a VXI command to be executed
};

/*!
  @brief  espBode provides only GET_PORT and selected VXI_11 procedures.
*/
enum procedures {

    GET_PORT = 3,            ///< Return the port on which the VXI_Server is currently listening
    VXI_11_CREATE_LINK = 10, ///< Create a link to handle a series of requests
    VXI_11_DEV_WRITE = 11,   ///< Write to the AWG
    VXI_11_DEV_READ = 12,    ///< Read from the AWG
    VXI_11_DESTROY_LINK = 23 ///< Destroy the link and cycle to the next port
};

/*!
  @brief  Error codes that can be returned in response to various VXI_11 commands.
*/
enum errors {

    NO_ERROR = 0,          ///< No error = success
    SYNTAX_ERROR = 1,      ///< Command contains invalid syntax
    NOT_ACCESSIBLE = 3,    ///< Cannot access the requested function
    INVALID_LINK = 4,      ///< The link id does not match
    PARAMETER_ERROR = 5,   ///< Invalid parameter
    NO_CHANNEL = 6,        ///< Cannot access the device
    INVALID_OPERATION = 8, ///< The requested function is not recognized
    OUT_OF_RESOURCES = 9,  ///< The device has run out of memory or other resources
    DEVICE_LOCKED = 11,    ///< The device has been locked by another process
    NO_LOCK_HELD = 12,     ///< The device has not been properly locked
    IO_TIMEOUT = 15,       ///< The requested data was not sent/received within the specified timeout interval
    LOCK_TIMEOUT = 17,     ///< Unable to secure a lock on the device within the specified timeout interval
    INVALID_ADDRESS = 21,  ///< No device exists at the specified address
    ABORT = 23,            ///< An abort command has come in via another RPC port
    DUPLICATE_CHANNEL = 29 ///< This channel is already in use (?)
};

/*!
  @brief  Indicates the reason for ending the read of data.
*/
enum reasons {

    END = 4,   ///< No more data available to read
    CHR = 2,   ///< Data reached a terminating character supplied in the read request packet
    REQCNT = 1 ///< Data reached the maximum count requested
};

}; // namespace rpc
