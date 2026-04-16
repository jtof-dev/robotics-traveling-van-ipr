#ifndef MOTOR_HPP
#define MOTOR_HPP

#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <Arduino.h>

class MOTOR {
public:
  MOTOR(int in1_pin, int in2_pin);
  void set_power(float power);

private:
  int _in1;
  int _in2;
};

#endif
