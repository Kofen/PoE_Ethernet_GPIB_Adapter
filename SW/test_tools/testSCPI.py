import argparse
import pyvisa
import time
import datetime
from typing import List


def test_query(inst, inst_name: str, m: str, write_delay_ms: int = 0):
    start_query = datetime.datetime.now()
    if inst_name:
        inst_name = f"{inst_name}: "
    else:
        inst_name = ""
    if m.endswith("?"):
        print(f"{inst_name}Query \"{m}\" reply: ", end='')
        try:
            r = inst.query(m).strip()
        except Exception as e:
            print(f"\nError on query: {e}")
            return False
        print(f"\"{r}\"", end='')
    else:
        print(f"{inst_name}Write \"{m}\"", end='')
        try:
            inst.write(m)
            # inst.flush(pyvisa.highlevel.constants.BufferOperation.flush_write_buffer)
        except Exception as e:
            print(f"\nError on write: {e}")
            return False                        
    delta_time = datetime.datetime.now() - start_query
    print(f", taking {delta_time.total_seconds() * 1000:.1f} ms.")
    if not m.endswith("?") and write_delay_ms > 0:
        time.sleep(write_delay_ms / 1000)
    
    return True


def test_device(instruments: List[str], repeat_query: int, timeout: int):
    rm = pyvisa.ResourceManager()
    inst = {}
    for my_inst_name in instruments:
        print(f"Connecting to '{my_inst_name}'", end='')
        start_connect = datetime.datetime.now()
        try:
            inst[my_inst_name] = rm.open_resource(my_inst_name, timeout=timeout)
        except Exception as e:
            print(f"\nError on connect: {e}")
            return False
        delta_time = datetime.datetime.now() - start_connect
        print(f" succeeded, taking {delta_time.total_seconds() * 1000:.1f} ms.")
        write_delay_ms = 0
        if my_inst_name.endswith("::SOCKET"):
            inst[my_inst_name].read_termination = "\n"
            inst[my_inst_name].write_termination = "\n"
            # write_delay_ms = 150  # for socket connections, a delay is needed between writes sometimes
    msgs = ["*IDN?"]
    # msgs = ["VOLT 1", "VOLT?", "VOLT 2", "VOLT?", "VOLT 3", "VOLT?"]
    # msgs = ["MEAS:VOLT?"]
    if repeat_query > 0:
        print(f"Repeating query tests for {repeat_query} seconds...")
        start = time.time()
        while time.time() - start < repeat_query:
            for m in msgs:
                for my_inst_name in instruments:
                    inst_name = my_inst_name
                    if len(instruments) == 0:
                        inst_name = None
                    if not test_query(inst[my_inst_name], inst_name, m, write_delay_ms):
                        inst[my_inst_name].close()
                        return False
    else:
        for m in msgs:
            for my_inst_name in instruments:
                inst_name = my_inst_name
                if len(instruments) == 0:
                    inst_name = None
                if not test_query(inst[my_inst_name], inst_name, m, write_delay_ms):
                    inst[my_inst_name].close()
                    return False
    return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Test simple SCPI communication via VISA.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("instruments", nargs='*', default=None, help="The devices to use for tests. Must be one or more Visa compatible connection strings.")
    parser.add_argument("-n", action="store_true", default=False, help="No SCPI device discovery preceding the test. Will be ignored when port is not defined.")
    parser.add_argument("-c", type=int, default=0, help="Test repeated connect calls for N seconds. If not specified: will do 1 connect. Will be ignored when port is not defined.")
    parser.add_argument("-q", type=int, default=0, help="After a connect, test repeated query calls for N seconds. If not specified: will do 1 call. Will be ignored when port is not defined.")
    parser.add_argument("-t", type=int, default=10000, help="Timeout for any VISA operation in milliseconds for the repeated tests (-q, -c).")
    args = parser.parse_args()
        
    rm = pyvisa.ResourceManager()
    repeat_query = args.q
    repeat_connect = args.c
    test_timeout = args.t
    skip_scan = args.n
    if args.instruments:
        if len(args.instruments) > 0:
            skip_scan = True
        instruments = [x.strip() for x in args.instruments]
    else:
        instruments = []
    if not skip_scan:
        print("Scanning for VISA resources...")
        print("VISA Resources found: ", end='')
        resources = rm.list_resources(query="?*")
        print(resources)
        for m in resources:            
            if m.startswith("ASRL/dev/cu."):
                # some of these devices are not SCPI compatible, like "ASRL/dev/cu.Bluetooth-Incoming-Port::INSTR"
                print(f"Skipping serial port \"{m}\"")
                continue
            inst = None
            try:
                inst = rm.open_resource(m, timeout=test_timeout)
            except:
                print(f"Cannot connect to device on address \"{m}\"")
                continue
            try:
                r = inst.query("*IDN?").strip()
                inst.close()
                print(f"Found \"{r}\" on address \"{m}\"")
            except:
                print(f"Found unknown device on address \"{m}\"")
            inst.close()    
    else:
        print("No scan for VISA resources.")
    if len(instruments) > 0:
        if repeat_connect:
            print(f"Repeating tests for {repeat_connect} seconds...")
            start = time.time()
            while time.time() - start < repeat_connect:
                test_device(instruments, repeat_query, test_timeout)            
        else:
            test_device(instruments, repeat_query, test_timeout)

    print("Done.")
