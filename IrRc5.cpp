#include "Arduino.h"
#include "IrRc5.h"


////////////////////////
// Defines
////////////////////////
#define HALF_BIT_LEN 889
#define BIT_LEN 1778
#define RC5_LEN 24900
#define ADDR_LEN 5
#define CMD_LEN 6

#define TOLERANCE 10
#define APPROX_EQ(A, B) ( (abs((A) - (B)) * 200 / ((A) + (B))) < (TOLERANCE) )

static void irIsr(void);


////////////////////////
// Types
////////////////////////
typedef enum {
  IR_STARTBIT1 = 0,
  IR_STARTBIT2 = 1,
  IR_TOGGLEBIT = 2,
  IR_ADDR = 3,
  IR_CMD = 4
} status_t;


////////////////////////
// Local variables
////////////////////////
static int irPin;
static status_t status;
static int toggle = -1;
static void(*ir_data_func)(rc5_cmd * data) = 0;


////////////////////////
// Public interface
////////////////////////
void IrRc5::init(int pin)
{
  irPin = pin;
  pinMode(irPin, INPUT);
  attachInterrupt(irPin, irIsr, CHANGE);
}

void IrRc5::registerCmdHandler(void(*func)(rc5_cmd * cmd))
{
  ir_data_func = func;
}


////////////////////////
// Interrupt handler
////////////////////////
static void irIsr(void)
{
  static int index = 0;
  static int addr = 0;
  static int cmd = 0;
  static int toggle_last = 0;
  static int cnt = 0;
  static int lastCnt = 0;

  const int time = micros();
  const int edge = digitalRead(irPin);

  // Save count
  cnt += time - lastCnt;
  lastCnt = time;

  if (cnt > RC5_LEN)
  {
    // Reset state
    status = IR_STARTBIT1;
  }

  switch (status)
  {
  case IR_STARTBIT1:
    if (edge == LOW)
    {
      status = IR_STARTBIT2;
      cnt = 0;
    }
    else
    {
      status = IR_STARTBIT1;
    }
    break;

  case IR_STARTBIT2:
    if (APPROX_EQ(cnt, BIT_LEN))
    {
      status = IR_TOGGLEBIT;
      cnt = 0;
    }
    else if (APPROX_EQ(cnt, HALF_BIT_LEN))
    {
      // Synchronize
      cnt = HALF_BIT_LEN;
      return;
    }
    else
    {
      status = IR_STARTBIT1;
      return;
    }
    break;

  case IR_TOGGLEBIT:
    if (APPROX_EQ(cnt, BIT_LEN))
    {
      toggle_last = toggle;

      if (LOW == edge)
      {
        toggle = 1;
      }
      else
      {
        toggle = 0;
      }

      index = 0;
      addr = 0;
      cmd = 0;

      cnt = 0;
      status = IR_ADDR;
    }
    else if (APPROX_EQ(cnt, HALF_BIT_LEN))
    {
      // Synchronize
      cnt = HALF_BIT_LEN;
      return;
    }
    else
    {
      status = IR_STARTBIT1;
      return;
    }
    break;

  case IR_ADDR:
    if (APPROX_EQ(cnt, BIT_LEN))
    {
      if (edge == LOW)
      {
        addr |= 1 << (ADDR_LEN - index - 1);
      }
      else
      {
        // Zero at position index
      }

      index++;
      if (index == ADDR_LEN)
      {
        index = 0;
        status = IR_CMD;
      }
      cnt = 0;
    }
    else if (APPROX_EQ(cnt, HALF_BIT_LEN))
    {
      cnt = HALF_BIT_LEN;
      return;
    }
    else
    {
      status = IR_STARTBIT1;
      return;
    }
    break;

  case IR_CMD:
    if (APPROX_EQ(cnt, BIT_LEN))
    {
      if (edge == LOW)
      {
        cmd |= 1 << (CMD_LEN - index - 1);
      }
      else
      {
        // Zero at position index
      }

      index++;
      if (index == CMD_LEN)
      {
        if (toggle != toggle_last)
        {
          if (ir_data_func)
          {
            static rc5_cmd tmp_data;
            tmp_data.address = addr;
            tmp_data.command = cmd;
            ir_data_func(&tmp_data);
          }
        }

        status = IR_STARTBIT1;
      }

      cnt = 0;
    }
    else if (APPROX_EQ(cnt, HALF_BIT_LEN))
    {
      // Synchronize
      cnt = HALF_BIT_LEN;
      return;
    }
    else
    {
      status = IR_STARTBIT1;
      return;
    }
    break;
  }
}
