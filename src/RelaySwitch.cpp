#include "RelaySwitch.h"

RelaySwitch::RelaySwitch() {}

void RelaySwitch::init(int pin1, int pin2, int pin3, int pin4) {
  // set all the relays OUTPUT
  pins[0] = pin1;
  pins[1] = pin2;
  pins[2] = pin3;
  pins[3] = pin4;

  for (int i = 0; i < 4; i++) {
    pinMode(pins[i], OUTPUT);
  }

  zeroRelay();
  update();
}

void RelaySwitch::zeroRelay() {
  for (int i = 0; i < 4; i++) {
    status[i] = 1;
  }
}

void RelaySwitch::setRelay(int pos, bool stat) { status[pos - 1] = stat; }

void RelaySwitch::setRelay(bool status_1, bool status_2, bool status_3,
                           bool status_4) {
  status[0] = status_1;
  status[1] = status_2;
  status[2] = status_3;
  status[3] = status_4;
}

void RelaySwitch::update() {
  for (int i = 0; i < 4; i++) {
    digitalWrite(pins[i], status[i]);
  }
  
}
void RelaySwitch::print() {
  for (int i = 0; i < 4; i++) {
    Serial.print(i);
    Serial.print("[");
    Serial.print(pins[i]);
    Serial.print("]: ");
    Serial.println(status[i]);
  }
}


