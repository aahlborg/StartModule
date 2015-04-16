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
