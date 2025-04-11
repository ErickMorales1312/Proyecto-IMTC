#ifndef HTML_h
#define HTML_h

#include <Arduino.h>

extern bool State2;
extern bool State1;
extern bool State0;
extern float heading;
extern float bearing;
extern float latitud;
extern float longitud;

String SendHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0);
String generateButtonHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0);


#endif