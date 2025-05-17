
# PoE Ethernet GPIB Adapter, with Prologix or VXI-11.2

## PoE-powered GPIB Adapter with Ethernet and USB-C Support

<a href="Img/adapter_connected.png" target="_blank">
    <img src="Img/adapter_connected.png" alt="Adapter Assembled" width="800">
</a>

The **PoE Ethernet GPIB Adapter** is designed to interface test equipment with GPIB / HPIB / IEEE-488 interfaces.

The primary goal of this project is to provide an easy solution to connect multiple GPIB-equipped instruments over Ethernet without excessive cabling. It can, at choice, use the Prologix protocol or support VXI-11.2 (VISA).

---

## Why?

The motivation behind this project comes from the challenges of using several generations of instruments with GPIB interfaces. In theory, only one GPIB master is needed, and up to 20+ instruments can be connected in any order using suitable GPIB cabling. However, there are drawbacks:

- Instruments must be fully compliant with the IEEE-488 standard. Many older instruments, predating the standard, do not comply. The main issue is that the bus does not go Hi-Z when off, requiring certain instruments to be powered on even if you're only using another device.
- GPIB cabling is bulky and expensive. While used options can sometimes be found, the overall cost remains high.

At work, this has been solved elegantly using commercial Ethernet adapters, each assigned an address and connected directly to the instrument. However, these adapters are priced around $500 USD—far from hobbyist-friendly. My initial solution was to buy one adapter and rely on daisy-chaining, but as my instrument collection grew to 20 devices requiring GPIB, this approach was no longer practical. Hence, the **PoE Ethernet GPIB Adapter** was born.

---

## Design Criteria

1. Lower cost: Total BOM should be less than 50 USD.
2. Support Power over Ethernet (PoE) to minimize cable clutter.
3. Include USB-C power as an alternative if PoE is unavailable.
4. Enable GPIB communication over both Ethernet and USB-C interfaces.
5. Use the same communication protocol as existing commercial units.
6. Minimize radiated and conducted noise to avoid interference in test environments.
7. Include a simple, easy-to-print 3D-printed enclosure to keep costs low.

---

## Results

All design goals have been met[^2]. The current unit price is approximately $45 USD when ordering parts for 20 units. Prices increase for smaller batch sizes.

[^2]: GPIB commands over USB-C is Work In Progress.

In my home lab, I assign each device a static IP address based on its MAC address and run a local DNS server to provide easy-to-remember domain names, making the adapter simple and intuitive to use.

Project's documentation:

- [Design Documentation](docs/dn.md)
- [Design Test Documentation](docs/dt.md)

---

## VXI-11 or Prologix

This code can produce either a VXI-11.2 device, or a Prologix device (the ROM is not big enough for both at the same time, although the code is compatible with cohabitation). Switching between VXI-11 and Prologix is done at compile time. If compiler option `-DINTERFACE_PROLOGIX` is used, the firmware produced is for Prologix.

VXI-11.2 allows discovery over the network, and uses the following VISA connection strings:

- Controller[^1]: `TCPIP::{IP address}::INSTR` (unless you have set the default instrument address to something else than 0)
- Instruments: `TCPIP::{IP address}::gpib,N::INSTR` or `TCPIP::{IP address}::instN::INSTR`, where `N` is their address on the GPIB bus (1..30)

Example: `TCPIP::192.168.7.105::gpib,2::INSTR` for instrument with GPIB address 2 on the gateway having IP address 192.168.7.105.

[^1]: controller, gateway, adapter: different names for the same.

### VXI-11.2 compatibility

With the limited resources, this device is meant to work with the most common tools, like for example pyVisa. It is not a full implementation, and lacks the following advanced features:

- secondary instrument addresses
- async VXI-11 operations
- instrument locking via VXI-11
- VXI-11 interrupts
- the VXI-11 abort channel

It is discoverable via UDP, but there is no publication via mDNS (yet).

---

## The number of instruments you can connect

