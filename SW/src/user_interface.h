#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

bool setup_ipaddress_surveillance_and_show_address(void);

// void setup_led(void);
// void loop_led(void);

void setup_serial_ui_and_led(const __FlashStringHelper* helloStr);
void end_of_setup();

void loop_serial_ui_and_led(int nrConnections);


#endif