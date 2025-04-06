#ifndef HTML_h
#define HTML_h

#include <Arduino.h>

extern float distance;
extern long duration;
extern bool State2;
extern bool State1;
extern bool State0;
extern float heading;
extern float bearing;


String SendHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0);
String generateButtonHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0);


#endif