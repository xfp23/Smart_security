#include "M_FileLog.h"
#include "Screen.h"
#include <SPI.h>
#include <Wire.h>

#define OLED_WIDTH 128
#define OLED_HIGH 64


OLED_Class::OLED_Class()
{
     display = new Adafruit_SSD1306(OLED_WIDTH, OLED_HIGH,OLED_MOSI,
     OLED_CLK, OLED_DC, OLED_RESET, OLED_CS); // 创建屏幕对象
    
}

OLED_Class::~OLED_Class()
{
    delete display;
}

/**
 * @brief Initializes the OLED display and prepares it for use.
 * This function should be called before any other display operations
 * to ensure the screen is properly configured.
 */

bool OLED_Class::begin()
{
    if(!display->begin(SSD1306_SWITCHCAPVCC)) {
    //Serial.println(F("SSD1306 allocation failed"));
    //for(;;); // Don't proceed, loop forever
     display->display();
     delay_ms(2000);
    return true;
  }
  return false;

}

void OLED_Class::Hum(uint8_t x, uint8_t y,float hum)
{
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(x, y);
    display->printf("Hum: %.2f",hum);
}

void OLED_Class::Temp(uint8_t x,uint8_t y,float temp)
{
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(x, y);
//    display->print("Temp:");
    display->printf("Temp: %.2f",temp);
}

void OLED_Class::Body(uint8_t x,uint8_t y,bool body)
{
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(x, y);
    if(body)
    {
     display->print("   Person");
     return;
    }
    display->print("NO Person");
}


void OLED_Class::smoke(uint8_t x, uint8_t y, float smoke)
{
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(x, y);
    display->printf("Smoke: %.2f",smoke);
}

