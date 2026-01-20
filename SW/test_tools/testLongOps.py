import time
import argparse
import pyvisa
import logging
from enum import Enum

NUM_WRITES_66332A_PROLOGIX = 6  # max number of writes for 66332A without it crapping out. The limits seems to be related to prologix vs VXI-11, lower limit with prologix
NUM_WRITES_66332A_VXI11 = 14  # max number of writes for 66332A without it crapping out. The limits seems to be related to prologix vs VXI-11, higher limit with VXI-11

TESTCONFIG = {
    "usb": {
        "inst": "ASRL/dev/cu.usbmodem21101::INSTR",
        "p": 1,
        "type": "66332A",
        "readings": 800,
        "writes": NUM_WRITES_66332A_PROLOGIX
    },
    "socket": {
        "inst": "TCPIP::192.168.7.206::1234::SOCKET",
        "p": 1,
        "type": "66332A",
        "readings": 800,
        "writes": NUM_WRITES_66332A_PROLOGIX
    },
    "prologix": {
        "inst": "PRLGX-TCPIP::192.168.7.206::INTFC",
        "p": 1,
        "type": "66332A",
        "readings": 800,
        "writes": NUM_WRITES_66332A_PROLOGIX
    },    
    "vxi": {
        "inst": "TCPIP::192.168.7.206::gpib,1::INSTR",
        "p": 0,
        "type": "66332A",
        "readings": 800,
        "writes": NUM_WRITES_66332A_VXI11
    },    
    "sa_vxi": {
        "inst": "TCPIP::192.168.7.206::gpib,18::INSTR",
        "p": 0,
        "type": "8590E",
        "readings": 1,
        "writes": 0
    },
    "sa_prologix": {
        "inst": "PRLGX-TCPIP::192.168.7.206::INTFC",
        "p": 18,
        "type": "8590E",
        "readings": 1,
        "writes": 0
    },
    "sa_socket": {
        "inst": "TCPIP::192.168.7.206::1234::SOCKET",
        "p": 18,
        "type": "8590E",
        "readings": 1,
        "writes": 0
    },          
    "direct": {
        "inst": "TCPIP::192.168.7.205::INSTR",
        "p": 0,
        "type": "DMM6500",
        "readings": 256,
        "writes": 0
    },
    "default": {
        "inst": "TCPIP::192.168.7.205::INSTR",
        "p": 0,
        "type": "DMM6500",
        "readings": 0,
        "writes": 0
    }    
}

DEFAULT_DEVICE = "sa_prologix"

PROLOGIX_SLEEP = 0.5  # seconds


def getIDString(device_type: str):
    if device_type == "8590E":
        return "*ID?"
    else:
        return "*IDN?"


def getErrorString(device_type: str):
    if device_type == "8590E":
        return "CMDERRQ?"
    else:
        return "SYST:ERR?"


# global vars
rm = None
prlgx = None
inst = None


def check_err(inst, device_type: str, use_prologix_commands: bool):
    time.sleep(0.5)
    
    have_err = True
    # have_printed_err = False
    errstr = getErrorString(device_type)
    while have_err:
        if use_prologix_commands:
            inst.write(errstr)
            time.sleep(PROLOGIX_SLEEP)
            inst.write("++read eoi")
            err = inst.read_raw().decode('ascii')        
        else:
            err = inst.query(errstr)
        err = err.strip()
        if not (err.startswith("+0,") or err.startswith("0,") or len(err) == 0):
            print(f"Error: \"{err}\"")
            have_err = True
            # have_printed_err = True
        else:
            # if not have_printed_err:
            #    print("OK.")
            have_err = False


class prologix_type(Enum):
    NONE = 0
    SERIAL = 1
    SOCKET = 1
    INTFC = 2
    

def get_prologix_type(device_address: str):
    if "ASRL" in device_address:
        return prologix_type.SERIAL
    elif "SOCKET" in device_address:
        return prologix_type.SOCKET
    elif "INTFC" in device_address:
        return prologix_type.INTFC
    else:
        return prologix_type.NONE
    

