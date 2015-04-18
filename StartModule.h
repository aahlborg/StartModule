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
  STATE_PROGRAM,
  STATE_RUNNING,
  STATE_STOP_SAFE,
  STATE_STOPPED
} robot_state;

class IntervalTimer;

class StartModule
{
  public:
    StartModule(int irPin, int ledPin, int eepromAddr, void(*stateChangeFunc)(robot_state newState));
    ~StartModule();
    void cmdHandler(int addr, int cmd);
    void timerHandler();
  private:
    void setState(robot_state state);
    void saveData();
    void loadData();
    void program(int cmd);
    void setLed(int state);

  	int irPin_;
  	int ledPin_;
    int cmdBase_;
    int eepromAddr_;
    robot_state state_;
    void(*stateChangeFunc_)(robot_state newState);
    bool blinkLed_;
    int blinkCount_;
    IntervalTimer * timer_;
};

#endif
