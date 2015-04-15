#include "StartModule.h"

#define IR_PIN 20
#define LED_PIN 13
#define START_MODULE_EEPROM_ADDR 0x40

void stateChange(robot_state newState)
{
  Serial.printf("New state: %d\n", newState);
}

void setup()
{
  Serial.begin(9600);
  delay(10000);
  StartModule startModule(IR_PIN,
                          LED_PIN,
                          START_MODULE_EEPROM_ADDR,
                          stateChange);
}

void loop()
{
  delay(1000);
}

