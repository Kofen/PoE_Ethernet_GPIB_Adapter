#ifndef AR488_GPIBbus_H
#define AR488_GPIBbus_H

//#include <SD.h>
#include "AR488_Config.h"
#include "AR488_Layouts.h"
#include "AR488_ComPorts.h"



/***** AR488_GPIBbus.cpp, ver. 0.53.02, 04/04/2025 *****/


/*********************************************/
/***** GPIB COMMAND & STATUS DEFINITIONS *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/
#ifdef AR488_GPIBconf_EXTEND
#define GPIB_CFG_SIZE 87
#else
#define GPIB_CFG_SIZE 83
#endif

/***** Debug Port *****/
#ifdef DB_SERIAL_ENABLE
extern Stream &debugPort;
/* Configured in Config.h */
//#define DEBUG_GPIBbus_RECEIVE
//#define DEBUG_GPIBbus_SEND
//#define DEBUG_GPIBbus_CONTROL
//#define SN7516X
/* Configured in Config.h */

#endif


/***** Universal Multiline commands (apply to all devices) *****/
#define GC_GTL 0x01
#define GC_SDC 0x04
#define GC_PPC 0x05
#define GC_GET 0x08
#define GC_TCT 0x09
#define GC_LLO 0x11
#define GC_DCL 0x14
#define GC_PPU 0x15
#define GC_SPE 0x18
#define GC_SPD 0x19
#define GC_LAD 0x20
#define GC_UNL 0x3F
#define GC_UNT 0x5F
#define GC_TAD 0x40
#define GC_PPE 0x60
#define GC_SAD 0x60
#define GC_PPD 0x70


/***** GPIB control states *****/
// Controller mode
#define CINI 0x01  // Controller idle state
#define CIDS 0x02  // Controller idle state
#define CCMS 0x03  // Controller command state
#define CTAS 0x04  // Controller talker active state
#define CLAS 0x05  // Controller listner active state
// Listner/device mode
#define DINI 0x06  // Device initialise state
#define DIDS 0x07  // Device idle state
#define DLAS 0x08  // Device listener active (listening/receiving)
#define DTAS 0x09  // Device talker active (sending) state


/***** Addressing direction *****/
#define TONONE 0
#define TOLISTEN 1
#define TOTALK 2


/***** Lastbyte - send EOI *****/
#define NO_EOI false
#define WITH_EOI true


/***** Handshake states *****/
enum gpibHandshakeStates {
  // Common
  HANDSHAKE_START,
  HANDSHAKE_COMPLETE,
  IFC_ASSERTED,
  ATN_ASSERTED,
  // Read
  WAIT_FOR_DATA,
  READ_DATA,
  DATA_ACCEPTED,
  // Write
  WAIT_FOR_RECEIVER_READY,
  PLACE_DATA,
  DATA_READY,
  RECEIVER_ACCEPTING
};


#define IFC_BIT (1 << 0)
#define NDAC_BIT (1 << 1)
#define NRFD_BIT (1 << 2)
#define DAV_BIT (1 << 3)
#define EOI_BIT (1 << 4)
#define REN_BIT (1 << 5)
#define SRQ_BIT (1 << 6)
#define ATN_BIT (1 << 7)
#define CTRL_BITS (0xE1)
#define HSHK_BITS (0x1E)
#define ALL_BITS (0xFF)


enum operatingModes {
  OP_IDLE,
  OP_CTRL,
  OP_DEVI
};


enum transmitModes {
  TM_IDLE,
  TM_RECV,
  TM_SEND
};


/***** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ *****/
/***** GPIB COMMAND & STATUS DEFINITIONS *****/
/*********************************************/


/****************************************/
/***** GPIB CLASS OBJECT DEFINITION *****/
/***** vvvvvvvvvvvvvvvvvvvvvvvvvvvv *****/

class GPIBbus {


public:

