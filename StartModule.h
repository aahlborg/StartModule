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