def init_device(device_address: str, device_bus_address: int, device_type: str, prologix: prologix_type) -> bool:
    global rm, inst, prlgx
    use_prologix_commands = False
    print("Device type: ", device_type)
    print("Device address: ", device_address)
    if prologix != prologix_type.NONE:
        print("Device bus address for prologix: ", device_bus_address)
        if prologix == prologix_type.SERIAL or prologix == prologix_type.SOCKET:
            use_prologix_commands = True
        
    print("Connecting to device...")    
    rm = pyvisa.ResourceManager()

    if prologix == prologix_type.INTFC:
        prlgx = rm.open_resource(device_address)
        time.sleep(1)  # let the interface settle
        inst = rm.open_resource(f"GPIB::{device_bus_address}::INSTR")
        inst.query_delay = PROLOGIX_SLEEP
    else:
        inst = rm.open_resource(device_address)        
    inst.timeout = 10000  # milli-seconds
    if use_prologix_commands:
        inst.read_termination = "\n"
        inst.write_termination = "\n"

    print("Init communication with device and resetting device...")

    if use_prologix_commands:
        inst.write("++auto 0")
        inst.write(f"++addr {device_bus_address}")
        inst.write("++mode 1")
        inst.write("++eoi 1")
        inst.write("++eos 3")

    if device_type != "8590E":
        inst.write("*rst")
        inst.write("*cls")
    else:
        inst.write("*CLS")
    # inst.write("*rcl")
    
    check_err(inst, device_type, use_prologix_commands)
    
    print("Identifying device...")
    idstr = getIDString(device_type)
    if use_prologix_commands:
        inst.write(idstr)
        inst.write("++read eoi")
        idn = inst.read_raw().decode('ascii')
    else:
        idn = inst.query(idstr)
    idn = idn.strip()
    print(f"Device ID: \"{idn}\"")
    
    check_err(inst, device_type, use_prologix_commands)
    
    return use_prologix_commands


def close_device():
    global rm, inst, prlgx
    try:
        if inst is not None:
            # print("Closing instrument interface...")
            inst.close()
            inst = None
    except:
        pass
    try:
        if prlgx is not None:
            # print("Closing prologix interface...")
            prlgx.close()
            prlgx = None
    except:
        pass
    try:
        if rm is not None:
            # print("Closing resource manager...")
            rm.close()
            rm = None
    except:
        pass


def write_device(device_address: str, device_bus_address: int, device_type: str, number_of_writes: int):
    global rm, inst
    prologix = prologix_type.NONE
    
    if device_bus_address > 0:
        prologix = get_prologix_type(device_address)
        if prologix == prologix_type.NONE:
            print("Device bus address provided but device address does not indicate a prologix device. Bailing out.")
            return
                
    if number_of_writes < 1:
        return
        
    print("WRITING DEVICE *********************")
    print("Number of writes to do: ", number_of_writes)
    
    use_prologix_commands = init_device(device_address, device_bus_address, device_type, prologix)
    
    if device_type == "8590E":
        print("Testing of 8590E series is not implemented yet.")
        return
        
    print("Doing writes...")
    if device_type == "66332A":
        cmd = "OUTP ON;VOLT 0;"
        m = 20.0 / number_of_writes
        for i in range(number_of_writes):
            cmd += f"VOLT {(i + 1) * m:.2f};*WAI;"  # adding WAI so I can see the progress on a scope or fast DMM

    inst.write(cmd)
    time.sleep(1)
    print("Read Errors if any...")
    check_err(inst, device_type, use_prologix_commands)
    print("Read Output...")

    if device_type == "66332A":
        readcmd = "MEAS:VOLT?"
        
    if use_prologix_commands:
        inst.write(readcmd)
        time.sleep(PROLOGIX_SLEEP)
        try:
            inst.write("++read eoi")
            voltage = inst.read_raw().decode('ascii')
            
        except Exception as e:
            print("Error reading data: ", e)
            voltage = ""
    else:
        voltage = inst.query(readcmd)
    
    print("Voltage read: ", voltage)    
        
    check_err(inst, device_type, use_prologix_commands)
    # final error status check
    check_err(inst, device_type, use_prologix_commands)

    print("Closing...")
    close_device()


