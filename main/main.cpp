#include <stdio.h>
#include <Arduino.h>
#include "hal_idf_bletooth.h"
void setup() {
    
Serial.begin(115200);
InitBle_adv("Smart Security",DEFAULT_SERVICE_UUID,DEFAULT_CHAR_UUID,APPEARANCE_GENERIC_ACTUATOR);
}


void loop()
{
delay(1000);
}