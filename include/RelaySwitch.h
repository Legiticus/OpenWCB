#ifndef RELAY_SWITCH
#define RELAY_SWITCH

#include <Arduino.h>

#define RELAY_ON 0
#define RELAY_OFF 1

class RelaySwitch {
private:
  bool status[4];
  int pins[4];

public:
  RelaySwitch(int pin1, int pin2, int pin3, int pin4);

  void init();

  void zeroRelay();

  void setRelay(int pos, bool stat);

  void setRelay(bool status_1, bool status_2, bool status_3, bool status_4);
  
  void update();

  void print();
};

#endif