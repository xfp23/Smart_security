#ifndef SCREEN_H
#define SCREEN_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class OLED_Class
{

public:
OLED_Class();//构造函数
~OLED_Class();//析构函数
bool begin();
void OLED_main(); // OLED主显示
void Temp(uint8_t,uint8_t,float); // 显示温度
void Hum(uint8_t x, uint8_t y,float hum); // 显示湿度
void Body(uint8_t x,uint8_t y,bool body);
void smoke(uint8_t x, uint8_t y, float smoke);
private :
//static const unsigned char logo_bmp[20];
Adafruit_SSD1306 *display;

};

#endif