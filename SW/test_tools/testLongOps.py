import time
import argparse
import pyvisa

TESTCONFIG = {
    "usb": {
        "inst": "ASRL/dev/cu.usbmodem21101::INSTR",
        "p": 1,
        "type": "66332A",
        "readings": 800,
        "writes": 14
    },
    "prologix": {
        "inst": "TCPIP::192.168.7.206::1234::SOCKET",
        "p": 1,
        "type": "66332A",
        "readings": 800,
        "writes": 14  # 14 is the limit because otherwise the instrument craps out
    },
    "vxi": {
        "inst": "TCPIP::192.168.7.206::gpib,1::INSTR",
        "p": 0,
        "type": "66332A",
        "readings": 800,
        "writes": 14  # 14 is the limit because otherwise the instrument craps out
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

DEFAULT_DEVICE = "default"

PROLOGIX_SLEEP = 0.1  # seconds


def check_err(inst, prologix: bool):
    time.sleep(0.5)
    
    have_err = True
    # have_printed_err = False
    while have_err:
        if prologix:
            inst.write("SYST:ERR?")
            time.sleep(PROLOGIX_SLEEP)
            err = inst.query("++read eoi")
        else:
            err = inst.query("SYST:ERR?")
        if not (err.startswith("+0,") or err.startswith("0,")):
            print(f"Error: \"{err}\"")
            have_err = True
            # have_printed_err = True
        else:
            # if not have_printed_err:
            #    print("OK.")
            have_err = False


def init_device(device_address: str, device_bus_address: int, prologix: bool):
    
    print("Device type: ", device_type)
    print("Device address: ", device_address)
    if prologix:
        print("Device bus address for prologix: ", device_bus_address)
        
    print("Connecting to device...")    
    rm = pyvisa.ResourceManager()
    inst = rm.open_resource(device_address)
    inst.timeout = 10000  # milli-seconds
    if prologix:
        inst.read_termination = "\n"
        inst.write_termination = "\n"

    print("Init communication with device and resetting device...")

    if prologix:
        inst.write("++auto 0")
        inst.write(f"++addr {device_bus_address}")
        inst.write("++mode 1")
        inst.write("++eoi 1")
        inst.write("++eos 3")

    inst.write("*rst")
    inst.write("*cls")
    # inst.write("*rcl")
    
    check_err(inst, prologix)
    
    print("Identifying device...")
    if prologix:
        inst.write("*IDN?")
        idn = inst.query("++read eoi")
    else:
        idn = inst.query("*IDN?")
    print(f"Device ID: \"{idn}\"")
    
    check_err(inst, prologix)
    
    return rm, inst


def write_device(device_address: str, device_bus_address: int, device_type: str, number_of_writes: int):
    prologix = False
    
    if device_bus_address > 0:
        prologix = True
        
    if number_of_writes < 1:
        return
        
    print("WRITING DEVICE *********************")
    print("Number of writes to do: ", number_of_writes)
    
    rm, inst = init_device(device_address, device_bus_address, prologix)
    
    print("Doing writes...")
    if device_type == "66332A":
        cmd = "OUTP ON;VOLT 0;"
        m = 20.0 / number_of_writes
        for i in range(number_of_writes):
            cmd += f"VOLT {(i + 1) * m:.3f};*WAI;"  # adding WAI so I can see the progress on a scope or fast DMM

    inst.write(cmd)
    time.sleep(1)
    print("Read Errors if any...")
    check_err(inst, prologix)
    print("Read Output...")

    if device_type == "66332A":
        readcmd = "MEAS:VOLT?"
        
    if prologix:
        inst.write(readcmd)
        time.sleep(PROLOGIX_SLEEP)
        try:
            voltage = inst.query("++read eoi")
        except Exception as e:
            print("Error reading data: ", e)
            voltage = ""
    else:
        voltage = inst.query(readcmd)
    
    print("Voltage read: ", voltage)    
        
    check_err(inst, prologix)
    # final error status check
    check_err(inst, prologix)

    inst.close()
    rm.close()


def read_device(device_address: str, device_bus_address: int, device_type: str, number_of_readings: int):
    prologix = False
    
    if device_bus_address > 0:
        prologix = True
        
    if number_of_readings < 1:
        return
        
    print("READING DEVICE *********************")
    print("Number of readings to do: ", number_of_readings)    
    rm, inst = init_device(device_address, device_bus_address, prologix)

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
    
    check_err(inst, prologix)        
        
    # Wait for the measurement to complete
    print("Sampling", end='')

    spin = True
    while (spin):
        time.sleep(0.5)
        print(".", end='', flush=True)
        if device_type == "K2000" and prologix:        
            inst.write("status:measurement?")
            if ((512 & int(inst.query("++read eoi"))) == 512):
                spin = False
        if device_type == "K2000" and not prologix:
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

    readcmd = ""
    if device_type == "K2000":
        readcmd = "trace:data?"
    if device_type == "DMM6500":
        readcmd = f"TRAC:DATA? 1, {number_of_readings}"
    if device_type == "66332A":
        readcmd = "MEAS:ARRAY:VOLT?"  # FETCH seems to be broken
    
    print("\nRetrieving...")
    if prologix:
        inst.write(readcmd)
        time.sleep(PROLOGIX_SLEEP)
        try:
            voltages = inst.query_ascii_values("++read eoi")
        except Exception as e:
            print("Error reading data: ", e)
            voltages = []
    else:
        voltages = inst.query_ascii_values(readcmd)
    if device_type == "K2000":
        inst.write("feed:control next")
    if device_type == "DMM6500":
        inst.write("trace:clear")

    # print(voltages)
    print("Readings requested: ", number_of_readings)
    print("Readings retrieved: ", len(voltages))
    
    # final error status check
    check_err(inst, prologix)

    inst.close()
    rm.close()


if __name__ == '__main__':
    # Default values for testing
    DEFAULT_INST = TESTCONFIG[DEFAULT_DEVICE]["inst"]
    DEFAULT_P = TESTCONFIG[DEFAULT_DEVICE]["p"]
    DEFAULT_TYPE = TESTCONFIG[DEFAULT_DEVICE]["type"]
    DEFAULT_READINGS = TESTCONFIG[DEFAULT_DEVICE]["readings"]
    DEFAULT_WRITES = TESTCONFIG[DEFAULT_DEVICE]["writes"]
    
    presets = TESTCONFIG.keys()
    presets = [p for p in presets if p != "default"]  # remove default from the list
    device_types = ["DMM6500", "K2000", "66332A"]
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Test long Reads or Writes via VXI-11, USB prologix or Ethernet prologix.",
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument("instrument", type=str, nargs="?", default=DEFAULT_INST, help="The device to use for tests. Must be a Visa compatible connection string.")
    parser.add_argument("-p", type=int, default=DEFAULT_P, help="The device address on the bus. Used with prologix. Use 0 for VXI-11.")
    parser.add_argument("-t", choices=device_types, default=DEFAULT_TYPE, help="The instrument type.")
    parser.add_argument("-r", type=int, default=DEFAULT_READINGS, help="Number of readings.")
    parser.add_argument("-w", type=int, default=DEFAULT_WRITES, help="Number of writes.")
    parser.add_argument("-d", choices=presets, default=None, help="Select one of the presets.")
    parser.epilog = "VXI-11 address example: \"TCPIP::192.168.1.84::gpib,1::INSTR\". USB Prologix address example: \"ASRL9::INSTR\". Ethernet Prologix address example: \"TCPIP::192.168.1.84::1234::SOCKET\". This code is NOT compatible with a RAW socket device, as I re-use the RAW socket address style for prologix."
    args = parser.parse_args()
        
    device_bus_address = args.p
    device_address = args.instrument
    device_type = args.t
    number_of_readings = args.r
    number_of_writes = args.w
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
