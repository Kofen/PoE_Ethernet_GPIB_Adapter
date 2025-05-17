#pragma once

#include <Arduino.h>

//needed all the time
void setup_gpibBusConfig(void);

// needed only for prologix
void setup_prologix(void);
int loop_prologix(void);
