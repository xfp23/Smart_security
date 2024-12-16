#include <stdio.h>
#include "hal_idf.h"
#include "ble.h"

Nimble_Server ble;
extern "C" void app_main(void)
{
    ble.begin("Smart_security");
    while (1)
    {
       if(ble.bleSta()) 
       {
        printf("%s",ble.get_BleData());
        DELAY_MS(5000);
        ble.Write((uint8_t *)"hello world",12);
       }
        DELAY_MS(1000);
    }
}
