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
