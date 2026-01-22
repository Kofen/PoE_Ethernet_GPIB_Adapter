import argparse
import pyvisa
import datetime
import time
from enum import Enum

CLOSE_WAIT_TIME = 0.2 # seconds

class instrument_type(Enum):
    UNKNOWN = 0
    HP_SA = 1
    HP_PSU = 2
    HP_SAMPLINGPSU = 3


def is_legacy_instrument(idn: str) -> bool:
    legacy_indicators = ["HP8590E", "HP8591E", "HP8593E", "HP8594E", "HP8595E"]
    for indicator in legacy_indicators:
        if indicator in idn:
            return True
    return False


def get_instrument_type(idn: str) -> instrument_type:
    sa_indicators = ["HP8590E", "HP8591E", "HP8593E", "HP8594E", "HP8595E"]
    psu_indicators = ["6632B", "6633B", "6634B"]
    sampling_psu_indicators = ["66332A", "66312A"]
    
    for indicator in sa_indicators:
        if indicator in idn:
            return instrument_type.HP_SA
    for indicator in psu_indicators:
        if indicator in idn:
            return instrument_type.HP_PSU
    for indicator in sampling_psu_indicators:
        if indicator in idn:
            return instrument_type.HP_SAMPLINGPSU
    return instrument_type.UNKNOWN


def getIDString(legacy: bool):
    if legacy:
        return "*ID?"
    else:
        return "*IDN?"


def check_err(inst, legacy: bool) -> str:
    if legacy:
        q = "CMDERRQ?"
    else:
        q = "SYST:ERR?"
        
    do_loop = True
    errstr = ""
    # have_printed_err = False
    while do_loop:
        try:
            err = inst.query(q)
        except Exception as e:
            errstr += f"Exception: {e}\n"
            break
        err = err.strip()
        if not (err.startswith("+0,") or err.startswith("0,") or len(err) == 0):
            errstr += err + "\n"
            do_loop = True
            # have_printed_err = True
        else:
            # if not have_printed_err:
            #    print("OK.")
            do_loop = False
    return errstr.strip()
    
    
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


def get_idn(inst):
    # get its IDN string. This may provoke a spurious error on the instrument, since I'm probing here
    # First try legacy instruments
    try:
        q = getIDString(legacy=True)
        idn = inst.query(q).strip()
        if idn and len(idn) > 0:
            return idn
    except Exception:
        pass
    # Now try modern ID query
    try:
        q = getIDString(legacy=False)
        idn = inst.query(q).strip()
        if idn and len(idn) > 0:
            return idn
    except Exception:
        pass
    return None


def init_instrument(inst, conn: str, islegacy: bool) -> bool:
    start_time = datetime.datetime.now()
    # print(f"{conn}: Initialization ... ", end='', flush=True)
    try:
        if not islegacy:
            inst.write("*RST")
        inst.write("*CLS")
    except Exception as e:
        print(f"\n{conn}: Exception on initialization: {e}")
        return False
    err = check_err(inst, islegacy)
    if len(err) > 0:
        print(f"\n{conn}: Errors after initialization:\n{err}")
        return False
    delta_time = datetime.datetime.now() - start_time
    print(f"{conn}: Initialization done, taking {delta_time.total_seconds() * 1000:.1f} ms.")
    return True


