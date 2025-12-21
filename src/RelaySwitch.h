#ifndef RELAY_SWITCH
#define RELAY_SWITCH

#include <Arduino.h>

class RelaySwitch {
private:
  bool status[4];
  int pins[4];

public:
  RelaySwitch();

  void init(int pin1, int pin2, int pin3, int pin4);

  void zeroRelay();

  void setRelay(int pos, bool stat);

  void setRelay(bool status_1, bool status_2, bool status_3, bool status_4);
  
  void update();

  void print();
};

#endif