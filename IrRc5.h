#ifndef IR_RC5_H
#define IR_RC5_H

#include "Arduino.h"

class IrRc5
{
  public:
    IrRc5(int pin);
  private:
    int _pin;
};

#endif