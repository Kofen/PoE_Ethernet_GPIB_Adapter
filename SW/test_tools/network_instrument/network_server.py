'''
This file contains the classes for the rpcbind port mapper (on TCP and UDP) and the VXI-11 listener.

It starts the port mappers as separate processes and runs the VXI-11 loop in the main process.

The port changes of the VXI-11 loop are communicated to the port mappers via a shared variable (with locking).
'''

import multiprocessing
import socket

# Host and ports to use.
# Setting host to 0.0.0.0 will bind the incoming connections to any interface.
#  PRCBIND port should always remain 111.
#  VXI-11 port can be changed to another value.
HOST = '0.0.0.0'
RPCBIND_PORT = 111
VXI11_PORTRANGE_START = 9010
VXI11_PORTRANGE_END = 9019


# dummy ID reply
ID_STRING = b"IDN-DUMMY"

# RPC/VXI-11 procedure ids
GET_PORT = 3
CREATE_LINK = 10
DEVICE_WRITE = 11
DEVICE_READ = 12
DESTROY_LINK = 23
vxi11_PROCEDURES = {
    10: "CREATE_LINK",
    11: "DEVICE_WRITE",
    12: "DEVICE_READ",
    23: "DESTROY_LINK"
}

# VXI-11 Core (395183)
VXI11_CORE_ID = 395183
# Function responses
NOT_VXI11_ERROR = -1
NOT_GET_PORT_ERROR = -2
UNKNOWN_COMMAND_ERROR = -4
OK = 0


class CommsObject(object):
    """
    Base class for the network interactions
    """
    
    def create_socket(self, host: str, port: int, on_udp: bool, myname: str):
        """Create a UDP or TCP socket, and starts listening

        :param host: host
        :type host: str
        :param port: port
        :type port: int
        :param on_udp: True for UDP
        :type on_udp: bool
        :param myname: the name to use in error messages
        :type myname: str
        :return: the socket
        :rtype: socket.socket
        """
        if on_udp:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
                sock.bind((host, port))
            except OSError as ex:
                print(f"{myname}: Fatal error: {ex}. Cannot open UDP port {port} on address {host} for listening.")
                exit(1)
        else:
            try:
                sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                # Disable the TIME_WAIT state of connected sockets, and allow reuse, as I switch ports rather quickly
                sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
                sock.bind((host, port))
                sock.listen(1)  # Become a server socket, maximum 1 connection
            except OSError as ex:
                print(f"{myname}: Fatal error: {ex}. Cannot open TCP port {port} on address {host} for listening.")
                exit(1)
        return sock
        
    def get_xid(self, rx_packet: bytes) -> bytes:
        """
        Extracts XID from the incoming RPC packet.
        """
        xid = rx_packet[0x00:0x04]
        return xid
    
    def generate_packet_size_header(self, size: int) -> bytes:
        """Generates the header containing reply packet size.

        :param size: size of the response payload
        :type size: int
        :return: the part of the header representing the size
        :rtype: bytes
        """
        # 1... .... .... .... .... .... .... .... = Last Fragment: Yes
        size = size | 0x80000000
        # .000 0000 0000 0000 0000 0000 0001 1100 = Fragment Length: 28
        res = self.uint_to_bytes(size)
        return res
    
    def generate_resp_data(self, xid: bytes, resp: bytes, on_udp: bool = False) -> bytes:
        """Generates the response data to be sent.

        :param xid: XID of the RPC request
        :type xid: bytes
        :param resp: payload in the response
        :type resp: bytes
        :param on_udp: True for UDP
        :type on_udp: bool
        :return: response data to be sent
        :rtype: bytes
        """
        # Generate RPC header
        rpc_hdr = self.generate_rpc_header(xid)
        if on_udp:
            # Merge all the headers
            resp_data = rpc_hdr + resp
        else:
            # Generate packet size header
            data_size = len(rpc_hdr) + len(resp)
            size_hdr = self.generate_packet_size_header(data_size)
            # Merge all the headers
            # debug: print(f"size_hdr: [{' '.join(format(x, '02x') for x in size_hdr)}]")
            # debug: print(f"rpc_hdr: [{' '.join(format(x, '02x') for x in rpc_hdr)}]")
            # debug: print(f"resp: [{' '.join(format(x, '02x') for x in resp)}]")
            resp_data = size_hdr + rpc_hdr + resp
        return resp_data    

    def generate_rpc_header(self, xid: bytes) -> bytes:
        """
        Generates RPC header for replying to requests.
        :param xid: XID of the RPC request
        :type xid: bytes
        :return: response header to be sent
        :rtype: bytes        
        """
        hdr = b""

        # XID: 0xXXXXXXXX (4 bytes)
        hdr += xid
        # Message Type: Reply (1)
        hdr += b"\x00\x00\x00\x01"
        # Reply State: accepted (0)
        hdr += b"\x00\x00\x00\x00"
        # Verifier
        #  Flavor: AUTH_NULL (0)
        hdr += b"\x00\x00\x00\x00"
        #  Length: 0
        hdr += b"\x00\x00\x00\x00"
        # Accept State: RPC executed successfully (0)
        hdr += b"\x00\x00\x00\x00"
        return hdr

    # =========================================================================
    #   Helper functions
    # =========================================================================

    def bytes_to_uint(self, bytes_seq) -> int:
        """
        Converts a sequence of 4 bytes to 32-bit integer. Byte 0 is MSB.
        """
        return int.from_bytes(bytes_seq, "big")
        # num = ord(bytes_seq[0])
        # num = num * 0x100 + ord(bytes_seq[1])
        # num = num * 0x100 + ord(bytes_seq[2])
        # num = num * 0x100 + ord(bytes_seq[3])
        # return num

    def uint_to_bytes(self, num):
        """
        Converts a 32-bit integer to a sequence of 4 bytes. Byte 0 is MSB.
        """
        return num.to_bytes(4, "big")
        # byte3 = (num / 0x1000000) & 0xFF
        # byte2 = (num / 0x10000) & 0xFF
        # byte1 = (num / 0x100) & 0xFF
        # byte0 = num & 0xFF
        # bytes_seq = bytearray((byte3, byte2, byte1, byte0))
        # return bytes_seq

    def print_as_hex(self, buf: bytes):
        """
        Prints a buffer as a set of hexadecimal numbers.
        Created for debug purposes.
        """
        buf_str = ""
        for b in buf:
            buf_str += "0x%X " % ord(b)
        print(buf_str)
        

