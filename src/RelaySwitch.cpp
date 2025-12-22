#include "RelaySwitch.h"

RelaySwitch::RelaySwitch(int pin1, int pin2, int pin3, int pin4) : pins{pin1, pin2, pin3, pin4}, status{} {}

void RelaySwitch::init() {
  Serial.println("Relay init");

  for (int i = 0; i < 4; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(pins[i]);
    Serial.print("\n");
    pinMode(pins[i], OUTPUT);
  }
  
  Serial.println("Zero Relay");
  zeroRelay();
  Serial.println("Update Relay");
  update();
}

void RelaySwitch::zeroRelay() {
  for (int i = 0; i < 4; i++) {
    Serial.println(i);
    status[i] = RELAY_OFF;
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


