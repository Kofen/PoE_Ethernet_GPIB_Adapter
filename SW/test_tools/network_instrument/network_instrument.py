import argparse
from network_server import NetworkServer

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="VXI-11 network instrument server simulator", formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('--udp', default=True, help="Do or do not use the UDP port mapper. Default: use it", action=argparse.BooleanOptionalAction)
    parser.add_argument('--tcp', default=True, help="Do or do not use the TCP port mapper. Default: use it", action=argparse.BooleanOptionalAction)
    parser.add_argument('-n', default=0, help="The number of GPIB instruments to simulate as being hosted behind the network instrument server. \nDefault: 0, meaning that the network instrument server directly represents an instrument, \nand uses the instrument string 'TCPIP::<IP_ADDRESS>::inst0::INSTR', \nOtherwise the following will also be supported: 'TCPIP::<IP_ADDRESS>::gpib0,[1..N]::INSTR'", type=int)
    parser.add_argument('-v', default=0, help="Verbosity level. Specify one or more 'v' for more detail in the logs.", action="count", dest="verbosity")
    args = parser.parse_args()

    # Using the logging module in multiprocessing makes the code more complicated to read.
    # So I keep it simple
    log_mapping = False
    log_VXI = False
    use_udp_portmapper = args.udp
    use_tcp_portmapper = args.tcp
    if not use_udp_portmapper and not use_tcp_portmapper:
        print("At least one of --udp or --tcp must be True.")
        exit(1)
        
    num_instruments = args.n
    if num_instruments < 0:
        print("The number of instruments must be 0 or greater.")
        exit(1)
    if num_instruments > 10:
        print("The number of instruments must be 10 or less.")
        exit(1)
    
    if args.verbosity > 0:
        log_VXI = True
    if args.verbosity > 1:
        log_mapping = True

    # Initialize 
    print("Initializing...")

    # Run server
    server = None
    try:
        server = NetworkServer(num_instruments, use_tcp_portmapper, use_udp_portmapper, log_VXI=log_VXI, log_mapping=log_mapping)
        print("Running server...")
        server.start()

    except KeyboardInterrupt:
        print('Ctrl+C pressed. Exiting...')

    finally:
        if server is not None:
            server.close_sockets()

    print("Bye.")