  /***** Controller configuration *****/
  union GPIBconf {
    struct {
      bool eot_en;      // Enable/disable append EOT char to string received from GPIB bus before sending to USB
      bool eoi;         // Assert EOI on last data char written to GPIB - 0-disable, 1-enable
      uint8_t cmode;    // Controller/device mode (0=unset, 1=device, 2=controller)
      uint8_t caddr;    // This interface address
      uint8_t paddr;    // Primary address to use when addressing a device
      uint8_t saddr;    // Secondary address to use when addressing a device
      uint8_t eos;      // EOS (end of send to GPIB) characters [0=CRLF, 1=CR, 2=LF, 3=None]
      uint8_t stat;     // Status byte to return in response to a serial poll
      uint8_t amode;    // Auto mode setting (0=off; 1=Prologix; 2=onquery; 3=continuous);
      uint16_t rtmo;    // Read timout (read_tmo_ms) in milliseconds - 0-32000 - value depends on instrument
      char eot_ch;      // EOT character to append to USB output when EOI signal detected
      char vstr[48];    // Custom version string
      uint8_t eor;      // EOR (end of receive from GPIB instrument) characters [0=CRLF, 1=CR, 2=LF, 3=None, 4=LFCR, 5=ETX, 6=CRLF+ETX, 7=SPACE]
      char sname[16];   // Interface short name
      uint32_t serial;  // Serial number
      uint8_t idn;      // Send ID in response to *idn? 0=disable, 1=send name; 2=send name+serial
      uint8_t hflags;   // Handshaking indicator flags
#ifdef AR488_GPIBconf_EXTEND      
      uint8_t ip[4];    // IP address
#endif
    };
    uint8_t db[GPIB_CFG_SIZE];
  };

  union GPIBconf cfg;

  uint8_t cstate = 0;

  GPIBbus();

  void begin();
  void stop();

  void setDefaultCfg();

  void startControllerMode();
  void startDeviceMode();

  void setOperatingMode(enum operatingModes mode);
  void setTransmitMode(enum transmitModes mode);
  void assertSignal(uint8_t sig);
  void clearSignal(uint8_t sig);
  void clearAllSignals();


  bool isController();

  void sendIFC();
  bool sendLLO();
  bool sendGTL();
  bool sendGET(uint8_t addr);
  bool sendSDC();
  bool sendTCT(uint8_t addr);
  void sendAllClear();

  bool sendUNT();
  bool sendUNL();
  bool sendMTA();
  bool sendMLA();
  bool sendMSA(uint8_t addr);

  bool isAsserted(uint8_t gpibsig);
  void setControls(uint8_t state);
  void sendStatus();

  void setStatus(uint8_t statusByte);
  bool sendCmd(uint8_t cmdByte);
  bool sendSecondaryCmd(uint8_t paddr, uint8_t saddr, char * data, uint8_t dsize);
  enum gpibHandshakeStates readByte(uint8_t *db, bool readWithEoi, bool *eoi);
  enum gpibHandshakeStates writeByte(uint8_t db, bool isLastByte);
  bool receiveData(Stream &dataStream, bool detectEoi, bool detectEndByte, uint8_t endByte);
  void sendData(const char *data, uint8_t dsize);
  void clearDataBus();
  void setControlVal(uint8_t value);
  void setDataVal(uint8_t value);

  bool isDeviceAddressedToListen();
  bool isDeviceAddressedToTalk();
  bool isDeviceInIdleState();

  void signalBreak();

  bool addressDevice(uint8_t pri, uint8_t sec, uint8_t dir);
  bool unAddressDevice();
  bool haveAddressedDevice();

private:

  bool txBreak;  // Signal to break the GPIB transmission
  uint8_t deviceAddressed;
  bool isTerminatorDetected(uint8_t bytes[3], uint8_t eorSequence);

  // Interrupt flag for MCP23S17
#ifdef AR488_MCP23S17
//    extern volatile bool mcpIntA;  // MCP23S17 interrupt handler
//    uint8_t mcpPinAssertedReg = 0;
#endif
};


#endif  // AR488_GPIBbus_H
