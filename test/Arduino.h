/*
 * Stub of Arduino defines
 */
#ifndef _ARDUINO_H_
#define _ARDUINO_H_

#include <cstddef>
#include <cstdlib>

/////////////////////
// Defines

#define LOW 0
#define HIGH 1

enum PinFunction
{
  INPUT,
  OUTPUT
};

enum PinEvent
{
  RISING,
  FALLING,
  CHANGE
};

/////////////////////
// Classes

class SerialObj
{
public:
  static void begin(int baudRate) {}
  static void printf(char * format, ...);
};

extern SerialObj Serial;

/////////////////////
// Functions

void delay(int ticks);
unsigned long micros(void);

void pinMode(int pin, PinFunction mode);
void attachInterrupt(int pin, void(*isr)(void), PinEvent event);

int digitalRead(int pin);
void digitalWrite(int pin, int value);

#endif
