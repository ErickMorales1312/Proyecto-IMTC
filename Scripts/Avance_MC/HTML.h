#ifndef HTML_h
#define HTML_h

#include <Arduino.h>

extern int LDRValue1;
extern int LDRValue2;
extern float distance;
extern long duration;
extern bool State2;
extern bool State1;
extern bool State0;

String SendHTML(uint8_t Status2, uint8_t Status1, uint8_t Status0);

#endif