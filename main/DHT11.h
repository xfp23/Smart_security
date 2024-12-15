#ifndef DHT11_H
#define DHT11_H
#include <utility>
#include <tuple>
#include <iostream>
using namespace std;

class DHT11_Class
{

public:
DHT11_Class(unsigned int);
~DHT11_Class();
// void begin();
// float readHum();
// float readTemp();
pair<float, float> read();

private:
bool PinSTA = false;
unsigned int Data_pin;
uint8_t REDATA();
void WRDATA();
};


#endif
