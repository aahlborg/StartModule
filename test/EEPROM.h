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
 * EEPROM library stub
 */
#ifndef _EEPROM_H_
#define _EEPROM_H_

#include <Arduino.h>

#define EEPROM_SIZE 1024

class Eeprom
{
  public:
    char read(int addr);
    void write(int addr, char data);
  private:
    char _data[EEPROM_SIZE];
};

extern Eeprom EEPROM;

#endif