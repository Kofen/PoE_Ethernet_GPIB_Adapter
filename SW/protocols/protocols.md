# prologix vs RAW vs VXI-11.2 vs hislip

## Needs in number of network connections

..provided we need compatibility with 1 pyvisa program only.

Suppose N is the number of instruments to be represented:

* prologix: 1: TCP server
* RAW: N or N+1 (if you also want the instrument server): all TCP servers in 'auto' mode
* VXI-11.2: N+2 or N+3 (if you also want the instrument server): 1 UDP portmapper, 1 TCP portmapper and the rest RPC servers in non-'auto' mode.
* hislip: not investigated, requires a LOT

## prologix

not (yet) suppored by pyvisa

## RAW

Example of connection string:

TCPIP::192.168.1.105::[port number]::SOCKET

Requires 1 server, and 1 client socket per instrument. Multiple commands tend to be sent concatenated in 1 network packet, unless a question is sent.

## VXI-11.2

Example of connection string:

* TCPIP::192.168.1.105::gpib,5::INSTR   (gpib or hpib or gpibN or hpibN): VXI-11.2
* TCPIP::192.168.1.105::inst::INSTR     (instN): 'normal' vxi-11

gpib0 or inst0: default or only, hpibN or instN: others

Requires 3 servers, and 1 client socket per instrument:

* port mappers on UDP and TCP that point to the VXI-11 RPC server
* the VXI-11 RPC server. VXI-11 consists of 3 separate RPC programs with the numeric identifiers: 0x0607AF (DEVICE_CORE), 0x0607B0 (DEVICE_ASYNC) and 0x0607B1 (DEVICE_INTR).
* you may also add mDNS

For compatibility with basic pyvisa use, you only need DEVICE_CORE, but the guidelines say you should also support the others.


# Large replies

VXI-11 DEVICE_READ returns contain 3 main flags:

* Requested count Reached
* Termination Character Seen
* EOI Set

On large replies, the latter is False. In that case, a new DEVICE_READ must be issued by the client, and the server will return the next chunk of data, etc.
The data itself is considered opaque: if a large list of values is returned, any separators in the list are not aligned with the chunks.
