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

#ifndef START_MODULE_H
#define START_MODULE_H

#include "Arduino.h"

typedef enum {
  STATE_IDLE,
  STATE_RUNNING,
  STATE_STOPPED
} robot_state;

class StartModule
{
  public:
    StartModule(int irPin, int ledPin, int eepromAddr, void(*stateChangeFunc)(robot_state newState));
    void cmdHandler(int addr, int cmd);
  private:
    void setState(robot_state state);
    void saveData();
    void program(int cmd);

  	int _irPin;
  	int _ledPin;
    int _cmdBase;
    int _eepromAddr;
    robot_state _state;
    void(*_stateChangeFunc)(robot_state newState);
};

#endif
