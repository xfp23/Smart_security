#include <stdio.h>
#include "hal_idf.h"
#include "ble.h"
#include "oled.h"
#include "Beep.h"
#include "DHT11.h"
#include "pins.h"
#include "MQ_2.h"
Nimble_Server ble;
DHT11_Class dht(DHT11_PIN);
Beep_Class beep;
MQ_2_Class MQ;

extern "C" void app_main(void)
{
    ble.begin("Smart_security");
    MQ.begin(ADC2,ADC_CHANNEL_9);
  // OLED_Init();
   //OLED_Display_On();
   //LED_ON;
   DELAY_MS(1000);

   OLED_ShowString(0,0,(u8 *)"hello world");
   gpio_set_level(OLED_DC_PIN,1);
    beep.begin(BEEP_PIN);
   // beep.turn_on(2,150);
    while (1)
    {
       if(ble.bleSta()) 
       {

        printf("%s",ble.get_BleData());

        DELAY_MS(5000);
        ble.Write((uint8_t *)"hello world",12);

       }
       
        dht.read();
        printf("Hum : %.2f  Temp : %.2f MQ-2 : %.2f%%\n",dht.getHum(),dht.getTemp(),MQ.read());
        //OLED_ShowNum(0,0,MQ.read(),2,16);
        DELAY_MS(1000);
    }
}
