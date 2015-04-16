/*
StartModule
Copyright (C) 2015 Marcus Ahlberg

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <ArduinoTest.h>
#include <cstdio>
#include <cstdarg>
#include <cassert>

/////////////////////
// Global variables
SerialObj Serial;

/////////////////////
// Local variables
static unsigned long time_;
#define NUM_PINS 30
static int pinFunctions_[NUM_PINS];
static void(*pinIsr_[NUM_PINS])(void);
static int pinVal_[NUM_PINS];

/////////////////////
// Stub functions

void SerialObj::printf(char * format, ...)
{
  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
}

void delay(int ticks)
{
  time_ += ticks * 1000;
}

unsigned long micros(void)
{
  return time_;
}

void pinMode(int pin, PinFunction mode)
{
  pinFunctions_[pin] = mode;
}

void attachInterrupt(int pin, void(*isr)(void), PinEvent event)
{
  // TODO: different events
  pinIsr_[pin] = isr;
}

int digitalRead(int pin)
{
  return pinVal_[pin];
}

void digitalWrite(int pin, int value)
{
  pinVal_[pin] = !!value;
}

/////////////////////
// Test interface

void playPinEvents(int pin, pinevent_t events[], int numEvents)
{
  if (pinIsr_[pin])
  {
    int startTime = time_;
    time_ += events[0].time;
    pinVal_[pin] = !!events[0].pinVal;
    //printf("%lu: pin %d: %d\n", time_, pin, pinVal_[pin]);
    pinIsr_[pin]();
    for (int i = 1; i < numEvents; ++i)
    {
      assert(events[i].time > events[0].time);
      time_ = startTime + events[i].time;
      pinVal_[pin] = !!events[i].pinVal;
      //printf("%lu: pin %d: %d\n", time_, pin, pinVal_[pin]);
      pinIsr_[pin]();
    }
  }
}
