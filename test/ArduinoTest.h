/*
 * Test interface for Arduino stub
 */
#ifndef _ARDUINO_TEST_H_
#define _ARDUINO_TEST_H_

#include <Arduino.h>

struct pinevent_t
{
  unsigned long time;
  int pinVal;
};

void playPinEvents(int pin, pinevent_t events[], int numEvents);

#endif