class Portmapper(CommsObject, multiprocessing.Process):
    """Port mapper "thread" class
    """
    
    def __init__(self, host: str, rpcbind_port: int, on_udp: bool, vxi11_port, log_verbose: bool):
        """init the port mapper.
        It will start the portmapper in a separate process.

        :param host: host
        :type host: str
        :param rpcbind_port: port
        :type rpcbind_port: int
        :param on_udp: True for UDP
        :type on_udp: bool
        :param vxi11_port: the port used by the VXI-11 service
        :type vxi11_port: multiprocessing.Value
        :param log_verbose: True logging of the packets
        :type log_verbose: bool
        """
        multiprocessing.Process.__init__(self)
        self.exit = multiprocessing.Event()

        if host is not None:
            self.host = host
        else:
            self.host = HOST

        if not isinstance(rpcbind_port, (int, type(None))):
            raise TypeError("rpcbind_port must be an integer.")
        if rpcbind_port is not None:
            self.rpcbind_port = rpcbind_port
        else:
            self.rpcbind_port = RPCBIND_PORT
        self.on_udp = on_udp
        self.vxi11_port = vxi11_port
        self.myname = f"{'UDP' if on_udp else 'TCP'}Portmapper"
        self.log_verbose = log_verbose
        
    def run(self):
        """
        Run the main loop of the mapper
        """
        try:
            # Create RPCBIND socket
            self.rpcbind_socket = self.create_socket(self.host, self.rpcbind_port, self.on_udp, self.myname)
            
            while not self.exit.is_set():
                # VXI-11 requests are processed after receiving a valid RPCBIND request.
                # print(f"{self.myname}: Waiting for connection request...")
                if self.on_udp:
                    res = self.process_rpcbind_request_udp()
                else:
                    res = self.process_rpcbind_request_tcp()
                if res != OK:
                    if self.log_verbose:
                        print("Incompatible RPCBIND request.")
        except KeyboardInterrupt:
            pass                
        # print(f"{self.myname}: shut down")
    
    def terminate(self):
        # not used normally, just in case the server shuts down
        if self.log_verbose:
            print(f"{self.myname}: terminate()")
        self.exit.set()
           
    def validate_rpcbind_request(self, address, rx_data: bytes, on_udp: bool):
        """Validates a RPC bind request and generates the reply

        :param address: the address of the caller
        :type address: _RetAddress
        :param rx_data: the received request
        :type rx_data: bytes
        :param on_udp: True for UDP
        :type on_udp: bool
        :return: OK|NOT_GET_PORT_ERROR|NOT_VXI11_ERROR, response data
        :rtype: int, bytes
        """
        if self.log_verbose:
            print(f"{self.myname}: Incoming connection from {address[0]}:{address[1]}.")
        # Validate the request.
        #  If the request is not GETPORT or does not come from VXI-11 Core (395183),
        #  we have nothing to do with it
        # If the request buffer is too small, also reject
        if len(rx_data) > 0x2C:  # 0x2C = from get_program_id
            procedure = self.get_procedure(rx_data)
            if procedure != GET_PORT:
                return NOT_GET_PORT_ERROR, None
            program_id = self.get_program_id(rx_data)
            if program_id != VXI11_CORE_ID:
                return NOT_VXI11_ERROR, None
            # Generate and send response
            resp = self.generate_rpcbind_response()
            xid = self.get_xid(rx_data)
            resp_data = self.generate_resp_data(xid, resp, on_udp)            
            return OK, resp_data
        else:
            return NOT_VXI11_ERROR, None
        
    def process_rpcbind_request_tcp(self) -> int:
        """Replies to TCP RPCBIND/Portmap request and sends VXI-11 port number.
        :return: OK|NOT_GET_PORT_ERROR|NOT_VXI11_ERROR
        :rtype: int
        """
        # RFC 1057 and RFC 1833 apply here. The scope uses V2, so RFC 1057 suffices.

        connection, address = self.rpcbind_socket.accept()
        rx_data = connection.recv(128)
        if len(rx_data) > 4:
            rx_data = rx_data[0x04:]  # start from XID, as with UDP
        rv, resp_data = self.validate_rpcbind_request(address, rx_data, False)
        if rv == OK:
            connection.send(resp_data)
        # Close connection and RPCBIND socket.
        connection.close()
        return rv
    
    def process_rpcbind_request_udp(self):
        """Replies to UDP RPCBIND/Portmap request and sends VXI-11 port number.
        :return: OK|NOT_GET_PORT_ERROR|NOT_VXI11_ERROR
        :rtype: int
        """
        # RFC 1057 and RFC 1833 apply here. The scope uses V2, so RFC 1057 suffices.
        
        bufferSize = 1024
        bytesAddressPair = self.rpcbind_socket.recvfrom(bufferSize)

        rx_data = bytesAddressPair[0]
        address = bytesAddressPair[1]

        rv, resp_data = self.validate_rpcbind_request(address, rx_data, True)
        if rv == OK:
            self.rpcbind_socket.sendto(resp_data, address)
        return rv
    
    def generate_rpcbind_response(self) -> bytes:
        """Returns VXI-11 port number as response to RPCBIND request."""
        # self.vxi11_port is a multiprocessing.Value object, that is a ctypes object in shared memory
        # that is synchronized using RLock. So it always gets the latest value.
        myport = self.vxi11_port.value
        if self.log_verbose:
            print(f"{self.myname}: Sending to TCP port {myport}")
        resp = self.uint_to_bytes(myport)
        return resp
    
    def get_procedure(self, rx_packet: bytes) -> int:
        """
        Extracts procedure from the incoming RPC packet.
        """
        return self.bytes_to_uint(rx_packet[0x14:0x18])

    def get_program_id(self, rx_packet: bytes) -> int:
        """
        Extracts program_id from the incoming RPC packet.
        """
        return self.bytes_to_uint(rx_packet[0x28:0x2C])
        
    def close_socket(self):
        """
        Closes RPCBIND socket.
        """
        try:
            self.rpcbind_socket.close()
        except:
            pass

    def __del__(self):
        self.close_socket()    
    

