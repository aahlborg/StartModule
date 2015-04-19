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

#include <stdio.h>
#include <stdint.h>
#include <StartModule.h>
#include <IrRc5.h>
#include <EEPROM.h>
#include <IntervalTimer.h>
#include <ArduinoTest.h>

using namespace std;

// Test macros
#define RUN(test) do { \
  printf("\nStarting test " #test "\n"); \
  testPassed = true; \
  test(); \
  if (!testPassed) { suitePassed = false; } \
  printf("Test " #test " %s\n", testPassed ? "SUCCESSFUL" : "FAILED"); \
  } while(0)

#define START_TEST(suite) do { \
  suitePassed = true; \
  printf("Running test suite " #suite "\n"); \
  } while(0)

#define END_TEST do { \
  if (suitePassed) { printf("\nAll tests passed SUCCESSFULLY\n"); } \
  else { printf("\nSome tests FAILED\n"); } \
  } while(0)

#define VERIFY(expr) do { \
  if (!(expr)) \
  { \
    testPassed = false; \
    printf("Expression evaluated false: \"" #expr "\"\n"); \
  } \
  } while(0)

static bool testPassed, suitePassed;

// Test interface for StartModule
void testTriggerIrCmd(rc5_cmd * data);

#define IR_PIN 20
#define LED_PIN 13
#define EEPROM_ADDR 0x40

pinevent_t cmd5[] = {
  {0, 0},
  {934, 1},
  {1758, 0},
  {3552, 1},
  {4404, 0},
  {5305, 1},
  {6161, 0},
  {7058, 1},
  {7918, 0},
  {8821, 1},
  {9675, 0},
  {10574, 1},
  {11432, 0},
  {12338, 1},
  {13189, 0},
  {14091, 1},
  {14946, 0},
  {15875, 1},
  {16703, 0},
  {17639, 1},
  {19328, 0},
  {21144, 1},
  {22842, 0},
  {23774, 1}
};

pinevent_t cmd4[] = {
  {0, 0},
  {933, 1},
  {1757, 0},
  {2686, 1},
  {3514, 0},
  {5336, 1},
  {6160, 0},
  {7058, 1},
  {7917, 0},
  {8821, 1},
  {9674, 0},
  {10574, 1},
  {11430, 0},
  {12327, 1},
  {13187, 0},
  {14090, 1},
  {14945, 0},
  {15843, 1},
  {16700, 0},
  {17596, 1},
  {19324, 0},
  {21113, 1},
  {21969, 0},
  {22865, 1}
};

static int motorState_;

static void stateChangeFunc(robot_state newState)
{
  //Serial.printf("Hello state %d\n", newState);
  if (STATE_RUNNING == newState)
  {
    motorState_ = 1;
  }
  else
  {
    motorState_ = 0;
  }
}

/*static void printEeprom()
{
  for (int i = 0; i < 3; ++i)
  {
    uint8_t data = EEPROM.read(EEPROM_ADDR + i);
    Serial.printf("0x%04x: 0x%02x\n", EEPROM_ADDR + i, data);
  }
}*/

static void writeEepromData(robot_state state, int cmdBase)
{
  EEPROM.write(EEPROM_ADDR, 0xab);
  EEPROM.write(EEPROM_ADDR + 1, cmdBase);
  EEPROM.write(EEPROM_ADDR + 2, state);
}

static void resetEeprom()
{
  EEPROM.write(EEPROM_ADDR, 0);
  EEPROM.write(EEPROM_ADDR + 1, 0);
  EEPROM.write(EEPROM_ADDR + 2, 0);
}

/////////////////////
// Tests

static void testRc5()
{
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(0 == motorState_);

  playPinEvents(IR_PIN, cmd5, sizeof(cmd5) / sizeof(cmd5[0]));

  VERIFY(startModule.getState() == STATE_RUNNING);
  VERIFY(digitalRead(LED_PIN) == 1);
  VERIFY(1 == motorState_);

  playPinEvents(IR_PIN, cmd4, sizeof(cmd4) / sizeof(cmd4[0]));

  VERIFY(startModule.getState() == STATE_STOP_SAFE);
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(0 == motorState_);

  resetEeprom();
}

static void testProgram()
{
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  rc5_cmd prgm = {0xb, 0xa};
  rc5_cmd start = {0x7, 0xa + 1};

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  testTriggerIrCmd(&start);

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  testTriggerIrCmd(&prgm);

  VERIFY(startModule.getState() == STATE_PROGRAM);
  VERIFY(0 == motorState_);
  VERIFY(digitalRead(LED_PIN) == 1);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 1);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  testTriggerIrCmd(&start);

  VERIFY(startModule.getState() == STATE_RUNNING);
  VERIFY(1 == motorState_);

  resetEeprom();
}

static void testStopSafe()
{
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  rc5_cmd stop = {0x7, 0x4};

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  testTriggerIrCmd(&stop);

  VERIFY(startModule.getState() == STATE_STOP_SAFE);
  VERIFY(0 == motorState_);
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(startModule.getState() == STATE_STOPPED);
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(0 == motorState_);

  resetEeprom();
}

static void testRestartStopped()
{
  writeEepromData(STATE_STOPPED, 0x04);
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  resetEeprom();
}

static void testRestartStopSafe()
{
  writeEepromData(STATE_STOP_SAFE, 0x04);
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  VERIFY(startModule.getState() == STATE_STOP_SAFE);
  VERIFY(0 == motorState_);
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(startModule.getState() == STATE_STOPPED);
  VERIFY(0 == motorState_);

  resetEeprom();
}

static void testRestartProgram()
{
  writeEepromData(STATE_PROGRAM, 0x04);
  motorState_ = -1;

  StartModule startModule(IR_PIN, LED_PIN, EEPROM_ADDR, stateChangeFunc);

  VERIFY(startModule.getState() == STATE_PROGRAM);
  VERIFY(0 == motorState_);
  VERIFY(digitalRead(LED_PIN) == 1);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 1);
  triggerHwTimer();
  VERIFY(digitalRead(LED_PIN) == 0);
  VERIFY(startModule.getState() == STATE_IDLE);
  VERIFY(0 == motorState_);

  resetEeprom();
}

int main(int argc, char * argv[])
{
  Serial.begin(9600);

  START_TEST(StartModule);

  RUN(testRc5);
  RUN(testProgram);
  RUN(testStopSafe);
  RUN(testRestartStopped);
  RUN(testRestartStopSafe);
  RUN(testRestartProgram);

  END_TEST;

  return 0;
}