def read_device(device_address: str, device_bus_address: int, device_type: str, number_of_readings: int):
    global rm, inst
    prologix = prologix_type.NONE
    
    if device_bus_address > 0:
        prologix = get_prologix_type(device_address)
        if prologix == prologix_type.NONE:
            print("Device bus address provided but device address does not indicate a prologix device. Bailing out.")
            return
        
    if number_of_readings < 1:
        return
    
    if device_type == "8590E":
        number_of_readings = 1  # only one reading possible, it is already rather big
    
    print("READING DEVICE *********************")
    print("Number of readings to do: ", number_of_readings)    

    use_prologix_commands = init_device(device_address, device_bus_address, device_type, prologix)
        
    print("Initialising measurements...")
    interval_in_ms = 100
    if device_type == "K2000":
        inst.write("func 'volt:dc'")
        inst.write("status:measurement:enable 512")
        inst.write("*sre 1")
        inst.write(f"sample:count {number_of_readings}")
        inst.write("trigger:source bus")
        inst.write(f"trigger:delay {interval_in_ms / 1000.0:.6f}")
        inst.write(f"trace:points {number_of_readings}")
        inst.write("trace:feed sense1")
        inst.write("feed:control next")
        inst.write("initiate")
        inst.write("*TRG")
    if device_type == "DMM6500":
        inst.write("func 'volt:dc'")        
        # I will be using the default buffer with enough space for the readings, so no need for 'trace:points'
        inst.write("TRACE:CLEAR")
        inst.write(f"COUNT {number_of_readings}")
        inst.write(f"TRIG:LOAD \"SimpleLoop\", {number_of_readings}, {interval_in_ms / 1000.0:.6f}")
        inst.write("INIT")
    if device_type == "66332A":
        # I ignore interval_in_ms for now, since freely setting it has side implications
        # default setting for interval:
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
    if device_type == "8590E":
        pass
    
    check_err(inst, device_type, use_prologix_commands)        
        
    # Wait for the measurement to complete
    print("Sampling", end='')

    spin = True
    while (spin):
        time.sleep(0.5)
        print(".", end='', flush=True)
        if device_type == "K2000" and use_prologix_commands:        
            inst.write("status:measurement?")
            if ((512 & int(inst.query("++read eoi"))) == 512):
                spin = False
        if device_type == "K2000" and not use_prologix_commands:
            inst.write("status:measurement?")
            if ((512 & int(inst.query("status:measurement?"))) == 512):
                spin = False
        if device_type == "DMM6500":
            st = inst.query(":TRIGger:STATe?")
            if "RUNNING" not in st and "WAITING" not in st:
                spin = False
        if device_type == "66332A":
            # TODO
            spin = False
        if device_type == "8590E":
            # TODO
            spin = False

    readcmd = ""
    if device_type == "K2000":
        readcmd = "trace:data?"
    if device_type == "DMM6500":
        readcmd = f"TRAC:DATA? 1, {number_of_readings}"
    if device_type == "66332A":
        readcmd = "MEAS:ARRAY:VOLT?"  # FETCH seems to be broken
    if device_type == "8590E":
        readcmd = "USTATE?"
    
    print("\nRetrieving...")
    readresult = ""
    if use_prologix_commands:
        inst.write(readcmd)
        time.sleep(PROLOGIX_SLEEP)
        inst.write("++read eoi")        
        if device_type == "8590E":
            try:
                readresult = inst.read_binary_values(datatype="B", header_fmt="hp", is_big_endian=True, expect_termination=True)
            except Exception as e:
                print("Error reading data: ", e)
                readresult = b""                
            # for comparison: this is the raw way, with less checking. 
            # readresult = b""
            # while True:
            #     try:
            #         chunk = inst.read_raw()
            #         print(f"Read chunk of size {len(chunk)} bytes")
            #         if len(chunk) == 0:
            #             break
            #         readresult += chunk
            #         if chunk[-2:] == b'\r\n':
            #             break                    
            #     except:
            #         break
        else:
            try:
                readresult = inst.read_ascii_values()
            except Exception as e:
                print("Error reading data: ", e)
                readresult = []
    else:
        if device_type == "8590E":
            # if this is a PRLGX-TCPIP device, I cannot do write() and then read_raw(), as it will not print the ++read command
            # so: I do this the proper way
            readresult = inst.query_binary_values(readcmd, datatype="B", header_fmt="hp", is_big_endian=True, expect_termination=True)
        else:
            readresult = inst.query_ascii_values(readcmd)

    if device_type == "K2000":
        inst.write("feed:control next")
    if device_type == "DMM6500":
        inst.write("trace:clear")

    if device_type != "8590E":
        print("Readings requested: ", number_of_readings)
        print("Readings retrieved: ", len(readresult))
    else:
        print(f"Binary data length retrieved: {len(readresult)}")
    
    # final error status check
    check_err(inst, device_type, use_prologix_commands)

    print("Closing...")
    close_device()
    

