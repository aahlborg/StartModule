#include "Arduino.h"
#include "StartModule.h"
#include "IrRc5.h"


#define PROGRAM_ADDR 0x0b
#define COMPETITION_ADDR 0x07
#define TESTING_ADDR 0x00

#define DEFAULT_STOP_CMD 0x04

static void irCmdHandler(rc5_cmd * data);

static StartModule * _startModule = NULL;

StartModule::StartModule(int irPin, int ledPin, int eepromAddr, void(*stateChangeFunc)(robot_state newState))
{
  _irPin = irPin;
  _ledPin = ledPin;
  _cmdBase = DEFAULT_STOP_CMD;
  _eepromAddr = eepromAddr;
  _stateChangeFunc = stateChangeFunc;
  _state = STATE_IDLE;

  IrRc5::init(_irPin);
  IrRc5::registerCmdHandler(irCmdHandler);

  _startModule = this;
  delay(7000);
  Serial.printf("_state: %d, _cmdBase: %d\n", _state, _cmdBase);
}

void StartModule::cmdHandler(int addr, int cmd)
{
  Serial.printf("Received RC5 event: addr 0x%x, cmd 0x%x\n", addr, cmd);

  if (STATE_STOPPED == _state)
  {
    // Don't do anything
    return;
  }

  switch (addr)
  {
  case PROGRAM_ADDR:
    Serial.printf("Programming address\n");
    // Program command
    program(cmd);
    break;
  case COMPETITION_ADDR:
    Serial.printf("Competition address\n");
    // Run command if correct dohyo
    break;
  case TESTING_ADDR:
    Serial.printf("Testing address\n");
    Serial.printf("_state: %d, cmd: %d, _cmdBase: %d\n", _state, cmd, _cmdBase);
    // Run command anyway
    if ((STATE_IDLE == _state) && (cmd == DEFAULT_STOP_CMD + 1)) //(cmd == _cmdBase + 1))
    {
      // Start
      setState(STATE_RUNNING);
    }
    else if ((STATE_RUNNING == _state) && (cmd == DEFAULT_STOP_CMD)) //(cmd == _cmdBase))
    {
      // Stop
      setState(STATE_STOPPED);
    }
    break;
  }
}

void StartModule::setState(robot_state state)
{
    Serial.printf("1Setting state %d\n", state);
  if (state <= STATE_STOPPED && state >= STATE_IDLE)
  {
    Serial.printf("2Setting state %d\n", state);
    // Update state
    _state = state;
    if (_stateChangeFunc)
    {
      // Notify
      _stateChangeFunc(_state);
    }
    // Save state?
  }
}

void StartModule::program(int cmd)
{
  Serial.printf("Programming cmd base %d\n", cmd);
  _cmdBase = cmd & 0xfe;
}

static void irCmdHandler(rc5_cmd * data)
{
  if (_startModule)
  {
    _startModule->cmdHandler(data->address, data->command);
  }
}
