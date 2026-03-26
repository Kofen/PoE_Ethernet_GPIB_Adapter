import pyvisa
rm = pyvisa.ResourceManager()
prlgx = rm.open_resource("PRLGX-TCPIP::192.168.7.206::INTFC")  # This is the gateway, and pyvisa will pick port 1234 automatically. It will propose itself as a native GPIB interface, so you can't also have a GPIB card in your system
inst1 = rm.open_resource("GPIB::1::INSTR")  # instrument at address 1
inst2 = rm.open_resource("GPIB::2::INSTR")  # etc
inst18 = rm.open_resource("GPIB::18::INSTR")
# and now you can talk to the instruments (one at a time please):
print(inst1.query("*IDN?"))
print(inst2.query("*IDN?"))
print(inst18.query("*ID?"))