if __name__ == '__main__':
    # Default values for testing
    DEFAULT_INST = TESTCONFIG[DEFAULT_DEVICE]["inst"]
    DEFAULT_P = TESTCONFIG[DEFAULT_DEVICE]["p"]
    DEFAULT_TYPE = TESTCONFIG[DEFAULT_DEVICE]["type"]
    DEFAULT_READINGS = TESTCONFIG[DEFAULT_DEVICE]["readings"]
    DEFAULT_WRITES = TESTCONFIG[DEFAULT_DEVICE]["writes"]
    
    presets = TESTCONFIG.keys()
    presets = [p for p in presets if p != "default"]  # remove default from the list
    device_types = ["DMM6500", "K2000", "66332A", "8590E"]
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Test long Reads or Writes via VXI-11, USB prologix or Ethernet prologix.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("instrument", type=str, nargs="?", default=DEFAULT_INST, help="The device to use for tests. Must be a Visa compatible connection string.")
    parser.add_argument("-p", type=int, default=DEFAULT_P, help="The device address on the bus. Used with prologix. Use 0 for VXI-11.")
    parser.add_argument("-t", choices=device_types, default=DEFAULT_TYPE, help="The instrument type.")
    parser.add_argument("-r", type=int, default=DEFAULT_READINGS, help="Number of readings.")
    parser.add_argument("-w", type=int, default=DEFAULT_WRITES, help="Number of writes.")
    parser.add_argument("-d", choices=presets, default=None, help="Select one of the presets.")
    parser.add_argument("-v", action="store_true", help="Enable verbose output.")
    parser.epilog = "VXI-11 address example: \"TCPIP::192.168.1.84::gpib,1::INSTR\". USB Prologix address example: \"ASRL9::INSTR\". Ethernet Prologix address example: \"TCPIP::192.168.1.84::1234::SOCKET\". This code is NOT compatible with a RAW socket device, as I re-use the RAW socket address style for prologix."
    args = parser.parse_args()
        
    device_bus_address = args.p
    device_address = args.instrument
    device_type = args.t
    number_of_readings = args.r
    number_of_writes = args.w
    verbose = args.v
    
    if verbose:
        logging.basicConfig(level=logging.DEBUG)
        logging.getLogger('pyvisa').setLevel(logging.DEBUG)
        logging.getLogger('pyvisa-py').setLevel(logging.DEBUG)
    else:
        logging.basicConfig(level=logging.INFO)
        logging.getLogger('pyvisa').setLevel(logging.INFO)
        logging.getLogger('pyvisa-py').setLevel(logging.INFO)
    preset = args.d
    if preset is not None:
        device_address = TESTCONFIG[preset]["inst"]
        device_bus_address = TESTCONFIG[preset]["p"]
        device_type = TESTCONFIG[preset]["type"]
        number_of_readings = TESTCONFIG[preset]["readings"]
        number_of_writes = TESTCONFIG[preset]["writes"]
    
    if number_of_writes == 0 and number_of_readings == 0:
        print("Nothing to do. Exiting. See -h for help.")
        exit(0)
        
    if device_address is None or device_address == "":
        print("No device address provided. Exiting.")
        exit(1)
    read_device(device_address, device_bus_address, device_type, number_of_readings)
    write_device(device_address, device_bus_address, device_type, number_of_writes)

    print("Done.")