def interrogate_instrument(inst, conn: str, mytype: instrument_type) -> bool:
    start_time = datetime.datetime.now()

    print(f"{conn}: Interrogation ... ", end='', flush=True)
    
    if mytype == instrument_type.UNKNOWN:
        print(f"\n{conn}: Unknown instrument type, skipping interrogation.")
        return False

    if mytype == instrument_type.HP_PSU:
        number_of_readings = 10
        for i in range(number_of_readings):
            try:
                readcmd = "MEAS:CURR?"  # FETCH seems to be broken
                readresult = inst.query_ascii_values(readcmd)
            except Exception as e:
                print(f"\n{conn}: Error reading data: {e}")
                return False
            if len(readresult) != 1:
                print(f"\n{conn}: Expected {number_of_readings} readings, got {len(readresult)}.")
                return False 
        print(f"Read {number_of_readings} current readings: OK.")
    
    if mytype == instrument_type.HP_SAMPLINGPSU:
        number_of_readings = 800
        # Measure voltage
        # inst.write("OUTP ON") # no need to switch on the output on
        inst.write("INIT:CONT:SEQ OFF")
        inst.write("SENS:FUNC \"VOLT\"")
        # inst.write("SENS:CURR:DET ACDC")
        # inst.write("SENS:CURR:RANG MAX")
        inst.write("TRIG:ACQ:SOUR BUS")
        inst.write("SENS:SWE:TINT 15.6E-6")
        inst.write(f"SENSE:SWEEP:POINTS {number_of_readings}")
        # inst.write("SENS:SWE:OFFS:POIN 0")
        
        # inst.write(f"TRIG:ACQ:COUN:VOLT {number_of_readings}")  # no, not this, as it averages over N samples
        inst.write("TRIG:IMM")
        inst.write("INIT:CONT:SEQ ON")
        err = check_err(inst, False)
        if len(err) > 0:
            print(f"\n{conn}: Errors after starting voltage measurement:\n{err}")
            return False
        time.sleep(0.1)  # wait a bit for measurements to complete
        readresult = [] 
        try:
            readcmd = "MEAS:ARRAY:VOLT?"  # FETCH seems to be broken
            readresult = inst.query_ascii_values(readcmd)
        except Exception as e:
            print(f"\n{conn}: Error reading data: {e}")
            return False
        if len(readresult) != number_of_readings:
            print(f"\n{conn}: Expected {number_of_readings} readings, got {len(readresult)}.")
            return False 
        print(f"Read {len(readresult)} voltage readings: OK.")
        
    if mytype == instrument_type.HP_SA:
        readcmd = "USTATE?"
        expected_len = 7192
        try:        
            readresult = inst.query_binary_values(readcmd, datatype="B", header_fmt="hp", is_big_endian=True, expect_termination=True)
        except Exception as e:
            print(f"\n{conn}: Error reading data: {e}")
            return False
        if len(readresult) != expected_len:
            print(f"\n{conn}: Expected {expected_len} bytes of USTATE data, got {len(readresult)}.")
            return False 
        print(f"Read {len(readresult)} bytes: OK.")

    delta_time = datetime.datetime.now() - start_time
    print(f"{conn}: Interrogation done, taking {delta_time.total_seconds() * 1000:.1f} ms.")
    return True


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Stress test devices on the bus, using connections on 'gpib0,1'..'gpib0,N'.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("-i", type=str, default="192.168.7.206", help="Device IP address.")
    parser.add_argument("-N", type=int, default=2, help="Number of devices to test.")
    parser.add_argument("-d", type=int, default=1, help="Run duration in seconds.")
    parser.add_argument("-t", type=int, default=10000, help="Timeout for any VISA operation in milliseconds.")
    args = parser.parse_args()
        
    rm = pyvisa.ResourceManager()
    timeout = args.t
    num = args.N
    duration = args.d
    instruments = {}
    for i in range(1, num + 1):
        instruments[i] = {}
        conn = f"TCPIP::{args.i}::gpib0,{i}::INSTR"
        instruments[i]["conn"] = conn
        inst = connect_instrument(rm, conn, timeout)
        if inst is None:
            print(f"{conn}: Could not connect, aborting.")
            exit(1)
        idn = get_idn(inst)
        if idn is None:
            print(f"{conn}: Could not get IDN string, aborting.")
            exit(1)
        print(f"{conn}: Identified as \"{idn}\".")            
        islegacy = is_legacy_instrument(idn)
        instruments[i]["name"] = idn
        instruments[i]["inst"] = inst
        instruments[i]["legacy"] = islegacy
        instrument_type_val = get_instrument_type(idn)
        instruments[i]["type"] = instrument_type_val
        init_success = init_instrument(inst, conn, islegacy)
        if not init_success:
            print(f"{conn}: Initialization failed, aborting.")
            exit(1)
        
    print(f"Starting stress test for {duration} seconds...")
    start_time = datetime.datetime.now()
    while (datetime.datetime.now() - start_time).total_seconds() < duration:
        # Run the interrogation loop
        for k, my_inst in instruments.items():
            if not interrogate_instrument(my_inst["inst"], my_inst["conn"], my_inst["type"]):
                exit(1)

    print("Done.")
