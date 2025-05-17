import argparse
import pyvisa
import datetime
import time

CLOSE_WAIT_TIME = 0.2 # seconds

def connect_instrument(rm, my_inst_name, timeout: int):
    print(f"{my_inst_name}: Connecting", end='')
    start_connect = datetime.datetime.now()
    try:
        inst = rm.open_resource(my_inst_name, timeout=timeout)
    except Exception as e:
        print(f"\nError on connect: {e}")
        return None
    delta_time = datetime.datetime.now() - start_connect
    print(f" succeeded, taking {delta_time.total_seconds() * 1000:.1f} ms.")
    return inst


def query_instrument(inst, inst_name: str, m: str):
    start_query = datetime.datetime.now()
    print(f"{inst_name}: Query \"{m}\" reply: ", end='')
    try:
        r = inst.query(m).strip()
    except Exception as e:
        print(f"\nError on query: {e}")
        return False
    print(f"\"{r}\"", end='')
    delta_time = datetime.datetime.now() - start_query
    print(f", taking {delta_time.total_seconds() * 1000:.1f} ms.")
    return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Test the number of simultaneous connections, using connections on 'gpib0,1'..'gpib0,B', up to 'gpib9,1'..'gpib9,B', while maintaining up to N connections open.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-i", type=str, default="192.168.7.206", help="Device IP address.")
    parser.add_argument("-b", type=int, default=2, help="Number of devices that are actually on the bus.")
    parser.add_argument("-n", type=int, default=4, help="Number of simultaneous connections to test.")
    parser.add_argument("-t", type=int, default=10000, help="Timeout for any VISA operation in milliseconds.")
    args = parser.parse_args()
        
    rm = pyvisa.ResourceManager()
    b = args.b
    timeout = args.t
    num = args.n
    instruments = []
    for i in range(0, 10):
        for j in range(1, b + 1):
            instruments.append(f"TCPIP::{args.i}::gpib{i},{j}::INSTR")
    instrument_inst = {}
    instrument_name = {}
    for i in range(0, num):
        instrument_inst[i] = None
        instrument_name[i] = None
        
    j = 0
    for my_inst_name in instruments:
        if instrument_inst[j] is not None:
            print(f"{instrument_name[j]}: Closing.")    
            instrument_inst[j].close()
            instrument_inst[j] = None
            instrument_name[j] = None
            time.sleep(CLOSE_WAIT_TIME)
        instrument_name[j] = my_inst_name
        instrument_inst[j] = connect_instrument(rm, instrument_name[j], timeout)
        if instrument_inst[j] is None:
            exit(1)
        for i in range(0, num):
            if instrument_inst[i] is not None:
                query_instrument(instrument_inst[i], instrument_name[i], "*IDN?")
        j += 1
        if j >= num:
            j = 0

    print("Done.")
