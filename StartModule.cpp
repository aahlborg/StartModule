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

#include "Arduino.h"
#include "StartModule.h"
#include "IrRc5.h"
#include "EEPROM.h"
#include "IntervalTimer.h"
#include <stdint.h>

#define PROGRAM_ADDR 0x0b
#define COMPETITION_ADDR 0x07
#define TESTING_ADDR 0x00

#define DEFAULT_STOP_CMD 0x04

#define EEPROM_MAGIC 0xab

#define TIMER_PERIOD 250000
#define ONE_SEC_IN_TIMER_TICKS 4

// State data saved in EEPROM
struct stateData {
  uint8_t magic;
  uint8_t cmdBase;
  uint8_t state;
};

// State transitions
enum operation {
  OP_NOP,
  OP_PROGRAM,
  OP_START,
  OP_STOP
};

static void irCmdHandler(rc5_cmd * data);
static void timerIsr();

static StartModule * startModule_ = NULL;

StartModule::StartModule(int irPin, int ledPin, int eepromAddr, void(*stateChangeFunc)(robot_state newState))
{
  irPin_ = irPin;
  ledPin_ = ledPin;
  eepromAddr_ = eepromAddr;
  stateChangeFunc_ = stateChangeFunc;

  // Init IR receiver
  IrRc5::init(irPin_);
  IrRc5::registerCmdHandler(irCmdHandler);

  pinMode(ledPin_, INPUT);

  timer_ = new IntervalTimer();

  // Read state and cmdBase from EEPROM
  loadData();
  if (STATE_STOPPED == state_)
  {
    state_ = STATE_IDLE;
  }
  setState(state_);

  startModule_ = this;
}

StartModule::~StartModule()
{
  delete timer_;
}

void StartModule::cmdHandler(int addr, int cmd)
{
  operation op = OP_NOP;
  //Serial.printf("Received RC5 event: addr 0x%x, cmd 0x%x\n", addr, cmd);

  switch (addr)
  {
  case PROGRAM_ADDR:
    //Serial.printf("Programming address\n");
    op = OP_PROGRAM;
    break;
  case COMPETITION_ADDR:
    //Serial.printf("Competition address\n");
    // Run command if correct dohyo
    if (cmd == cmdBase_ + 1)
    {
      op = OP_START;
    }
    else if (cmd == cmdBase_)
    {
      op = OP_STOP;
    }
    break;
  case TESTING_ADDR:
    //Serial.printf("Testing address\n");
    // Run default test command
    if (cmd == DEFAULT_STOP_CMD + 1)
    {
      op = OP_START;
    }
    else if (cmd == DEFAULT_STOP_CMD)
    {
      op = OP_STOP;
    }
    break;
  }

  switch (state_)
  {
  case STATE_IDLE:
    if (OP_PROGRAM == op)
    {
      program(cmd);
      setState(STATE_PROGRAM);
    }
    else if (OP_START == op)
    {
      setState(STATE_RUNNING);
    }
    else if (OP_STOP == op)
    {
      setState(STATE_STOP_SAFE);
    }
    break;

  case STATE_PROGRAM:
    /*if (OP_START == op)
    {}
    else if (OP_STOP == op)
    {}*/
    break;

  case STATE_RUNNING:
    if (OP_PROGRAM == op)
    {
      program(cmd);
      setState(STATE_PROGRAM);
    }
    else if (OP_STOP == op)
    {
      setState(STATE_STOP_SAFE);
    }
    break;

  case STATE_STOP_SAFE:
  case STATE_STOPPED:
    // No operations allowed
    break;

  default:
    // Something is wrong, stop
    setState(STATE_STOP_SAFE);
    break;
  }
}

void StartModule::timerHandler()
{
  --blinkCount_;
  if (blinkLed_)
  {
    digitalWrite(ledPin_, blinkCount_ & 0x1);
  }

  //Serial.printf("StartModule::timerHandler: blinkCount %d\n", blinkCount_);

  if (0 == blinkCount_)
  {
    timer_->end();
    blinkLed_ = false;
    switch (state_)
    {
    case STATE_PROGRAM:
      // Go to idle
      setState(STATE_IDLE);
      break;

    case STATE_STOPPED:
      // Restart timer to blink forever
      setLed(2);
      break;

    case STATE_STOP_SAFE:
    default:
      // Go to stopped
      setState(STATE_STOPPED);
      break;
    }
  }
}

void StartModule::setState(robot_state state)
{
  if (state >= STATE_IDLE && state <= STATE_STOPPED)
  {
    //Serial.printf("Setting state %d\n", state);
    // Update state
    state_ = state;
    if (stateChangeFunc_)
    {
      // Notify
      stateChangeFunc_(state_);
    }
    // Save state
    saveData();
    switch (state_)
    {
    case STATE_RUNNING:
      setLed(1);
      break;

    case STATE_PROGRAM:
    case STATE_STOP_SAFE:
    case STATE_STOPPED:
      setLed(2);
      break;

    default:
      setLed(0);
      break;
    }
  }
}

void StartModule::saveData()
{
  stateData data;
  data.magic = EEPROM_MAGIC;
  data.cmdBase = (uint8_t)cmdBase_;
  data.state = (uint8_t)state_;

  for (uint i = 0; i < sizeof(data); ++i)
  {
    uint8_t * data_p = (uint8_t *)&data;
    EEPROM.write(eepromAddr_ + i, data_p[i]);
  }
}

void StartModule::loadData()
{
  stateData data;

  for (uint i = 0; i < sizeof(data); ++i)
  {
    uint8_t * data_p = (uint8_t *)&data;
    data_p[i] = EEPROM.read(eepromAddr_ + i);
  }

  if (EEPROM_MAGIC == data.magic &&
      (data.cmdBase & 0x1) == 0 &&
      data.state <= STATE_STOPPED)
  {
    cmdBase_ = (int)data.cmdBase;
    state_ = (robot_state)data.state;
  }
  else
  {
    cmdBase_ = DEFAULT_STOP_CMD;
    state_ = STATE_IDLE;
  }
}

void StartModule::program(int cmd)
{
  //Serial.printf("Programming cmd base %d\n", cmd);
  cmdBase_ = cmd & 0xfe;
}

void StartModule::setLed(int state)
{
  blinkCount_ = 0;
  blinkLed_ = false;
  if (1 == state)
  {
    // On
    digitalWrite(ledPin_, HIGH);
  }
  else if (2 == state)
  {
    // Blink
    digitalWrite(ledPin_, LOW);
    blinkLed_ = true;
    blinkCount_ = ONE_SEC_IN_TIMER_TICKS;
    timer_->begin(timerIsr, TIMER_PERIOD);
  }
  else
  {
    // Off
    digitalWrite(ledPin_, LOW);
  }
}

static void irCmdHandler(rc5_cmd * data)
{
  if (startModule_)
  {
    startModule_->cmdHandler(data->address, data->command);
  }
}

static void timerIsr()
{
  if (startModule_)
  {
    startModule_->timerHandler();
  }
}

// Interface for unit testing
#ifdef TEST
void testTriggerIrCmd(rc5_cmd * data)
{
  irCmdHandler(data);
}
#endif
