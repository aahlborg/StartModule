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

#ifndef IR_RC5_H
#define IR_RC5_H

#include "Arduino.h"

typedef struct {
  unsigned char address;
  unsigned char command;
} rc5_cmd;

class IrRc5
{
  public:
    static void init(int pin);
    static void registerCmdHandler(void(*func)(rc5_cmd * cmd));
  private:
    IrRc5();
};

#endif
