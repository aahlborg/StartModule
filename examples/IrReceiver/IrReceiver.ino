#include <StartModule.h>
#include <EEPROM.h>
#include <IntervalTimer.h>

#define IR_PIN 20
#define LED_PIN 13
#define START_MODULE_EEPROM_ADDR 0x40

// StartModule object
static StartModule * startModule;

// Motor state
int motorState = 0;

// Callback function
void stateChange(robot_state newState)
{
  if (STATE_RUNNING == newState)
    motorState = 1;
  else
    motorState = 0;

  Serial.printf("New state: %d motors: %d\n", newState, motorState);
}

void setup()
{
  Serial.begin(9600);
  startModule = new StartModule(
    IR_PIN,
    LED_PIN,
    START_MODULE_EEPROM_ADDR,
    stateChange);
}

void loop()
{
  delay(1000);
}

