#include <iostream>
#include <StartModule.h>
#include <ArduinoTest.h>

using namespace std;

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

static void stateChangeFunc(robot_state newState)
{
  cout << "Hello state " << newState << "\n";
}

int main(int argc, char * argv[])
{
  Serial.begin(9600);
  Serial.printf("Hello, world\n");

  StartModule startModule(20, 13, 0, stateChangeFunc);

  delay(100000);
  playPinEvents(20, cmd5, sizeof(cmd5) / sizeof(cmd5[0]));
  delay(100000);
  playPinEvents(20, cmd4, sizeof(cmd4) / sizeof(cmd4[0]));

  return 0;
}