class NetworkServer(CommsObject):

    def __init__(self, num_instruments: int = 0,
                 use_tcp_portmapper: bool = True, use_udp_portmapper: bool = True,
                 host: str = None, rpcbind_port: int = None, 
                 vxi11_portrange_start: int = None, vxi11_portrange_end: int = None, 
                 log_VXI: bool = False, log_mapping: bool = False):
        if host is not None:
            self.host = host
        else:
            self.host = HOST
            
        self.num_instruments = num_instruments
        if num_instruments < 0:
            num_instruments = 0
        if num_instruments >= 1000:
            raise ValueError("The number of instruments must be less than 1000.")
        self.num_instruments = num_instruments
        self.links = [False] * (num_instruments + 1)

        if not isinstance(rpcbind_port, (int, type(None))):
            raise TypeError("rpcbind_port must be an integer.")
        if rpcbind_port is not None:
            self.rpcbind_port = rpcbind_port
        else:
            self.rpcbind_port = RPCBIND_PORT

        if not isinstance(vxi11_portrange_start, (int, type(None))):
            raise TypeError("vxi11_port range start must be an integer.")
        if vxi11_portrange_start is not None:
            self.vxi11_portrange_start = vxi11_portrange_start
        else:
            self.vxi11_portrange_start = VXI11_PORTRANGE_START

        if not isinstance(vxi11_portrange_end, (int, type(None))):
            raise TypeError("vxi11_port range start must be an integer.")
        if vxi11_portrange_end is not None:
            self.vxi11_portrange_end = vxi11_portrange_end
        else:
            self.vxi11_portrange_end = VXI11_PORTRANGE_END

        self.vxi11_port = multiprocessing.Value('I', self.vxi11_portrange_start)
        
        self.use_tcp_portmapper = use_tcp_portmapper
        self.use_udp_portmapper = use_udp_portmapper
            
        self.myname = "VXI-11"
        self.pm1 = None
        self.pm2 = None
        self.log_VXI = log_VXI
        self.log_mapping = log_mapping
            
    def start(self):
        """
        Makes all required initializations and starts the server.
        """

        print("Starting network server...")
        
        if self.use_tcp_portmapper:
            if self.log_mapping:
                print(f"Portmapper: Listening to TCP ports on {self.host}:{self.rpcbind_port}")
            self.pm1 = Portmapper(self.host, self.rpcbind_port, False, self.vxi11_port, self.log_mapping).start()
        if self.use_udp_portmapper:
            if self.log_mapping:
                print(f"Portmapper: Listening to UDP ports on {self.host}:{self.rpcbind_port}")
            self.pm2 = Portmapper(self.host, self.rpcbind_port, True, self.vxi11_port, self.log_mapping).start()
        # Create VXI-11 socket
        if self.log_mapping:
            print(f"{self.myname}: Listening to TCP port {self.host}:{self.vxi11_port.value}")
        self.vxi11_socket = self.create_socket(self.host, self.vxi11_port.value, False, self.myname)

        # Run the server
        self.main_loop()

    def main_loop(self):
        """
        The main loop of the server.
        """

        # Run the VXI-11 server
        while True:
            # if self.log_mapping:
            #     print("Waiting for vxi11 request.")
            self.process_vxi11_requests()
            
            # every request must go to a new socket for some clients, so we do it here
            self.close_vxi11_sockets()
            self.vxi11_port.value += 1
            if self.vxi11_port.value > self.vxi11_portrange_end:
                self.vxi11_port.value = self.vxi11_portrange_start
                
            if self.log_mapping:
                print(f"{self.myname}: moving to TCP port {self.vxi11_port.value}")
            self.vxi11_socket = self.create_socket(self.host, self.vxi11_port.value, False, self.myname)
    
    def process_vxi11_requests(self):
        connection, address = self.vxi11_socket.accept()
        if self.log_VXI:
            print(f"VXI-11: Incoming connection from {address[0]}:{address[1]}.")
            
        # FIXME: this handles only one connection at a time, to be adapted. This is a bug.
            
        while True:
            rx_buf = connection.recv(255)
            if len(rx_buf) > 0:
                resp = b''  # default
                
                # Parse incoming VXI-11 command. Will have logged.
                status, vxi11_procedure, scpi_command, cmd_length, link_id = self.parse_vxi11_request(rx_buf)

                # TODO: check if this is correct
                if status == NOT_VXI11_ERROR:
                    break
                elif status == UNKNOWN_COMMAND_ERROR:
                    break

                if vxi11_procedure != CREATE_LINK:
                    # TODO: produce the correct error code if the link is not open or the link ID is invalid
                    if link_id is None:
                        if self.log_VXI:
                            print("VXI-11: ERROR: No link ID in request.")
                        break
                    if not self.links[link_id]:
                        if self.log_VXI:
                            print(f"VXI-11: ERROR: Link ID {link_id} is not open. Ignoring request.")
                        break

                # Process the received VXI-11 request
                if vxi11_procedure == CREATE_LINK:
                    link_id = self.get_link_id_from_device_name(scpi_command)
                    if link_id is None:
                        if self.log_VXI:
                            print(f"VXI-11: Invalid device name {scpi_command} in CREATE_LINK request. Ignoring.")
                        break
                    if self.links[link_id]:
                        # TODO: validate if this is the right way to handle this
                        if self.log_VXI:
                            print(f"VXI-11: WARNING: Link ID {link_id} already in use. Ignoring creation request.")
                        break
                    self.links[link_id] = True
                    if self.log_VXI:
                        print(f"VXI-11: creating link with LID={link_id}")
                    resp = self.generate_vxi11_create_link_response(link_id)

                elif vxi11_procedure == DEVICE_WRITE:
                    """
                    The parser parses and executes the received SCPI command.
                    VXI-11 DEVICE_WRITE function requires an empty reply.
                    """
                    resp = self.generate_vxi11_device_write_response(cmd_length)

                elif vxi11_procedure == DEVICE_READ:
                    """
                    DEVICE_READ request is sent to a device when an answer after
                    command execution is expected. 
                    Here, we always reply with a dummy ID string.
                    """
                    resp = self.generate_vxi11_read_response(ID_STRING + str(link_id).encode())

                elif vxi11_procedure == DESTROY_LINK:
                    """
                    If DESTROY_LINK is received, the requester ends the session
                    opened by CREATE_LINK request and won't send any commands before
                    issuing a new CREATE_LINK request.
                    All we have to do is to exit the loop and continue listening to
                    RPCBIND requests.
                    """
                    self.links[link_id] = False
                    resp = self.generate_vxi11_destroy_link_response()

                else:
                    """
                    If the received command is none of the above, something
                    went wrong and we should exit the loop and continue
                    listening to RPCBIND requests.
                    """
                    break

                # Generate and send response
                xid = self.get_xid(rx_buf[0x04:])
                resp_data = self.generate_resp_data(xid, resp, False)
                connection.send(resp_data)
                
                if vxi11_procedure == DESTROY_LINK:
                    for i in range(0, len(self.links)):
                        if self.links[i]:
                            # break here means: there are still open links, do not kill the connection
                            break
                    else:
                        # break here means: kill the connection
                        break

        # Close connection
        connection.close()

    def get_link_id_from_device_name(self, device_name):
        """
        Returns the link ID for the given device name.
        """
        if device_name is None:
            return None
        device_name = device_name.strip()
        if device_name == "":
            return None
        if device_name.lower() in ("inst", "inst0"):
            return 0
        if "," not in device_name:
            return None
        device_parts = device_name.split(",")
        if len(device_parts) != 2:
            return None
        if device_parts[0].lower() not in ("gpib", "gpib0", "hpib", "hpib0"):
            return None
        gpib_address = device_parts[1]
        if not gpib_address.isdigit():
            return None
        gpib_address = int(gpib_address)
        if gpib_address < 1 or gpib_address > self.num_instruments:
            return None
        return gpib_address

    def parse_vxi11_request(self, rx_data):
        """Parses VXI-11 request. Returns VXI-11 command code and SCPI command if it exists.
        @param rx_data: bytes array containing the source packet.
        @return: a tuple with 3 values:
                1. status - is 0 if the request could be processed, error code otherwise.
                2. VXI-11 procedure id if it is known, None otherwise.
                3. string containing SCPI command if it exists in the request, in utf-8.
                4. the length of the sent command, in bytes (needed for some replies).
                5. the given link ID"""
        # Validate source program id.
        #  If the request doesn't come from VXI-11 Core (395183), it is ignored.
        program_id = self.bytes_to_uint(rx_data[0x10:0x14])
        if program_id != VXI11_CORE_ID:
            if self.log_VXI:
                print(f"VXI-11: Unsupported VXI-11 procedure received. program id {program_id}'")                        
            return (NOT_VXI11_ERROR, None, None)

        # Procedure: CREATE_LINK (10), DESTROY_LINK (23), DEVICE_WRITE (11), DEVICE_READ (12)
        vxi11_procedure = self.bytes_to_uint(rx_data[0x18:0x1c])
        # default values
        scpi_command = None
        status = OK
        cmd_length = 0
        link_id = None

        # Process the remaining data according to the received VXI-11 request
        if vxi11_procedure == CREATE_LINK:
            cmd_length = self.bytes_to_uint(rx_data[0x38:0x3C])
            # ignoring Client ID, Lock_Device, Lock_Timeout
            # Device Name is put in scpi_command
            scpi_command = rx_data[0x3C:0x3C + cmd_length]
            scpi_command = scpi_command.decode('utf-8').strip()
            if self.log_VXI:
                print(f"VXI-11: {vxi11_PROCEDURES[vxi11_procedure]}, '{scpi_command}'")            
        elif vxi11_procedure == DEVICE_WRITE:
            cmd_length = self.bytes_to_uint(rx_data[0x3C:0x40])
            link_id = self.bytes_to_uint(rx_data[0x2C:0x30])
            # ignoring IO_timeout, Lock_timeout, flags
            # data is put in scpi_command
            scpi_command = rx_data[0x40:0x40 + cmd_length]
            scpi_command = scpi_command.decode('utf-8').strip()
            if self.log_VXI:
                print(f"VXI-11: {vxi11_PROCEDURES[vxi11_procedure]}, LID={link_id} '{scpi_command}'")            
        elif vxi11_procedure == DEVICE_READ:
            # ignoring Request_Size, IO_timeout, Lock_timeout, flags, term_char
            link_id = self.bytes_to_uint(rx_data[0x2C:0x30])
            if self.log_VXI:
                print(f"VXI-11: {vxi11_PROCEDURES[vxi11_procedure]}, LID={link_id}")            
        elif vxi11_procedure == DESTROY_LINK:
            link_id = self.bytes_to_uint(rx_data[0x2C:0x30])
            if self.log_VXI:
                print(f"VXI-11: {vxi11_PROCEDURES[vxi11_procedure]}, LID={link_id}")            
        else:
            status = UNKNOWN_COMMAND_ERROR  # or unsupported command
            if self.log_VXI:
                print(f"VXI-11: Unsupported VXI-11 command received. Code {vxi11_procedure}")

        return (status, vxi11_procedure, scpi_command, cmd_length, link_id)

    # =========================================================================
    #   Response data generators
    # =========================================================================

    def generate_vxi11_create_link_response(self, link_id=0):
        """Generates reply to VXI-11 CREATE_LINK request."""
        # VXI-11 response
        #  Error Code: No Error (0)
        resp = b"\x00\x00\x00\x00"
        #  Link ID: 
        resp += self.uint_to_bytes(link_id)
        #  Abort Port: 0
        resp += b"\x00\x00\x00\x00"
        #  Maximum Receive Size: 8388608=0x00800000
        # resp += self.uint_to_bytes(8388608)
        # This is OK in python. Embedded it will need to be limited. Can go down to 1024. (00 00 40 00)
        resp += b"\x00\x80\x00\x00"
        return resp
    
    def generate_vxi11_destroy_link_response(self):
        """Generates reply to VXI-11 DESTROY_LINK request."""
        # VXI-11 response
        #  Error Code: No Error (0)
        resp = b"\x00\x00\x00\x00"
        return resp

    def generate_vxi11_device_write_response(self, cmd_length):
        """Generates reply to VXI-11 DEVICE_WRITE request."""
        # VXI-11 response
        #  Error Code: No Error (0)
        resp = b"\x00\x00\x00\x00"
        #  Size: the size of the original command
        resp += self.uint_to_bytes(cmd_length)
        return resp        
        
    def generate_vxi11_read_response(self, response):
        """Generates reply to VXI-11 DEVICE_READ request."""
        # Error Code: No Error (0)
        resp = b"\x00\x00\x00\x00"
        # Reason: 0x00000004 (END)
        resp += b"\x00\x00\x00\x04"
        # Add the response string
        r_length = len(response) + 1
        resp += self.uint_to_bytes(r_length)
        resp += response
        # The sequence ends with \n and two \0 fill bytes.
        resp += b"\x0A\x00\x00"
        return resp

    def close_vxi11_sockets(self):
        """
        Closes VXI-11 socket.
        """
        try:
            self.vxi11_socket.close()
        except:
            pass

    def close_sockets(self):
        self.close_vxi11_sockets()
        if self.pm1:
            self.pm1.terminate()
        if self.pm2:
            self.pm2.terminate()
        
    def __del__(self):
        self.close_sockets()


if __name__ == '__main__':
    raise Exception("This module is not for running. Run network_instrument.py instead.")