The GPIB bus protocol itself allows up to 30 instruments. This does not mean you can connect 30 instruments to the gateway, as the gateway device has its electrical limits, depending on the length of the cables and instruments themselves. And also the software has its limits.

The Prologix service will accept as many instruments as the gateway hardware can drive reliably.

The VXI-11 service is easier to integrate with many tools, but it consumes more resources on the gateway. The Ethernet chip can only maintain a limited number of network sockets, and each instrument requires a separate socket with VXI-11. Prologix uses only 1 socket, no matter the number of instruments. This means that regardless of the limitations of the hardware, there is a limit to the number of instruments you can connect to via VXI-11 compatible client software:

- up to 4 instruments: no restriction
- 5 instruments: only if you do not use the web server
- 6 instruments: only if you disable the web server (use the compile option `-DDISABLE_WEB_SERVER`)
- 7 or more: not possible via VXI-11

This does not mean that you cannot physically connect more instruments to the gateway, it just means that you cannot connect to more of them, via your client software, at the same time.

Also, be aware that the GPIB bus is a shared bus. Do not try to control instruments on the bus from different software clients at the same time. VXI-11 is somewhat more forgiving in this matter, but the prologix service simply doesn't allow multiple connections.

---

## The User Interface of the device

There are 3 parts:

- the **LED**. It indicates different states:
  - blue solid for waiting for DHCP
  - red solid for error in network or DHCP
  - red single flash when the assigned IP address changes
  - green flashing for idle
  - green/blue flashing for busy
- the **Web Server** (on port 80): it shows some help texts, the number of connected clients, and allows interaction with any of the connected instruments. The latter functionality is however only available with the VXI-11 service, as Prologix does not leave enough ROM space available.
- the **serial console** (via USB): This console shows startup information, ports used, and has a small menu.

### The serial menu

This menu is rather basic. Be aware that it requires 'enter' for each command.

It has the following options:

- Setting of IP address. By default, the device starts with DHCP. You can however force a fixed IP address.
- Setting of the default instrument address (only with VXI-11, as Prologix has its own command for that). The default is 0, meaning: the gateway itself. If you only have 1 instrument connected, or want to designate a "preferred" instrument, you can set it to the address of any instrument on the bus. That way, the gateway becomes transparent, and you can use the default (and the discoverable) VISA connection string to address that instrument.

### The Web server

An example of the web page when using VXI-11:

<kbd>
<img src="./Img/web_server.png">
</kbd>

Note that the interactive part is not available when using Prologix.

Explanation of the buttons:

* <kbd>Find</kbd>: scan the GPIB bus for instruments
* <kbd>&lt;</kbd>: populate the Command field with one of the standard commands from the drop down list
* <kbd>Query</kbd>: Query the selected instrument. This is the same as a Send followed by a Read provided the command ends with a "?". This is also executed when pressing the "enter" key while entering data in the command text field.
* <kbd>Send</kbd>: Send command to the selected instrument.
* <kbd>Read</kbd>: Read from the selected instrument.
* <kbd>Clear history</kbd>: clear the contents of history.

Do not interact with the instruments via the web interface while you also interact with the instruments from the VXI or Prologix interface.

---

## Project files

Latest version is avaiable under [Releases](https://github.com/Kofen/PoE_Ethernet_GPIB_Adapter/releases)

And all the source code for all the parts should be in their respective folders in the repro. If anything is missing, let me know!

## How to compile

The easiest is for compilation with PlatformIO, from the [SW](/SW) directory.

## License

This project is licensed under the GPL V3. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- A huge thanks to the [AR488 project](https://github.com/Twilight-Logic/AR488), run by [Twilight-Logic](https://github.com/Twilight-Logic) and its community contributors. The current software is a fork of AR488. For more information about this, see [here](SW/README.md).
- The VXI-11 driver is partially inherited from [espBode](https://github.com/awakephd/espBode), and went through various evolutions before arriving here.
